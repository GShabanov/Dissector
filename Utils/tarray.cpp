#include "stdafx.h"

#include "tarray.h"

#ifndef _USE_THROW
__declspec(noreturn) 
void CArrayFatal()
{
}
#endif // _USE_THROW

bool
CArrayP::SetSize(INT_PTR nNewSize, INT_PTR nGrowBy)
{

    if (nNewSize < 0) {

        return false;
    }

    if (nGrowBy >= 0) {

        m_nGrowBy = nGrowBy;  // set new size
    }

    if (nNewSize == 0)
    {
        //
        // shrink to NULL size
        //
        if (m_pData != NULL)
        {

            for (INT_PTR i = 0; i < m_nSize; i++ )
            {
                this->destroyElement(m_pData + (i * m_sizeofType));
            }

            delete[] (BYTE*)m_pData;
            m_pData = NULL;
        }

        m_nSize = m_nMaxSize = 0;

    }
    else if (m_pData == NULL)
    {
        //
        // create buffer big enough to hold number of requested elements or
        // m_nGrowBy elements, whichever is larger.
        //
#ifdef SIZE_T_MAX
        ASSERT(nNewSize <= SIZE_T_MAX / m_sizeofType);    // no overflow
#endif
        size_t nAllocSize = max(nNewSize, m_nGrowBy);
        m_pData = (BYTE *) new BYTE[(size_t)nAllocSize * m_sizeofType];

        if (m_pData != 0)
        {

            memset((void*)m_pData, 0, (size_t)nAllocSize * m_sizeofType);

            for (INT_PTR i = 0; i < nNewSize; i++)
            {
                this->initElement(m_pData + i * m_sizeofType);
            }

            m_nSize = nNewSize;
            m_nMaxSize = nAllocSize;

        }
        else
        {
            return false;
        }

    }
    else if (nNewSize <= m_nMaxSize)
    {
        //
        // it fits
        //
        if (nNewSize > m_nSize)
        {
            //
            // initialize the new elements
            //
            memset(
                (void*)(m_pData + m_nSize * m_sizeofType),
                0,
                (size_t)(nNewSize - m_nSize) * m_sizeofType);

            for (INT_PTR i = 0; i < nNewSize - m_nSize; i++ )
            {
                this->initElement(m_pData + (m_nSize + i) * m_sizeofType);
            }

        }
        else if (m_nSize > nNewSize)
        {
            //
            // destroy the old elements
            //
            for (INT_PTR i = 0; i < m_nSize - nNewSize; i++ )
            {
                this->destroyElement(m_pData + (nNewSize + i) * m_sizeofType);
            }
        }

        m_nSize = nNewSize;
    }
    else
    {
        //
        // otherwise, grow array
        //
        nGrowBy = m_nGrowBy;

        if (nGrowBy == 0)
        {
            // heuristically determine growth when nGrowBy == 0
            //  (this avoids heap fragmentation in many situations)
            nGrowBy = m_nSize >> 3;
            nGrowBy = (nGrowBy < 4) ? 4 : ((nGrowBy > 1024) ? 1024 : nGrowBy);
        }

        INT_PTR nNewMax;

        if (nNewSize < m_nMaxSize + nGrowBy)
            nNewMax = m_nMaxSize + nGrowBy;  // granularity
        else
            nNewMax = nNewSize;  // no slush
        
        if (nNewMax < m_nMaxSize)
        {
            return false;
        }

#ifdef SIZE_T_MAX
        ASSERT(nNewMax <= SIZE_T_MAX / m_sizeofType); // no overflow
#endif
        BYTE *pNewData = new BYTE[(size_t)nNewMax * m_sizeofType];

        if (pNewData != 0)
        {

            // copy new data from old
            memcpy(pNewData, m_pData, (size_t)m_nSize * m_sizeofType);

            memset((void*)(pNewData + m_nSize), 0, (size_t)(nNewSize - m_nSize) * m_sizeofType);

            for (INT_PTR i = 0; i < nNewSize - m_nSize; i++ )
            {
                this->initElement(pNewData + (m_nSize + i) * m_sizeofType);
            }

            //
            // cleanup old data
            //
            delete[] (BYTE*)m_pData;

            m_pData = (BYTE *)pNewData;
            m_nSize = nNewSize;
            m_nMaxSize = nNewMax;
        }
        else
        {
            return false;
        }
    }

    return true;
}

void 
CArrayP::FreeExtra()
{

    if (m_nSize != m_nMaxSize)
    {
        // shrink to desired size
#ifdef SIZE_T_MAX
        ASSERT(m_nSize <= SIZE_T_MAX / m_sizeofType); // no overflow
#endif
        BYTE *pNewData = NULL;

        if (m_nSize != 0)
        {
            pNewData = new BYTE[m_nSize * m_sizeofType];
            //
            // copy new data from old
            //
            memcpy(pNewData, m_pData, m_nSize * m_sizeofType);
        }

        //
        // cleanup old data
        //
        delete[] (BYTE*)m_pData;

        m_pData = pNewData;
        m_nMaxSize = m_nSize;
    }
}

INT_PTR 
CArrayP::Append(const CArrayP& src)
{
    ASSERT(this != &src);
    
    if (this == &src) {

        CARRAY_THROW(0);
    }

    INT_PTR nOldSize = m_nSize;

    if (SetSize(m_nSize + src.m_nSize) != true)
    {
        return -1;
    }

    memcpy(m_pData + nOldSize * m_sizeofType, src.m_pData, src.m_nSize * m_sizeofType);

    return nOldSize;
}

void
CArrayP::Copy(const CArrayP& src)
{
    ASSERT(this != &src);

    if (this != &src)
    {
        if (SetSize(src.m_nSize) == true)
        {
            memcpy(m_pData, src.m_pData, src.m_nSize * m_sizeofType);
        }

    }
}

void
CArrayP::RemoveAt(INT_PTR nIndex, INT_PTR nCount)
{
    ASSERT(nIndex >= 0);
    ASSERT(nCount >= 0);

    INT_PTR nUpperBound = nIndex + nCount;
    ASSERT(nUpperBound <= m_nSize && nUpperBound >= nIndex && nUpperBound >= nCount);

    if (nIndex < 0 ||
        nCount < 0 ||
        (nUpperBound > m_nSize) ||
        (nUpperBound < nIndex)  ||
        (nUpperBound < nCount))
    {
        CARRAY_THROW(0);
    }

    // just remove a range
    INT_PTR nMoveCount = m_nSize - (nUpperBound);

    for (INT_PTR i = 0; i < nCount; i++ ) {

        destroyElement(m_pData + (nIndex + i) * m_sizeofType);
    }

    if (nMoveCount)
    {
        memmove(
            m_pData + (nIndex * m_sizeofType),
            m_pData + (nUpperBound * m_sizeofType),
            (size_t)nMoveCount * m_sizeofType);
    }

    m_nSize -= nCount;
}
