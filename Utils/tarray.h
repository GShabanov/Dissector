#pragma once
#ifndef __TARRAY__H__
#define __TARRAY__H__

template<typename Type> class allocator {};

#ifdef _USE_THROW
#define CARRAY_THROW(x)   throw(x)
#else
__declspec(noreturn) extern void CArrayFatal();
#define CARRAY_THROW(x) CArrayFatal();
#endif // _USE_THROW

class CArrayP
{
protected:
    // Construction
    CArrayP(SIZE_T sizeOfType)  throw()
        : m_sizeofType(sizeOfType)
    {
        m_pData = NULL;
        m_nSize = 0;
        m_nMaxSize = 0;
        m_nGrowBy = 0;
    }

public:

    // Attributes
    INT_PTR size() const {
        return m_nSize;
    }

    INT_PTR count() const {
        return m_nSize;
    }

    bool isEmpty() const {
        return m_nSize == 0; 
    }

    INT_PTR GetUpperBound() const {
        return (INT_PTR)(m_nSize - 1);
    }

    void FreeExtra();
    INT_PTR Append(const CArrayP& src);

    bool SetSize(INT_PTR nNewSize, INT_PTR nGrowBy = -1);
    void Copy(const CArrayP& src);

    void RemoveAt(INT_PTR nIndex, INT_PTR nCount = 1);

protected:

    virtual void initElement(PVOID element) = 0;
    virtual void destroyElement(PVOID element) = 0;

    // Direct Access to the element data (may return NULL)
    const BYTE * GetData() const {
        return (const BYTE *)m_pData;
    }

    BYTE *GetData() {
        return (BYTE *)m_pData;
    }

    SIZE_T         m_sizeofType;
    unsigned char *m_pData;     // the actual array of data
    INT_PTR        m_nSize;     // # of elements (upperBound - 1)
    INT_PTR        m_nMaxSize;  // max allocated
    INT_PTR        m_nGrowBy;   // grow amount

};

/////////////////////////////////////////////////////////////////////////////
// template for shared using 
//

template<class T,
         class RTYPE = const T&,
         typename Allocator = allocator<T>>
class TArray : public CArrayP
{
public:
    //
    // Construction
    //
    TArray()   throw()
        : CArrayP(sizeof(T))
    {
    }

    ~TArray()
    {
        if (m_pData != NULL) {

            for (INT_PTR i = 0; i < m_nSize; i++) {

                this->destroyElement(m_pData + (i * m_sizeofType));
            }

            delete[] (BYTE*)m_pData;
        }
    }

    // Operations
    // Clean up
    void RemoveAll() { SetSize(0, -1); }

    T& GetAt(INT_PTR nIndex)
    { 
        ASSERT(nIndex >= 0 && nIndex < m_nSize);

        if (nIndex >= 0 && nIndex < m_nSize)
            return ((T *)GetData())[nIndex]; 

        CARRAY_THROW(0);
    }

    const T& GetAt(INT_PTR index) const
    {
        ASSERT(nIndex >= 0 && nIndex < m_nSize);

        if (nIndex >= 0 && nIndex < m_nSize)
            return ((const T *)GetData())[nIndex]; 

        CARRAY_THROW(0);
    }


    bool SetAt(INT_PTR nIndex, RTYPE newElement);

    //
    // growing and insert to array
    //
    bool SetAtGrow(INT_PTR nIndex, RTYPE newElement);

    INT_PTR Add(const RTYPE newElement);

    //
    // overloaded operator helpers
    //
    const T& operator[] (INT_PTR nIndex) const {
        return GetAt(nIndex);
    }

    T& operator[] (INT_PTR nIndex) {
        return GetAt(nIndex);
    }

    // Operations that move elements around
    bool InsertAt(INT_PTR nIndex, RTYPE newElement, INT_PTR nCount = 1);
    bool InsertAt(INT_PTR nStartIndex, TArray* pNewArray);


public:
    /*TArray(const TArray& src)
    {
        m_pData = NULL;
        m_nSize = 0;
        m_nMaxSize = 0;
        m_nGrowBy = 0;

        SetSize(src.m_nSize);

        memcpy(m_pData, src.m_pData, src.m_nSize * m_sizeofType);
    }*/

private:

    virtual void initElement(PVOID element)
    {

        T *type;

        type = (T *)element;

        ::new (element) T;
    }

    virtual void destroyElement(PVOID element)
    {
        T *type;

        type = (T *)element;

        type->~T();
    }

    TArray(const TArray& src);                    // no implementation
    void operator=(const TArray& objectSrc);      // no implementation

};

template<class T, class RTYPE, typename Allocator>
__inline bool 
TArray<T, RTYPE, Allocator>::SetAtGrow(INT_PTR nIndex, RTYPE newElement)
{
    
    if (nIndex < 0)
        return false;

    if (nIndex >= m_nSize)
    {
        if (SetSize(nIndex + 1, -1) != true) {
            return false;
        }
    }

    *((T *)m_pData + nIndex) = newElement;

    return true;
}

template<class T, class TYPE, typename Allocator>
__inline bool 
TArray<T, TYPE, Allocator>::SetAt(INT_PTR nIndex, TYPE newElement)
{ 
    ASSERT(nIndex >= 0 && nIndex < m_nSize);

    if (nIndex >= 0 && nIndex < m_nSize) {
        ((const T *)GetData())[nIndex] = newElement; 
    }
    else
    {
        return false;
    }

    return true;
}


template<class T, class RTYPE, typename Allocator>
__inline INT_PTR 
TArray<T, RTYPE, Allocator>::Add(RTYPE newElement)
{
    INT_PTR nIndex = m_nSize;
    if (SetAtGrow(nIndex, newElement) != true)
        return -1;

    return nIndex;
}



template<class T, class RTYPE, typename Allocator>
bool
TArray<T, RTYPE, Allocator>::InsertAt(INT_PTR nIndex, RTYPE newElement, INT_PTR nCount /*=1*/)
{
    ASSERT(nIndex >= 0);    // will expand to meet need
    ASSERT(nCount > 0);     // zero or negative size not allowed

    if (nIndex < 0 || nCount <= 0)
        return false;

    if (nIndex >= m_nSize)
    {
        //
        // adding after the end of the array
        // grow so nIndex is valid
        //
        if (SetSize(nIndex + nCount, -1) != true) {

            return false;
        }
    }
    else
    {
        //
        // inserting in the middle of the array
        //
        INT_PTR nOldSize = m_nSize;

        //
        // grow it to new size
        //
        if (SetSize(m_nSize + nCount, -1) != true)
            return false;

        //
        // destroy intial data before copying over it
        //
        for (int i = 0; i < nCount; i++)
        {
            this->destroyElement(m_pData + (nOldSize + i) * m_sizeofType);
        }

        //
        // shift old data up to fill gap
        //
        memmove(m_pData + nIndex + nCount, m_pData + nIndex, (nOldSize - nIndex) * m_sizeofType);

        //
        // re-init slots with null we copied from
        //
        memset(m_pData + nIndex * m_sizeofType, 0, (size_t)nCount * m_sizeofType);

        for (int i = 0; i < nCount; i++) {

            this->initElement(m_pData + (nIndex + i) * m_sizeofType);
        }

    }

    //
    // insert new value in the gap
    //
    ASSERT(nIndex + nCount <= m_nSize);

    while (nCount--) {

        m_pData[nIndex++] = newElement;
    }

    return true;
}


template<class T, class RTYPE, typename Allocator>
__inline bool 
TArray<T, RTYPE, Allocator>::InsertAt(INT_PTR nStartIndex, TArray* pNewArray)
{
    ASSERT(pNewArray != NULL);
    ASSERT_VALID(pNewArray);
    ASSERT(nStartIndex >= 0);

    if (pNewArray == NULL || nStartIndex < 0) {

        return false;
    }

    if (pNewArray->GetSize() > 0)
    {
        if (InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize()) != true) {

            return false;
        }

        for (INT_PTR i = 0; i < pNewArray->GetSize(); i++)
        {

            if (SetAt(nStartIndex + i, pNewArray->GetAt(i)) != true) {

                return false;
            }
        }
    }

    return true;
}

#endif // __TARRAY__H__