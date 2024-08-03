#pragma once

#if _MSC_VER >= 1700

# include <xtr1common>

# define ASSERT_TYPE_FLOAT(   T ) static_assert( std::is_floating_point<T>::value, "A floating point type is required here!" )
# define ASSERT_TYPE_INT(     T ) static_assert( std::is_integral<T>::value,       "An integer type is required here!" )
# define ASSERT_TYPE_POINTER( T ) static_assert( std::is_pointer<T>::value,        "A pointer type is required here!" )
# define ASSERT_TYPE_ARRAY(   T ) static_assert( std::is_array<T>::value,          "An array type is required here!" )
# define ASSERT_TYPE_ENUM(    T ) static_assert( std::is_enum<T>::value,           "An enumerated type is required here!" )
# define ASSERT_TYPE_CLASS(   T ) static_assert( std::is_class<T>::value,          "A class or struct type is required here!" )
# define ASSERT_TYPE_UNION(   T ) static_assert( std::is_union<T>::value,          "A union type is required here!" )

# define ASSERT_TYPE_SAME(     T, U ) static_assert( std::is_same<T, U>::value,        "A specific type is required here!" )
# define ASSERT_TYPE_BASEOF(   T, U ) static_assert( std::is_base_of<T, U>::value,     "A specific type or descendant is required here!" )
# define ASSERT_TYPE_FUNDAMENTAL( T ) static_assert( std::is_fundamental<T>::value,    "A fundamental type is required here!" )
# define ASSERT_TYPE_INT_OR_FLT(  T ) static_assert( std::is_arithmetic<T>::value,     "An integer or floating point type is required here!" )
# define ASSERT_TYPE_MEMBER_PTR(  T ) static_assert( std::is_member_pointer<T>::value, "A member pointer type is required here!" )
# define ASSERT_TYPE_OBJECT(      T ) static_assert( std::is_object<T>::value,         "An object type is required here!" )

#else

# define ASSERT_TYPE_FLOAT(   T )
# define ASSERT_TYPE_INT(     T )
# define ASSERT_TYPE_POINTER( T )
# define ASSERT_TYPE_ARRAY(   T )
# define ASSERT_TYPE_ENUM(    T )
# define ASSERT_TYPE_CLASS(   T )
# define ASSERT_TYPE_UNION(   T )

# define ASSERT_TYPE_SAME(     T, U )
# define ASSERT_TYPE_BASEOF(   T, U )
# define ASSERT_TYPE_FUNDAMENTAL( T )
# define ASSERT_TYPE_INT_OR_FLT(  T )
# define ASSERT_TYPE_MEMBER_PTR(  T )

#endif // _MSC_VER >= 1700
