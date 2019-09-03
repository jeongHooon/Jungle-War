#pragma once

#define FRAME_BUFFER_WIDTH		800
#define FRAME_BUFFER_HEIGHT		600

#include "Timer.h"
#include "Player.h"
#include "Scene.h"
#include "Shader.h"
#include "ServerMgr.h"
#include "ServerMgr.h"

class CGameFramework
{
private:
	// 서버 관련 
	ServerMgr server_mgr;
	bool is_pushed[11] = { 0 };

	XMFLOAT3 sc_player_pos[MAX_PLAYER_SIZE];
	bool first_recv;
	int recvd_client_id; 

	bool player_ready = false;

	bool is_chat = false;
	// 아이템 생성
	bool is_item_gen = false;
	
	XMFLOAT3 item_pos;
	//빌딩
	XMFLOAT3 building_pos[OBJECT_BUILDING];
	XMFLOAT3 buliding_extents[OBJECT_BUILDING];


	// 총알 충돌프레임
	bool is_collide = false;
	int collide_frame = 100;
	XMFLOAT3 collideLookVector;

public:
	static int my_client_id;
	static XMFLOAT3 buildingPos[OBJECT_BUILDING];
	// 룩벡터
	static XMFLOAT3 sendLook;
	int charstate = 0;
	bool isRun = false;
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateDirect3DDevice();
#ifdef _WITH_DIRECT2D
	void CreateDirect2DDevice();
#endif
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void CreateCommandQueueAndList();

	void OnResizeBackBuffers();

    void BuildObjects();
    void ReleaseObjects();

	// 로긴용
	void SendLoginREQ(char inputID[]);
	// 채팅용
	void SendChatREQ(char inputChat[]);
	void SwapText(int clientID, wchar_t inputChat[20]);
	void SwapText();


    void ProcessInput();
    void AnimateObjects(CCamera *pCamera);
    void FrameAdvance();

	void WaitForGpuComplete();
	void MoveToNextFrame();

	char *ConvertWCtoC(wchar_t* str);
	wchar_t* ConverCtoWC(char* str);

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	static CPlayer				*m_pPlayer[4];
	static CShadow				*m_pShadow[4];
	static CPlayer				*m_pObject[NUM_OBJECT];
	static CShadowTree			*m_pShadowObject[NUM_OBJECT];
	static CPlayer				*m_pObject2[NUM_OBJECT2];
	CPlayer						*m_pBlueBox[2];

	int							bulletDropCount = 0;
	BoundingOrientedBox			mapoobb;

	int							myTeamNum;
	bool						winCheck = false;
	bool						TreeShadowON = true;
	static CCamera				*m_pCamera;
	static int					boxBound;
	float						playerHp = 100;
	int							gameMode = 1;
	bool						damageCheck = false;
	bool						writeMode = true;
	bool						writeStart = false;
	int							writeNum = 0;
	int r = 42;
private:
	int							mainScreenSelect = 0;
	bool						itemUI[4] = {};
	bool						alphaMapOn = false;
	bool						blueScreenMode = false;
	int							blueScreenCount = 0;
	HINSTANCE					m_hInstance;
	HWND						m_hWnd; 

	int							m_nWndClientWidth;
	int							m_nWndClientHeight;
        
	IDXGIFactory4				*m_pdxgiFactory = NULL;
	IDXGISwapChain3				*m_pdxgiSwapChain = NULL;
	ID3D12Device				*m_pd3dDevice = NULL;

	bool						m_bMsaa4xEnable = false;
	UINT						m_nMsaa4xQualityLevels = 0;

	static const UINT			m_nSwapChainBuffers = 2;
	UINT						m_nSwapChainBufferIndex;

	ID3D12Resource				*m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap		*m_pd3dRtvDescriptorHeap = NULL;
	UINT						m_nRtvDescriptorIncrementSize;

	ID3D12Resource				*m_pd3dDepthStencilBuffer = NULL;
	ID3D12DescriptorHeap		*m_pd3dDsvDescriptorHeap = NULL;
	UINT						m_nDsvDescriptorIncrementSize;

	ID3D12CommandAllocator		*m_pd3dCommandAllocator = NULL;
	ID3D12CommandQueue			*m_pd3dCommandQueue = NULL;
	ID3D12GraphicsCommandList	*m_pd3dCommandList = NULL;

	ID3D12Fence					*m_pd3dFence = NULL;
	UINT64						m_nFenceValues[m_nSwapChainBuffers];
	HANDLE						m_hFenceEvent;
	//아이템 드롭
	int							itemDropCount = 0;
	bool						itemDropCheck = false;
	bool						dropStart = false;

	//채팅
	wchar_t						inputtext[100] = L"";
	wchar_t						*outputtext = L"";
	wchar_t						outputtexts[17][100] = {L"",L"",L"",L"",L"",L"",L"",L"",L"",L"" };
	wchar_t						playerName[4][100] = {L"SangWoo", L"Byungchul", L"Sujin", L"JeongHoon"};
	bool						playerReady[4] = { 0,0,0,0 };
	int							playerChat[17] = {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4};

#if defined(_DEBUG)
	ID3D12Debug					*m_pd3dDebugController;
#endif

	CGameTimer					m_GameTimer;

	CScene						*m_pScene = NULL;

	POINT						m_ptOldCursorPos;

	_TCHAR						m_pszFrameRate[50];

#ifdef _WITH_DIRECT2D
	ID3D11On12Device			*m_pd3d11On12Device = NULL;
	ID3D11DeviceContext			*m_pd3d11DeviceContext = NULL;
	ID2D1Factory3				*m_pd2dFactory = NULL;
	IDWriteFactory				*m_pdWriteFactory = NULL;
	ID2D1Device2				*m_pd2dDevice = NULL;
	ID2D1DeviceContext2			*m_pd2dDeviceContext = NULL;

	ID3D11Resource				*m_ppd3d11WrappedBackBuffers[m_nSwapChainBuffers];
	ID2D1Bitmap1				*m_ppd2dRenderTargets[m_nSwapChainBuffers];

	ID2D1SolidColorBrush		*m_pd2dbrBackground = NULL;
	ID2D1SolidColorBrush		*m_pd2dbrBorder = NULL;
	IDWriteTextFormat			*m_pdwFont = NULL;
	IDWriteTextLayout			*m_pdwTextLayout = NULL;
	ID2D1SolidColorBrush		*m_pd2dbrText = NULL;

#ifdef _WITH_DIRECT2D_IMAGE_EFFECT
	IWICImagingFactory			*m_pwicImagingFactory = NULL;
	ID2D1Effect					*m_pd2dfxBitmapSource = NULL;
	ID2D1Effect					*m_pd2dfxGaussianBlur = NULL;
	ID2D1DrawingStateBlock1		*m_pd2dsbDrawingState = NULL;
	IWICFormatConverter			*m_pwicFormatConverter = NULL;
#endif
#endif
};

