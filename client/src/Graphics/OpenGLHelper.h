#ifndef GRAPHICS_OPENGLHELPER_H
#define GRAPHICS_OPENGLHELPER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>

////////////////////////////////////////////////////////////
/// Glew library includes
////////////////////////////////////////////////////////////
#include <Graphics/Glew/Glew.h>

#if defined(SYSTEM_WINDOWS)
	#include <Graphics/Glew/Wglew.h>
#endif

////////////////////////////////////////////////////////////
/// This file just includes the OpenGL (GL and GLU) headers,
/// which have actually different paths on each system
////////////////////////////////////////////////////////////
#if defined(SYSTEM_WINDOWS)

    #include <GL/gl.h>
    #include <GL/glu.h>

#elif defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

    #include <GL/gl.h>
    #include <GL/glu.h>

#elif defined(SYSTEM_MACOS)

    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>

#endif

////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////
#define GL_HELPER_SCREEN_NEAR		1.0f
#define GL_HELPER_SCREEN_DEPTH		1000.0f
#define GL_HELPER_CLEAR_DEPTH		true
#define GL_HELPER_RENDER_2D			false
#define GL_HELPER_WIREFRAME			false

////////////////////////////////////////////////////////////
/// Configure OpenGL states for rendering
///
////////////////////////////////////////////////////////////
bool OpenGLInitialize();

////////////////////////////////////////////////////////////
/// Clear OpenGL scene
///
////////////////////////////////////////////////////////////
void OpenGLClear();

////////////////////////////////////////////////////////////
/// Resize OpenGL scene
///
////////////////////////////////////////////////////////////
void OpenGLResizeScene();

////////////////////////////////////////////////////////////
/// Print graphic device and OpenGL features
///
////////////////////////////////////////////////////////////
void OpenGLPrintFeatures();

////////////////////////////////////////////////////////////
/// Save all OpenGL states
///
////////////////////////////////////////////////////////////
void OpenGLSaveStates();

////////////////////////////////////////////////////////////
/// Restore all OpenGL states
///
////////////////////////////////////////////////////////////
void OpenGLRestoreStates();

////////////////////////////////////////////////////////////
/// Reset the view port
////////////////////////////////////////////////////////////
void OpenGLResetViewPort();

////////////////////////////////////////////////////////////
/// Enable 2D projection
////////////////////////////////////////////////////////////
void OpenGLEnable2D();

////////////////////////////////////////////////////////////
/// Disable 2D projection
////////////////////////////////////////////////////////////
void OpenGLDisable2D();

////////////////////////////////////////////////////////////
/// Let's define a macro to quickly check every OpenGL API calls
////////////////////////////////////////////////////////////
#ifdef DEBUG

    // In debug mode, perform a test on every OpenGL call
    #define GLCheck(Func) ((Func), GLCheckError(__FILE__, __LINE__))

#else

    // Else, we don't add any overhead
    #define GLCheck(Func) (Func)

#endif


////////////////////////////////////////////////////////////
/// Check the last OpenGL error
///
////////////////////////////////////////////////////////////
void GLCheckError(char * File, unsigned int Line);

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
bool SupportsTextureNPOT;		    ///< Texture npot
bool SupportsVBO;					///< Vertex buffer objects
bool SupportsCVA;				    ///< Compiled vertex arrays
bool SupportsEdgeClamp;			    ///< Edge clapmp
bool SupportsPBuffer;			    ///< Pixel buffer
bool SupportsMakeCurrentRead;	    ///< ARB Make current read
bool SupportsVARFence;			    ///< Vertex array range
bool SupportsTextureCombine;	    ///< Texture NV Combine

GLint TextureUnitsCounter;		    ///< Multitexture units
GLint TextureMaxSize;			    ///< Texture size max

////////////////////////////////////////////////////////////
// OpenGL Definitions
////////////////////////////////////////////////////////////

#define GL_CLAMP_TO_EDGE                    0x812F	///< EXT_texture_edge_clamp
#define GL_MAX_TEXTURES_UNITS_ARB           0x84E2	///< ARB_multitexture
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_MAX_ELEMENTS_VERTICES            0x80E8
#define GL_MAX_ELEMENTS_INDICES             0x80E9
#define GL_COMBINE_ARB                      0x8570	///< ARB_texture_env_combine
#define GL_COMBINE_RGB_ARB                  0x8571
#define GL_COMBINE_ALPHA_ARB                0x8572
#define GL_SOURCE0_RGB_ARB                  0x8580
#define GL_SOURCE1_RGB_ARB                  0x8581
#define GL_SOURCE2_RGB_ARB                  0x8582
#define GL_SOURCE0_ALPHA_ARB                0x8588
#define GL_SOURCE1_ALPHA_ARB                0x8589
#define GL_SOURCE2_ALPHA_ARB                0x858A
#define GL_OPERAND0_RGB_ARB                 0x8590
#define GL_OPERAND1_RGB_ARB                 0x8591
#define GL_OPERAND2_RGB_ARB                 0x8592
#define GL_OPERAND0_ALPHA_ARB               0x8598
#define GL_OPERAND1_ALPHA_ARB               0x8599
#define GL_OPERAND2_ALPHA_ARB               0x859A
#define GL_RGB_SCALE_ARB                    0x8573
#define GL_ADD_SIGNED_ARB                   0x8574
#define GL_INTERPOLATE_ARB                  0x8575
#define GL_SUBTRACT_ARB                     0x84E7
#define GL_CONSTANT_ARB                     0x8576
#define GL_PRIMARY_COLOR_ARB                0x8577
#define GL_PREVIOUS_ARB                     0x8578
#define WGL_NUMBER_PIXEL_FORMATS_ARB        0x2000	///< WGL_ARB_pixel_format
#define WGL_DRAW_TO_WINDOW_ARB              0x2001
#define WGL_DRAW_TO_BITMAP_ARB              0x2002
#define WGL_ACCELERATION_ARB                0x2003
#define WGL_NEED_PALETTE_ARB                0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB         0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB          0x2006
#define WGL_SWAP_METHOD_ARB                 0x2007
#define WGL_NUMBER_OVERLAYS_ARB             0x2008
#define WGL_NUMBER_UNDERLAYS_ARB            0x2009
#define WGL_TRANSPARENT_ARB                 0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB       0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB     0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB      0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB     0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB     0x203B
#define WGL_SHARE_DEPTH_ARB                 0x200C
#define WGL_SHARE_STENCIL_ARB               0x200D
#define WGL_SHARE_ACCUM_ARB                 0x200E
#define WGL_SUPPORT_GDI_ARB                 0x200F
#define WGL_SUPPORT_OPENGL_ARB              0x2010
#define WGL_DOUBLE_BUFFER_ARB               0x2011
#define WGL_STEREO_ARB                      0x2012
#define WGL_PIXEL_TYPE_ARB                  0x2013
#define WGL_COLOR_BITS_ARB                  0x2014
#define WGL_RED_BITS_ARB                    0x2015
#define WGL_RED_SHIFT_ARB                   0x2016
#define WGL_GREEN_BITS_ARB                  0x2017
#define WGL_GREEN_SHIFT_ARB                 0x2018
#define WGL_BLUE_BITS_ARB                   0x2019
#define WGL_BLUE_SHIFT_ARB                  0x201A
#define WGL_ALPHA_BITS_ARB                  0x201B
#define WGL_ALPHA_SHIFT_ARB                 0x201C
#define WGL_ACCUM_BITS_ARB                  0x201D
#define WGL_ACCUM_RED_BITS_ARB              0x201E
#define WGL_ACCUM_GREEN_BITS_ARB            0x201F
#define WGL_ACCUM_BLUE_BITS_ARB             0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB            0x2021
#define WGL_DEPTH_BITS_ARB                  0x2022
#define WGL_STENCIL_BITS_ARB                0x2023
#define WGL_AUX_BUFFERS_ARB                 0x2024
#define WGL_NO_ACCELERATION_ARB             0x2025
#define WGL_GENERIC_ACCELERATION_ARB        0x2026
#define WGL_FULL_ACCELERATION_ARB           0x2027
#define WGL_SWAP_EXCHANGE_ARB               0x2028
#define WGL_SWAP_COPY_ARB                   0x2029
#define WGL_SWAP_UNDEFINED_ARB              0x202A
#define WGL_TYPE_RGBA_ARB                   0x202B
#define WGL_TYPE_COLORINDEX_ARB             0x202C

#endif // GRAPHICS_OPENGLHELPER_H
