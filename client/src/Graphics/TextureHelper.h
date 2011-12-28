#ifndef GRAPHICS_STB_IMAGE_AUGMENTED
#define GRAPHICS_STB_IMAGE_AUGMENTED

///////////////////////////////////////////////////////////////////////////
///
/// stbi - 1.08 - public domain PNG reader - http://nothings.org/stb_image.c
///
/// QUICK NOTES:
///      Primarily of interest to game developers and other people who can
///      avoid problematic images and only need the trivial interface
///
///		PNG non-interlaced
///      Writes BMP
///
/// Limitations:
///    - no progressive/interlaced support
///    - 8-bit samples only
///    - not threadsafe
///
/// Standard parameters:
///    int * x       -- outputs image width in pixels
///    int * y       -- outputs image height in pixels
///    int * comp    -- outputs # of image components in image file
///    int req_comp  -- if non-zero, # of image components requested in result
///
/// The return value from an image loader is an 'UInt8 *' which points
/// to the pixel data. The pixel data consists of *y scanlines of *x pixels,
/// with each pixel consisting of N interleaved 8-bit components; the first
/// pixel pointed to is top-left-most in the image. There is no padding between
/// image scanlines or between pixels, regardless of format. The number of
/// components N is 'req_comp' if req_comp is non-zero, or *comp otherwise.
/// If req_comp is non-zero, *comp has the number of components that _would_
/// have been output otherwise. E.g. if you set req_comp to 4, you will always
/// get RGBA output, but you can check *comp to easily see if it's opaque.
///
/// An output image with N components has the following components interleaved
/// in this order in each pixel:
///
///     N=#comp     components
///       1           grey
///       2           grey, alpha
///       3           red, green, blue
///       4           red, green, blue, alpha
///
/// If image loading fails for any reason, the return value will be NULL,
/// and * x, * y, * comp will be unchanged. The function stbi_failure_reason()
/// can be queried for an extremely brief, end-user unfriendly explanation
/// of why the load failed.
///
/// Paletted PNG images are automatically depalettized.
///////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
/// Load texture by filename, open file, or memory buffer
////////////////////////////////////////////////////////////
UInt8 * stbi_load_from_file  (char * filename,           Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp);
UInt8 * stbi_load_from_memory(UInt8 * buffer, Int32 len, Int32 * x, Int32 * y, Int32 * comp, Int32 req_comp);

////////////////////////////////////////////////////////////
/// Get a VERY brief reason for failure
////////////////////////////////////////////////////////////
char  * stbi_failure_reason  ();

////////////////////////////////////////////////////////////
/// Zlib functions (used by PNG, available for other purposes)
////////////////////////////////////////////////////////////
Int8 * stbi_zlib_decode_malloc(Int8 * buffer, Int32 len, Int32 * outlen, bool parse_header);
Int32  stbi_zlib_decode_buffer(Int8 * obuffer, Int32 olen, Int8 * ibuffer, Int32 ilen, bool parse_header);

////////////////////////////////////////////////////////////
/// Write a BMP file given tightly packed 'comp' channels
/// You must include the appropriate extension in the filename
////////////////////////////////////////////////////////////
bool    stbi_write_bmp	    (char * filename, Int32 x, Int32 y, Int32 comp, void * data);

#endif // GRAPHICS_STB_IMAGE_AUGMENTED
