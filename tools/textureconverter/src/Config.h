#ifndef CONFIG_H
#define CONFIG_H

////////////////////////////////////////////////////////////
// Identify the operating system
////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(__WIN32__)

    // Windows
    #define SYSTEM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif

    #include <windows.h>

#elif defined(linux) || defined(__linux)

    // Linux
    #define SYSTEM_LINUX

#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)

    // MacOS
    #define SYSTEM_MACOS

#elif defined(__FreeBSD__)

    // FreeBSD
    #define SYSTEM_FREEBSD

#else

    // Unsupported system
    #error This operating system is not supported

#endif


////////////////////////////////////////////////////////////
// Identify the endianess
////////////////////////////////////////////////////////////
#if defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || defined(__sparc__) || defined(__hppa__)

    // Big endian
    #define ENDIAN_BIG

#else

    // Little endian
    #define ENDIAN_LITTLE

#endif

////////////////////////////////////////////////////////////
// Define a portable debug macro
////////////////////////////////////////////////////////////
#if !defined(NDEBUG)

    #define DEBUG

#endif


////////////////////////////////////////////////////////////
// Define portable fixed-size types
////////////////////////////////////////////////////////////
#include <limits.h>

// 8 bits integer types
#if UCHAR_MAX == 0xFF
    typedef signed   char Int8;
    typedef unsigned char UInt8;
#else
    #error No 8 bits integer type for this platform
#endif

// 16 bits integer types
#if USHRT_MAX == 0xFFFF
    typedef signed   short Int16;
    typedef unsigned short UInt16;
#elif UINT_MAX == 0xFFFF
    typedef signed   int Int16;
    typedef unsigned int UInt16;
#elif ULONG_MAX == 0xFFFF
    typedef signed   long Int16;
    typedef unsigned long UInt16;
#else
    #error No 16 bits integer type for this platform
#endif

// 32 bits integer types
#if USHRT_MAX == 0xFFFFFFFF
    typedef signed   short Int32;
    typedef unsigned short UInt32;
#elif UINT_MAX == 0xFFFFFFFF
    typedef signed   int Int32;
    typedef unsigned int UInt32;
#elif ULONG_MAX == 0xFFFFFFFF
    typedef signed   long Int32;
    typedef unsigned long UInt32;
#else
    #error No 32 bits integer type for this platform
#endif

// ANSI C types

#ifndef bool // Boolean
	#define bool	UInt8
#endif

#ifndef false
	#define false	0x00
#endif

#ifndef true
	#define true 	0x01
#endif

#ifndef NULL // NULL
	#define NULL ((void*)0)
#endif

#ifndef NULLCHAR
	#define NULLCHAR '\0'
#endif

////////////////////////////////////////////////////////////
// Application version definition
////////////////////////////////////////////////////////////
#define APP_MAJOR_VERSION		1
#define APP_MINOR_VERSION		0
#define APP_REVISION_VERSION 	0

#endif // CONFIG_H
