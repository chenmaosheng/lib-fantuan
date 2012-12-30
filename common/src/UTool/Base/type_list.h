#ifndef _H_TYPELIST
#define _H_TYPELIST

namespace Fantuan
{

class NullType{};

template<typename T, typename U>
struct TypeList
{
	typedef T Head;
	typedef U Tail;
};

#define TYPELIST_0					NullType
#define TYPELIST_1(T1)				TypeList<T1, TYPELIST_0>
#define TYPELIST_2(T1, T2)			TypeList<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3)		TypeList<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4)	TypeList<T1, TYPELIST_3(T2, T3, T4)>

template<typename TList>
struct Length;

template<>
struct Length<NullType>
{
	enum { value = 0, };
};

template<typename T, typename U>
struct Length< TypeList<T, U> >
{
	enum { value = 1 + Length<U>::value, };
};

template<typename TList, unsigned int index>
struct TypeAt;

template<typename Head, typename Tail>
struct TypeAt<TypeList<Head, Tail>, 0>
{
	typedef Head Result;
};

template<typename Head, typename Tail, unsigned int index>
struct TypeAt<TypeList<Head, Tail>, index>
{
	typedef typename TypeAt<Tail, index - 1>::Result Result;
};

template<typename T>
struct Type2Type
{
	typedef T OriginalType;
	Type2Type(){}
};

template <typename T, typename U>
struct IsSameType
{
private:
    template<typename>
    struct In 
    { enum { value = false }; };

    template<>
    struct In<T>
    { enum { value = true };  };

public:
    enum { value = In<U>::value };
};

template <typename TList, typename T> 
struct IndexOf
{
    typedef typename TList::Head Head;
    typedef typename TList::Tail Tail;

private:
	template<typename TList1>
	struct In
    {
    private:
        typedef typename TList1::Tail Tail;

        enum { temp = (IndexOf<Tail, T>::value) };

    public:
        enum { value = temp == -1 ? -1 : 1 + temp  };
    };

    template<>
    struct In< TypeList<T, Tail> >
    {
        enum { value = 0 };
    };

    template<>
    struct In< NullType >
    {
        enum { value = -1 };
    };

public:
    enum { value = In<TList>::value };
};

template<typename T>
struct IndexOf< NullType, T >
{
	enum { value = -1 };
};

template <bool flag, typename T, typename U>
struct Select
{
private:
    template<bool>
    struct In 
    { typedef T Result; };

    template<>
    struct In<false>
    { typedef U Result; };

public:
    typedef typename In<flag>::Result Result;
};

}

#endif

