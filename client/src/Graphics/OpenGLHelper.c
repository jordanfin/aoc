////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/OpenGLHelper.h>

////////////////////////////////////////////////////////////
/// Window includes
////////////////////////////////////////////////////////////
#include <Window/Window.h>

////////////////////////////////////////////////////////////
/// Standard library includes
////////////////////////////////////////////////////////////
#include <System/IOHelper.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
// Member data
////////////////////////////////////////////////////////////
bool DepthEvenFlag;

////////////////////////////////////////////////////////////
/// Configure OpenGL states for rendering
////////////////////////////////////////////////////////////
bool OpenGLInitialize()
{
	// Initialize GLEW
	GLenum Err = glewInit();

	if (GLEW_OK != Err)
	{
		// glewInit failed, something is seriously wrong
		MessageError("Glew Error", "Error: %s\n", glewGetErrorString(Err));
		return false;
	}

	// Check if the pc can use npot textures
	SupportsTextureNPOT  	= glewIsSupported("GL_ARB_texture_non_power_of_two");
	SupportsVBO				= glewIsSupported("GL_ARB_vertex_buffer_object");
	SupportsEdgeClamp 		= glewIsSupported("GL_EXT_texture_edge_clamp");
	SupportsTextureCombine	= glewIsSupported("GL_ARB_texture_env_combine");
	SupportsCVA				= glewIsSupported("GL_EXT_compiled_vertex_array");
	SupportsVARFence		= (glewIsSupported("GL_NV_vertex_array_range") && glewIsSupported("GL_NV_Fence"));
	SupportsPBuffer 		= (glewIsSupported("WGL_ARB_pixel_format") && glewIsSupported("WGL_ARB_pbuffer"));
	SupportsMakeCurrentRead	= glewIsSupported("WGL_ARB_make_current_read");

	GLCheck(glGetIntegerv(GL_MAX_TEXTURES_UNITS_ARB, &TextureUnitsCounter));
	GLCheck(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &TextureMaxSize));

	// Shade model
	glShadeModel(GL_SMOOTH); // GL_FLAT

	// Clear color
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);

	// Initialize depth test
	if (GL_HELPER_CLEAR_DEPTH)
	{
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		
		if (GL_HELPER_RENDER_2D)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glDepthRange(0.0f, 0.5f);
			glDepthFunc(GL_LESS);

			DepthEvenFlag = true;
		}
	}

	// Set perspective calculations
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST); // GL_NICEST);

	if (GL_HELPER_WIREFRAME)
	{
		// Set the width of the lines
		glLineWidth(1.0f);

		// Polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		// Polygon mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Print features
	#ifdef DEBUG
		OpenGLPrintFeatures();
	#endif

	return true;
}

////////////////////////////////////////////////////////////
/// Clear OpenGL scene
////////////////////////////////////////////////////////////
void OpenGLClear()
{
	// Clear Screen And Depth Buffer
	if (GL_HELPER_CLEAR_DEPTH)
	{
		if (GL_HELPER_RENDER_2D)
		{
			if (DepthEvenFlag)
			{
				glDepthFunc( GL_LESS );
				glDepthRange( 0.0, 0.5 );
			}
			else
			{
				glDepthFunc( GL_GREATER );
				glDepthRange( 1.0, 0.5 );
			}

			DepthEvenFlag = !DepthEvenFlag;
		}

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
	else
	{
		glClear( GL_COLOR_BUFFER_BIT );
	}
}

////////////////////////////////////////////////////////////
/// Resize OpenGL scene
////////////////////////////////////////////////////////////
void OpenGLResizeScene()
{
	// Reset the current viewport
	OpenGLResetViewPort();

	// Select the projection matrix
	glMatrixMode(GL_PROJECTION);

	// Reset the projection matrix
	glLoadIdentity();

	// Calculate the aspect ratio of the window
	gluPerspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, GL_HELPER_SCREEN_NEAR, GL_HELPER_SCREEN_DEPTH);

	// Select the modelview matrix
	glMatrixMode(GL_MODELVIEW);

	// Reset the modelview matrix
	glLoadIdentity();
}

////////////////////////////////////////////////////////////
/// Print graphic device and OpenGL features
////////////////////////////////////////////////////////////
void OpenGLPrintFeatures()
{
	printf("Vendor:         %s\nRenderer: 	%s\nOpenGL Version: %s\nGlew Version:   %s\n",
		   (char*)(glGetString(GL_VENDOR)),
		   (char*)(glGetString(GL_RENDERER)),
		   (char*)(glGetString(GL_VERSION)),
		   (char*)(glewGetString(GLEW_VERSION)));
}

////////////////////////////////////////////////////////////
/// Save all OpenGL states (slow)
////////////////////////////////////////////////////////////
void OpenGLSaveStates()
{
	// Save render states
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// Save matrices
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
}

////////////////////////////////////////////////////////////
/// Restore all OpenGL states (slow)
////////////////////////////////////////////////////////////
void OpenGLRestoreStates()
{
	// Restore render states
	glPopAttrib();

	// Restore matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

////////////////////////////////////////////////////////////
/// Reset the view port
////////////////////////////////////////////////////////////
void OpenGLResetViewPort()
{
	glViewport(0, 0, WindowWidth, WindowHeight);
}

////////////////////////////////////////////////////////////
/// Enable 2D projection
////////////////////////////////////////////////////////////
void OpenGLEnable2D()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0f, WindowWidth, WindowHeight, 0.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

////////////////////////////////////////////////////////////
/// Disable 2D projection
////////////////////////////////////////////////////////////
void OpenGLDisable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

////////////////////////////////////////////////////////////
/// Check the last OpenGL error
////////////////////////////////////////////////////////////
void GLCheckError(char * File, unsigned int Line)
{
    // Get the last error
    GLenum ErrorCode = glGetError();

    if (ErrorCode != GL_NO_ERROR)
    {
        char * Error = "unknown error";
        char * Desc  = "no description";

        // Decode the error code
        switch (ErrorCode)
        {
            case GL_INVALID_ENUM :
            {
                Error = "GL_INVALID_ENUM";
                Desc  = "an unacceptable value has been specified for an enumerated argument";
                break;
            }

            case GL_INVALID_VALUE :
            {
                Error = "GL_INVALID_VALUE";
                Desc  = "a numeric argument is out of range";
                break;
            }

            case GL_INVALID_OPERATION :
            {
                Error = "GL_INVALID_OPERATION";
                Desc  = "the specified operation is not allowed in the current state";
                break;
            }

            case GL_STACK_OVERFLOW :
            {
                Error = "GL_STACK_OVERFLOW";
                Desc  = "this command would cause a stack overflow";
                break;
            }

            case GL_STACK_UNDERFLOW :
            {
                Error = "GL_STACK_UNDERFLOW";
                Desc  = "this command would cause a stack underflow";
                break;
            }

            case GL_OUT_OF_MEMORY :
            {
                Error = "GL_OUT_OF_MEMORY";
                Desc  = "there is not enough memory left to execute the command";
                break;
            }

            case GL_INVALID_FRAMEBUFFER_OPERATION_EXT :
            {
                Error = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
                Desc  = "the object bound to FRAMEBUFFER_BINDING_EXT is not \"framebuffer complete\"";
                break;
            }
        }

        // Log the error
		MessageError("GLCheckError", "An internal OpenGL call failed in %s ( %d ) : %s, %s.", File, Line, Error, Desc);
    }
}
