//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"
#include"resource.h"
#include "ServerMgr.h"
#include "Chat.h"

#pragma comment (lib,"winmm")
int CShader::shootBullet;
CShader*	CScene::m_pBuildings;
bool ServerMgr::damageCheck = false;

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	m_nRtvDescriptorIncrementSize = 0;
	m_nDsvDescriptorIncrementSize = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	for (int i = 0; i < 4; ++i)
		m_pPlayer[i] = NULL;
	for (int i = 0; i < NUM_OBJECT; ++i)
		m_pObject[i] = NULL; 
	for (int i = 0; i < NUM_OBJECT2; ++i)
		m_pObject2[i] = NULL;
	m_pBlueBox[0] = NULL;
	_tcscpy_s(m_pszFrameRate, _T("Jungle War ("));

	for (int i = 0; i < 4; ++i) itemUI[i] = false;
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	// 이거 주석치고 아이피 부분 확인
	server_mgr.IPInput();
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();

	BuildObjects();
	charstate = 0;
	// 여기서 핸들을 받아서 비동기 방식으로 통신하면 됨
	first_recv = true;
	printf("통신모듈 초기화 시작\n");
	server_mgr.Initialize(hMainWnd);
	printf("통신모듈 초기화 끝\n");
	return(true);
}

//#define _WITH_SWAPCHAIN

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	dxgiSwapChainDesc.Flags = 0;
#else
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	dxgiSwapChainDesc.Flags = 0;
#else
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
#endif

	if (!m_pdxgiSwapChain)
	{
		MessageBox(NULL, L"Swap Chain Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

#if defined(_DEBUG)
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pd3dDebugController);
	m_pd3dDebugController->EnableDebugLayer();
#endif

	hResult = ::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	IDXGIAdapter1 *pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}

	if (!m_pd3dDevice)
	{
		hResult = m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIAdapter1), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	if (!m_pd3dDevice)
	{
		MessageBox(NULL, L"Direct3D 12 Device Cannot be Created.", L"Error", MB_OK);
		::PostQuitMessage(0);
		return;
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 1;
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pd3dCommandList);
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, d3dDsvCPUDescriptorHandle);
	//	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::OnResizeBackBuffers()
{
	WaitForGpuComplete();

	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	m_nSwapChainBufferIndex = 0;
#else
	//m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);
	m_nSwapChainBufferIndex = 0;
#endif
	CreateRenderTargetViews();
	CreateDepthStencilView();

	m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pScene) m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		for (int i = 0; i < 4; ++i) {

		}
		//printf("=======================\n");
		if (CShader::shootBullet == 0) {
			//CShader::shootBullet = 1;
			sndPlaySound(L"../Assets/Sounds/RifleSound1.wav", SND_ASYNC);	// 사운드
			m_pPlayer[my_client_id]->MinusPlayerBullet();
			m_pPlayer[my_client_id]->ActiveShot();
		}
		else
			CShader::shootBullet = 0;

		//server_mgr.SendPacket(CS_MOUSE_MOVE, m_pPlayer[my_client_id]->GetLook());
		server_mgr.SendPacket(CS_LEFT_BUTTON_DOWN, m_pPlayer[my_client_id]->GetLook());
		break;
	case WM_RBUTTONDOWN:
		//::SetCapture(hWnd);
		//::GetCursorPos(&m_ptOldCursorPos);
		server_mgr.SendPacket(CS_RIGHT_BUTTON_DOWN, m_pPlayer[my_client_id]->GetLook());
		m_pCamera = m_pPlayer[my_client_id]->ChangeCamera(SPACESHIP_CAMERA, m_GameTimer.GetTimeElapsed());	// 마우스 우클시 카메라 변환
		printf("마우스 우클릭\n");
		break;
	


	case WM_LBUTTONUP:
		//server_mgr.SendPacket(CS_MOUSE_MOVE, m_pPlayer[my_client_id]->GetLook());
		//m_pPlayer[my_client_id]->ActiveShot();
		server_mgr.SendPacket(CS_LEFT_BUTTON_UP, m_pPlayer[my_client_id]->GetLook());
		break;
	case WM_RBUTTONUP:
		server_mgr.SendPacket(CS_RIGHT_BUTTON_UP, m_pPlayer[my_client_id]->GetLook());
		m_pCamera = m_pPlayer[my_client_id]->ChangeCamera(FIRST_PERSON_CAMERA, m_GameTimer.GetTimeElapsed());	// 마우스 우클시 카메라 변환
		break;
	case WM_MOUSEMOVE:
		if (boxBound == 1) 
			server_mgr.SendPacket(CS_MOUSE_MOVE, sendLook);
		else 
			server_mgr.SendPacket(CS_MOUSE_MOVE, m_pPlayer[my_client_id]->GetLook());
		boxBound = 0;
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	int key_buffer = wParam;
	if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
	case WM_KEYDOWN: {
		if (writeMode == true) {
			switch (key_buffer) {
			case 'a':
			case 'A':
				cout << "a";
				break;
			case 'b':
			case 'B':
				cout << "b";
				break;
			case 'c':
			case 'C':
				cout << "c";
				break;
			case 'd':
			case 'D':
				cout << "d";
				break;
			case 'e':
			case 'E':
				cout << "e";
				break;
			case 'f':
			case 'F':
				cout << "f";
				break;
			case 'g':
			case 'G':
				cout << "g";
				break;
			case 'h':
			case 'H':
				cout << "h";
				break;
			case 'i':
			case 'I':
				cout << "i";
				break;
			case 'j':
			case 'J':
				cout << "j";
				break;
			case 'k':
			case 'K':
				cout << "k";
				break;
			case 'l':
			case 'L':
				cout << "l";
				break;
			case 'm':
			case 'M':
				cout << "m";
				break;
			case 'n':
			case 'N':
				cout << "n";
				break;
			case 'o':
			case 'O':
				cout << "o";
				break;
			case 'p':
			case 'P':
				cout << "p";
				break;
			case 'q':
			case 'Q':
				cout << "q";
				break;
			case 'r':
			case 'R':
				cout << "r";
				break;
			case 's':
			case 'S':
				cout << "s";
				break;
			case 't':
			case 'T':
				cout << "t";
				break;
			case 'u':
			case 'U':
				cout << "u";
				break;
			case 'v':
			case 'V':
				cout << "v";
				break;
			case 'w':
			case 'W':
				cout << "w";
				break;
			case 'x':
			case 'X':
				cout << "x";
				break;
			case 'y':
			case 'Y':
				cout << "y";
				break;
			case 'z':
			case 'Z':
				cout << "z";
				break;
			case '1':
				cout << "1";
				break;
			case '2':
				cout << "2";
				break;
			case '3':
				cout << "3";
				break;
			case '4':
				cout << "4";
				break;
			case '5':
				cout << "5";
				break;
			case '6':
				cout << "6";
				break;
			case '7':
				cout << "7";
				break;
			case '8':
				cout << "8";
				break;
			case '9':
				cout << "9";
				break;
			case '0':
				cout << "0";
				break;
			}


		}

		if (wParam == VK_SHIFT) {
			if (is_pushed[CS_KEY_PRESS_SHIFT] == false) {
				//printf("[WM_KEYUP] : Shift 키 입력\n");
				isRun = true;
				if (charstate == 3) {
					m_pPlayer[my_client_id]->GetKeyInput(1);
					charstate = 1;
				}
				else if (charstate == 5) {
					m_pPlayer[my_client_id]->GetKeyInput(14);
					charstate = 14;
				}
				else if (charstate == 6) {
					m_pPlayer[my_client_id]->GetKeyInput(13);
					charstate = 13;
				}
				else if (charstate == 8) {
					m_pPlayer[my_client_id]->GetKeyInput(12);
					charstate = 12;
				}
				else if (charstate == 9) {
					m_pPlayer[my_client_id]->GetKeyInput(11);
					charstate = 11;
				}
				else if (charstate == 4) {
					m_pPlayer[my_client_id]->GetKeyInput(10);
					charstate = 10;
				}
				server_mgr.SendPacket(CS_KEY_PRESS_SHIFT);
				is_pushed[CS_KEY_PRESS_SHIFT] = true;
			}
		}
		if (wParam == VK_SPACE) {
			if (is_pushed[CS_KEY_PRESS_SPACE] == false) {
				//printf("[WM_KEYUP] : Space 키 입력\n");
				server_mgr.SendPacket(CS_KEY_PRESS_SPACE);
				is_pushed[CS_KEY_PRESS_SPACE] = true;
				m_pPlayer[my_client_id]->GetKeyInput(15);
				charstate = 15;
			}
		}
		if (wParam == VK_F5) {
			if (player_ready) {
				//printf("Ready 취소\n");
				server_mgr.SendPacket(CS_PLAYER_READY_CANCLE);
				player_ready = false;
			}
			else {
				//printf("Ready\n");
				server_mgr.SendPacket(CS_PLAYER_READY);
				player_ready = true;
			}
		}

		// char 형 key들 입력 처리 
		switch (key_buffer) {
		case 'w':
		case 'W':
			if (is_pushed[CS_KEY_PRESS_UP] == false) {
				//server_mgr.SendPacket(CS_KEY_PRESS_UP);
				if (charstate == 6) {
					m_pPlayer[my_client_id]->GetKeyInput(6);
					charstate = 6;
					if (is_pushed[CS_KEY_PRESS_SHIFT]) {
						m_pPlayer[my_client_id]->GetKeyInput(13);
						charstate = 13;
					}
				}
				else if (charstate == 5) {
					m_pPlayer[my_client_id]->GetKeyInput(5);
					charstate = 5;
					
				}
				else if (charstate == 13) {
					m_pPlayer[my_client_id]->GetKeyInput(13);
					charstate = 13;
				}
				else if (charstate == 14) {
					m_pPlayer[my_client_id]->GetKeyInput(14);
					charstate = 14;
				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(3);
					m_pPlayer[my_client_id]->InitAnimation(3);
					charstate = 3;
				}
				server_mgr.SendPacket(CS_KEY_PRESS_UP, m_pPlayer[my_client_id]->GetLook());
				//printf("Look Vector : %lf, %lf, %lf\n", m_pPlayer[my_client_id]->GetLook().x, m_pPlayer[my_client_id]->GetLook().y, m_pPlayer[my_client_id]->GetLook().z);
				//printf("w를 눌렀는데 my_client_id는 이거임  %d  \n", my_client_id);
				is_pushed[CS_KEY_PRESS_UP] = true;
				//sndPlaySound(L"../Assets/Sounds/FootStep.wav", SND_ASYNC);	//���� ���
			}
			break;
		case 'a':
		case 'A':
			if (is_pushed[CS_KEY_PRESS_LEFT] == false) {
				//server_mgr.SendPacket(CS_KEY_PRESS_LEFT);
				if (charstate == 4) {
					m_pPlayer[my_client_id]->GetKeyInput(9);
					charstate = 9;
				}
				else if (charstate == 3) {
					m_pPlayer[my_client_id]->GetKeyInput(6);
					charstate = 6;
					//printf("215\n");
				}
				else if (charstate == 1) {
					m_pPlayer[my_client_id]->GetKeyInput(13);
					charstate = 13;
					
				}
				else if (charstate == 10) {
					m_pPlayer[my_client_id]->GetKeyInput(11);
					charstate = 11;

				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(6);
					charstate = 6;
				}
				server_mgr.SendPacket(CS_KEY_PRESS_LEFT, m_pPlayer[my_client_id]->GetLook());

				is_pushed[CS_KEY_PRESS_LEFT] = true;
			}
			break;
		case 's':
		case 'S':
			if (is_pushed[CS_KEY_PRESS_DOWN] == false) {
				//server_mgr.SendPacket(CS_KEY_PRESS_DOWN);
				if (charstate == 6) {
					m_pPlayer[my_client_id]->GetKeyInput(9);
					charstate = 9;
				}
				else if (charstate == 5) {
					m_pPlayer[my_client_id]->GetKeyInput(8);
					charstate = 8;
				}
				else if (charstate == 13) {
					m_pPlayer[my_client_id]->GetKeyInput(11);
					charstate = 11;
				}
				else if (charstate == 14) {
					m_pPlayer[my_client_id]->GetKeyInput(12);
					charstate = 12;
				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(4);
					charstate = 4;
				}
				server_mgr.SendPacket(CS_KEY_PRESS_DOWN, m_pPlayer[my_client_id]->GetLook());

				is_pushed[CS_KEY_PRESS_DOWN] = true;
			}
			break;
		case 'd':
		case 'D':
			if (is_pushed[CS_KEY_PRESS_RIGHT] == false) {
				//server_mgr.SendPacket(CS_KEY_PRESS_RIGHT);
				if (charstate == 4) {
					m_pPlayer[my_client_id]->GetKeyInput(8);
					charstate = 8;
				}
				else if (charstate == 3) {
					m_pPlayer[my_client_id]->GetKeyInput(5);
					charstate = 5;
				}
				else if (charstate == 1) {
					m_pPlayer[my_client_id]->GetKeyInput(14);
					charstate = 14;
				}
				else if (charstate == 10) {
					m_pPlayer[my_client_id]->GetKeyInput(12);
					charstate = 12;
				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(5);
					charstate = 5;
				}
				server_mgr.SendPacket(CS_KEY_PRESS_RIGHT, m_pPlayer[my_client_id]->GetLook());

				is_pushed[CS_KEY_PRESS_RIGHT] = true;
			}
			break;
		case 'c':
		case 'C':

			//printf("캐릭터 Y %f  \n", m_pPlayer[my_client_id]->GetPosition().y);
			//printf("터레인 높이 %f \n", m_pScene->GetTerrain()->GetHeight(m_pPlayer[my_client_id]->GetPosition().x, m_pPlayer[my_client_id]->GetPosition().z));
			if (is_pushed[CS_KEY_PRESS_CROUCH] == false) {
				m_pPlayer[my_client_id]->GetKeyInput(7);
				server_mgr.SendPacket(CS_KEY_PRESS_CROUCH, m_pPlayer[my_client_id]->GetLook());
				is_pushed[CS_KEY_PRESS_CROUCH] = true;
			}
			break;
		case '1':
			if (is_pushed[CS_KEY_PRESS_1] == false) {
				server_mgr.SendPacket(CS_KEY_PRESS_1);
				is_pushed[CS_KEY_PRESS_1] = true;
			}
			itemUI[0] = !itemUI[0];
			break;
		case '2':
			if (is_pushed[CS_KEY_PRESS_2] == false) {
				server_mgr.SendPacket(CS_KEY_PRESS_2);
				is_pushed[CS_KEY_PRESS_2] = true;
			}
			itemUI[1] = !itemUI[1];
			break;
		case '3':
			itemUI[2] = !itemUI[2];
			break;
		case '4':
			itemUI[3] = !itemUI[3];
			break;
		case 'm':
		case 'M':
			alphaMapOn = !alphaMapOn;
			break;
		case '0':
			++gameMode;
			if (gameMode > 2)
				gameMode = 0;
			break;
		case 'Q':
			if (is_pushed[CS_KEY_PRESS_Q] == false) {
				server_mgr.SendPacket(CS_KEY_PRESS_Q, m_pPlayer[my_client_id]->GetLook());
				is_pushed[CS_KEY_PRESS_Q] = true;
			}
			break;
		}
		//
		break;
	}
	case WM_KEYUP:
		// Shift 
		if (wParam == VK_ESCAPE) {
			exit(-1);
		}
		else if (wParam == VK_SHIFT) {
			//printf("[WM_KEYUP] : Shift 키 놓음\n");
			if (is_pushed[CS_KEY_PRESS_UP] == false)
				m_pPlayer[my_client_id]->GetKeyInput(0);
			else
				m_pPlayer[my_client_id]->GetKeyInput(3);

			if (charstate == 1) {
				m_pPlayer[my_client_id]->GetKeyInput(3);
				charstate = 3;
			}
			else if (charstate == 14) {
				m_pPlayer[my_client_id]->GetKeyInput(5);
				charstate = 5;
			}
			else if (charstate == 13) {
				m_pPlayer[my_client_id]->GetKeyInput(6);
				charstate = 6;
			}
			else if (charstate == 12) {
				m_pPlayer[my_client_id]->GetKeyInput(8);
				charstate = 8;
			}
			else if (charstate == 11) {
				m_pPlayer[my_client_id]->GetKeyInput(9);
				charstate = 9;
			}
			else if (charstate == 10) {
				m_pPlayer[my_client_id]->GetKeyInput(4);
				charstate = 4;
			}
			server_mgr.SendPacket(CS_KEY_RELEASE_SHIFT);
			is_pushed[CS_KEY_PRESS_SHIFT] = false;

		}
		else if (wParam == VK_SPACE) {
			//printf("[WM_KEYUP] : Space 키 놓음\n");
			server_mgr.SendPacket(CS_KEY_RELEASE_SPACE);
			is_pushed[CS_KEY_PRESS_SPACE] = false;
			m_pPlayer[my_client_id]->GetKeyInput(0);
			charstate = 0;
		}

		switch (key_buffer) {
		case 'w':
		case 'W':
			if (is_pushed[CS_KEY_PRESS_UP] == true) {
				//printf("[WM_KEYDOWN] : w,W키 놓음 \n");
				if (charstate == 6) {
					m_pPlayer[my_client_id]->GetKeyInput(6);
					charstate = 6;
				}
				else if (charstate == 5) {
					m_pPlayer[my_client_id]->GetKeyInput(5);
					charstate = 5;
				}
				else if (charstate == 13) {
					m_pPlayer[my_client_id]->GetKeyInput(13);
					charstate = 13;
				}
				else if (charstate == 14) {
					m_pPlayer[my_client_id]->GetKeyInput(14);
					charstate = 14;
				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(0);
					charstate = 0;
				}
				server_mgr.SendPacket(CS_KEY_RELEASE_UP);
				is_pushed[CS_KEY_PRESS_UP] = false;
			}
			break;
		case 'a':
		case 'A':
			if (is_pushed[CS_KEY_PRESS_LEFT] == true) {
				//printf("[WM_KEYDOWN] : a,A키 놓음 \n");
				if (charstate == 6 && is_pushed[CS_KEY_PRESS_UP]) {
					m_pPlayer[my_client_id]->GetKeyInput(3);
					charstate = 3;
				}
				else if (charstate == 9 && is_pushed[CS_KEY_PRESS_DOWN]) {
					m_pPlayer[my_client_id]->GetKeyInput(3);
					charstate = 4;
				}
				else if (charstate == 13 ) {
					m_pPlayer[my_client_id]->GetKeyInput(1);
					charstate = 1;
				}
				else if (charstate == 11) {
					m_pPlayer[my_client_id]->GetKeyInput(10);
					charstate = 10;
				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(0);
					charstate = 0;
				}
				server_mgr.SendPacket(CS_KEY_RELEASE_LEFT);
				is_pushed[CS_KEY_PRESS_LEFT] = false;
			}
			break;
		case 's':
		case 'S':
			if (is_pushed[CS_KEY_PRESS_DOWN] == true) {
				//printf("[WM_KEYDOWN] : s,S키 놓음 \n");
				if (charstate == 9) {
					m_pPlayer[my_client_id]->GetKeyInput(6);
					charstate = 6;
				}
				else if (charstate == 8) {
					m_pPlayer[my_client_id]->GetKeyInput(5);
					charstate = 5;
				}
				else if (charstate == 11) {
					m_pPlayer[my_client_id]->GetKeyInput(13);
					charstate = 13;
				}
				else if (charstate == 12) {
					m_pPlayer[my_client_id]->GetKeyInput(14);
					charstate = 14;
				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(0);
					charstate = 0;
				}
				server_mgr.SendPacket(CS_KEY_RELEASE_DOWN);
				is_pushed[CS_KEY_PRESS_DOWN] = false;
			}
			break;
		case 'd':
		case 'D':
			if (is_pushed[CS_KEY_PRESS_RIGHT] == true) {
				//printf("[WM_KEYDOWN] : d,D키 놓음 \n");
				if (charstate == 5 && is_pushed[CS_KEY_PRESS_UP]) {
					m_pPlayer[my_client_id]->GetKeyInput(3);
					charstate = 3;
				}
				else if (charstate == 8 && is_pushed[CS_KEY_PRESS_DOWN]) {
					m_pPlayer[my_client_id]->GetKeyInput(3);
					charstate = 4;
				}
				else if (charstate == 14) {
					m_pPlayer[my_client_id]->GetKeyInput(1);
					charstate = 1;
				}
				else if (charstate == 12) {
					m_pPlayer[my_client_id]->GetKeyInput(10);
					charstate = 10;
				}
				else {
					m_pPlayer[my_client_id]->GetKeyInput(0);
					charstate = 0;
				}
				server_mgr.SendPacket(CS_KEY_RELEASE_RIGHT);
				is_pushed[CS_KEY_PRESS_RIGHT] = false;
			}
			break;
		case 'c':
		case 'C':
			if (is_pushed[CS_KEY_PRESS_CROUCH] == true) {
				//printf("[WM_KEYDOWN] : c,C키 놓음 \n");
				m_pPlayer[my_client_id]->GetKeyInput(0);
				server_mgr.SendPacket(CS_KEY_RELEASE_CROUCH);
				is_pushed[CS_KEY_PRESS_CROUCH] = false;
			}
			break;
		case '1':
			if (is_pushed[CS_KEY_PRESS_1] == true) {
				//printf("[WM_KEYDOWN] : 1키 놓음 \n");
				server_mgr.SendPacket(CS_KEY_RELEASE_1);
				is_pushed[CS_KEY_PRESS_1] = false;
			}
			break;
		case '2':
			if (is_pushed[CS_KEY_PRESS_2] == true) {
				//printf("[WM_KEYDOWN] : 2키 놓음 \n");
				server_mgr.SendPacket(CS_KEY_RELEASE_2);
				is_pushed[CS_KEY_PRESS_2] = false;
			}
			break;

		case '9':
			SendLoginREQ();
			break;
		case 'Q':
			if (is_pushed[CS_KEY_PRESS_Q] == true) {
				//printf("[WM_KEYDOWN] : Q키 놓음 \n");
				server_mgr.SendPacket(CS_KEY_RELEASE_Q);
				is_pushed[CS_KEY_PRESS_Q] = false;
				
			}
			break;
		case VK_UP:
			--mainScreenSelect;
			if (mainScreenSelect < 0)
				mainScreenSelect = 2;
			break;
		case VK_DOWN:
			++mainScreenSelect;
			if (mainScreenSelect > 2)
				mainScreenSelect = 0;
			break;
		case VK_RETURN:
			if (gameMode == 0 && mainScreenSelect == 1)
				++gameMode;
			break;
		}
		switch (wParam)
		{
		case VK_ESCAPE:
			exit(-1);
			break;
		case VK_RETURN:
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			m_pCamera = m_pPlayer[my_client_id]->ChangeCamera((DWORD)(wParam - VK_F1 + 1), m_GameTimer.GetTimeElapsed());
			break;
		case VK_F9:
		{
			BOOL bFullScreenState = FALSE;
			m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
			m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

			DXGI_MODE_DESC dxgiTargetParameters;
			dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			dxgiTargetParameters.Width = m_nWndClientWidth;
			dxgiTargetParameters.Height = m_nWndClientHeight;
			dxgiTargetParameters.RefreshRate.Numerator = 60;
			dxgiTargetParameters.RefreshRate.Denominator = 1;
			dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

			OnResizeBackBuffers();

			break;
		}
		case VK_F10:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
			m_GameTimer.Stop();
		else
			m_GameTimer.Start();
		break;
	}
	case WM_SIZE:
	{
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);

		OnResizeBackBuffers();
		break;
	}
	case WM_SOCKET: {
		if (WSAGETSELECTERROR(lParam)) {
			closesocket((SOCKET)wParam);
			server_mgr.ClientError();
			break;
		}
		switch (WSAGETSELECTEVENT(lParam)) {
		case FD_READ:
			// 첫번째 읽을때 아이디 저장
			XMFLOAT3 read_buf;
			server_mgr.ReadPacket();
			if (first_recv) {
				printf("드루와\n");
				first_recv = false;
				my_client_id = server_mgr.ReturnCameraID();
				m_pCamera = m_pPlayer[my_client_id]->GetCamera();
				printf("카메라는 %d에 고정\n", my_client_id);
				server_mgr.ReturnBuildingPosition(building_pos);
				server_mgr.ReturnBuildingExtents(buliding_extents);
				for (int i = 0; i < OBJECT_BUILDING; ++i) {
					//printf("[%d]번 빌딩 [%f, %f, %f] \n", i, building_pos[i].x,
					//	building_pos[i].y,
					//	building_pos[i].z);

					//printf("[%d] 빌딩 [%f, %f, %f] 크기 : [%f, %f, %f] \n", i,
					//	building_pos[i].x,
					//	building_pos[i].y,
					//	building_pos[i].z,
					//	buliding_extents[i].x,
					//	buliding_extents[i].y,
					//	buliding_extents[i].z);

					buildingPos[i] = XMFLOAT3(building_pos[i].x, building_pos[i].y, building_pos[i].z);
				}

			}
			if (server_mgr.GetClientID() != my_client_id) {
				m_pPlayer[server_mgr.GetClientID()]->SetLookTemp(server_mgr.ReturnLookVector());
				//m_pPlayer[server_mgr.GetClientID()]->SetLook(XMFLOAT3(0.0f,0.0f,1.0f));
			}

			m_pPlayer[server_mgr.GetClientID()]->SetPosition(server_mgr.ReturnPlayerPosStatus(server_mgr.GetClientID()).pos);
			


			//printf("상태 : %d\n",server_mgr.ReturnPlayerPosStatus(server_mgr.GetClientID()).player_status);

			if (server_mgr.GetClientID() != my_client_id) {
				m_pPlayer[server_mgr.GetClientID()]->GetKeyInput(server_mgr.ReturnPlayerPosStatus(server_mgr.GetClientID()).player_status);
				//printf("Player 상태 %d\n", server_mgr.ReturnPlayerPosStatus(server_mgr.GetClientID()).player_status);
			}

			//m_pScene->m_ppShaders[2]->SetPosition(server_mgr.GetBullet().id,
			//	XMFLOAT3(server_mgr.GetBullet().x, server_mgr.GetBullet().y, server_mgr.GetBullet().z));
			
			for (int i = 0; i < MAX_PLAYER_SIZE*MAX_BOX_SIZE; ++i) {
				if (server_mgr.GetBoxInuse(i) == 0) {
					m_pScene->m_pBuildings->SetBoxPosition(i, XMFLOAT3(0, 0, 0));	
				}
				else {
					m_pScene->m_pBuildings->SetBoxPosition(i, XMFLOAT3(server_mgr.GetBox(i).x, server_mgr.GetBox(i).y, server_mgr.GetBox(i).z));
				}
			}	//박스위치
			// 아이템생성
			if (server_mgr.IsItemGen()) {
				server_mgr.ReturnItemPosition();
			}
			// 플레이어 체력	(PlayerNum을 인자로 받음)
			if (ServerMgr::damageCheck == true) {
				playerHp += server_mgr.GetPlayerHP(my_client_id);
				ServerMgr::damageCheck = false;
			}
			
			//printf("%f", playerHp);
			// 빌딩은 총 10개 0~9 로 접근 가능.
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wParam);
			server_mgr.ClientError();
			break;
		}
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

void CGameFramework::SendLoginREQ() {
	char userid[256];
	char passwd[256];

	cout << "id를 입력해 주세요 ";
	cin >> userid;
	cout << "암호를 입력해 주세요 ";
	cin >> passwd;

	char protoBuffer[1024];
	ProtoCommand *cmd = (ProtoCommand *)protoBuffer;
	StrLoginREQ *login = (StrLoginREQ *)cmd->data;

	cmd->command = ComLoginREQ;

	login->userid[maxUserIDLen - 1] = '\0';

	strncpy_s((char *)login->passwd, maxPasswdLen, passwd, maxPasswdLen);
	login->passwd[maxPasswdLen - 1] = '\0';

	SendChatREQ();

}
void CGameFramework::SendChatREQ() {
//	while (true) {
		char buffer[1024];
		cout << "전송할 문자열 : ";
		cin >> buffer;

		//	if (strcmp(buffer, "/w") == 0)  // 입력 문자열이 "/w"라면 귓속말
		//		SendWisper(toServer);
//	}
}


void CGameFramework::OnDestroy()
{
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

#if defined(_DEBUG)
	if (m_pd3dDebugController) m_pd3dDebugController->Release();
#endif

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(TRUE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();
}

void CGameFramework::BuildObjects()
{
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	m_pScene = new CScene();
	m_pScene->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		m_pScene->m_pPlayer[i] = m_pPlayer[i] = new CAirplanePlayer(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1);
		m_pPlayer[i]->SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	}

	for (int i = 0; i < NUM_OBJECT; ++i) {
		m_pScene->m_pObject[i] = m_pObject[i] = new CTreeObject(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1);
		/*if(i == 0)
			m_pObject[i]->SetLook(XMFLOAT3(0.0f, 0.0f, 0.0f));
		else if(i==1)
			m_pObject[i]->SetLook(XMFLOAT3(1.0f, 0.0f, 0.0f));
		else if(i==2)
			m_pObject[i]->SetLook(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		else if(i==3)
			m_pObject[i]->SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
		else if(i==4)
			m_pObject[i]->SetLook(XMFLOAT3(0.0f, 0.0f, -1.0f));*/

	}
	for (int i = 0; i < NUM_OBJECT2; ++i) {
		m_pScene->m_pObject2[i] = m_pObject2[i] = new CRockObject(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1);
		
	}
	m_pScene->m_pBlueBox[0] = m_pBlueBox[0] = new CBlueBox(m_pd3dDevice, m_pd3dCommandList, m_pScene->GetGraphicsRootSignature(), m_pScene->GetTerrain(), 1);
	//m_pCamera = m_pPlayer[my_client_id]->GetCamera();

#ifdef _WITH_APACHE_MODEL
	m_pPlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, -300.0f));
	m_pPlayer->Rotate(0.0f, -45.0f, 0.0f);
#endif
#ifdef _WITH_GUNSHIP_MODEL
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		m_pPlayer[i]->SetPosition(XMFLOAT3(30 * i, -100.0f, 0.0f));
		/*if(i!=my_client_id)
			m_pPlayer[i]->SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));*/
	}
	for (int i = 0; i < NUM_OBJECT; ++i) {
		float xPosition;
		float zPosition;

		if (i == 0) xPosition = 230, zPosition = 280;
		else if (i == 1) xPosition = 110, zPosition = 270;
		else if (i == 2) xPosition = 135, zPosition = 200;
		else if (i == 3) xPosition = 260, zPosition = 340;
		else if (i == 4) xPosition = 310, zPosition = 230;
		else if (i == 5) xPosition = 360, zPosition = 370;
		else if (i == 6) xPosition = 430, zPosition = 150;

		else if (i == 7) xPosition = 510, zPosition = 310;
		else if (i == 8) xPosition = 560, zPosition = 290;
		else if (i == 9) xPosition = 570, zPosition = 190;
		else if (i == 10) xPosition = 755, zPosition = 240;
		else if (i == 11) xPosition = 665, zPosition = 360;
		else if (i == 12) xPosition = 880, zPosition = 335;
		else if (i == 13) xPosition = 795, zPosition = 420;

		else if (i == 14) xPosition = 325, zPosition = 700;
		else if (i == 15) xPosition = 350, zPosition = 550;
		else if (i == 16) xPosition = 400, zPosition = 710;
		else if (i == 17) xPosition = 420, zPosition = 860;
		else if (i == 18) xPosition = 430, zPosition = 645;
		else if (i == 19) xPosition = 450, zPosition = 550;
		else if (i == 20) xPosition = 460, zPosition = 770;

		else if (i == 21) xPosition = 560, zPosition = 840;
		else if (i == 22) xPosition = 660, zPosition = 800;
		else if (i == 23) xPosition = 590, zPosition = 690;
		else if (i == 24) xPosition = 670, zPosition = 580;
		else if (i == 25) xPosition = 730, zPosition = 720;
		else if (i == 26) xPosition = 750, zPosition = 620;
		else if (i == 27) xPosition = 830, zPosition = 620;

		else if (i == 28) xPosition = 420, zPosition = 290;
		else if (i == 29) xPosition = 424, zPosition = 370;
		else if (i == 30) xPosition = 560, zPosition = 500;
		else if (i == 31) xPosition = 474, zPosition = 440;
		else if (i == 32) xPosition = 630, zPosition = 510;
		else if (i == 33) xPosition = 600, zPosition = 440;
		else if (i == 34) xPosition = 540, zPosition = 450;

		else if (i == 35) xPosition = 520, zPosition = 390;
		else if (i == 36) xPosition = 524, zPosition = 470;
		else if (i == 37) xPosition = 560, zPosition = 300;
		else if (i == 38) xPosition = 574, zPosition = 340;
		else if (i == 39) xPosition = 530, zPosition = 410;
		else if (i == 40) xPosition = 500, zPosition = 340;
		else if (i == 41) xPosition = 540, zPosition = 450;
		/*else if (i == 28) xPosition = 602, zPosition = 1122;
		else if (i == 29) xPosition = 3000, zPosition = 3000;*/
		float fHeight = m_pScene->GetTerrain()->GetHeight(xPosition, zPosition);
		m_pObject[i]->SetPosition(XMFLOAT3(xPosition, fHeight, zPosition));
		m_pObject[i]->SetOOBB(m_pObject[i]->GetPosition(), XMFLOAT3(1, 1, 1), XMFLOAT4(0, 0, 0, 1));
		
	}
	for (int i = 0; i < NUM_OBJECT2; ++i) {
		float xPosition;
		float zPosition;

		if (i == 0) xPosition = 230, zPosition = 280;
		else if (i == 1) xPosition = 140, zPosition = 270;
		else if (i == 2) xPosition = 200, zPosition = 200;
		else if (i == 3) xPosition = 210, zPosition = 340;
		else if (i == 4) xPosition = 250, zPosition = 230;
		else if (i == 5) xPosition = 170, zPosition = 370;
		else if (i == 6) xPosition = 330, zPosition = 150;

		else if (i == 7) xPosition = 412, zPosition = 310;
		else if (i == 8) xPosition = 526, zPosition = 290;
		else if (i == 9) xPosition = 342, zPosition = 190;
		else if (i == 10) xPosition = 643, zPosition = 240;
		else if (i == 11) xPosition = 332, zPosition = 360;
		else if (i == 12) xPosition = 173, zPosition = 335;
		else if (i == 13) xPosition = 642, zPosition = 420;

		else if (i == 14) xPosition = 627, zPosition = 700;
		else if (i == 15) xPosition = 522, zPosition = 550;
		else if (i == 16) xPosition = 380, zPosition = 710;
		else if (i == 17) xPosition = 270, zPosition = 860;
		else if (i == 18) xPosition = 190, zPosition = 645;
		else if (i == 19) xPosition = 320, zPosition = 550;
		
		float fHeight = m_pScene->GetTerrain()->GetHeight(xPosition, zPosition);
		m_pObject2[i]->SetPosition(XMFLOAT3(xPosition, fHeight, zPosition));

	}
#endif

	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	for (int i = 0; i < MAX_PLAYER_SIZE; ++i)
		if (m_pPlayer[i]) m_pPlayer[i]->ReleaseUploadBuffers();
	for (int i = 0; i < NUM_OBJECT; ++i)
		if (m_pObject[i]) m_pObject[i]->ReleaseUploadBuffers();
	for (int i = 0; i < NUM_OBJECT2; ++i)
		if (m_pObject2[i]) m_pObject2[i]->ReleaseUploadBuffers();
	if (m_pBlueBox[0]) m_pBlueBox[0]->ReleaseUploadBuffers();

	if (m_pScene) m_pScene->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i)
		if (m_pPlayer[i]) delete m_pPlayer[i];
	for (int i = 0; i < NUM_OBJECT; ++i)
		if (m_pObject[i]) delete m_pObject[i];
	for (int i = 0; i < NUM_OBJECT2; ++i)
		 if (m_pObject2[i]) delete m_pObject2[i];
	if (m_pBlueBox[0])delete m_pBlueBox[0];
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
}
void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pScene) bProcessedByScene = m_pScene->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene && gameMode != 2 && alphaMapOn == false)
	{
		DWORD dwDirection = 0;
		// 플레이어 움직임 (중요)
		//if (pKeysBuffer[0x57] & 0xF0) dwDirection |= DIR_FORWARD;
		//if (pKeysBuffer[0x53] & 0xF0) dwDirection |= DIR_BACKWARD;
		//if (pKeysBuffer[0x41] & 0xF0) dwDirection |= DIR_LEFT;
		//if (pKeysBuffer[0x44] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		/*if (pKeysBuffer[VK_SPACE] & 0xF0) {	// 총알발사
		if (CShader::shootBullet == 0)
		CShader::shootBullet = 1;
		}
		else
		CShader::shootBullet = 0;*/

		float cxDelta = 0.0f, cyDelta = 0.0f;

		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (m_pCamera->GetMode() == FIRST_PERSON_CAMERA) {
				if (cxDelta || cyDelta)
				{
					if (pKeysBuffer[VK_RBUTTON] & 0xF0)

						;// m_pPlayer[my_client_id]->Rotate(cyDelta, 0.0f, -cxDelta);
					else
						for (int i = 0; i < MAX_PLAYER_SIZE; ++i)
							if (i == my_client_id)
								m_pPlayer[i]->Rotate(cyDelta, cxDelta, 0.0f);
				}
				if (dwDirection) {
					for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
						// 중요
						//if(i == my_client_id)
							//m_pPlayer[i]->Move(dwDirection, WALK_SPEED * METER_PER_PIXEL * m_GameTimer.GetTimeElapsed(), false);
					}
				}
			}
			else {
				if (cxDelta || cyDelta)
				{
					//줌 회전
					if (pKeysBuffer[VK_RBUTTON] & 0xF0)
						 m_pPlayer[my_client_id]->Rotate(cyDelta, cxDelta, 0.0f);
					else
						;// m_pPlayer[my_client_id]->Rotate(cyDelta, 0.0f, -cxDelta);
				}
				if (dwDirection) {
					for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
						// 중요
						//if (i == my_client_id)
							//m_pPlayer[i]->Move(dwDirection, WALK_SPEED * METER_PER_PIXEL * m_GameTimer.GetTimeElapsed(), false);
					}
				}
			}
		}
	}
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		m_pPlayer[i]->Update(m_GameTimer.GetTimeElapsed());
	}
	
}

void CGameFramework::AnimateObjects(CCamera *pCamera)
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		if (m_pPlayer) m_pPlayer[i]->Animate(fTimeElapsed);
		if (i == my_client_id)
		{
			;
		}
		else if (i != my_client_id) {
			m_pPlayer[i]->rrrotate((atan2(m_pPlayer[i]->LookTemp.z, m_pPlayer[i]->LookTemp.x)));
		}
	}

	//애니메이트
	for (int i = 0; i < NUM_OBJECT; ++i)
		if (m_pObject) m_pObject[i]->Animate(fTimeElapsed);
	for (int i = 0; i < NUM_OBJECT2; ++i)
		if (m_pObject2) m_pObject2[i]->Animate(fTimeElapsed,i);
	if (m_pBlueBox[0])m_pBlueBox[0]->Animate(fTimeElapsed);
	if (m_pScene) m_pScene->AnimateObjects(fTimeElapsed, pCamera);

	bool dummy_bool;
	server_mgr.ReturnCollsionPosition(&is_collide);
	if (is_collide)
		collide_frame = 0;
	if (collide_frame < 15) {
		m_pScene->m_ppShaders[3]->SetPosition(0, XMFLOAT3(server_mgr.ReturnCollsionPosition(&dummy_bool).x,
			server_mgr.ReturnCollsionPosition(&dummy_bool).y + 70.f, server_mgr.ReturnCollsionPosition(&dummy_bool).z));
		collide_frame++;
		//printf("collide_frame : %d \n", collide_frame);
	}
	else {
		m_pScene->m_ppShaders[3]->SetPosition(0, XMFLOAT3(-1000.f,
			-1000.f, -1000.f));
	}

	//if ((server_mgr.ReturnCollsionPosition(&is_collide).x != 0.0)) {
	//	m_pScene->m_ppShaders[3]->SetPosition(0, XMFLOAT3(server_mgr.ReturnCollsionPosition(&is_collide).x,
	//		server_mgr.ReturnCollsionPosition(&is_collide).y + 70.f, server_mgr.ReturnCollsionPosition(&is_collide).z));
	//	collide_frame++;
	//	//- 10 * m_pPlayer[my_client_id]->GetLook().z
	//	//printf("좌표변경!");
	//}
}

void CGameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//m_nSwapChainBufferIndex = (m_nSwapChainBufferIndex + 1) % m_nSwapChainBuffers;

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP

void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f);

	ProcessInput();

	AnimateObjects(m_pCamera);

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor/*Colors::Azure*/, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	m_pScene->Render(m_pd3dCommandList, m_pCamera);


#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		m_pPlayer[i]->UpdateTransform(NULL);
		if (i == my_client_id && m_pCamera->GetMode() == SPACESHIP_CAMERA);
		else {
			/*if(i != my_client_id)
				m_pPlayer[i]->SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));*/
			m_pPlayer[i]->Render(m_pd3dCommandList, m_pCamera);
		}
	}

	for (int i = 0; i < NUM_OBJECT; ++i) {
		m_pObject[i]->UpdateTransform(NULL);
		m_pObject[i]->SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
		if(server_mgr.GetTreeInuse(i) == true)
			m_pObject[i]->Render(m_pd3dCommandList, 1, m_pCamera);
	}
	for (int i = 0; i < NUM_OBJECT2; ++i) {
		m_pObject2[i]->UpdateTransform(NULL);
		m_pObject2[i]->SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
		m_pObject2[i]->Render(m_pd3dCommandList, m_pCamera);
	}
	m_pBlueBox[0]->UpdateTransform(NULL);
	m_pBlueBox[0]->SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_pBlueBox[0]->Render(m_pd3dCommandList, m_pCamera);
	
	if (gameMode == 0) {
		m_pScene->m_ppMainUIShaders[0]->Render(m_pd3dCommandList, m_pCamera); // 메인화면
		if (mainScreenSelect == 1)
			m_pScene->m_ppMainUIShaders[1]->Render(m_pd3dCommandList, m_pCamera); // 메인화면 선택창
		if (mainScreenSelect == 2)
			m_pScene->m_ppMainUIShaders[2]->Render(m_pd3dCommandList, m_pCamera); // 메인화면 선택창
	}
	else if (gameMode == 1 || gameMode == 2) { // UI 렌더
		m_pScene->m_ppUIShaders[0]->Render(m_pd3dCommandList, m_pCamera); // 미니맵

																		  //printf("%f", playerHp);
		m_pScene->m_ppUIShaders[2]->Render(m_pd3dCommandList, m_pCamera, playerHp);
		m_pScene->m_ppUIShaders[3]->Render(m_pd3dCommandList, m_pCamera);//아이템 검은색
		for (int i = 0; i < 4; ++i) {
			if (itemUI[i] == true)
				m_pScene->m_ppUIShaders[i + 4]->Render(m_pd3dCommandList, m_pCamera);
		}

		if (itemUI[3] == true)
			m_pScene->m_ppUIShaders[8]->Render(m_pd3dCommandList, m_pCamera);

		m_pScene->m_ppUIShaders[10]->Render(m_pd3dCommandList, m_pCamera); // 총
		//m_pScene->m_ppUIShaders[11]->Render(m_pd3dCommandList, m_pCamera); // 총

		if (m_pCamera->GetMode() == SPACESHIP_CAMERA)
			m_pScene->m_ppUIShaders[1]->Render(m_pd3dCommandList, m_pCamera);// UI렌더 바꿔야함.

		if (alphaMapOn == true) {
			//m_pCamera->SetLook(XMFLOAT3(m_pCamera->GetLookVector().x, 0, m_pCamera->GetLookVector().z));
			m_pCamera->SetLook(XMFLOAT3(1, 0, 0));
			m_pScene->m_ppUIShaders[9]->Render(m_pd3dCommandList, m_pCamera); // 맵
			m_pScene->m_ppShaders[2]->Render(m_pd3dCommandList, m_pCamera); // 맵에 현재 위치
		}
		else
			m_pCamera->SetLook(XMFLOAT3(m_pPlayer[my_client_id]->GetLook().x, m_pCamera->GetLookVector().y, m_pPlayer[my_client_id]->GetLook().z));

		

																			  // 숫자 시작
		if (m_pPlayer[my_client_id]->GetPlayerBullet() / 10 > 0)
			m_pScene->m_ppUIShaders[11 + m_pPlayer[my_client_id]->GetPlayerBullet() / 10]->Render(m_pd3dCommandList, m_pCamera); // 앞 숫자
		if (m_pPlayer[my_client_id]->GetPlayerBullet() > 0)
			m_pScene->m_ppUIShaders[16 + m_pPlayer[my_client_id]->GetPlayerBullet() % 10]->Render(m_pd3dCommandList, m_pCamera); // 뒷 숫자
		
		if(blueScreenMode)
			m_pScene->m_ppUIShaders[26]->Render(m_pd3dCommandList, m_pCamera);
		
		

	}
	
	if (playerHp < 1) {
		gameMode = 2;
		m_pPlayer[my_client_id]->ImDie();
	}

	if(gameMode == 2)
		m_pScene->m_ppMainUIShaders[3]->Render(m_pd3dCommandList, m_pCamera);//게임오버 화면

	m_pScene->m_ppUIShaders[27]->Render(m_pd3dCommandList, m_pCamera);
	// 렌더
	//printf("%f %f %f \n", m_pPlayer[0]->GetPosition().x, m_pPlayer[0]->GetPosition().y, m_pPlayer[0]->GetPosition().z);

	m_pBlueBox[0]->SetBoxScale(server_mgr.GetElecCount());

	// 자기장 충돌체크
	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		ContainmentType containType = CGameFramework::m_pPlayer[CGameFramework::my_client_id]->bounding_box.Contains(m_pBlueBox[0]->bounding_box);
		switch (containType)
		{
		case DISJOINT:
		{
			blueScreenMode = true;
			playerHp -= 0.01;
			break;
		}
		case INTERSECTS:
		{
			blueScreenMode = false;
			break;
		}
		case CONTAINS:
		{
			blueScreenMode = true;
			break;
			break;
		}
		}
	}
	////

	/////// 오브젝트 충돌체크
	
	bool check = false;
	//충돌체크
	for (int i = 0; i < NUM_OBJECT; ++i) {
		if (server_mgr.GetTreeInuse(i)) {
			ContainmentType containType = CGameFramework::m_pPlayer[CGameFramework::my_client_id]->bounding_box.Contains(m_pObject[i]->bounding_box);
			switch (containType)
			{
			case DISJOINT:
			{
				break;
			}
			case INTERSECTS:
			{
				printf("오브젝트충돌예에\n");
				if ((m_pObject[i]->GetPosition().x - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().x) * (m_pObject[i]->GetPosition().x - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().x)
					< (m_pObject[i]->GetPosition().z - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().z) * (m_pObject[i]->GetPosition().z - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().z)) {
					if (m_pObject[i]->GetPosition().z - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().z > 0) { m_pObject[i]->look = XMFLOAT3(0, 0, -1); }
					else { m_pObject[i]->look = XMFLOAT3(0, 0, 1); }
				}
				else {
					if (m_pObject[i]->GetPosition().x - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().x > 0) { m_pObject[i]->look = XMFLOAT3(-1, 0, 0); }
					else { m_pObject[i]->look = XMFLOAT3(1, 0, 0); }
				}
				XMFLOAT3 xmf3Result;
				XMFLOAT3 xmf3Result_1;
				XMFLOAT3 xmf3Result_2;
				XMStoreFloat3(&xmf3Result_1, XMVector3Dot(XMLoadFloat3(&CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetLook()), XMLoadFloat3(&m_pObject[i]->look)));
				XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&m_pObject[i]->look), XMLoadFloat3(&xmf3Result_1)));
				xmf3Result_2 = XMFLOAT3(Vector3::Subtract(CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetLook(), xmf3Result));
				CGameFramework::sendLook = XMFLOAT3(2 * xmf3Result_2.x / 3, 2 * xmf3Result_2.y / 3, 2 * xmf3Result_2.z / 3);
				check = true;
				break;
			}
			case CONTAINS:

				break;
			}
		}
	}
	if (check == true)
		CGameFramework::boxBound = 1;
	/////////

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = m_pd3dCommandList->Close();

	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters;
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = NULL;
	dxgiPresentParameters.pScrollRect = NULL;
	dxgiPresentParameters.pScrollOffset = NULL;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
#ifdef _WITH_SYNCH_SWAPCHAIN
	m_pdxgiSwapChain->Present(1, 0);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif
#endif

	//	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	MoveToNextFrame();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
	//for(int i=0;i<50;++i)
	//	printf("%d\t", m_pszFrameRate[i]);
}

