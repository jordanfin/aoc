////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Graphics/VideoRender.h>
#include <Graphics/OpenGLHelper.h>
#include <Graphics/Texture.h>
#include <Graphics/TextureManager.h>
#include <Math/General.h>
#include <System/Error.h>

////////////////////////////////////////////////////////////
// Declarations
////////////////////////////////////////////////////////////
static bool Initialized = false;		///< Check if the library is initialized

////////////////////////////////////////////////////////////
// Swap red color to blue color (OGL Compability)
////////////////////////////////////////////////////////////
inline static UInt32 VideoSwapBytes(void * Buffer, UInt32 Size)
{
	register UInt32 TextureSize = Size;

	__asm
	{
		mov ecx, TextureSize
		mov ebx, Buffer
		label:
			mov al, [ebx+0]
			mov ah, [ebx+2]
			mov [ebx+2], al
			mov [ebx+0], ah
				
			add ebx, 3
			dec ecx
			jnz label
	}

	//jmp [ebx+3], 0 // Color key
}

////////////////////////////////////////////////////////////
// Error handler
////////////////////////////////////////////////////////////
void VideoHandleError(HRESULT hr, char * FilePath)
{
	char Error[0xFF];

	switch (hr)
	{
		case AVIERR_BADFORMAT: 
			strcpy(Error, "The file couldn't be read, indicating a corrupt file or an unrecognized format");
			break;

		case AVIERR_MEMORY:		
			strcpy(Error, "The file could not be opened because of insufficient memory"); 
			break;

		case AVIERR_FILEREAD:
			strcpy(Error, "A disk error occurred while reading the file"); 
			break;

		case AVIERR_FILEOPEN:		
			strcpy(Error, "A disk error occurred while opening the file");
			break;

		case REGDB_E_CLASSNOTREG:		
			strcpy(Error, "According to the registry, the type of file specified in AVIFileOpen does not have a handler to process it");
			break;

		default :
			strcpy(Error, "Unknown");
			break;
	}

	MessageError("VideoLoad", "Failed to open the AVI frame: %s. Reason: %s.", FilePath, Error);
}

////////////////////////////////////////////////////////////
// Load a video
////////////////////////////////////////////////////////////
void VideoLoad(struct Video * VideoData, char * FilePath)
{
	HRESULT hr;

	if (!Initialized)
	{
		AVIFileInit();
		Initialized = true;
	}

	// Opens the AVI stream
	if ((hr = AVIStreamOpenFromFile(&VideoData->Object.Stream, (LPCSTR)(&FilePath[0]),
									streamtypeVIDEO, 0, OF_READ, NULL)) != AVIERR_OK)
	{
		// An error occurred opening the stream
		VideoHandleError(hr, FilePath);
		VideoData->Loaded = false;
		return;
	}

	AVIStreamInfo(VideoData->Object.Stream, &VideoData->Object.StreamInfo, sizeof(VideoData->Object.StreamInfo));

	// Set information
	VideoData->Width		= VideoData->Object.StreamInfo.rcFrame.right  - VideoData->Object.StreamInfo.rcFrame.left;
	VideoData->Height		= VideoData->Object.StreamInfo.rcFrame.bottom - VideoData->Object.StreamInfo.rcFrame.top;
	VideoData->LastFrame	= AVIStreamLength(VideoData->Object.Stream);
	VideoData->MsPerFrame	= AVIStreamSampleToTime(VideoData->Object.Stream, VideoData->LastFrame) / VideoData->LastFrame;
	VideoData->Object.File	= AVIStreamGetFrameOpen(VideoData->Object.Stream, NULL);
	VideoData->Finished		= false;
	VideoData->Started		= false;
	VideoData->CurrentFrame = 0;

	if (VideoData->Object.File	== NULL)
	{
		// An error occurred opening the frame
		MessageError("VideoLoad", "Failed to open the AVI frame: %s", FilePath);
		VideoData->Loaded = false;
		return;
	}
	
	VideoData->Loaded = true;
}

////////////////////////////////////////////////////////////
// Render a frame of the video
////////////////////////////////////////////////////////////
void VideoRender(struct Video * VideoData, bool Loop, UInt32 X, UInt32 Y, UInt32 Width, UInt32 Height)
{
	if (VideoData->Loaded && !VideoData->Finished)
	{
		LPBITMAPINFOHEADER BitmapHeader;
		UInt8 *	Pixels;
		UInt32	TextureWidth, TextureHeight;
		GLint	PreviousTexture;
		GLuint	Texture;

		// Load the frame
		BitmapHeader = (LPBITMAPINFOHEADER)AVIStreamGetFrame(VideoData->Object.File, VideoData->CurrentFrame);
		Pixels = (UInt8 *)BitmapHeader + BitmapHeader->biSize + BitmapHeader->biClrUsed * sizeof(RGBQUAD);

		// Check if it is loaded
		if (!Pixels)
			return;

		// Swap the red and blue bytes (OGL Compatability)
		// VideoSwapBytes((void *)Pixels, 128*128);

		// Adjust internal texture dimensions depending on NPOT textures support
		TextureWidth  = TextureValidSize(VideoData->Width);
		TextureHeight = TextureValidSize(VideoData->Height);

		// Check the maximum texture size
		if ((TextureWidth > (UInt32)(TextureMaxSize)) || (TextureHeight > (UInt32)(TextureMaxSize)))
		{
			MessageError("VideoRender", "Failed to create image, its internal size is too high (%dx%d).", TextureWidth, TextureHeight);
			return;
		}

		// Bind the texture
		PreviousTexture = 0;
		Texture = 0;

		glEnable(GL_TEXTURE_2D);

		glGetIntegerv(GL_TEXTURE_BINDING_2D, &PreviousTexture);

		glGenTextures(1, &Texture);
		glBindTexture(GL_TEXTURE_2D, Texture);

		if (SupportsEdgeClamp)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if (SupportsTextureNPOT != 0)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, 3, VideoData->Width, VideoData->Height,
											0, GL_BGR, GL_UNSIGNED_BYTE, &Pixels[0]);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureWidth, TextureHeight,
											0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, VideoData->Width, VideoData->Height, GL_BGR,
														GL_UNSIGNED_BYTE, &Pixels[0]);
		}

		// Render
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1.0f, 1.0f); glVertex2i(X + Width, Y);
			glTexCoord2f(0.0f, 1.0f); glVertex2i(X, Y);
			glTexCoord2f(1.0f, 0.0f); glVertex2i(X + Width, Y + Height);
			glTexCoord2f(0.0f, 0.0f); glVertex2i(X, Y + Height);
		glEnd();

		// Delete Texture
		glDeleteTextures(1, &Texture);

		// Bind previous
		glBindTexture(GL_TEXTURE_2D, PreviousTexture);

		// Update
		if (VideoData->Started == false)
		{
			// Reset the timer, initialize it
			TimerReset(&VideoData->TimerData);
			VideoData->Started = true;
		}
		
		VideoData->CurrentFrame = TimerGetTime(&VideoData->TimerData) / VideoData->MsPerFrame;

		if (VideoData->CurrentFrame >= VideoData->LastFrame)
		{
			// Set data
			if (Loop)
			{
				// Reset
				TimerReset(&VideoData->TimerData);
				VideoData->CurrentFrame = 0;
			}
			else
			{
				// Finish it
				VideoData->Finished = true;
				VideoData->Started = false;
			}
		}
	}
}

////////////////////////////////////////////////////////////
// Destroy a video
////////////////////////////////////////////////////////////
void VideoDelete(struct Video * VideoData)
{
	// Deallocates the GetFrame resources
	AVIStreamGetFrameClose(VideoData->Object.File);

	// Release the stream
	AVIStreamRelease(VideoData->Object.Stream);
}

////////////////////////////////////////////////////////////
// Destroy the video library
////////////////////////////////////////////////////////////
void VideoDestroy()
{
	// Release the library
	if (Initialized)
		AVIFileExit();
}
