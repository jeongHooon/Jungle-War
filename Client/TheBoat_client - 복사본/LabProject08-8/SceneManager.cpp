/*����� �߰��ϰ���� Scene Ŭ������ �߰��Ѵ�.*/
#include "stdafx.h"
#include "SceneManager.h"
#include "Scene.h"


SceneManager::SceneManager()
{
}


SceneManager::~SceneManager()
{
	m_pScene->ReleaseObjects();
	delete m_pScene;
}

bool SceneManager::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	//CreateScene<Scene>(SC_CURRENT, pd3dDevice, pd3dCommandList);
	//CreateScene<DayForestScene>(SC_NEXT, pd3dDevice, pd3dCommandList);

	return true;
}

void SceneManager::ProcessInput(float fDeltaTime)
{
	if (!nextState)
		;// m_pScene->ProcessInput(fDeltaTime);
	else
		;// m_pNextScene->ProcessInput(fDeltaTime);
}

void SceneManager::Update(float fDeltaTime, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (!nextState)
	{
		;// m_pScene->Update(fDeltaTime, pd3dDevice, pd3dCommandList);

	}
	else
		;// m_pNextScene->Update(fDeltaTime, pd3dDevice, pd3dCommandList);
}

void SceneManager::EffectUpdate(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	if (!nextState)
		;// m_pScene->EffectUpdate(pd3dDevice, pd3dCommandList);
	//else
		//m_pNextScene->Update(fDeltaTime);
}

void SceneManager::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (!nextState)
		m_pScene->Render(pd3dCommandList);
	else
		m_pNextScene->Render(pd3dCommandList);
}

void SceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!nextState)
	{
		if (m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam)) {
			//soundMgr->soundBasePlay();
			nextState = true;
			//CNetWorkManager::GET_SINGLE()->SendStartPacket();
		}
	}
}

void SceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (nextState)
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	else
		m_pNextScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}

void SceneManager::ReleaseUploadBuffers()
{
	if (!nextState)
		m_pScene->ReleaseUploadBuffers();
	else
		m_pNextScene->ReleaseUploadBuffers();
}

void SceneManager::PacketProcess(char * packet)
{
	//m_pNextScene->PacketProcess(packet);
	//m_pScene->PacketProcess(packet);
}

