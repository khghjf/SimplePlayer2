
// SimplePlayer2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SimplePlayer2.h"
#include "SimplePlayer2Dlg.h"
#include "afxdialogex.h"

#define __STDC_CONSTANT_MACROS

extern "C"
{
#include "libavcodec/avcodec.h"		//用于解码
#include "libavformat/avformat.h"	//用于定义数据结构
#include "libswscale/swscale.h"		//用于格式转换
#include "SDL2/SDL.h"
}


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSimplePlayer2Dlg 对话框



CSimplePlayer2Dlg::CSimplePlayer2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLEPLAYER2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimplePlayer2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TXT_URL, file_url);
}

BEGIN_MESSAGE_MAP(CSimplePlayer2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CSimplePlayer2Dlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CSimplePlayer2Dlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CSimplePlayer2Dlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CSimplePlayer2Dlg::OnBnClickedButtonAbout)
	ON_BN_CLICKED(IDC_BUTTON_FILESELECT, &CSimplePlayer2Dlg::OnBnClickedButtonFileselect)
END_MESSAGE_MAP()


////播放处理函数
////自己定义事件
////Refresh Event
//#define REFRESH_EVENT (SDL_USEREVENT + 1)
////Break
//#define BREAK_EVENT (SDL_USEREVENT + 2)
//
////保证线程安全退出
//int thread_exit = 0;
//int thread_pause = 0;
//
//
////线程函数，主函数wait 线程运行  线程函数唤醒主函数
//int refresh_video(void* opaque)
//{
//	thread_exit = 0;
//	thread_pause = 0;
//	while (thread_exit == 0)
//	{
//		//没有暂停才会不断发送refresh事件
//		if (thread_pause == 0)
//		{
//			SDL_Event event;
//			event.type = REFRESH_EVENT;
//			SDL_PushEvent(&event);	//wait接收后就可以立马运行
//		}
//		SDL_Delay(20);
//	}
//	//Break  此时thread_exit == 1 没有循环了
//	SDL_Event event;
//	event.type = BREAK_EVENT;
//	SDL_PushEvent(&event);
//	thread_exit = 0;
//	thread_pause = 0;
//	return 0;
//}
//
////LPVOID 表示无类型  等于 void*
////int main(int argc, char* argv[])
//UINT ffmpegplayer(LPVOID lpParam)
//{
//	CSimplePlayer2Dlg* dlg = (CSimplePlayer2Dlg*)lpParam;
//	char filepath[500] = { 0 };
//	//win的API 
//	GetWindowTextA(dlg->file_url, (LPSTR)filepath, 500);
//	//char filepath[] = "cuc_ieschool.flv";
//
//	//先定义FFmpeg重要的七个数据结构	一定要注意之间的嵌套关系
//	AVFormatContext* pFormatCtx;	//记录整个视频文件的基本内容 包含以下所有结构体 嵌套型的
//	//AVInputFormat					//每种封装格式对应一个结构体(如FLV、MP4、AVI)
//	//AVStream						//整个视频所包含的所有数据流，是个数组
//	AVCodecContext* pCodecCtx;		//记录每个视频流(或音频流)的相关信息 
//	AVCodecParameters* pCodecCtx_Par;	//新版记录信息
//
//	//新版avcodec_find_decoder返回const类型
//	const AVCodec* pCodec;				//记录每个视频流(或音频流)编解码内容(H264)
//	AVPacket* packet;				//用来存储一个流的数据(未解码)
//	AVFrame* pFrame, * pFrameYUV;	//用来存储解码后的一个流的数据
//
//	int videoindex;		//记录视频流所在的stream下标
//	struct SwsContext* img_convert_ctx;	//用于格式转换
//
//
//	//开始启动FFmpeg
//	//av_register_all();  高版本不需要添加了 可以直接使用FFmpeg相关函数
//	//avformat_network_init();	使用一些网络功能  运用socket服务
//
//	//初始化结构体  包含AVInputFormat和Stream
//	pFormatCtx = avformat_alloc_context();
//	//给结构体填入AVInputFormat信息
//	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
//	{
//		printf("Can't open input stream !\n");
//		return -1;
//	}
//	//给结构体填入stream的信息
//	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
//	{
//		printf("Can't find stream infomation !\n");
//		return -1;
//	}
//	videoindex = -1;	//开始从stream查找h264的下标
//	for (int i = 0; i < pFormatCtx->nb_streams; i++)
//	{
//		//新版本AVStream的封装流参数由codec替换codecpar
//		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
//		{
//			videoindex = i;
//			break;
//		}
//	}
//	if (videoindex == -1)
//	{
//		printf("Didn't find a video stream !\n");
//		return -1;
//	}
//
//	//根据编解码器id初始化编解码器信息  找到视频解码器
//	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoindex]->codecpar->codec_id);
//	if (pCodec == NULL)
//	{
//		printf("Codec not found !\n");
//		return -1;
//	}
//	//独立的解码上下文
//	pCodecCtx = avcodec_alloc_context3(pCodec);
//	avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
//	//将编解码器信息和这个视频流进行关联，初始化AVCodecContext
//	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
//	{
//		printf("Can't open codec !\n");
//		return -1;
//	}
//
//
//	//Output Info-----------------------------
//	printf("long: %d\n", pFormatCtx->duration);
//	printf("format: %s\n", pFormatCtx->iformat->name);
//	//每个都是包含多个信息的结构体  层层嵌套
//	AVCodecParameters* curAVC = pFormatCtx->streams[videoindex]->codecpar;
//	printf("w&h: %d * %d\n", curAVC->width, curAVC->height);
//	printf("--------------- File Information ----------------\n");
//	//打印关于输入或输出格式的详细信息，例如持续时间，比特率，流，容器，程序，元数据，边数据，编解码器和时基。
//	av_dump_format(pFormatCtx, 0, filepath, 0);
//	printf("-------------------------------------------------\n");
//
//
//	//开始为packet和frame分配空间
//	packet = (AVPacket*)av_malloc(sizeof(AVPacket));
//	//av_frame_alloc其中部分参数默认赋值，原始数据没有分配空间
//	pFrame = av_frame_alloc();
//	pFrameYUV = av_frame_alloc();
//	//分配一张图片大小的空间
//	//让pFrameYUV的data指向out_buffer(为pFrameYUV的data分配空间)
//	pFrameYUV->width = pCodecCtx->width;
//	pFrameYUV->height = pCodecCtx->height;
//	pFrameYUV->format = AV_PIX_FMT_YUV420P;
//	if (av_frame_get_buffer(pFrameYUV, 0) < 0 || av_frame_make_writable(pFrameYUV) < 0)
//	{
//		printf("Can't allocate the space !\n");
//		return -1;
//	}
//
//
//
//
//
//	//开始进行格式转换 将前面格式转换成后面的格式()
//	/*
//	sws_getContext()	初始化
//	sws_scale()			核心 sws_scale转换包含像素格式转换和缩放拉伸转换，输入输出可以是rgb或yuv中的任意一种。
//	sws_freeContext()	释放
//	*/
//	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL
//	);
//
//	//开始进行SDL初始化 屏幕->渲染器->纹理
//	//作用是容SDL is not ready，SDL_SetError 若ready进行一些默认配置
//	//各个flags子系统有不同的初始化函数
//	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
//		printf("Could not initialize SDL - %s\n", SDL_GetError());
//		return -1;
//	}
//
//	SDL_Window* screen;
//	int maxScreen_w = pCodecCtx->width;
//	int maxScreen_h = pCodecCtx->height;
//	int pixel_w = pCodecCtx->width;
//	int pixel_h = pCodecCtx->height;
//
//	//显示在弹出窗口
//	//screen = SDL_CreateWindow("SimplePlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
//	//	maxScreen_w, maxScreen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
//	//显示在MFC控件上
//	screen = SDL_CreateWindowFrom(dlg->GetDlgItem(IDC_SCREEN)->GetSafeHwnd());
//
//	if (!screen)
//	{
//		printf("SDL: can't create window - exiting:%s\n", SDL_GetError());
//		return -1;
//	}
//	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
//	Uint32 pixformat = 0;
//	//IYUV: Y + U + V  (3 planes)
//	//YV12: Y + V + U  (3 planes)
//	pixformat = SDL_PIXELFORMAT_IYUV;	//表示IYUV这四个字符，每个1B 设置纹理的格式
//	//定义纹理
//	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);
//	//设置显示视频的大小
//	SDL_Rect sdlRect;
//	sdlRect.x = 0;
//	sdlRect.y = 0;
//	sdlRect.w = maxScreen_w;
//	sdlRect.h = maxScreen_h;
//	SDL_Event sdlEvent;
//	bool quit = true;
//	SDL_Point leftPoint;
//
//	int cnt = 1;
//	//开始播放  先只播放一遍
//
//	//运行线程
//	SDL_Thread* refresh_thread = SDL_CreateThread(refresh_video, NULL, NULL);
//	SDL_Event event;
//	//
//
//	while (av_read_frame(pFormatCtx, packet) >= 0)
//	{
//		//从数据中找到H264解码成YUV 之后在进行事件判断
//		if (packet->stream_index == videoindex)
//		{
//			//此时为相应的H264包
//			//1.先将数据包发送给解码器 返回0成功
//			if (avcodec_send_packet(pCodecCtx, packet) != 0)
//			{
//				printf("当前解码器满的，无法接收新的数据包!\n");
//				return -1;
//			}
//
//			int res = avcodec_receive_frame(pCodecCtx, pFrame);
//			/*if (res != 0)
//			{
//				printf("解码失败！%d\n" , res);
//				return -1;
//			}*/
//
//			//现在curFrame存放未处理的YUV，通过sws_scale来处理
//			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//				pFrameYUV->data, pFrameYUV->linesize);
//
//			SDL_WaitEvent(&event);
//			if (event.type == REFRESH_EVENT)
//			{
//				//这里有问题
//				SDL_UpdateYUVTexture(sdlTexture, NULL,
//					pFrameYUV->data[0], pFrameYUV->linesize[0],
//					pFrameYUV->data[1], pFrameYUV->linesize[1],
//					pFrameYUV->data[2], pFrameYUV->linesize[2]);
//				SDL_RenderClear(sdlRenderer);
//				SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
//				SDL_RenderPresent(sdlRenderer);
//			}
//			else if (event.type == SDL_QUIT)
//			{
//				thread_exit = 1;
//			}
//			else if (event.type == BREAK_EVENT)
//			{
//				break;
//			}
//			//键盘事件
//			else if (event.type == SDL_KEYDOWN)
//			{
//				if (event.key.keysym.sym == SDLK_SPACE)
//				{
//					thread_pause = !thread_pause;
//				}/*
//				else if (event.key.keysym.sym == SDLK_ESCAPE)
//				{
//					thread_exit = 1;
//				}*/
//			}
//		}
//	}
//
//	//for (;;)
//	//{
//	//	//Wait
//	//	SDL_WaitEvent(&event);
//	//	if (event.type == REFRESH_EVENT)
//	//	{
//	//		//-------------------------
//	//		if (av_read_frame(pFormatCtx, packet) >= 0)
//	//		{
//	//			if (packet->stream_index == videoindex)
//	//			{
//	//				if (avcodec_send_packet(pCodecCtx, packet) != 0)
//	//				{
//	//					AfxMessageBox(_T("当前解码器满的，无法接收新的数据包!\n"));
//	//					return -1;
//	//				}
//	//				avcodec_receive_frame(pCodecCtx, pFrame);
//	//				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//	//								pFrameYUV->data, pFrameYUV->linesize);
//	//				SDL_UpdateYUVTexture(sdlTexture, NULL,
//	//								pFrameYUV->data[0], pFrameYUV->linesize[0],
//	//								pFrameYUV->data[1], pFrameYUV->linesize[1],
//	//								pFrameYUV->data[2], pFrameYUV->linesize[2]);
//	//				SDL_RenderClear(sdlRenderer);
//	//				SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
//	//				SDL_RenderPresent(sdlRenderer);
//
//	//				TRACE("Decode 1 frame \n");
//
//	//			}
//	//		}
//	//		else
//	//		{
//	//			//Exit Thread
//	//			thread_exit = 1;
//	//		}
//
//	//	}
//	//	else if (event.type == SDL_QUIT)
//	//	{
//	//		thread_exit = 1;
//	//	}
//	//	else if (event.type == BREAK_EVENT)
//	//	{
//	//		break;
//	//	}
//	//}
//
//	av_packet_free(&packet);
//
//	//sdl文件关闭
//	SDL_DestroyWindow(screen);
//	SDL_Quit();
//	//ffmpeg文件关闭防止泄露
//	if (pFrameYUV) {
//		av_frame_free(&pFrameYUV);
//		pFrameYUV = nullptr;
//	}
//
//	//FIX Small Bug
//	//SDL Hide Window When it finished
//	dlg->GetDlgItem(IDC_SCREEN)->ShowWindow(SW_SHOWNORMAL);
//	//--------------
//
//	sws_freeContext(img_convert_ctx);
//	av_frame_free(&pFrameYUV);
//	av_frame_free(&pFrame);
//	avcodec_close(pCodecCtx);
//	avformat_close_input(&pFormatCtx);
//
//	return 0;
//}
//
//
//UINT Thread_Play(LPVOID lpParam)
//{
//	CSimplePlayer2Dlg* dlg = (CSimplePlayer2Dlg*)lpParam;
//	ffmpegplayer(lpParam);
//	return 0;
//}





// CSimplePlayer2Dlg 消息处理程序

BOOL CSimplePlayer2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSimplePlayer2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSimplePlayer2Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSimplePlayer2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//Refresh Event
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)

int thread_exit = 0;
int thread_pause = 0;

int sfp_refresh_thread(void* opaque) {

	thread_exit = 0;
	thread_pause = 0;

	while (thread_exit == 0) {
		if (!thread_pause) {
			SDL_Event event;
			event.type = SFM_REFRESH_EVENT;
			SDL_PushEvent(&event);
		}
		SDL_Delay(10);
	}
	//Quit
	SDL_Event event;
	event.type = SFM_BREAK_EVENT;
	SDL_PushEvent(&event);
	thread_exit = 0;
	thread_pause = 0;
	return 0;
}


int simplest_ffmpeg_player(LPVOID lpParam)
{

	AVFormatContext* pFormatCtx;
	int				i, videoindex;
	AVCodecContext* pCodecCtx;
	const AVCodec* pCodec;
	AVFrame* pFrame, * pFrameYUV;
	uint8_t* out_buffer;
	AVPacket* packet;
	int ret, got_picture;

	//------------SDL----------------
	int screen_w, screen_h;
	SDL_Window* screen;
	SDL_Renderer* sdlRenderer;
	SDL_Texture* sdlTexture;
	SDL_Rect sdlRect;
	SDL_Thread* video_tid;
	SDL_Event event;

	struct SwsContext* img_convert_ctx;
	//===========================================
	//文件路径
	CSimplePlayer2Dlg* dlg = (CSimplePlayer2Dlg*)lpParam;
	char filepath[250] = { 0 };
	GetWindowTextA(dlg->file_url, (LPSTR)filepath, 250);
	//===========================================

	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0) {
		AfxMessageBox(_T("Couldn't open input stream.\n"));
		return -1;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		AfxMessageBox(_T("Couldn't find stream information.\n"));
		return -1;
	}
	videoindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			break;
		}

	if (videoindex == -1) {
		AfxMessageBox(_T("Didn't find a video stream.\n"));
		return -1;
	}
	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoindex]->codecpar->codec_id);
	pCodecCtx = avcodec_alloc_context3(pCodec);
	avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[videoindex]->codecpar);
	//pCodecCtx = pFormatCtx->streams[videoindex]->codec;

	if (pCodec == NULL) {
		AfxMessageBox(_T("Codec not found.\n"));
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		AfxMessageBox(_T("Could not open codec.\n"));
		return -1;
	}

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	pFrameYUV->width = pCodecCtx->width;
	pFrameYUV->height = pCodecCtx->height;
	pFrameYUV->format = AV_PIX_FMT_YUV420P;
	if (av_frame_get_buffer(pFrameYUV, 0) < 0 || av_frame_make_writable(pFrameYUV) < 0)
	{
		printf("Can't allocate the space !\n");
		return -1;
	}

	//out_buffer = (uint8_t*)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	//avpicture_fill((AVPicture*)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);


	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
		AfxMessageBox(_T("Could not initialize SDL\n"));
		return -1;
	}
	//SDL 2.0 Support for multiple windows
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;

	//显示在弹出窗口
	//screen = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	//	screen_w, screen_h,SDL_WINDOW_OPENGL);
	//===========================================
	//显示在MFC控件上
	screen = SDL_CreateWindowFrom(dlg->GetDlgItem(IDC_VIDEO)->GetSafeHwnd());

	//===========================================
	if (!screen) {
		AfxMessageBox(_T("SDL: could not create window - exiting\n"));
		return -1;
	}
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;

	packet = (AVPacket*)av_malloc(sizeof(AVPacket));

	video_tid = SDL_CreateThread(sfp_refresh_thread, NULL, NULL);
	//------------SDL End------------
	//Event Loop

	for (;;) {
		//Wait
		SDL_PollEvent(&event);
		if (event.type == SFM_REFRESH_EVENT) {
			//------------------------------
			if (av_read_frame(pFormatCtx, packet) >= 0) {
				if (packet->stream_index == videoindex) {
					/*
					ffmpeg3版本的解码接口做了不少调整，之前的视频解码接口avcodec_decode_video2和avcodec_decode_audio4音频解码被设置为deprecated，
					对这两个接口做了合并，使用统一的接口。并且将音视频解码步骤分为了两步，
					第一步avcodec_send_packet，
					第二步avcodec_receive_frame，
					通过接口名字我们就可以知道第一步是发送编码数据包，第二步是接收解码后数据。
					*/
					//ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
					if (avcodec_send_packet(pCodecCtx, packet) != 0)
					{
						AfxMessageBox(_T("当前解码器满的，无法接收新的数据包!\n"));
						return -1;
					}
					avcodec_receive_frame(pCodecCtx, pFrame);


					sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
					//SDL---------------------------
					//SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
					SDL_UpdateYUVTexture(sdlTexture, NULL,
						pFrameYUV->data[0], pFrameYUV->linesize[0],
						pFrameYUV->data[1], pFrameYUV->linesize[1],
						pFrameYUV->data[2], pFrameYUV->linesize[2]);
					SDL_RenderClear(sdlRenderer);
					//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
					SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
					SDL_RenderPresent(sdlRenderer);
					//SDL End-----------------------
					TRACE("Decode 1 frame\n");
				}
			}
			else {
				//Exit Thread
				thread_exit = 1;
			}
		}
		else if (event.type == SDL_QUIT) {
			thread_exit = 1;
		}
		else if (event.type == SFM_BREAK_EVENT) {
			break;
		}

	}

	sws_freeContext(img_convert_ctx);

	SDL_DestroyWindow(screen);
	SDL_Quit();
	//FIX Small Bug
	//SDL Hide Window When it finished
	dlg->GetDlgItem(IDC_VIDEO)->ShowWindow(SW_SHOWNORMAL);
	//--------------
	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}



UINT Thread_Play(LPVOID lpParam) {
	CSimplePlayer2Dlg* dlg = (CSimplePlayer2Dlg*)lpParam;
	simplest_ffmpeg_player(lpParam);
	return 0;
}




void CSimplePlayer2Dlg::OnBnClickedButtonPlay()
{
	pThreadPlay = AfxBeginThread(Thread_Play, this);//开启线程
}


void CSimplePlayer2Dlg::OnBnClickedButtonPause()
{
	thread_pause = !thread_pause;
}


void CSimplePlayer2Dlg::OnBnClickedButtonStop()
{
	thread_exit = 1;
}


void CSimplePlayer2Dlg::OnBnClickedButtonAbout()
{
	CAboutDlg dlg1;
	dlg1.DoModal();
	//这个对话框的图片在.rc文件
	//对话框类的实现在最上面
}


void CSimplePlayer2Dlg::OnBnClickedButtonFileselect()
{
	//CString str1;
	////注意单字节char转换成宽字节char
	//file_url.GetWindowText(str1);
	////str1.Format(_T("%s"), CStringW(avcodec_configuration()));
	//MessageBox(str1);
	CString FilePathName;
	CFileDialog dlg(TRUE, NULL, NULL, NULL, NULL);///TRUE为OPEN对话框，FALSE为SAVE AS对话框 
	if (dlg.DoModal() == IDOK)
	{
		FilePathName = dlg.GetPathName();
		//该函数改变指定窗口的标题栏的文本内容
		//hWnd：要改变文本内容的窗口或控件的句柄。
		file_url.SetWindowText(FilePathName);
	}


}
