#ifndef GRAPHICS_VIDEORENDER_H
#define GRAPHICS_VIDEORENDER_H

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <Config.h>
#include <System/Timer.h>

////////////////////////////////////////////////////////////
// Default SO includes and declarations
////////////////////////////////////////////////////////////
#ifdef SYSTEM_WINDOWS
	#include <vfw.h>				///< Header file for video for windows

	typedef struct
	{
		AVISTREAMINFO	StreamInfo;	///< Pointer to a structure containing stream info
		PAVISTREAM		Stream;		///< Handle to an open stream
		PGETFRAME		File;		///< Pointer to a get frame oject
	} VideoType;

#else

    #ifdef _cplusplus

	#include <avifile/avifile.h>
	#include <avifile/version.h>

	#if AVIFILE_MINOR_VERSION > 6
	  #include <avifile/avm_default.h>
	#else
	  #include <avifile/default.h>
	#endif

	#include <avifile/image.h>
	#include <avifile/StreamInfo.h>

	#endif

	typedef struct
	{
	    #ifdef _cplusplus
		avm::IReadFile		*File;			///< Pointer to a structure containing stream info
		avm::IReadStream	*Stream;		///< Handle to an open stream
		StreamInfo			*StreamInfo;	///< Pointer to a get frame oject
		#endif

	} VideoType;
#endif

struct Video
{
	bool			Loaded;			///< Check if the video is loaded
	bool			Finished;		///< Check if the video is finished
	bool			Started;		///< Check if the video is started
	UInt32			Width;			///< Video width size
	UInt32			Height;			///< Video height size
	UInt32			CurrentFrame;	///< Current video render frame
	UInt32			LastFrame;		///< Frame length of the video
	UInt32			MsPerFrame;		///< Miliseconds per frame
	VideoType		Object;			///< Video specific SO
	struct Timer	TimerData;		///< Video timer controller
};

////////////////////////////////////////////////////////////
// Load a video
////////////////////////////////////////////////////////////
void VideoLoad(struct Video * VideoData, char * FilePath);

////////////////////////////////////////////////////////////
// Render a frame of the video
////////////////////////////////////////////////////////////
void VideoRender(struct Video * VideoData, bool Loop, UInt32 X, UInt32 Y, UInt32 Width, UInt32 Height);

////////////////////////////////////////////////////////////
// Destroy a video
////////////////////////////////////////////////////////////
void VideoDelete(struct Video * VideoData);

////////////////////////////////////////////////////////////
// Destroy the video library
////////////////////////////////////////////////////////////
void VideoDestroy();

#endif // GRAPHICS_VIDEORENDER_H
