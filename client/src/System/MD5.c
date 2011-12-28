////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <System/MD5.h>
#include <System/IOHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
/// Internal methods
////////////////////////////////////////////////////////////
static void MD5Transform(UInt32 State[4], unsigned char Block[64]);
static void Encode(unsigned char * Output, UInt32 * Input, unsigned int Len);
static void Decode(UInt32 * Output, unsigned char * Input, unsigned int Len);

static unsigned char Padding[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

////////////////////////////////////////////////////////////
/// Basic MD5 functions
////////////////////////////////////////////////////////////
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

////////////////////////////////////////////////////////////
/// ROTATE_LEFT rotates x left n Bits
////////////////////////////////////////////////////////////
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

////////////////////////////////////////////////////////////
/// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4
/// Rotation is separate from addition to prevent recomputation
////////////////////////////////////////////////////////////
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (UInt32)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

////////////////////////////////////////////////////////////
/// MD5 initialization. Begins an MD5 operation, writing a new context
////////////////////////////////////////////////////////////
void MD5Init(struct MD5Context * Context)
{
  Context->Count[0] = Context->Count[1] = 0;
  // Load magic initialization constants.

  Context->State[0] = 0x67452301;
  Context->State[1] = 0xefcdab89;
  Context->State[2] = 0x98badcfe;
  Context->State[3] = 0x10325476;
}

////////////////////////////////////////////////////////////
/// MD5 block update operation. Continues an MD5 message-Digest
/// operation, processing another message block, and updating the
/// context
////////////////////////////////////////////////////////////
void MD5Update(struct MD5Context * Context, unsigned char * Input, unsigned int InputLen)
{
	unsigned int i, Index, PartLen;

	// Compute number of bytes mod 64
	Index = (unsigned int)((Context->Count[0] >> 3) & 0x3F);

	// Update number of bits
	if ((Context->Count[0] += ((UInt32)InputLen << 3)) < ((UInt32)InputLen << 3))
		Context->Count[1]++;

	Context->Count[1] += ((UInt32)InputLen >> 29);

	PartLen = 64 - Index;

	// Transform as many times as possible
	if (InputLen >= PartLen)
	{
		memcpy((UInt8*)&Context->Buffer[Index], (UInt8*)Input, PartLen);
		MD5Transform(Context->State, Context->Buffer);

		for (i = PartLen; i + 63 < InputLen; i += 64)
			MD5Transform(Context->State, &Input[i]);

		Index = 0;
	}
	else
	{
		i = 0;
	}

	// Buffer remaining Input
	memcpy((UInt8*)&Context->Buffer[Index], (UInt8*)&Input[i], InputLen-i);
}

////////////////////////////////////////////////////////////
/// MD5 finalization. Ends an MD5 message-Digest operation
/// writing the the message Digest and zeroizing the Context
////////////////////////////////////////////////////////////
void MD5Final(unsigned char Digest[16], struct MD5Context * Context)
{
	unsigned char Bits[8];
	unsigned int Index, PadLen;

	// Save number of Bits
	Encode(Bits, Context->Count, 8);

	// Pad out to 56 mod 64.
	Index = (unsigned int)((Context->Count[0] >> 3) & 0x3f);
	PadLen = (Index < 56) ? (56 - Index) : (120 - Index);
	MD5Update(Context, Padding, PadLen);

	// Append length (before padding)
	MD5Update(Context, Bits, 8);

	// Store State in Digest
	Encode(Digest, Context->State, 16);

	// Zeroize sensitive information.
	memset((UInt8*)Context, 0, sizeof(*Context));
}

////////////////////////////////////////////////////////////
/// MD5 basic transformation
/// Transforms State based on block
////////////////////////////////////////////////////////////
static void MD5Transform(UInt32 State[4], unsigned char Block[64])
{
	UInt32 a = State[0], b = State[1], c = State[2], d = State[3], x[16];

	Decode(x, Block, 64);

	// Round 1
	FF(a, b, c, d, x[ 0], 7 , 0xd76aa478);
	FF(d, a, b, c, x[ 1], 12, 0xe8c7b756);
	FF(c, d, a, b, x[ 2], 17, 0x242070db);
	FF(b, c, d, a, x[ 3], 22, 0xc1bdceee);
	FF(a, b, c, d, x[ 4], 7 , 0xf57c0faf);
	FF(d, a, b, c, x[ 5], 12, 0x4787c62a);
	FF(c, d, a, b, x[ 6], 17, 0xa8304613);
	FF(b, c, d, a, x[ 7], 22, 0xfd469501);
	FF(a, b, c, d, x[ 8], 7 , 0x698098d8);
	FF(d, a, b, c, x[ 9], 12, 0x8b44f7af);
	FF(c, d, a, b, x[10], 17, 0xffff5bb1);
	FF(b, c, d, a, x[11], 22, 0x895cd7be);
	FF(a, b, c, d, x[12], 7 , 0x6b901122);
	FF(d, a, b, c, x[13], 12, 0xfd987193);
	FF(c, d, a, b, x[14], 17, 0xa679438e);
	FF(b, c, d, a, x[15], 22, 0x49b40821);

	// Round 2
	GG(a, b, c, d, x[ 1], 5 , 0xf61e2562);
	GG(d, a, b, c, x[ 6], 9 , 0xc040b340);
	GG(c, d, a, b, x[11], 14, 0x265e5a51);
	GG(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
	GG(a, b, c, d, x[ 5], 5 , 0xd62f105d);
	GG(d, a, b, c, x[10], 9 ,  0x2441453);
	GG(c, d, a, b, x[15], 14, 0xd8a1e681);
	GG(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
	GG(a, b, c, d, x[ 9], 5 , 0x21e1cde6);
	GG(d, a, b, c, x[14], 9 , 0xc33707d6);
	GG(c, d, a, b, x[ 3], 14, 0xf4d50d87);
	GG(b, c, d, a, x[ 8], 20, 0x455a14ed);
	GG(a, b, c, d, x[13], 5 , 0xa9e3e905);
	GG(d, a, b, c, x[ 2], 9 , 0xfcefa3f8);
	GG(c, d, a, b, x[ 7], 14, 0x676f02d9);
	GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);

	// Round 3
	HH(a, b, c, d, x[ 5], 4 , 0xfffa3942);
	HH(d, a, b, c, x[ 8], 11, 0x8771f681);
	HH(c, d, a, b, x[11], 16, 0x6d9d6122);
	HH(b, c, d, a, x[14], 23, 0xfde5380c);
	HH(a, b, c, d, x[ 1], 4 , 0xa4beea44);
	HH(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
	HH(c, d, a, b, x[ 7], 16, 0xf6bb4b60);
	HH(b, c, d, a, x[10], 23, 0xbebfbc70);
	HH(a, b, c, d, x[13], 4 , 0x289b7ec6);
	HH(d, a, b, c, x[ 0], 11, 0xeaa127fa);
	HH(c, d, a, b, x[ 3], 16, 0xd4ef3085);
	HH(b, c, d, a, x[ 6], 23,  0x4881d05);
	HH(a, b, c, d, x[ 9], 4 , 0xd9d4d039);
	HH(d, a, b, c, x[12], 11, 0xe6db99e5);
	HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
	HH(b, c, d, a, x[ 2], 23, 0xc4ac5665);

	// Round 4
	II(a, b, c, d, x[ 0], 6 , 0xf4292244);
	II(d, a, b, c, x[ 7], 10, 0x432aff97);
	II(c, d, a, b, x[14], 15, 0xab9423a7);
	II(b, c, d, a, x[ 5], 21, 0xfc93a039);
	II(a, b, c, d, x[12], 6 , 0x655b59c3);
	II(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
	II(c, d, a, b, x[10], 15, 0xffeff47d);
	II(b, c, d, a, x[ 1], 21, 0x85845dd1);
	II(a, b, c, d, x[ 8], 6 , 0x6fa87e4f);
	II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
	II(c, d, a, b, x[ 6], 15, 0xa3014314);
	II(b, c, d, a, x[13], 21, 0x4e0811a1);
	II(a, b, c, d, x[ 4], 6 , 0xf7537e82);
	II(d, a, b, c, x[11], 10, 0xbd3af235);
	II(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
	II(b, c, d, a, x[ 9], 21, 0xeb86d391);

	State[0] += a;
	State[1] += b;
	State[2] += c;
	State[3] += d;

	// Zeroize sensitive information.
	memset((UInt8*)x, 0, sizeof(x));
}

////////////////////////////////////////////////////////////
/// Encodes Input (UInt32) into output (unsigned char)
/// Assumes len is a multiple of 4
////////////////////////////////////////////////////////////
static void Encode(unsigned char * Output, UInt32 * Input, unsigned int Len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < Len; i++, j += 4)
	{
		Output[j] 	= (unsigned char)(Input[i] & 0xff);
		Output[j+1] = (unsigned char)((Input[i] >> 8) & 0xff);
		Output[j+2] = (unsigned char)((Input[i] >> 16) & 0xff);
		Output[j+3] = (unsigned char)((Input[i] >> 24) & 0xff);
	}
}

////////////////////////////////////////////////////////////
/// Decodes Input (unsigned char) into output (UInt32)
/// Assumes len is a multiple of 4
////////////////////////////////////////////////////////////
static void Decode(UInt32 * Output, unsigned char * Input, unsigned int Len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < Len; i++, j += 4)
		Output[i] = ((UInt32)Input[j]) | (((UInt32)Input[j+1]) << 8) |
		(((UInt32)Input[j+2]) << 16) | (((UInt32)Input[j+3]) << 24);
}

////////////////////////////////////////////////////////////
/// Digests a file and returns the result
////////////////////////////////////////////////////////////
void MD5File(char * File, unsigned char Digest[16])
{
	FILE * FileData;
	int Bytes;
	struct MD5Context Context;
	unsigned char Buffer[1024];

	// Check file
	fopen(&FileData, File, "rb");

	if ( FileData == NULL )
	{
		MessageError("MD5File", "Error when the program tried to make MD5Hash.");
		Digest = NULL;
	}

	// Initialize Context
	MD5Init(&Context);

	// Update Buffer
	while ((Bytes = fread(Buffer, 1, 1024, FileData)) != 0)
		MD5Update(&Context, Buffer, Bytes);

	MD5Final(Digest, &Context);

	// Close file
	fclose(FileData);
}

////////////////////////////////////////////////////////////
/// Digests a string and returns the result
////////////////////////////////////////////////////////////
void MD5String(unsigned char * String, unsigned char Digest[16])
{
	struct MD5Context Context;
	unsigned int Len = strlen((char*)String);

	// Initialize Context
	MD5Init(&Context);

	// Update
	MD5Update(&Context, String, Len);

	// Digest
	MD5Final(Digest, &Context);
}
