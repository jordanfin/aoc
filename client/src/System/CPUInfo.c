////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/CPUInfo.h>
#include <pdh.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
UInt32		CPUFeatures		= 0;		///< CPU features list
static bool CPUReadEnabled	= false;
HQUERY		CPUQueryHandle;
HCOUNTER	CPUCounterHandle;

////////////////////////////////////////////////////////////
// Methods
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
///	Obtains a list of features the cpu supports
////////////////////////////////////////////////////////////
UInt32 CPUFeaturesInitialize()
{
	UInt32 Features;

	__asm
	{
		mov eax, 1			// Set request type to a feature list from processor
		cpuid				// Send request
		lea ebx, Features	
		mov [ebx], edx		// Save value
	}

	CPUFeatures = Features;

	return Features;
}

////////////////////////////////////////////////////////////
///	Initialize the features list and the cpu queries
////////////////////////////////////////////////////////////
void CPUInitialize()
{
	//char CounterPath[0xFF] = "\\Processor(0)\\% Processor Time";
	
	//LPTSTR lpCounterPath = (TCHAR*)malloc(sizeof(TCHAR) * strlen(CounterPath));
	
	//_tcscpy_s(lpCounterPath, strlen(CounterPath), CounterPath);

	CPUFeaturesInitialize();

	// Create a query object to poll cpu usage
	//CPUReadEnabled = (bool)(PdhOpenQuery(NULL, 0, &CPUQueryHandle) == ERROR_SUCCESS);

	// Set query object to poll all cpus in the system	
	//CPUReadEnabled = (bool)(PdhAddCounter(CPUQueryHandle, lpCounterPath/*"\\Processor(_Total)\\% processor time"*/, 0, &CPUCounterHandle) == ERROR_SUCCESS);
}

////////////////////////////////////////////////////////////
///	Destroy all structures which we used for get the cpu info
////////////////////////////////////////////////////////////
void CPUDestroy()
{
	if (CPUReadEnabled)
	{
		PdhCloseQuery(CPUQueryHandle);
	}
}

////////////////////////////////////////////////////////////
/// Check if the cpu supports a feature
////////////////////////////////////////////////////////////
UInt32 CPUFeatureCheck(UInt32 Feature)
{
	// Check if it isn't initialized
	if (!CPUFeatures)
		return 0x10000001;

	// Check if the feature is available
	if (CPUFeatures & Feature)
		return Feature;

	return 0;
}

////////////////////////////////////////////////////////////
/// Gets the cpu id
////////////////////////////////////////////////////////////
Int8 * CPUGetID()
{
	static Int8 ID[13];

	__asm
	{
		lea ebx, ID
		mov cl, 0

CLEARID:
		inc cl
		mov dword ptr [ebx], 0
		cmp cl, 13
		jb CLEARID
	}

	__asm
	{
		xor eax, eax		// Set request type to the string ID
		cpuid				// Send request
		
		xchg eax, ebx		// Swap so address register can be used
		lea ebx, ID			// Get destination string offset
							
		mov [ebx], eax		// Save first four letters of string
		add ebx, 4			// Go up anther four bytes			
							// Repeat for next two registers
		mov [ebx], edx
		add ebx, 4

		mov [ebx], ecx
	}

	return ID;
}

////////////////////////////////////////////////////////////
///	Return the current usage of the cpu
////////////////////////////////////////////////////////////
UInt32 __cdecl CPUGetUsage()
{
	if (CPUReadEnabled)
	{
		PDH_FMT_COUNTERVALUE Value;

		PdhCollectQueryData(CPUQueryHandle);
        
		PdhGetFormattedCounterValue(CPUCounterHandle, PDH_FMT_LONG, NULL, &Value);

		return (UInt32)Value.longValue;
	}

	return 0;
}