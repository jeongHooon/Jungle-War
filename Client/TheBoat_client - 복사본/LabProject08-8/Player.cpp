//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "ServerMgr.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

#define BluBoxSpeed 0.01

int ServerMgr::elecCount;
XMFLOAT3 ServerMgr::elecPos;

CPlayer::CPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, int nMeshes) : CGameObject(nMeshes)
{
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;
	
}

void CPlayer::GetKeyInput(int key) {
	//printf("[애니메이션] : %d키 누름\n",key);
	
	//check
	//NewMD5Model.animations[key].currAnimTime = 0;
	animation_status = key;
	if (animation_status == 17)
		isDie = true;
}


CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

ID3D12Resource *CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbPlayer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbPlayer->Map(0, NULL, (void **)&m_pcbMappedPlayer);

	return(m_pd3dcbPlayer);
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();

	if (m_pd3dcbPlayer)
	{
		m_pd3dcbPlayer->Unmap(0, NULL);
		m_pd3dcbPlayer->Release();
	}

	CGameObject::ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	XMStoreFloat4x4(&m_pcbMappedPlayer->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = m_pd3dcbPlayer->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(ROOT_PARAMETER_PLAYER, d3dGpuVirtualAddress);
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		m_pCamera->Move(xmf3Shift);
	}
}

XMFLOAT3 CPlayer::GetCameraLook() {
	return m_pCamera->GetLookVector();
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			
			//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToDegrees(atan2(m_pCamera->GetLookVector().x, m_pCamera->GetLookVector().z)), 0.0f);
			//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), 90.f, 0.0f);
			//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
			///캐릭터 회전XMMatrixRotationRollPitchYaw
			//XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			XMMATRIX xmmtxRotate = XMMatrixRotationRollPitchYaw(0.0f,XMConvertToRadians(y),0.0f);
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));
	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * fTimeElapsed;
	//float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	//float fMaxVelocityXZ = m_fMaxVelocityXZ ;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}
	float fMaxVelocityY = m_fMaxVelocityY * fTimeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	//카메라 위치 조정
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, fTimeElapsed);
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	if (nCurrentCameraMode == FIRST_PERSON_CAMERA) {
		if(!isDie)
			m_pCamera->SetPosition(XMFLOAT3(GetPosition().x - 10 * GetLookVector().x, GetPosition().y + 13, GetPosition().z - 10 * GetLookVector().z));	// 1인칭 카메라 움직이기
		else {
			m_pCamera->SetPosition(XMFLOAT3(GetPosition().x - 120 * GetLookVector().x, GetPosition().y + 40, GetPosition().z - 120 * GetLookVector().z));	// 1인칭 카메라 움직이기
			//m_pCamera->SetLookAt(XMFLOAT3(0, -1, 0));
		}
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA) m_pCamera->SetPosition(XMFLOAT3(GetPosition().x, GetPosition().y + 15, GetPosition().z));	// 줌 카메라 움직이기

	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));

}

CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::Animate(float fTimeElapsed)
{
	if (!gameend) {
		if (isShot) {

			for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
				UpdateMD5Model(NewMD5Model, fTimeElapsed *0.85, 2, m_ppMeshes[i], i);
			shotTime += fTimeElapsed;
		}
		else if (isDie) {
			for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
				UpdateMD5Model(NewMD5Model, fTimeElapsed *0.4, 17, m_ppMeshes[i], i);
			dieTime += fTimeElapsed;
		}
		else {
			for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
				UpdateMD5Model(NewMD5Model, fTimeElapsed *0.85, animation_status, m_ppMeshes[i], i);
		}
	}
	if (shotTime > 0.5) {
		shotTime = 0.0f;
		isShot = false;
	}
	if (dieTime > 2.0) {
		shotTime = 0.0f;
		gameend = true;
	}

	m_xmf4x4ToParentTransform._11 = m_xmf3Right.x; m_xmf4x4ToParentTransform._12 = m_xmf3Right.y; m_xmf4x4ToParentTransform._13 = m_xmf3Right.z;
	m_xmf4x4ToParentTransform._21 = m_xmf3Up.x; m_xmf4x4ToParentTransform._22 = m_xmf3Up.y; m_xmf4x4ToParentTransform._23 = m_xmf3Up.z;
	m_xmf4x4ToParentTransform._31 = m_xmf3Look.x; m_xmf4x4ToParentTransform._32 = m_xmf3Look.y; m_xmf4x4ToParentTransform._33 = m_xmf3Look.z;
	m_xmf4x4ToParentTransform._41 = m_xmf3Position.x; m_xmf4x4ToParentTransform._42 = m_xmf3Position.y; m_xmf4x4ToParentTransform._43 = m_xmf3Position.z;

	SetOOBB(GetPosition(), XMFLOAT3(3, 6, 3), XMFLOAT4(0, 0, 0, 1));

	//// 캐릭터 회전
	time += fTimeElapsed;
	//if(time> PI*2)
	//	time = 0;
	//printf("%f \n", XMConvertToDegrees(atan2(m_pCamera->GetLookVector().z, m_pCamera->GetLookVector().x)));
	//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToDegrees(atan2(m_pCamera->GetLookVector().z, m_pCamera->GetLookVector().x)), 0.0f);

	//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), time, 0.0f);
	//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}
void CPlayer::Animate(float fTimeElapsed, int num)
{
	m_xmf4x4ToParentTransform._11 = m_xmf3Right.x; m_xmf4x4ToParentTransform._12 = m_xmf3Right.y; m_xmf4x4ToParentTransform._13 = m_xmf3Right.z;
	m_xmf4x4ToParentTransform._21 = m_xmf3Up.x; m_xmf4x4ToParentTransform._22 = m_xmf3Up.y; m_xmf4x4ToParentTransform._23 = m_xmf3Up.z;
	m_xmf4x4ToParentTransform._31 = m_xmf3Look.x; m_xmf4x4ToParentTransform._32 = m_xmf3Look.y; m_xmf4x4ToParentTransform._33 = m_xmf3Look.z;
	m_xmf4x4ToParentTransform._41 = m_xmf3Position.x; m_xmf4x4ToParentTransform._42 = m_xmf3Position.y; m_xmf4x4ToParentTransform._43 = m_xmf3Position.z;


	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(num * 5, 90 - num * 5, 0.0f);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);

	if (playerHp < 0) {
		isDie = true;
	}
}
void CPlayer::rrrotate(float deg)
{

	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), -(deg - PI / 2));
	//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(0.0f, -(deg - PI/2), 0.0f);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CPlayer::OnPrepareRender()
{
}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;CGameObject::Render(pd3dCommandList, pCamera);
}
void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstances, CCamera *pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00; CGameObject::Render(pd3dCommandList, pCamera, nInstances);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAirplanePlayer

CAirplanePlayer::CAirplanePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, int nMeshes) : CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Flyer.txt");
	PT = Player;
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);
	
}

CAirplanePlayer::~CAirplanePlayer()
{
}

CCamera *CAirplanePlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}
CTreeObject::CTreeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, int nMeshes) : CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	PT = Tree;
	LoadGeometryFromFile2(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Flyer.txt");

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);

}

CTreeObject::~CTreeObject()
{
}

void CTreeObject::Animate(float fTimeElapsed)
{
	/*for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
		UpdateMD5Model(NewMD5Model, fTimeElapsed *0.4, 0, m_ppMeshes[i], i);
	*/

	m_xmf4x4ToParentTransform._11 = m_xmf3Right.x; m_xmf4x4ToParentTransform._12 = m_xmf3Right.y; m_xmf4x4ToParentTransform._13 = m_xmf3Right.z;
	m_xmf4x4ToParentTransform._21 = m_xmf3Up.x; m_xmf4x4ToParentTransform._22 = m_xmf3Up.y; m_xmf4x4ToParentTransform._23 = m_xmf3Up.z;
	m_xmf4x4ToParentTransform._31 = m_xmf3Look.x; m_xmf4x4ToParentTransform._32 = m_xmf3Look.y; m_xmf4x4ToParentTransform._33 = m_xmf3Look.z;
	m_xmf4x4ToParentTransform._41 = m_xmf3Position.x; m_xmf4x4ToParentTransform._42 = m_xmf3Position.y; m_xmf4x4ToParentTransform._43 = m_xmf3Position.z;

	//SetOOBB(GetPosition(), XMFLOAT3(5, 10, 5), XMFLOAT4(0, 0, 0, 1));
	/*time += fTimeElapsed;
	if (time > 360.f)
		time = 0;*/
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), time, 0.0f);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

CCamera *CTreeObject::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}
//void CAirplanePlayer::OnPlayerUpdateCallback(float fTimeElapsed)
//{
//	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pPlayerUpdatedContext;
//	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
//	XMFLOAT3 xmf3PlayerPosition = GetPosition();
//	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
//	bool bReverseQuad = ((z % 2) != 0);
//	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad) + 6.0f;
//	
//	XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
//	xmf3PlayerVelocity.y = 0.0f;
//	SetVelocity(xmf3PlayerVelocity);
//	xmf3PlayerPosition.y = fHeight;
//	SetPosition(xmf3PlayerPosition);
//}
//
//void CAirplanePlayer::OnCameraUpdateCallback(float fTimeElapsed)
//{
//	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pCameraUpdatedContext;
//	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
//	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
//	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
//	bool bReverseQuad = ((z % 2) != 0);
//	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z, bReverseQuad) + 5.0f;
//	xmf3CameraPosition.y = fHeight;
//	m_pCamera->SetPosition(xmf3CameraPosition);
//	if (m_pCamera->GetMode() == FIRST_PERSON_CAMERA)
//	{
//		CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)m_pCamera;
//		p3rdPersonCamera->SetLookAt(GetPosition());
//	}
//}

CRockObject::CRockObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext, int nMeshes) : CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	PT = Rock;
	LoadGeometryFromFile4(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Flyer.txt");

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);

}

CRockObject::~CRockObject()
{
}

void CRockObject::Animate(float fTimeElapsed, int num)
{
	/*for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
		UpdateMD5Model(NewMD5Model, fTimeElapsed *0.4, 0, m_ppMeshes[i], i);
	*/

	m_xmf4x4ToParentTransform._11 = m_xmf3Right.x; m_xmf4x4ToParentTransform._12 = m_xmf3Right.y; m_xmf4x4ToParentTransform._13 = m_xmf3Right.z;
	m_xmf4x4ToParentTransform._21 = m_xmf3Up.x; m_xmf4x4ToParentTransform._22 = m_xmf3Up.y; m_xmf4x4ToParentTransform._23 = m_xmf3Up.z;
	m_xmf4x4ToParentTransform._31 = m_xmf3Look.x; m_xmf4x4ToParentTransform._32 = m_xmf3Look.y; m_xmf4x4ToParentTransform._33 = m_xmf3Look.z;
	m_xmf4x4ToParentTransform._41 = m_xmf3Position.x; m_xmf4x4ToParentTransform._42 = m_xmf3Position.y; m_xmf4x4ToParentTransform._43 = m_xmf3Position.z;


	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(num * 5, 90 - num*5, 0.0f);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

CCamera *CRockObject::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}


CBlueBox::CBlueBox(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void * pContext, int nMeshes) : CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	PT = Blue;
	LoadGeometryFromFile3(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Flyer.txt");

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);
}

CBlueBox::~CBlueBox()
{
}

void CBlueBox::Animate(float fTimeElapsed)
{
	m_xmf4x4ToParentTransform._11 = m_xmf3Right.x; m_xmf4x4ToParentTransform._12 = m_xmf3Right.y; m_xmf4x4ToParentTransform._13 = m_xmf3Right.z;
	m_xmf4x4ToParentTransform._21 = m_xmf3Up.x; m_xmf4x4ToParentTransform._22 = m_xmf3Up.y; m_xmf4x4ToParentTransform._23 = m_xmf3Up.z;
	m_xmf4x4ToParentTransform._31 = m_xmf3Look.x; m_xmf4x4ToParentTransform._32 = m_xmf3Look.y; m_xmf4x4ToParentTransform._33 = m_xmf3Look.z;
	m_xmf4x4ToParentTransform._41 = m_xmf3Position.x; m_xmf4x4ToParentTransform._42 = m_xmf3Position.y; m_xmf4x4ToParentTransform._43 = m_xmf3Position.z;

	/*time += fTimeElapsed;
	if (time > 360.f)
	time = 0;*/
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), time, 0.0f);
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
	if (0.05 * ServerMgr::elecCount < 1100)
		elecCount_ = ServerMgr::elecCount;

	SetScale(1200 - 0.02 * elecCount_, 5000, 1200 - 0.02 * elecCount_);
	SetOOBB(ServerMgr::elecPos, XMFLOAT3((1200 - 0.02 * elecCount_) / 2, 5000, (1200 - 0.02 * elecCount_) / 2), XMFLOAT4(0, 0, 0, 1));
	SetPosition(ServerMgr::elecPos);
}

void CBlueBox::SetBoxScale(int input) {
}

CCamera * CBlueBox::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}

CShadow::CShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext, int nMeshes) : CPlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, pContext, nMeshes)
{
	m_pCamera = ChangeCamera(FIRST_PERSON_CAMERA, 0.0f);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	PT = Player;
	//LoadGeometryFromFile2(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Flyer.txt");
	CMesh* pMesh = NULL;
	CMesh* pMesh1 = NULL;

	ResizeMeshes(2);


	LoadMD5Model(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Soldier_Mesh.MD5MESH", NewMD5Model, meshSRV, textureNameArray, pMesh);
	SetMesh(0, pMesh);
	//AddRef();
	LoadMD5Model(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, L"../Assets/Model/Soldier_Rifle.MD5MESH", NewMD5Model, meshSRV, textureNameArray, pMesh1);
	SetMesh(1, pMesh1);
	LoadMD5Anim(L"../Assets/Model/1241.MD5ANIM", NewMD5Model);//0
	LoadMD5Anim(L"../Assets/Model/Soldier_Run.MD5ANIM", NewMD5Model);//1
	LoadMD5Anim(L"../Assets/Model/Soldier_Shot.MD5ANIM", NewMD5Model);//2
	LoadMD5Anim(L"../Assets/Model/Soldier_WalkFront.MD5ANIM", NewMD5Model);//3
	LoadMD5Anim(L"../Assets/Model/Soldier_WalkBack.MD5ANIM", NewMD5Model);//4
	LoadMD5Anim(L"../Assets/Model/Soldier_WalkRight.MD5ANIM", NewMD5Model);//5
	LoadMD5Anim(L"../Assets/Model/Soldier_WalkLeft.MD5ANIM", NewMD5Model);//6
	LoadMD5Anim(L"../Assets/Model/Soldier_Crouch.MD5ANIM", NewMD5Model);//7

	LoadMD5Anim(L"../Assets/Model/wbr.MD5ANIM", NewMD5Model);//8
	LoadMD5Anim(L"../Assets/Model/wbl.MD5ANIM", NewMD5Model);//9
	LoadMD5Anim(L"../Assets/Model/RunBack.MD5ANIM", NewMD5Model);//10
	LoadMD5Anim(L"../Assets/Model/RunBackLeft.MD5ANIM", NewMD5Model);//11
	LoadMD5Anim(L"../Assets/Model/RunBackRight.MD5ANIM", NewMD5Model);//12
	LoadMD5Anim(L"../Assets/Model/RunLeft.MD5ANIM", NewMD5Model);//13
	LoadMD5Anim(L"../Assets/Model/RunRight.MD5ANIM", NewMD5Model);//14
	LoadMD5Anim(L"../Assets/Model/jumpM.MD5ANIM", NewMD5Model);//15
	LoadMD5Anim(L"../Assets/Model/takedamage.MD5ANIM", NewMD5Model);//16
	LoadMD5Anim(L"../Assets/Model/die.MD5ANIM", NewMD5Model);//17
	LoadMD5Anim(L"../Assets/Model/jump3.MD5ANIM", NewMD5Model);//7
	//LoadMD5Anim(L"../Assets/Model/CrouchShot.MD5ANIM", NewMD5Model); //15
	//LoadMD5Anim(L"../Assets/Model/CrouchWalk.MD5ANIM", NewMD5Model); //16



	m_pMaterial = new CMaterial();
	CTexture* pTexture = new CTexture(2, RESOURCE_TEXTURE2D, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/demo_soldier.dds", 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Model/warrior.dds", 1);


	m_pMaterial->SetTexture(pTexture);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	ID3D12Resource* pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);


	CTexturedShader* pShader = new CTexturedShader();
	pShader->isShadow = true;
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2);
	//pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, m_pd3dcbGameObject, ncbElementBytes);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	m_pMaterial->SetShader(pShader);
	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR toMainLight = -XMLoadFloat3(new XMFLOAT3(-0.3f, -1.0f, 0.0f));
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffSetY = XMMatrixTranslation(0.0f, 0.003f, 0.0f);
	MTShadow = S * shadowOffSetY;

	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;
	SetPlayerUpdatedContext(pTerrain);
	SetCameraUpdatedContext(pTerrain);
}

CShadow::~CShadow()
{
}

void CShadow::Animate(float fTimeElapsed, int num, XMFLOAT4X4 worldMt)
{	
	if (!m_bActive) return;
	//SetWMatrix(Matrix4x4::Multiply(worldMt, MTShadow));
	SetWMatrix(Matrix4x4::Multiply(worldMt, XMMatrixIdentity()));
	m_xmf4x4ToParentTransform._11 = m_xmf3Right.x; m_xmf4x4ToParentTransform._12 = m_xmf3Right.y; m_xmf4x4ToParentTransform._13 = m_xmf3Right.z;
	m_xmf4x4ToParentTransform._21 = m_xmf3Up.x; m_xmf4x4ToParentTransform._22 = m_xmf3Up.y; m_xmf4x4ToParentTransform._23 = m_xmf3Up.z;
	m_xmf4x4ToParentTransform._31 = m_xmf3Look.x; m_xmf4x4ToParentTransform._32 = m_xmf3Look.y; m_xmf4x4ToParentTransform._33 = m_xmf3Look.z;
	//m_xmf4x4ToParentTransform._43 -= 5;
	/*if (m_nMeshes > 0)
		for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
			UpdateMD5Model(NewMD5Model, fTimeElapsed * 0.85, 0, m_ppMeshes[i], i);*/
	/*else if (m_AnimState == ATTACKSTATE) {
		if ((m_pModels[CurMeshNum].animations[ATTACKSTATE].currAnimTime == 0.f)) m_AnimState = IDLESTATE;
	}*/
	if (!gameend) {
		if (isShot) {

			for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
				UpdateMD5Model2(NewMD5Model, fTimeElapsed * 0.85, 2, m_ppMeshes[i], i);
			shotTime += fTimeElapsed;
		}
		else if (isDie) {
			for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
				UpdateMD5Model2(NewMD5Model, fTimeElapsed * 0.4, 17, m_ppMeshes[i], i);
			dieTime += fTimeElapsed;
		}
		else {
			for (int i = 0; i < NewMD5Model.subsets.size(); ++i)
				UpdateMD5Model2(NewMD5Model, fTimeElapsed * 0.85, animation_status, m_ppMeshes[i], i);
		}
	}
	
	if (shotTime > 0.5) {
		shotTime = 0.0f;
		isShot = false;
	}
	if (dieTime > 2.0) {
		shotTime = 0.0f;
		gameend = true;
	}
	time += fTimeElapsed;
}

void CShadow::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader) {
			m_pMaterial->m_pShader->Render(pd3dCommandList, pCamera);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList);
	}
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
	if (m_nMeshes > 0) for (int i = 0; i < m_nMeshes; i++)
		if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
}

CCamera* CShadow::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -40.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}
	Update(fTimeElapsed);

	return(m_pCamera);
}

void CShadow::rrrotate(float deg)
{
	//XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), -(deg - PI / 2));
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(-90.0f,0.0f, 0.0f);
	//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
}

void CShadow::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{

			//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToDegrees(atan2(m_pCamera->GetLookVector().x, m_pCamera->GetLookVector().z)), 0.0f);
			//XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(0.0f), 90.f, 0.0f);
			//m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);
			///캐릭터 회전XMMatrixRotationRollPitchYaw
			//XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			XMMATRIX xmmtxRotate = XMMatrixRotationRollPitchYaw(-90.0f, XMConvertToRadians(y), 0.0f);
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}
