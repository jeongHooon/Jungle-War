#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Object.h"
#include "Camera.h"
enum PlayerType
{
	Player,
	Tree,
	Rock,
	Blue
};
struct CB_PLAYER_INFO
{
	XMFLOAT4X4					m_xmf4x4World;
};

class CPlayer : public CGameObject
{
protected:
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;

	LPVOID						m_pPlayerUpdatedContext = NULL;
	LPVOID						m_pCameraUpdatedContext = NULL;

	float						playerHp = 100.0;
	int							playerBullet = 10;
	int							treeNum = 10;

	CCamera						*m_pCamera = NULL;
	int animation_status = 0;
	PlayerType PT;
	bool isShot = false;
	float shotTime = 0.0f;
public:
	float dieTime = 0;
	bool gameend = false;
	bool isDie = false;
	float time = 0;

	void ActiveShot() { 
		isShot = true; shotTime = 0.0f;
		//if (isShot == false) {
		//	isShot = true; shotTime = 0.0f;
		//}
		//else {
		//	isShot = false;
		//}
	}
	void InitAnimation(int num) { NewMD5Model.animations[num].currAnimTime = 0; }
	CPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext=NULL, int nMeshes = 1);
	
	XMFLOAT3 LookTemp;
	void SetLookTemp(XMFLOAT3 xmf3Look) { LookTemp = xmf3Look; }
	XMFLOAT3 GetLookTempVector() { return(LookTemp); }
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetWPosition() { return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3 GetCameraLook();
	void GetKeyInput(int key);
	int GetAnimationState() { return animation_status; }
	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }
	void SetLook(XMFLOAT3 xmf3Look) { m_xmf3Look = xmf3Look; }
	virtual void rrrotate(float deg);
	void ImDie() { isDie = true; }
	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }
	int	  GetPlayerBullet() { return(playerBullet); }
	int   GetPlayerTree() { return(treeNum); }
	void  MinusPlayerBullet() { --playerBullet; }
	void  PlusPlayerBullet() { playerBullet += 8; if (playerBullet > 40) playerBullet = 40; }
	int GetPlayerHp() { return(playerHp); }
	void SetPlayerHp(float input) { playerHp = input; }
	virtual void SetBoxScale(int input) {}


	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	//void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	void Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	virtual void Rotate(float x, float y, float z);

	void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual ID3D12Resource *CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstances , CCamera *pCamera = NULL);
	virtual void Animate(float fTimeElapsed);
	virtual void Animate(float fTimeElapsed, int num);
	BoundingOrientedBox bounding_box;
	void SetOOBB(XMFLOAT3 xmCenter, XMFLOAT3 xmExtents, XMFLOAT4 xmOrientation) {
		bounding_box = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
	}

protected:
	ID3D12Resource					*m_pd3dcbPlayer = NULL;
	CB_PLAYER_INFO					*m_pcbMappedPlayer = NULL;
};

class CAirplanePlayer : public CPlayer
{
public:
	CAirplanePlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext=NULL, int nMeshes=1);
	virtual ~CAirplanePlayer();

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};
class CTreeObject : public CPlayer
{
public:
	CTreeObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL, int nMeshes = 1);
	virtual ~CTreeObject();
	void Animate(float fTimeElapsed);
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};
class CRockObject : public CPlayer
{
public:
	CRockObject(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext = NULL, int nMeshes = 1);
	virtual ~CRockObject();
	void Animate(float fTimeElapsed, int num);
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};
class CBlueBox : public CPlayer
{
private:
	int count = 0;
	int elecCount_;
public:
	CBlueBox(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, void * pContext = NULL, int nMeshes = 1);
	virtual ~CBlueBox();
	virtual void SetBoxScale(int input);
	void Animate(float fTimeElapsed);
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};
class CPrevBox : public CPlayer
{
private:
	int count = 0;
public:
	CPrevBox(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CPrevBox();
	virtual void SetBoxScale(int input);
	void Animate(float fTimeElapsed);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};
class CShadow : public CPlayer
{
public:
	CShadow(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CShadow();
	void Animate(float fTimeElapsed, int num, XMFLOAT4X4 worldMt);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void rrrotate(float deg);
	virtual void Rotate(float x, float y, float z);
	CHeightMapTerrain* pTerrain;
	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};
class CShadowTree : public CPlayer
{
public:
	CShadowTree(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CShadowTree();
	void Animate(float fTimeElapsed, int num, XMFLOAT4X4 worldMt);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void rrrotate(float deg);
	virtual void Rotate(float x, float y, float z);
	CHeightMapTerrain* pTerrain;
	
	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};
class CShadowRock : public CPlayer
{
public:
	CShadowRock(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL, int nMeshes = 1);
	virtual ~CShadowRock();
	void Animate(float fTimeElapsed, int num, XMFLOAT4X4 worldMt);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);
	virtual CCamera* ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
	virtual void rrrotate(float deg);
	virtual void Rotate(float x, float y, float z);
	CHeightMapTerrain* pTerrain;

	/*virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);*/
};