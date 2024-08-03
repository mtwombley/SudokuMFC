#pragma once

#include <cmath>
#include <cfloat>
#include "TemplateTypeAsserts.h"

namespace MathHelper
{
    template<typename T>
    struct EpsilonFor
    {
        ASSERT_TYPE_FLOAT( T );
        static T const value;
    };

    template<typename T>
    struct MaxValFor
    {
        ASSERT_TYPE_FLOAT( T );
        static T const value;
    };

    template<typename T>
    T Min( T a, T b )
    {
        ASSERT_TYPE_FLOAT( T );
        if ( le( a, b ) )
            return a;
        else
            return b;
    };

    template<typename T>
    T Max( T a, T b )
    {
        ASSERT_TYPE_FLOAT( T );
        if ( ge( a, b ) )
            return a;
        else
            return b;
    }

    template<>
    float const EpsilonFor<float>::value = 1.e-4f;

    template<>
    double const EpsilonFor<double>::value = 1.e-6;

    template<>
    float const MaxValFor<float>::value = FLT_MAX;

    template<>
    double const MaxValFor<double>::value = DBL_MAX;

    //This method is twice as fast as using the floor method
    template<typename T, typename U>
    T Round( const T& v1, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        ASSERT_TYPE_FLOAT( U );

        T intPart;
        T fractPart;
        fractPart = modf( v1, &intPart );
        fractPart /= p;

        fractPart += static_cast<T>( _copysign( 0.50000088, fractPart ) );

        T intPart2;
        modf( fractPart, &intPart2 );
        fractPart = intPart2 * p;

        return intPart + fractPart;
    }

    template<typename T>
    T Round( const T& v1 )
    {
        return Round( v1, EpsilonFor<T>::value );
    }

    template<typename T>
    T Truncate( const T& v1 )
    {
        return Truncate( v1, EpsilonFor<T>::value );
    }

    template<typename T, typename U>
    T Truncate( const T& v1, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        ASSERT_TYPE_FLOAT( U );
        T intPart, intPart2, fractPart;
        fractPart = modf( v1, &intPart );
        fractPart /= (T)p;
        modf( fractPart, &intPart2 );
        fractPart = intPart2 * (T)p;
        return intPart + fractPart;
    }

    template<typename T>
    T pi()
    {
        ASSERT_TYPE_FLOAT( T );
        return (T)3.1415926535897932384626433832795;
    }

    template<typename T>
    T pi2()
    {
        ASSERT_TYPE_FLOAT( T );
        return pi<T>() * (T)2.0;
    }

    //pi/2
    template<typename T>
    T pi_2()
    {
        ASSERT_TYPE_FLOAT( T );
        return pi<T>() / (T)2.0;
    }

    template<typename T>
    T pi15()
    {
        ASSERT_TYPE_FLOAT( T );
        return ( (T)3.0 * pi<T>() ) / (T)2.0;
    }

    //pi * 2.5
    template<typename T>
    T pi25()
    {
        ASSERT_TYPE_FLOAT( T );
        return (T)7.8539816339744830961566084581988;
    }

    //180/pi
    template<typename T>
    T calc180_pi()
    {
        ASSERT_TYPE_FLOAT( T );
        return (T)180.0 / pi<T>();
    }

    //pi/180
    template<typename T>
    T calcpi_180()
    {
        ASSERT_TYPE_FLOAT( T );
        return pi<T>() * (T)180.0;
    }

    //radians to degrees
    template<typename T>
    T rad2deg( const T& v1 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 * calc180_pi<T>();
    }

    //degrees to radians
    template<typename T>
    T deg2rad( const T& v1 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 * calcpi_180<T>();
    }

    // Make input between 0 and 2*pi
    template<typename T>
    T normalRad( const T& v1 )
    {
        ASSERT_TYPE_FLOAT( T );
        T retVal = v1;
        retVal = fmod( v1, pi2<T>() );
        if ( retVal < (T)0.0 )
        {
            retVal += pi2<T>();
        }
        return retVal;
    }

    // Make input between 0 and 2*pi
    template<typename T>
    void normalizeRad( T& v1 )
    {
        ASSERT_TYPE_FLOAT( T );
        v1 = normalRad<T>( v1 );
    }


    //Comparison functions

    //greater than
    template<typename T, typename U>
    bool gt( const T& v1, const T& v2, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        ASSERT_TYPE_FLOAT( U );
        return v1 - v2 > p;
    }

    template<typename T>
    bool gt( const T& v1, const T& v2 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 - v2 > EpsilonFor<T>::value;
    }

    //less than
    template<typename T, typename U>
    bool lt( const T& v1, const T& v2, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 - v2 < -p;
    }

    template<typename T>
    bool lt( const T& v1, const T& v2 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 - v2 < -EpsilonFor<T>::value;
    }

    //equal
    template<typename T, typename U>
    bool eq( const T& v1, const T& v2, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        T d = v1 - v2;
        return d < p && d > -p;
    }

    template<typename T>
    bool eq( const T& v1, const T& v2 )
    {
        ASSERT_TYPE_FLOAT( T );
        T d = v1 - v2;
        return d < EpsilonFor<T>::value&& d > -EpsilonFor<T>::value;
    }

    //not equal
    template<typename T, typename U>
    bool ne( const T& v1, const T& v2, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        T d = v1 - v2;
        return d >= p || d <= -p;
    }

    template<typename T>
    bool ne( const T& v1, const T& v2 )
    {
        return ne( v1, v2, EpsilonFor<T>::value );
    }

    //close to zero
    template<typename T, typename U>
    bool ez( const T& v1, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        ASSERT_TYPE_FLOAT( U );
        return v1 < p&& v1 > -p;
    }

    template<typename T>
    bool ez( const T& v1 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 < EpsilonFor<T>::value&& v1 > -EpsilonFor<T>::value;
    }

    //not close to zero
    template<typename T, typename U>
    bool nz( const T& v1, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        ASSERT_TYPE_FLOAT( U );
        return v1 >= p || v1 <= -p;
    }

    template<typename T>
    bool nz( const T& v1 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 >= EpsilonFor<T>::value || v1 <= -EpsilonFor<T>::value;
    }

    //greater than or equal
    template<typename T, typename U>
    bool ge( const T& v1, const T& v2, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        ASSERT_TYPE_FLOAT( U );
        return v1 - v2 > -p;
    }

    template<typename T>
    bool ge( const T& v1, const T& v2 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 - v2 > -EpsilonFor<T>::value;
    }

    //less than or equal
    template<typename T, typename U>
    bool le( const T& v1, const T& v2, const U p )
    {
        ASSERT_TYPE_FLOAT( T );
        ASSERT_TYPE_FLOAT( U );
        return v1 - v2 < p;
    }

    template<typename T>
    bool le( const T& v1, const T& v2 )
    {
        ASSERT_TYPE_FLOAT( T );
        return v1 - v2 < EpsilonFor<T>::value;
    }

    //Linear Interpolate
    template<typename T>
    T LinearInterpolate( const T& p1, const T& p2, const T& t1, const T& t2, const T& t3 )
    {
        return  ( p1 + ( p2 - p1 ) / ( t2 - t1 ) * ( t3 - t1 ) );
    }

    //is valid real number
    template<typename T>
    bool IsValidReal( const T& v1 )
    {
        ASSERT_TYPE_FLOAT( T );
        int	fpClass = _fpclass( v1 );
        switch ( fpClass )
        {
            case _FPCLASS_SNAN: /* signaling NaN */
            case _FPCLASS_QNAN: /* quiet NaN */
            case _FPCLASS_NINF: /* negative infinity */
            case _FPCLASS_ND:   /* negative denormalized */
            case _FPCLASS_PD:   /* positive denormalized */
            case _FPCLASS_PINF:	/* positive infinity */
                return false;
            case _FPCLASS_PN:  /* positive normal */
            case _FPCLASS_NN:  /* negative normal */
            case _FPCLASS_NZ:  /* -0 */
            case _FPCLASS_PZ:  /* +0 */
                return true;
        }
        return true;
    }
}
