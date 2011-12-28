////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/TextureHelper.h>
#include <Math/General.h>		///< ldexp
#include <System/IOHelper.h>
#include <memory.h>
#include <assert.h>

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#ifndef _MSC_VER
#define __forceinline
#endif

////////////////////////////////////////////////////////////
/// Png functions
////////////////////////////////////////////////////////////
Int32   stbi_png_test_memory      (UInt8 * buffer, Int32 len);
UInt8 * stbi_png_load_from_memory (UInt8 * buffer, Int32 len, Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp);

UInt8 * stbi_png_load             (char * filename,           Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp);
Int32   stbi_png_test_file        (FILE * f);
UInt8 * stbi_png_load_from_file   (FILE * f,                  Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp);

////////////////////////////////////////////////////////////
// Error manager functions
////////////////////////////////////////////////////////////
static char * failure_reason;

char * stbi_failure_reason()
{
   return failure_reason;
}

static Int32 TextureHandleError(char * str)
{
   failure_reason = str;
   return 0;
}

UInt8 * stbi_load_from_file(char * filename, Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp)
{
	UInt8 * result;
	FILE * f;

	if (!fopen(&f, filename, "rb"))
		return (UInt8*)TextureHandleError("Unable to open file");

	result = (stbi_png_test_file(f) ? stbi_png_load_from_file(f, x, y, comp, req_comp) :
									  (UInt8*)TextureHandleError("Texture not of any known type, or corrupt"));

	fclose(f);

	return result;
}

UInt8 * stbi_load_from_memory(UInt8 * buffer, Int32 len, Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp)
{
   if (stbi_png_test_memory(buffer, len))
      return stbi_png_load_from_memory(buffer, len, x, y, comp, req_comp);

   return (UInt8*)TextureHandleError("Texture not of any known type, or corrupt");
}

//////////////////////////////////////////////////////////////////////////////
// Common code used by all image loaders
//////////////////////////////////////////////////////////////////////////////

enum
{
   SCAN_LOAD = 0,
   SCAN_TYPE,
   SCAN_HEADER,
};


////////////////////////////////////////////////////////////
// Texture width, height, # components
////////////////////////////////////////////////////////////
static UInt32 img_x, img_y;
static Int32 img_n, img_out_n;

////////////////////////////////////////////////////////////
// An API for reading either from memory or file.
////////////////////////////////////////////////////////////
static FILE  * img_file;
static UInt8 * img_buffer, * img_buffer_end;

static void start_file(FILE * f)
{
   img_file = f;
}

static void start_mem(UInt8 * buffer, Int32 len)
{
   img_file = NULL;
   img_buffer = buffer;
   img_buffer_end = buffer+len;
}

static Int32 get8()
{
   if (img_file) {
      Int32 c = fgetc(img_file);
      return c == EOF ? 0 : c;
   }
   if (img_buffer < img_buffer_end)
      return *img_buffer++;
   return 0;
}

static UInt8 get8u()
{
   return (UInt8) get8();
}

static Int32 get16()
{
   Int32 z = get8();
   return (z << 8) + get8();
}

static UInt32 get32()
{
   UInt32 z = get16();
   return (z << 16) + get16();
}

//////////////////////////////////////////////////////////////////////////////
//  Generic converter from built-in img_n to req_comp
//    individual types do this automatically as much as possible png can
//    automatically interleave an alpha=255 channel
//	  but falls back to this for other cases
//
//  Assume data buffer is malloced, so malloc a new one and free that one
//  only failure mode is malloc failing
//////////////////////////////////////////////////////////////////////////////
static UInt8 compute_y(Int32 r, Int32 g, Int32 b)
{
   return (UInt8) (((r*77) + (g*150) +  (29*b)) >> 8);
}

static UInt8 * convert_format(UInt8 * data, Int32 img_n, Int32 req_comp)
{
   UInt32 i,j;
   UInt8 * good;

   if (req_comp == img_n) return data;
   assert(req_comp >= 1 && req_comp <= 4);

   good = (UInt8 *) malloc(req_comp * img_x * img_y);
   if (good == NULL) {
      Delete(data);
      return (UInt8*)TextureHandleError("Out of memory");
   }

   for (j=0; j < img_y; ++j) {
      UInt8 * src  = data + j * img_x * img_n   ;
      UInt8 * dest = good + j * img_x * req_comp;

      #define COMBO(a,b)  ((a)*8+(b))
      #define CASE(a,b)   case COMBO(a,b): for (i=0; i < img_x; ++i, src += a, dest += b)

      // convert source image with img_n components to one with req_comp components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch(COMBO(img_n, req_comp)) {
         CASE(1,2) dest[0]=src[0], dest[1]=255; break;
         CASE(1,3) dest[0]=dest[1]=dest[2]=src[0]; break;
         CASE(1,4) dest[0]=dest[1]=dest[2]=src[0], dest[3]=255; break;
         CASE(2,1) dest[0]=src[0]; break;
         CASE(2,3) dest[0]=dest[1]=dest[2]=src[0]; break;
         CASE(2,4) dest[0]=dest[1]=dest[2]=src[0], dest[3]=src[1]; break;
         CASE(3,4) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2],dest[3]=255; break;
         CASE(3,1) dest[0]=compute_y(src[0],src[1],src[2]); break;
         CASE(3,2) dest[0]=compute_y(src[0],src[1],src[2]), dest[1] = 255; break;
         CASE(4,1) dest[0]=compute_y(src[0],src[1],src[2]); break;
         CASE(4,2) dest[0]=compute_y(src[0],src[1],src[2]), dest[1] = src[3]; break;
         CASE(4,3) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2]; break;
         default: assert(0);
      }
      #undef CASE
   }

   Delete(data);
   img_out_n = req_comp;
   return good;
}

////////////////////////////////////////////////////////////
// public domain zlib decode    v0.2  Sean Barrett 2006/11/18
//    simple implementation
//      - all input must be provided in an upfront buffer
//      - all output is written to a single output buffer (can malloc/realloc)
//    performance
//      - fast huffman
//
// fast-way is faster to check than jpeg huffman, but slow way is slower
////////////////////////////////////////////////////////////
#define ZFAST_BITS  9 // accelerate all cases in default tables
#define ZFAST_MASK  ((1 << ZFAST_BITS) - 1)

////////////////////////////////////////////////////////////
// zlib-style huffman encoding
// (jpegs packs from left, zlib from right, so can't share code)
////////////////////////////////////////////////////////////
typedef struct
{
   UInt16 fast[1 << ZFAST_BITS];
   UInt16 firstcode[16];
   Int32 maxcode[17];
   UInt16 firstsymbol[16];
   UInt8  size[288];
   UInt16 value[288];
} zhuffman;

__forceinline static Int32 bitreverse16(Int32 n)
{
  n = ((n & 0xAAAA) >>  1) | ((n & 0x5555) << 1);
  n = ((n & 0xCCCC) >>  2) | ((n & 0x3333) << 2);
  n = ((n & 0xF0F0) >>  4) | ((n & 0x0F0F) << 4);
  n = ((n & 0xFF00) >>  8) | ((n & 0x00FF) << 8);
  return n;
}

__forceinline static Int32 bit_reverse(Int32 v, Int32 bits)
{
   assert(bits <= 16);
   // to bit reverse n bits, reverse 16 and shift
   // e.g. 11 bits, bit reverse and shift away 5
   return bitreverse16(v) >> (16-bits);
}

static Int32 zbuild_huffman(zhuffman *z, UInt8 *sizelist, Int32 num)
{
   Int32 i,k=0;
   Int32 code, next_code[16], sizes[17];

   // DEFLATE spec for generating codes
   memset(sizes, 0, sizeof(sizes));
   memset(z->fast, 255, sizeof(z->fast));
   for (i=0; i < num; ++i)
      ++sizes[sizelist[i]];
   sizes[0] = 0;
   for (i=1; i < 16; ++i)
      assert(sizes[i] <= (1 << i));
   code = 0;
   for (i=1; i < 16; ++i) {
      next_code[i] = code;
      z->firstcode[i] = (UInt16) code;
      z->firstsymbol[i] = (UInt16) k;
      code = (code + sizes[i]);
      if (sizes[i])
         if (code-1 >= (1 << i)) return TextureHandleError("Corrupt JPEG");
      z->maxcode[i] = code << (16-i); // preshift for inner loop
      code <<= 1;
      k += sizes[i];
   }
   z->maxcode[16] = 0x10000; // sentinel
   for (i=0; i < num; ++i) {
      Int32 s = sizelist[i];
      if (s) {
         Int32 c = next_code[s] - z->firstcode[s] + z->firstsymbol[s];
         z->size[c] = (UInt8)s;
         z->value[c] = (UInt16)i;
         if (s <= ZFAST_BITS) {
            Int32 k = bit_reverse(next_code[s],s);
            while (k < (1 << ZFAST_BITS)) {
               z->fast[k] = (UInt16) c;
               k += (1 << s);
            }
         }
         ++next_code[s];
      }
   }
   return 1;
}

////////////////////////////////////////////////////////////
// zlib-from-memory implementation for PNG reading
//    because PNG allows splitting the zlib stream arbitrarily,
//    and it's annoying structurally to have PNG call ZLIB call PNG,
//    we require PNG read all the IDATs and combine them into a single
//    memory buffer
////////////////////////////////////////////////////////////
typedef struct
{
   UInt8	* zbuffer, * zbuffer_end;
   Int32	num_bits;
   UInt32	code_buffer;

   Int8		* zout;
   Int8		* zout_start;
   Int8		* zout_end;
   UInt32   z_expandable;

   zhuffman z_length, z_distance;
} zbuf;

__forceinline static Int32 zget8(zbuf * z)
{
   if (z->zbuffer >= z->zbuffer_end) return 0;
   return *z->zbuffer++;
}

static void fill_bits(zbuf * z)
{
   do {
      assert(z->code_buffer < (1U << z->num_bits));
      z->code_buffer |= zget8(z) << z->num_bits;
      z->num_bits += 8;
   } while (z->num_bits <= 24);
}

__forceinline static UInt32 zreceive(zbuf * z, Int32 n)
{
   UInt32 k;
   if (z->num_bits < n) fill_bits(z);
   k = z->code_buffer & ((1 << n) - 1);
   z->code_buffer >>= n;
   z->num_bits -= n;
   return k;
}

__forceinline static Int32 zhuffman_decode(zbuf * a, zhuffman * z)
{
   Int32 b, s, k;
   if (a->num_bits < 16) fill_bits(a);
   b = z->fast[a->code_buffer & ZFAST_MASK];
   if (b < 0xffff) {
      s = z->size[b];
      a->code_buffer >>= s;
      a->num_bits -= s;
      return z->value[b];
   }

   // not resolved by fast table, so compute it the slow way
   // use jpeg approach, which requires MSbits at top
   k = bit_reverse(a->code_buffer, 16);
   for (s=ZFAST_BITS+1; ; ++s)
      if (k < z->maxcode[s])
         break;
   if (s == 16) return -1; // invalid code!
   // code size is s, so:
   b = (k >> (16-s)) - z->firstcode[s] + z->firstsymbol[s];
   assert(z->size[b] == s);
   a->code_buffer >>= s;
   a->num_bits -= s;
   return z->value[b];
}

static Int32 expand(zbuf * z, Int32 n)  // need to make room for n bytes
{
   char * q;
   Int32 cur, limit;
   if (!z->z_expandable) return TextureHandleError("Corrupt PNG (output buffer limit)");
   cur   = (int) (z->zout     - z->zout_start);
   limit = (int) (z->zout_end - z->zout_start);
   while (cur + n > limit)
      limit *= 2;
   q = (char*) realloc(z->zout_start, limit);
   if (q == NULL) return TextureHandleError("Out of memory");
   z->zout_start = (Int8*)q;
   z->zout       = (Int8*)(q + cur);
   z->zout_end   = (Int8*)(q + limit);
   return 1;
}

static Int32 length_base[31] = {
   3,4,5,6,7,8,9,10,11,13,
   15,17,19,23,27,31,35,43,51,59,
   67,83,99,115,131,163,195,227,258,0,0 };

static Int32 length_extra[31]=
{ 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0,0,0 };

static Int32 dist_base[32] = { 1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,
257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577,0,0};

static Int32 dist_extra[32] =
{ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};

static Int32 parse_huffman_block(zbuf * a)
{
   for (;;) {
      Int32 z = zhuffman_decode(a, &a->z_length);
      if (z < 256) {
         if (z < 0) return TextureHandleError("Corrupt PNG (bad huffman code)"); // error in huffman codes
         if (a->zout >= a->zout_end) if (!expand(a, 1)) return 0;
         *a->zout++ = (char) z;
      } else {
         UInt8 * p;
         Int32 len, dist;
         if (z == 256) return 1;
         z -= 257;
         len = length_base[z];
         if (length_extra[z]) len += zreceive(a, length_extra[z]);
         z = zhuffman_decode(a, &a->z_distance);
         if (z < 0) return TextureHandleError("Corrupt PNG (bad huffman code)");
         dist = dist_base[z];
         if (dist_extra[z]) dist += zreceive(a, dist_extra[z]);
         if (a->zout - a->zout_start < dist) return TextureHandleError("Corrupt PNG (bad dist)");
         if (a->zout + len > a->zout_end) if (!expand(a, len)) return 0;
         p = (UInt8 *) (a->zout - dist);
         while (len--)
            *a->zout++ = *p++;
      }
   }
}

static Int32 compute_huffman_codes(zbuf * a)
{
   static UInt8 length_dezigzag[19] = { 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
   zhuffman z_codelength;
   UInt8 lencodes[286+32+137];//padding for maximum single op
   UInt8 codelength_sizes[19];
   UInt32 i, n;

   UInt32 hlit  = zreceive(a,5) + 257;
   UInt32 hdist = zreceive(a,5) + 1;
   UInt32 hclen = zreceive(a,4) + 4;

   memset(codelength_sizes, 0, sizeof(codelength_sizes));
   for (i=0; i < hclen; ++i) {
      Int32 s = zreceive(a,3);
      codelength_sizes[length_dezigzag[i]] = (UInt8) s;
   }
   if (!zbuild_huffman(&z_codelength, codelength_sizes, 19)) return 0;

   n = 0;
   while (n < hlit + hdist) {
      Int32 c = zhuffman_decode(a, &z_codelength);
      assert(c >= 0 && c < 19);
      if (c < 16)
         lencodes[n++] = (UInt8) c;
      else if (c == 16) {
         c = zreceive(a,2)+3;
         memset(lencodes+n, lencodes[n-1], c);
         n += c;
      } else if (c == 17) {
         c = zreceive(a,3)+3;
         memset(lencodes+n, 0, c);
         n += c;
      } else {
         assert(c == 18);
         c = zreceive(a,7)+11;
         memset(lencodes+n, 0, c);
         n += c;
      }
   }
   if (n != hlit+hdist) return TextureHandleError("Corrupt PNG (bad codelengths)");
   if (!zbuild_huffman(&a->z_length, lencodes, hlit)) return 0;
   if (!zbuild_huffman(&a->z_distance, lencodes+hlit, hdist)) return 0;
   return 1;
}

static Int32 parse_uncompressed_block(zbuf * a)
{
   UInt8 header[4];
   Int32 len, nlen, k;
   if (a->num_bits & 7)
      zreceive(a, a->num_bits & 7); // discard
   // drain the bit-packed data into header
   k = 0;
   while (a->num_bits > 0) {
      header[k++] = (UInt8) (a->code_buffer & 255); // wtf this warns?
      a->code_buffer >>= 8;
      a->num_bits -= 8;
   }
   assert(a->num_bits == 0);
   // now fill header the normal way
   while (k < 4)
      header[k++] = (UInt8) zget8(a);
   len  = header[1] * 256 + header[0];
   nlen = header[3] * 256 + header[2];
   if (nlen != (len ^ 0xffff)) return TextureHandleError("Corrupt PNG (zlib corrupt)");
   if (a->zbuffer + len > a->zbuffer_end) return TextureHandleError("Corrupt PNG (read past buffer)");
   if (a->zout + len > a->zout_end)
      if (!expand(a, len)) return 0;
   memcpy(a->zout, a->zbuffer, len);
   a->zbuffer += len;
   a->zout += len;
   return 1;
}

static Int32 parse_zlib_header(zbuf * a)
{
   Int32 cmf   = zget8(a);
   Int32 cm    = cmf & 15;
   // Int32 cinfo = cmf >> 4;
   Int32 flg   = zget8(a);
   if ((cmf*256+flg) % 31 != 0) return TextureHandleError("Corrupt PNG (bad zlib header)"); // zlib spec
   if (flg & 32) return TextureHandleError("Corrupt PNG (no preset dict)"); // preset dictionary not allowed in png
   if (cm != 8) return TextureHandleError("Corrupt PNG (bad compression)"); // DEFLATE required for png
   // window = 1 << (8 + cinfo)... but who cares, we fully buffer output
   return 1;
}

// @TODO: should statically initialize these for optimal thread safety
static UInt8 default_length[288], default_distance[32];
static void init_defaults()
{
   Int32 i;   // use <= to match clearly with spec
   for (i=0; i <= 143; ++i)     default_length[i]   = 8;
   for (   ; i <= 255; ++i)     default_length[i]   = 9;
   for (   ; i <= 279; ++i)     default_length[i]   = 7;
   for (   ; i <= 287; ++i)     default_length[i]   = 8;

   for (i=0; i <=  31; ++i)     default_distance[i] = 5;
}

Int32 stbi_png_partial; // a quick hack to only allow decoding some of a PNG... I should implement real streaming support instead
static Int32 parse_zlib(zbuf * a, bool parse_header)
{
   Int32 final, type;
   if (parse_header)
      if (!parse_zlib_header(a)) return 0;
   a->num_bits = 0;
   a->code_buffer = 0;
   do {
      final = zreceive(a,1);
      type = zreceive(a,2);
      if (type == 0) {
         if (!parse_uncompressed_block(a)) return 0;
      } else if (type == 3) {
         return 0;
      } else {
         if (type == 1) {
            // use fixed code lengths
            if (!default_distance[31]) init_defaults();
            if (!zbuild_huffman(&a->z_length  , default_length  , 288)) return 0;
            if (!zbuild_huffman(&a->z_distance, default_distance,  32)) return 0;
         } else {
            if (!compute_huffman_codes(a)) return 0;
         }
         if (!parse_huffman_block(a)) return 0;
      }
      if (stbi_png_partial && a->zout - a->zout_start > 65536)
         break;
   } while (!final);
   return 1;
}

static Int32 do_zlib(zbuf * a, Int8 * obuf, Int32 olen, Int32 exp, bool parse_header)
{
   a->zout_start = obuf;
   a->zout       = obuf;
   a->zout_end   = obuf + olen;
   a->z_expandable = exp;

   return parse_zlib(a, parse_header);
}

Int8 * stbi_zlib_decode_malloc(Int8 * buffer, Int32 len, Int32 * outlen, bool parse_header)
{
	zbuf a;
	Int8 * p = (Int8*)malloc(16384);
	if (p == NULL) return NULL;
	a.zbuffer = (UInt8*) buffer;
	a.zbuffer_end = (UInt8*)buffer + len;
	if (do_zlib(&a, p, 16384, 1, parse_header)) {
      *outlen = (Int32) (a.zout - a.zout_start);
      return a.zout_start;
   } else {
      Delete(a.zout_start);
      return NULL;
   }
}

Int32 stbi_zlib_decode_buffer(Int8 * obuffer, Int32 olen, Int8 * ibuffer, Int32 ilen, bool parse_header)
{
	zbuf a;
	a.zbuffer = (UInt8*)ibuffer;
	a.zbuffer_end = (UInt8*)ibuffer + ilen;
   if (do_zlib(&a, obuffer, olen, 0, parse_header))
      return (Int32)(a.zout - a.zout_start);
   else
      return -1;
}

////////////////////////////////////////////////////////////
// Public domain "baseline" PNG decoder   v0.10  Sean Barrett 2006/11/18
//    simple implementation
//      - only 8-bit samples
//      - no CRC checking
//      - allocates lots of intermediate memory
//        - avoids problem of streaming data between subsystems
//        - avoids explicit window management
//    performance
//      - uses stb_zlib, a PD zlib implementation with fast huffman decoding
////////////////////////////////////////////////////////////
typedef struct
{
   UInt32 length;
   UInt32 type;
} chunk;

#define PNG_TYPE(a,b,c,d)  (((a) << 24) + ((b) << 16) + ((c) << 8) + (d))

static chunk get_chunk_header()
{
   chunk c;
   c.length = get32();
   c.type   = get32();
   return c;
}

static Int32 check_png_header()
{
   static UInt8 png_sig[8] = { 137,80,78,71,13,10,26,10 };
   Int32 i;
   for (i=0; i < 8; ++i)
      if (get8() != png_sig[i]) return TextureHandleError("Corrupt PNG (bad png sig)");
   return 1;
}

static UInt8 * idata, * expanded, * out;

enum {
   F_none		= 0,
   F_sub		= 1,
   F_up			= 2,
   F_avg		= 3,
   F_paeth		= 4,
   F_avg_first,
   F_paeth_first,
};

static UInt8 first_row_filter[5] =
{
   F_none, F_sub, F_none, F_avg_first, F_paeth_first
};

static Int32 paeth(Int32 a, Int32 b, Int32 c)
{
   Int32 p = a + b - c;
   Int32 pa = abs(p-a);
   Int32 pb = abs(p-b);
   Int32 pc = abs(p-c);
   if (pa <= pb && pa <= pc) return a;
   if (pb <= pc) return b;
   return c;
}

////////////////////////////////////////////////////////////
// Create the png data from post-deflated data
////////////////////////////////////////////////////////////
static Int32 create_png_image(UInt8 *raw, UInt32 raw_len, Int32 out_n)
{
   UInt32 i, j, stride = img_x * out_n;
   Int32 k;
   assert(out_n == img_n || out_n == img_n+1);
   out = (UInt8 *) malloc(img_x * img_y * out_n);
   if (!out) return TextureHandleError("Out of memory");
   if (raw_len != (img_n * img_x + 1) * img_y) return TextureHandleError("Corrupt PNG (not enough pixels)");
   for (j=0; j < img_y; ++j) {
      UInt8 * cur = out + stride * j;
      UInt8 * prior = cur - stride;
      Int32 filter = *raw++;
      if (filter > 4) return TextureHandleError("Corrupt PNG (invalid filter)");
      // if first row, use special filter that doesn't sample previous row
      if (j == 0) filter = first_row_filter[filter];
      // handle first pixel explicitly
      for (k=0; k < img_n; ++k) {
         switch(filter) {
            case F_none       : cur[k] = raw[k]; break;
            case F_sub        : cur[k] = raw[k]; break;
            case F_up         : cur[k] = raw[k] + prior[k]; break;
            case F_avg        : cur[k] = raw[k] + (prior[k]>>1); break;
            case F_paeth      : cur[k] = (UInt8) (raw[k] + paeth(0,prior[k],0)); break;
            case F_avg_first  : cur[k] = raw[k]; break;
            case F_paeth_first: cur[k] = raw[k]; break;
         }
      }
      if (img_n != out_n) cur[img_n] = 255;
      raw += img_n;
      cur += out_n;
      prior += out_n;
      // this is a little gross, so that we don't switch per-pixel or per-component
      if (img_n == out_n) {
         #define CASE(f) \
             case f:     \
                for (i=1; i < img_x; ++i, raw+=img_n,cur+=img_n,prior+=img_n) \
                   for (k=0; k < img_n; ++k)
         switch(filter) {
            CASE(F_none)  cur[k] = raw[k]; break;
            CASE(F_sub)   cur[k] = raw[k] + cur[k-img_n]; break;
            CASE(F_up)    cur[k] = raw[k] + prior[k]; break;
            CASE(F_avg)   cur[k] = raw[k] + ((prior[k] + cur[k-img_n])>>1); break;
            CASE(F_paeth)  cur[k] = (UInt8) (raw[k] + paeth(cur[k-img_n],prior[k],prior[k-img_n])); break;
            CASE(F_avg_first)    cur[k] = raw[k] + (cur[k-img_n] >> 1); break;
            CASE(F_paeth_first)  cur[k] = (UInt8) (raw[k] + paeth(cur[k-img_n],0,0)); break;
         }
         #undef CASE
      } else {
         assert(img_n+1 == out_n);
         #define CASE(f) \
             case f:     \
                for (i=1; i < img_x; ++i, cur[img_n]=255,raw+=img_n,cur+=out_n,prior+=out_n) \
                   for (k=0; k < img_n; ++k)
         switch(filter) {
            CASE(F_none)  cur[k] = raw[k]; break;
            CASE(F_sub)   cur[k] = raw[k] + cur[k-out_n]; break;
            CASE(F_up)    cur[k] = raw[k] + prior[k]; break;
            CASE(F_avg)   cur[k] = raw[k] + ((prior[k] + cur[k-out_n])>>1); break;
            CASE(F_paeth)  cur[k] = (UInt8) (raw[k] + paeth(cur[k-out_n],prior[k],prior[k-out_n])); break;
            CASE(F_avg_first)    cur[k] = raw[k] + (cur[k-out_n] >> 1); break;
            CASE(F_paeth_first)  cur[k] = (UInt8) (raw[k] + paeth(cur[k-out_n],0,0)); break;
         }
         #undef CASE
      }
   }
   return 1;
}

static Int32 compute_transparency(UInt8 tc[3], Int32 out_n)
{
   UInt32 i, pixel_count = img_x * img_y;
   UInt8 *p = out;

   // compute color-based transparency, assuming we've
   // already got 255 as the alpha value in the output
   assert(out_n == 2 || out_n == 4);

   p = out;
   if (out_n == 2) {
      for (i=0; i < pixel_count; ++i) {
         p[1] = (p[0] == tc[0] ? 0 : 255);
         p += 2;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         if (p[0] == tc[0] && p[1] == tc[1] && p[2] == tc[2])
            p[3] = 0;
         p += 4;
      }
   }
   return 1;
}

static Int32 expand_palette(UInt8 *palette, Int32 len, Int32 pal_img_n)
{
   UInt32 i, pixel_count = img_x * img_y;
   UInt8 * p, * temp_out, * orig = out;

   p = (UInt8 *) malloc(pixel_count * pal_img_n);
   if (p == NULL) return TextureHandleError("Out of memory");

   // between here and free(out) below, exitting would leak
   temp_out = p;

   if (pal_img_n == 3) {
      for (i=0; i < pixel_count; ++i) {
         Int32 n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p += 3;
      }
   } else {
      for (i=0; i < pixel_count; ++i) {
         Int32 n = orig[i]*4;
         p[0] = palette[n  ];
         p[1] = palette[n+1];
         p[2] = palette[n+2];
         p[3] = palette[n+3];
         p += 4;
      }
   }

   Delete(out);
   out = temp_out;
   return 1;
}

static Int32 parse_png_file(Int32 scan, Int32 req_comp)
{
   UInt8 palette[1024], pal_img_n=0;
   UInt8 has_trans=0, tc[3];
   UInt32 ioff=0, idata_limit=0, i, pal_len=0;
   Int32 first=1,k;

   if (!check_png_header()) return 0;

   if (scan == SCAN_TYPE) return 1;

   for (;;first=0) {
      chunk c = get_chunk_header();
      if (first && c.type != PNG_TYPE('I','H','D','R'))
         return TextureHandleError("Corrupt PNG (first not IHDR");
      switch (c.type) {
         case PNG_TYPE('I','H','D','R'): {
            Int32 depth,color,interlace,comp,filter;
            if (!first) return TextureHandleError("Corrupt PNG (multiple IHDR)");
            if (c.length != 13) return TextureHandleError("Corrupt PNG (bad IHDR len)");
            img_x = get32(); if (img_x > (1 << 24)) return TextureHandleError("Corrupt PNG (Very large image (corrupt?))");
            img_y = get32(); if (img_y > (1 << 24)) return TextureHandleError("Corrupt PNG (Very large image (corrupt?))");
            depth = get8();  if (depth != 8)        return TextureHandleError("PNG not supported: 8-bit only");
            color = get8();  if (color > 6)         return TextureHandleError("Corrupt PNG (bad ctype)");
            if (color == 3) pal_img_n = 3; else if (color & 1) return TextureHandleError("Corrupt PNG (bad ctype)");
            comp  = get8();  if (comp) return TextureHandleError("Corrupt PNG (bad comp method)");
            filter= get8();  if (filter) return TextureHandleError("Corrupt PNG (bad filter method)");
            interlace = get8(); if (interlace) return TextureHandleError("PNG not supported: interlaced mode");
            if (!img_x || !img_y) return TextureHandleError("Corrupt PNG (0-pixel image)");
            if (!pal_img_n) {
               img_n = (color & 2 ? 3 : 1) + (color & 4 ? 1 : 0);
               if ((1 << 30) / img_x / img_n < img_y) return TextureHandleError("Texture too large to decode");
               if (scan == SCAN_HEADER) return 1;
            } else {
               // if paletted, then pal_n is our final components, and
               // img_n is # components to decompress/filter.
               img_n = 1;
               if ((1 << 30) / img_x / 4 < img_y) return TextureHandleError("Corrupt PNG (too large)");
               // if SCAN_HEADER, have to scan to see if we have a tRNS
            }
            break;
         }

         case PNG_TYPE('P','L','T','E'):  {
            if (c.length > 256*3) return TextureHandleError("Corrupt PNG (invalid PLTE)");
            pal_len = c.length / 3;
            if (pal_len * 3 != c.length) return TextureHandleError("Corrupt PNG (invalid PLTE)");
            for (i=0; i < pal_len; ++i) {
               palette[i*4+0] = get8u();
               palette[i*4+1] = get8u();
               palette[i*4+2] = get8u();
               palette[i*4+3] = 255;
            }
            break;
         }

         case PNG_TYPE('t','R','N','S'): {
            if (idata) return TextureHandleError("Corrupt PNG (tRNS after IDAT)");
            if (pal_img_n) {
               if (scan == SCAN_HEADER) { img_n = 4; return 1; }
               if (pal_len == 0) return TextureHandleError("Corrupt PNG (tRNS before PLTE)");
               if (c.length > pal_len) return TextureHandleError("Corrupt PNG (bad tRNS len)");
               pal_img_n = 4;
               for (i=0; i < c.length; ++i)
                  palette[i*4+3] = get8u();
            } else {
               if (!(img_n & 1)) return TextureHandleError("Corrupt PNG (tRNS with alpha)");
               if (c.length != (UInt32) img_n*2) return TextureHandleError("Corrupt PNG (bad tRNS len)");
               has_trans = 1;
               for (k=0; k < img_n; ++k)
                  tc[k] = (UInt8) get16(); // non 8-bit images will be larger
            }
            break;
         }

         case PNG_TYPE('I','D','A','T'): {
            if (pal_img_n && !pal_len) return TextureHandleError("Corrupt PNG (no PLTE)");
            if (scan == SCAN_HEADER) { img_n = pal_img_n; return 1; }
            if (ioff + c.length > idata_limit) {
               UInt8 *p;
               if (idata_limit == 0) idata_limit = c.length > 4096 ? c.length : 4096;
               while (ioff + c.length > idata_limit)
                  idata_limit <<= 1;
               p = (UInt8 *) realloc(idata, idata_limit); if (p == NULL) return TextureHandleError("Out of memory");
               idata = p;
            }

            if (img_file)
            {
               if (fread(idata+ioff,1,c.length,img_file) != c.length) return TextureHandleError("Corrupt PNG (outofdata)");
            }
            else
            {
               memcpy(idata+ioff, img_buffer, c.length);
               img_buffer += c.length;
            }
            ioff += c.length;
            break;
         }

         case PNG_TYPE('I','E','N','D'): {
            UInt32 raw_len;
            if (scan != SCAN_LOAD) return 1;
            if (idata == NULL) return TextureHandleError("Corrupt PNG (no IDAT)");
            expanded = (UInt8*)stbi_zlib_decode_malloc((Int8*)idata, ioff, (Int32*) &raw_len, true);
            if (expanded == NULL) return 0; // zlib should set error
            Delete(idata);
            if ((req_comp == img_n+1 && req_comp != 3 && !pal_img_n) || has_trans)
               img_out_n = img_n+1;
            else
               img_out_n = img_n;
            if (!create_png_image(expanded, raw_len, img_out_n)) return 0;
            if (has_trans)
               if (!compute_transparency(tc, img_out_n)) return 0;
            if (pal_img_n) {
               // pal_img_n == 3 or 4
               img_n = pal_img_n; // record the actual colors we had
               img_out_n = pal_img_n;
               if (req_comp >= 3) img_out_n = req_comp;
               if (!expand_palette(palette, pal_len, img_out_n))
                  return 0;
            }
            Delete(expanded);
            return 1;
         }

         default:
            // if critical, fail
            if ((c.type & (1 << 29)) == 0)
               return TextureHandleError("PNG not supported: unknown chunk type");
		    if (img_file)
			   fseek(img_file, c.length, SEEK_CUR);
		    else
			   img_buffer += c.length;
            break;
      }
      // end of chunk, read and skip CRC
      get8(); get8(); get8(); get8();
   }
}

static UInt8 * do_png(Int32 * x, Int32 * y, Int32 * n, Int32 req_comp)
{
   UInt8 * result = NULL;
   if (req_comp < 0 || req_comp > 4) return (UInt8*)TextureHandleError("Internal error (do_png)");
   if (parse_png_file(SCAN_LOAD, req_comp)) {
      result = out;
      out = NULL;
      if (req_comp && req_comp != img_out_n) {
         result = convert_format(result, img_out_n, req_comp);
         if (result == NULL) return result;
      }
      *x = img_x;
      *y = img_y;
      if (n) *n = img_n;
   }

   Delete(out);
   Delete(expanded);
   Delete(idata);

   return result;
}

UInt8 * stbi_png_load_from_file(FILE *f, Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp)
{
	start_file(f);
	return do_png(x, y, comp, req_comp);
}

UInt8 * stbi_png_load(char * filename, Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp)
{
	UInt8 * data;
	FILE * f;
	if (!fopen(&f, filename, "rb"))
		return NULL;
	data = stbi_png_load_from_file(f, x, y, comp, req_comp);
	fclose(f);
	return data;
}

UInt8 * stbi_png_load_from_memory(UInt8 * buffer, Int32 len, Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp)
{
   start_mem(buffer, len);
   return do_png(x, y, comp, req_comp);
}

Int32 stbi_png_test_file(FILE *f)
{
   Int32 n,r;
   n = ftell(f);
   start_file(f);
   r = parse_png_file(SCAN_TYPE, 0);
   fseek(f,n,SEEK_SET);
   return r;
}

Int32 stbi_png_test_memory(unsigned char * buffer, Int32 len)
{
   start_mem(buffer, len);
   return parse_png_file(SCAN_TYPE, 0);
}

///////////////////////////////////////////////////////////
// Write texture
////////////////////////////////////////////////////////////
static void write8(FILE * f, Int32 x) { UInt8 z = (UInt8) x; fwrite(&z,1,1,f); }

static void writefv(FILE *f, char * fmt, va_list v)
{
   while (*fmt) {
      switch (*fmt++)
      {
         case ' ': break;
         case '1': { UInt8 x = va_arg(v, Int32); write8(f,x); break; }
         case '2': { Int16 x = va_arg(v, Int32); write8(f,x); write8(f,x>>8); break; }
         case '4': { Int32 x = va_arg(v, Int32); write8(f,x); write8(f,x>>8); write8(f,x>>16); write8(f,x>>24); break; }
         default:
            assert(0);
            va_end(v);
            return;
      }
   }
}

static void writef(FILE *f, char * fmt, ...)
{
   va_list v;
   va_start(v, fmt);
   writefv(f,fmt,v);
   va_end(v);
}

static void write_pixels(FILE *f, Int32 rgb_dir, Int32 vdir, Int32 x, Int32 y, Int32 comp, void * data, Int32 write_alpha, Int32 scanline_pad)
{
   UInt8 bg[3] = { 255, 0, 255}, px[3];
   UInt32 zero = 0;
   Int32 i,j,k, j_end;

   if (vdir < 0)
      j_end = -1, j = y-1;
   else
      j_end =  y, j = 0;

   for (; j != j_end; j += vdir) {
      for (i=0; i < x; ++i) {
         UInt8 *d = (UInt8 *) data + (j*x+i)*comp;
         if (write_alpha < 0)
            fwrite(&d[comp-1], 1, 1, f);
         switch (comp) {
            case 1:
            case 2: writef(f, "111", d[0],d[0],d[0]);
                    break;
            case 4:
               if (!write_alpha) {
                  for (k=0; k < 3; ++k)
                     px[k] = bg[k] + ((d[k] - bg[k]) * d[3])/255;
                  writef(f, "111", px[1-rgb_dir],px[1],px[1+rgb_dir]);
                  break;
               }
               // FALLTHROUGH
            case 3:
               writef(f, "111", d[1-rgb_dir],d[1],d[1+rgb_dir]);
               break;
         }
         if (write_alpha > 0)
            fwrite(&d[comp-1], 1, 1, f);
      }
      fwrite(&zero,scanline_pad,1,f);
   }
}

static bool outfile(char * filename, Int32 rgb_dir, Int32 vdir, Int32 x, Int32 y, Int32 comp, void * data, Int32 alpha, Int32 pad, char *fmt, ...)
{
	FILE *f;
	if (fopen(&f, filename, "wb"))
	{
      va_list v;
      va_start(v, fmt);
      writefv(f, fmt, v);
      va_end(v);
      write_pixels(f,rgb_dir,vdir,x,y,comp,data,alpha,pad);
      fclose(f);
	}
	return (f != NULL);
}

bool stbi_write_bmp(char * filename, Int32 x, Int32 y, Int32 comp, void * data)
{
   Int32 pad = (-x*3) & 3;

   return outfile(filename, -1, -1, x, y, comp, data, 0, pad,
		  "11 4 22 4" "4 44 22 444444",
          'B', 'M', 14+40+(x*3+pad)*y, 0, 0, 14+40,  // file header
          40, x, y, 1, 24, 0, 0, 0, 0, 0, 0);        // bitmap header
}
