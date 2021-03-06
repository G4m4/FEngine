#pragma once

#include "core/fanSystem.hpp"

#ifdef NDEBUG
    #define fanAssertMsg( _expression, _msg )
    #define fanAssert( _expression )
#else
    namespace fan
    {
        struct AssertUtils
        {
            static bool sFanAssertBreakEnabled;
            static void LogAssertMessage( const char* _msg, const char* _func, int _line, const char* _file );
        };
    }

    #define fanAssertMsg( _expression, _msg )                                               \
        do                                                                                  \
        {                                                                                   \
            if( ! (_expression) )                                                           \
            {                                                                               \
                if( fan::AssertUtils::sFanAssertBreakEnabled ){ FAN_DEBUG_BREAK }           \
                fan::AssertUtils::LogAssertMessage( _msg, __func__, __LINE__, __FILE__ );   \
            }                                                                               \
        }                                                                                   \
        while( false )

    #define fanAssert( _expression ) fanAssertMsg( _expression, "" )
#endif


