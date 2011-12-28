////////////////////////////////////////////////////////////
///	When cpuid is called with a value of 0 in the
///	EAX register, it returns a list of values in the
///	EDX register. Each bit represents a feature
///	(excluding reserved bits). If the bit is set, the
///	feature exists. If it's cleared the feature
///	isn't supported.
////////////////////////////////////////////////////////////

#ifndef SYSTEM_CPUINFO_H
#define SYSTEM_CPUINFO_H

// TODO: Improve this with the other one

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define FEATURE_ERROR   0x10000001	///< CPU feature check had a problem
#define FEATURE_X87FPU  0x00000001	///< CPU has x87 fpu
#define FEATURE_VMODEE  0x00000002  ///< CPU has Virtual 8086 mode enhancement
#define FEATURE_DEBEXT  0x00000004  ///< CPU has debugging extensions
#define FEATURE_PGSEXT	0x00000008  ///< CPU has page extensions
#define FEATURE_TSCOUN	0x00000010  ///< CPU has time stamp counter
#define FEATURE_XXMSRS	0x00000020  ///< CPU has RDMSR/WRMSR inst support
#define FEATURE_PHAEXT	0x00000040  ///< CPU has physical address extension
#define FEATURE_MCHEXT	0x00000080  ///< CPU has machine check exception
#define FEATURE_CMPXCH	0x00000100  ///< CPU has 8 byte cmp and xchg inst support
#define FEATURE_APICON	0x00000200  ///< CPU has APIC on chip
#define FEATURE_SYSXXX	0x00000800  ///< CPU has SYSENTER/SYSLEAVE
#define FEATURE_MTRREG	0x00001000  ///< CPU has memory type range registers
#define FEATURE_PTEBIT	0x00002000  ///< CPU has global PTE bit
#define FEATURE_MCKARC	0x00004000  ///< CPU has machine check architexture
#define FEATURE_CMOVIN	0x00008000  ///< CPU has CMOV inst support
#define FEATURE_PGATTT	0x00010000  ///< CPU has page attribute table
#define FEATURE_PGSEX2	0x00020000  ///< CPU has page size extension
#define FEATURE_SERNUM	0x00040000  ///< CPU has processor serial number
#define FEATURE_CFLUSH	0x00080000  ///< processor supports CFLUSH inst
#define FEATURE_DBSTOR	0x00200000  ///< CPU has debug store
#define FEATURE_ACPICT	0x00400000  ///< ACPI on processor
#define FEATURE_MMXCMP	0x00800000  ///< CPU has MMX technology
#define FEATURE_FXXSTO	0x01000000	///< CPU supports FXSAVE/FXRSTORE inst
#define FEATURE_SSEEXT	0x02000000  ///< CPU has SSE extensions
#define FEATURE_SSE2EX  0x04000000  ///< CPU has sse2 extensions
#define FEATURE_SSNOOP	0x08000000  ///< CPU supports system snoop
#define FEATURE_PTHERM	0x10000000  ///< CPU has process thermomitor

////////////////////////////////////////////////////////////
///	Initialize the features list and the cpu queries
////////////////////////////////////////////////////////////
void CPUInitialize();

////////////////////////////////////////////////////////////
///	Destroy all structures which we used for get the cpu info
////////////////////////////////////////////////////////////
void CPUDestroy();

////////////////////////////////////////////////////////////
/// Pass this one of the above defined features, -1 = error and
/// if it returns the same value passed the feature exists if it
/// returns 0 then the feature is un supported
////////////////////////////////////////////////////////////
UInt32 __cdecl CPUFeatureCheck(UInt32 Feature); 

////////////////////////////////////////////////////////////
/// Returns a pointer to a string that the processor sends to
/// the EBX, EDX, and ECX registers. e.g. GenuineIntel or
/// AuthenticAMD. 
////////////////////////////////////////////////////////////
Int8 * __cdecl CPUGetID();

////////////////////////////////////////////////////////////
///	Return the current usage of the cpu
////////////////////////////////////////////////////////////
UInt32 __cdecl CPUGetUsage();

#endif // SYSTEM_CPUINFO_H
