//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"
#define NUM_OBJECT 90
#define NUM_OBJECT2 20
struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

struct MATERIAL
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4				m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL				m_pReflections[MAX_MATERIALS];
};

class CScene
{
public:
    CScene();
    ~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	void BuildLightsAndMaterials();
	void SetMaterial(int nIndex, MATERIAL *pMaterial);
	void UpdateMaterial(CGameObject *pObject);

	ID3D12RootSignature *CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature *GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	void SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	bool ProcessInput(UCHAR *pKeysBuffer);
    void AnimateObjects(float fTimeElapsed, CCamera *pCamera);
    void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL);

	void ReleaseUploadBuffers();

	CHeightMapTerrain *GetTerrain() { return(m_pTerrain); }

	CPlayer						*m_pPlayer[4];
	CShadow						*m_pShadow[4];
	CPlayer						*m_pObject[NUM_OBJECT]; //오브젝트 갯수
	CShadowTree					*m_pShadowObject[NUM_OBJECT];
	CPlayer						*m_pObject2[NUM_OBJECT2]; //오브젝트 갯수
	CShadowRock					*m_pShadowObject2[NUM_OBJECT2]; //오브젝트 갯수
	CPlayer						*m_pBlueBox[2];
	CPlayer						*m_pPrevBox[1];

	static int					makeParticleIndex;
	static bool					particleOn;
	static XMFLOAT3				particlePosition;

protected:
	ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

	CGameObject					**m_ppObjects = NULL;
	int							m_nObjects = 0;

	CGameObject					**m_ppBullets = NULL;
	int							m_nBullets = 0;
	

public:
	CShader						**m_ppShaders = NULL;
	int							m_nShaders = 0;

	CShader						**m_ppUIShaders = NULL;
	int							m_nUIShaders = 0;

	CShader						**m_ppNumShaders = NULL;
	int							m_nNumShaders = 0;

	CShader						**m_ppMainUIShaders = NULL;
	int							m_nMainUIShaders = 0;

	static CShader				*m_pBuildings;

protected:
	LIGHTS						*m_pLights = NULL;

	ID3D12Resource				*m_pd3dcbLights = NULL;
	LIGHTS						*m_pcbMappedLights = NULL;

	MATERIALS					*m_pMaterials = NULL;
	int							m_nMaterials = 0;

	ID3D12Resource				*m_pd3dcbMaterials = NULL;
	MATERIAL					*m_pcbMappedMaterials = NULL;

	CHeightMapTerrain			*m_pTerrain = NULL;
	CSkyBox						*m_pSkyBox = NULL;

};
