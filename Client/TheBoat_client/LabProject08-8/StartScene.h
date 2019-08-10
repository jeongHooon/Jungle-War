#pragma once
#include "Scene.h"

class StartScene : public CScene
{
private:
	CCamera*             m_pStartCamera;

public:
	StartScene();
	virtual ~StartScene();

	virtual bool Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	virtual void ProcessInput(float fDeltaTime) {};
	virtual void Update(float fDeltaTime, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {
		m_pStartCamera->Update(XMFLOAT3(0,0,0),fDeltaTime);
		m_pStartCamera->RegenerateViewMatrix();
	};
	virtual void EffectUpdate(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {};
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return true; };
	virtual void PacketProcess(char* packet) {};
	virtual void BuildLightsAndMaterials() {}
	virtual void ReleaseUploadBuffers() {}

};