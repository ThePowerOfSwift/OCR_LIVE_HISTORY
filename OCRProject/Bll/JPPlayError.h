#define     PLAYER_SUCCESS                          0
//没有错误

#define     PLAYER_INIT_DIRECTDRAW_FAILED           1
//初始化directdraw对象失败

#define     PLAYER_INIT_PRIMARY_SURFACE_FAILED      2
//初始化directdraw主表面失败

#define     PLAYER_INIT_OFFSCREEN_SURFACE_FAILED    3
//初始化directdraw离屏表面失败，可能是显卡不支持

#define     PLAYER_CAN_NOT_OPEN_FILE                4
//无法打开指定文件

#define     PLAYER_FILE_HAS_NO_VALID_INFORMATION    5
//文件不包含有效信息

#define     PLAYER_CREATE_BUFFER_FAILED             6
//创建缓冲区失败

#define     PLAYER_INIT_VIDEO_DECODER_FAILED        7
//初始化视频解码器失败

#define     PLAYER_UNSUPPORTED_COLOR_DEPTH          8
//不支持的系统色深

#define     PLAYER_INVALID_DIRECTDRAW_SURFACE       9
//无效的directdraw表面，可能是初始化directdraw表面时没有成功

#define     PLAYER_PARAMETER_OUT_OF_RANGE           10
//参数不在范围内

#define     PLAYER_STREAM_IS_NOT_OPENED             11
//流没有打开

#define     PLAYER_STREAM_IS_NOT_PLAYED             12
//流没有开始播放

#define     PLAYER_FILE_IS_NOT_OPENED               13
//文件没有打开

#define     PLAYER_INIT_AUDIO_DECODER_FAILED        14
//初始化音频解码器失败

#define     PLAYER_UNSUPPORT_CURRENT_PLAY_MODE      15
//该函数不支持当前的播放模式

#define     PLAYER_STREAM_IS_NOT_CLOSED             16
//流没有关闭

#define     PLAYER_FILE_IS_NOT_CLOSED               17
//文件没有关闭

#define     PLAYER_UNSUPPORT_CURRENT_STATUS         18
//该函数不支持当前的状态

#define     PLAYER_SET_FRAME_OUT_OF_RANGE           19
//设置的帧位置大于文件的总帧数

#define     PLAYER_SET_TIME_OUT_OF_RANGE            20
//设置的时间大于文件总时间

#define     PLAYER_CAN_NOT_SEARCH_IFRAME            21
//搜索不到i帧