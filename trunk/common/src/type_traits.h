#ifndef _H_TYPE_TRAITS
#define _H_TYPE_TRAITS

#include "type_list.h"
#include "type.h"

namespace Fantuan
{

typedef TYPELIST_4(uint8, uint16, uint32, uint64)	UnsignedInts;
typedef TYPELIST_4(int8, int16, int32, int64)		SignedInts;
typedef TYPELIST_3(bool, char, wchar_t)				OtherInts;
typedef TYPELIST_2(float32, float64)				Floats;

template<typename T>
class IsArray
{
private:
	template<typename>
	struct Type2Type2 {};

	typedef char (&yes)[1];
	typedef char (&no)[2];

	template<typename U, size_t N>
	static yes	is_array1(Type2Type2<U[N]>*);
	static no	is_array1(...);

	template<typename U>
	static yes	is_array2(Type2Type2<U[]>*);
	static no	is_array2(...);

public:
	enum { value = sizeof(is_array1((Type2Type2<T>*)0)) == sizeof(yes) ||
		sizeof(is_array2((Type2Type2<T>*)0)) == sizeof(yes) };
};

template<typename T>
class TypeTraits
{
	typedef char	(&yes)[1];
	typedef char	(&no)[2];

	template<typename U>
	static yes	is_reference(Type2Type<U&>);
	static no	is_reference(...);

	template<typename U>
    static yes is_pointer1(Type2Type<U*>);
    static no  is_pointer1(...);

    template<typename U>
    static yes is_pointer2(Type2Type<U const *>);
    static no  is_pointer2(...);

    template<typename U>
    static yes is_pointer3(Type2Type<U volatile *>);
    static no  is_pointer3(...);

    template<typename U>
    static yes is_pointer4(Type2Type<U const volatile *>);
    static no  is_pointer4(...);

    template<typename U, typename V>
    static yes is_pointer2member(Type2Type<U V::*>);
    static no  is_pointer2member(...);

    template<typename U>
    static yes is_const(Type2Type<const U>);
    static no  is_const(...);

    template<typename U>
    static yes is_volatile(Type2Type<volatile U>);
    static no  is_volatile(...);

public:
	enum { isReference = sizeof(is_reference(Type2Type<T>())) == sizeof(yes) };

	enum { isPointer = sizeof(is_pointer1(Type2Type<T>())) == sizeof(yes) ||
				sizeof(is_pointer2(Type2Type<T>())) == sizeof(yes) ||
				sizeof(is_pointer3(Type2Type<T>())) == sizeof(yes) ||
				sizeof(is_pointer4(Type2Type<T>())) == sizeof(yes) };

	enum { isMemberPointer = sizeof(is_pointer2member(Type2Type<T>())) == sizeof(yes) };
    
    enum { isArray = IsArray<T>::value };

    enum { isVoid = IsSameType<T, void>::value  ||
				IsSameType<T, const void>::value    ||
				IsSameType<T, volatile void>::value ||
				IsSameType<T, const volatile void>::value };

	enum { isUnsignedInt = IndexOf< UnsignedInts, T >::value >= 0 };
    enum { isSignedInt = IndexOf<SignedInts, T>::value >= 0 };
    enum { isIntegral = isUnsignedInt || isSignedInt || IndexOf<OtherInts, T>::value >= 0 };
    enum { isFloat = IndexOf<Floats, T>::value >= 0 };
    enum { isArith = isIntegral || isFloat };
    enum { isFundamental = isArith || isFloat || isVoid };

    enum { isConst = sizeof(is_const(Type2Type<T>())) == sizeof(yes) };

    enum { isVolatile = sizeof(is_volatile(Type2Type<T>())) == sizeof(yes) };

	struct is_scalar
    {
    private:
        struct BoolConvert { BoolConvert(bool); };

        static yes check(BoolConvert);
        static no  check(...);

        struct NotScalar {};

        typedef typename Select
        <
            isVoid || isReference || isArray, 
            NotScalar, T
        >
        ::Result RetType;
        
        static RetType& get();

    public:
#ifdef WIN32
#pragma warning (disable: 4800)
#endif
        enum { value = sizeof(check(get())) == sizeof(yes) };
    };

	enum { isScalar = is_scalar::value, 
			isClass = !isScalar && !isArray && !isReference && !isVoid, };
};

}

#endif
