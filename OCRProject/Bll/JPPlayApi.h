#ifndef	_JPPLAYAPI_H_
#define	_JPPLAYAPI_H_
///////////////////////////////////////////////////////////////

#include "windows.h"
#define BY_FRAMENUM   0
#define BY_FRAMETIME   1

#define MAX_DISPLAY_WINDOW 4

#ifndef FrameType_t
typedef enum {
    IFrame			= 0,
    PFrame			= 1,
    AudioFrame		= 2,
	MotionDetection = 3,
	UnKnownType     = -1
}FrameType_t;
#endif

typedef struct
{
	long FilePos;         //文件位置；
	long FrameNum;        //帧序号；
	long FrameTime;       //帧时标（ms）;
}FRAME_POS,*PFRAME_POS;

#define FILEMODE       1
#define STREAMMODE     2
#define STREAMFILEMODE 3

#define FRAME_TYPE_AUDIO  0x01
#define FRAME_TYPE_YV12   0x02
#define FRAME_TYPE_RGB32  0x04
#define FRAME_TYPE_YUY2   0x08
#define FRAME_TYPE_RGB24  0x10

#ifdef __cplusplus
extern "C"
{
#endif 

BOOL __stdcall Player_InitialDirectDraw();

BOOL __stdcall Player_ReleaseDirectDraw();

BOOL __stdcall Player_OpenFile(LONG Number,LPSTR FileName);

BOOL __stdcall Player_CloseFile(LONG Number);

BOOL __stdcall Player_InputData(LONG Number,BYTE *Buffer,DWORD Size);

BOOL __stdcall Player_CloseStream(LONG Number);

BOOL __stdcall Player_Play(LONG Number,HWND Hwnd);

BOOL __stdcall Player_OpenSound(LONG Number);

DWORD __stdcall Player_GetVolume(LONG nPort);

BOOL __stdcall Player_SetVolume(LONG nPort,WORD Volume);

BOOL __stdcall Player_CloseSound(LONG nPort);

BOOL __stdcall Player_Stop(LONG Number);

BOOL __stdcall Player_Pause(LONG Number);

BOOL __stdcall Player_Fast(LONG Number);

BOOL __stdcall Player_Slow(LONG Number);

BOOL __stdcall Player_Back(LONG Number);

BOOL __stdcall Player_Next(LONG Number);

DWORD __stdcall Player_GetFileTime(LONG Number);

DWORD __stdcall Player_GetFileFrames(LONG Number);

DWORD __stdcall Player_GetPlayedTime(LONG Number);

DWORD __stdcall Player_GetPlayedFrames(LONG Number);

BOOL __stdcall Player_Refresh(LONG Number);

BOOL __stdcall Player_GetPictureSize(LONG Number,
                                     LONG *Width,
									 LONG *Height);

float __stdcall Player_GetPlayPosition(LONG Number);

BOOL __stdcall Player_SetPlayPosition(LONG Number,float Position);

BOOL __stdcall Player_SetPicQuality(LONG Number,BOOL HighQuality);

BOOL __stdcall Player_SnapPicture(LONG Number,char *FileName);

BOOL __stdcall Player_SetPlayEndMsg(LONG Number,HWND hwnd,UINT nMsg);

DWORD _stdcall Player_GetLastError(LONG Number);

BOOL __stdcall Player_SetPlayedFrames(LONG Number,DWORD FrameNumber);

BOOL _stdcall Player_GetKeyFramePos(LONG Number,DWORD Value,DWORD Type,PFRAME_POS pFramePos);

BOOL _stdcall Player_GetNextKeyFramePos(LONG Number,DWORD Value,DWORD Type,PFRAME_POS pFramePos);

BOOL _stdcall Player_SetDisplayRegion(LONG Number,DWORD RegionNumber,RECT *pSrcRect,HWND hDestWnd,BOOL Enable);

BOOL _stdcall Player_SetPlayedTime(LONG Number,DWORD Time);

BOOL  _stdcall Player_OpenStream(LONG Number,DWORD	nPoolBufSize = 0);

BOOL  _stdcall Player_SetStreamOpenMode(LONG Number,DWORD nMode);

LONG  _stdcall Player_GetStreamOpenMode(LONG Number);

DWORD _stdcall Player_GetSourceBufferRemain(LONG Number);

DWORD _stdcall Player_GetSdkVersion();

BOOL __stdcall Player_Pause(LONG Number);

BOOL __stdcall Player_RestartStream(LONG Number);

BOOL __stdcall Player_SetDecodeCallBack (LONG Number, LONG dwDataType, void (CALLBACK *DecodeCallBack)(LONG dwNumber, BYTE *pBuf, LONG dwSize, LONG dwWidth, LONG dwHeight, LONG dwTimeStamp, LONG dwType));

BOOL __stdcall Player_SetFileRefCallBack (LONG Number, void (CALLBACK *funcFileRefDone)(LONG dwNumber));

int __stdcall  Player_SetPlayPositionEx(LONG Number, float Position);

int __stdcall  Player_ClearStreamBuffer(LONG Number);

#ifdef __cplusplus
}
#endif 

///////////////////////////////////////////////////////////////
#endif // _JPPLAYAPI_H_