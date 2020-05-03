#pragma once
#ifndef __SINGLETON__H__
#define __SINGLETON__H__

class SingletonBase
{
};

template<
    class Impl,
    class Base = SingletonBase
    >
class Singleton : public Base
{
public:
    static inline Impl *getInstance()
    {		
        return (Impl)m_body;
    }

    inline void* operator new( size_t /*size*/ )
    {
        return m_body;
    }

    inline void operator delete( void* /*p*/ )
    {
    }

    inline operator bool() const
    {
        return m_inited;
    }

    inline void Initalized()
    {
        m_inited = true;
    }

protected:
    inline Singleton()
    {
    }

    inline ~Singleton()
    {
    }

private:
    static char m_body[];
    static bool m_inited;

    Singleton( const Singleton& );
    Singleton& operator=( const Singleton& );
};

template<
    class Impl,
    class Base
    >
char Singleton<Impl, Base>::m_body[ sizeof( Impl ) ];

template<
    class Impl,
    class Base
    >
bool Singleton<Impl, Base>::m_inited;

#endif // __SINGLETON__H__