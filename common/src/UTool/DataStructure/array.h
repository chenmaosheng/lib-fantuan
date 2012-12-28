#ifndef _H_ARRAY
#define _H_ARRAY

namespace Fantuan
{

template<typename T, std::size_t N>
class Array
{
public:
	T	m_Elements[N];

public:
	// type definitions
	typedef T			value_type;
	typedef T*			interator;
	typedef const T*	const_interator;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef std::size_t	size_type;

	// operator[]
	reference operator[](size_type i)
	{
		// todo: assert
		return m_Elements[i];
	}

	const_reference operator[](size_t i) const
	{
		// todo: assert
		return m_Elements[i];
	}

};

// template specification
template<typename T>
class Array<T, 0>
{
public:
	// todo: all opeations are invalid
};

}

#endif

