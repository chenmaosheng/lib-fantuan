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
#define TYPELIST_1(T1)				TypeList<T1, TYPEliST_0>
#define TYPELIST_2(T1, T2)			TypeList<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3)		TypeList<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4>	TypeList<T1, TYPELIST_3(T2, T3, T4)>

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
	typedef typename TypeAt<Tail, i - 1>::Result Result;
};

}

#endif

