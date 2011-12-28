////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Math/General.h>
#include <System/CPUInfo.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define ANGLE_CIRCLE_SIZE			512
#define ANGLE_CIRCLE_HALF_SIZE		(ANGLE_CIRCLE_SIZE / 2)
#define ANGLE_CIRCLE_QUARTER_SIZE	(ANGLE_CIRCLE_SIZE / 4)
#define ANGLE_CIRCLE_MASK			(ANGLE_CIRCLE_SIZE - 1)

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
float SinCosAngleTable[ANGLE_CIRCLE_SIZE]; ///< Fast sinus and cosinus table

////////////////////////////////////////////////////////////
/// Square root SSE function
////////////////////////////////////////////////////////////
inline float MathSqrtSSE(float Value)
{
    __asm
    {
        movss xmm0, Value
        rsqrtss xmm0, xmm0
        rcpss xmm0, xmm0
        movss Value, xmm0
    }

    return Value;
}

////////////////////////////////////////////////////////////
/// Converts a float to int (by NVidia web site)
////////////////////////////////////////////////////////////
inline void ftoi(int * intptr, float f) 
{
    __asm
    {
		fld f
		mov edx, intptr
		FRNDINT
		fistp dword ptr [edx];
	}
}

////////////////////////////////////////////////////////////
/// Initialize the library
////////////////////////////////////////////////////////////
void MathInitialize()
{
	UInt32 i;

	if (CPUFeatureCheck(FEATURE_SSEEXT))
	{
		MathSqrt = &MathSqrtSSE;
	}
	else
	{
		MathSqrt = (float(*)(float))&sqrt;
	}

	// Create de sinus and cosinus table
	for (i = 0; i < ANGLE_CIRCLE_SIZE; i++)
	{
		SinCosAngleTable[i] = (float)sin((double)i * MATH_PI / ANGLE_CIRCLE_HALF_SIZE);
	}

}

////////////////////////////////////////////////////////////
/// Sinus fast function
////////////////////////////////////////////////////////////
float MathSin(float Radians)
{
	int Value;

	ftoi(&Value, Radians * ANGLE_CIRCLE_HALF_SIZE / MATH_PI);
	
	if (Value < 0)
	{
		return SinCosAngleTable[(-((-Value) & ANGLE_CIRCLE_MASK)) + ANGLE_CIRCLE_SIZE];
	}
	else
	{
		return SinCosAngleTable[Value & ANGLE_CIRCLE_MASK];
	}
}

////////////////////////////////////////////////////////////
/// Cosinus fast function
////////////////////////////////////////////////////////////
float MathCos(float Radians)
{
	int Value;

	ftoi(&Value, Radians * ANGLE_CIRCLE_HALF_SIZE / MATH_PI);

	if (Value < 0)
	{
		return SinCosAngleTable[((-Value) + ANGLE_CIRCLE_QUARTER_SIZE) & ANGLE_CIRCLE_MASK];
	}
	else
	{
		return SinCosAngleTable[(Value + ANGLE_CIRCLE_QUARTER_SIZE) & ANGLE_CIRCLE_MASK];
	}
}

////////////////////////////////////////////////////////////
/// Calculate the next power of two
////////////////////////////////////////////////////////////
UInt32 MathPowerOfTwo(UInt32 Value)
{
	register UInt32 Result;

	for (Result = Value & (~Value + 1); Result < Value; Result <<= 1);

	return Result;
}

////////////////////////////////////////////////////////////
/// Determines if two floating-point are close enough
////////////////////////////////////////////////////////////
bool MathCloseEnough(float f1, float f2)
{
	return (bool)(fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < MATH_EPSILON);
}

////////////////////////////////////////////////////////////
/// Returns a gradual transition of x ranging [0 1]
////////////////////////////////////////////////////////////
float MathSmoothStep(float a, float b, float x)
{
	if (x < a)
	{
		return 0.0f;
	}
	else if (x >= b)
	{
		return 1.0f;
	}
	else
	{
		x = (x - a) / (b - a);
		return sqr(x) * (3.0f - 2.0f * x);
	}
}

////////////////////////////////////////////////////////////
/// Determines if a number is in the range [Min Max]
////////////////////////////////////////////////////////////
bool MathCheckBounds(Int32 * Value, Int32 Min, Int32 Max)
{
	bool IsInBounds = true;

	if (*Value < Min)
	{
		*Value = Min;
		IsInBounds = false;
	}

	if (*Value > Max)
	{
		*Value = Max;
		IsInBounds = false;
	}

	return IsInBounds;
}