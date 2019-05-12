//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"
#include "GameFramework.h"

CCamera *CGameFramework::m_pCamera;

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildLightsAndMaterials()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.35f, 0.35f, 0.35f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.58f, 0.58f, 0.58f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
	m_pLights->m_pLights[4].m_bEnable = true;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[4].m_xmf3Direction = XMFLOAT3(1.0f, 1.0f, 1.0f);

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));

	m_pMaterials->m_pReflections[0] ={ XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] ={ XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] ={ XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] ={ XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] ={ XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] ={ XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] ={ XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] ={ XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };

	m_nMaterials = 8;

	for (int i = 0; i < m_nObjects; i++) UpdateMaterial(m_ppObjects[i]);
}

void CScene::UpdateMaterial(CGameObject *pObject)
{
	CMaterial *pMaterial = pObject->m_pMaterial;
	if (pMaterial)
	{
		if (m_nMaterials < MAX_MATERIALS)
		{
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Ambient = pMaterial->m_xmf4Albedo;
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Diffuse = pMaterial->m_xmf4Albedo;
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f);
			m_pMaterials->m_pReflections[m_nMaterials].m_xmf4Emissive = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			pMaterial->m_nReflection = m_nMaterials++;
		}
		else
			pMaterial->m_nReflection = 0;
	}

	if (pObject->m_pSibling) UpdateMaterial(pObject->m_pSibling);
	if (pObject->m_pChild) UpdateMaterial(pObject->m_pChild);
}

void CScene::SetMaterial(int nIndex, MATERIAL *pMaterial)
{
	m_pMaterials->m_pReflections[nIndex] = *pMaterial;
}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	/*m_nObjects = 5;
	m_ppObjects = new CGameObject*[m_nObjects];

#ifdef _WITH_GUNSHIP_MODEL
	m_ppObjects[0] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppObjects[0]->SetPosition(XMFLOAT3(0.0f, 0.0f, 50.0f));
	m_ppObjects[1] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppObjects[1]->SetPosition(XMFLOAT3(-30.0f, 5.0f, 30.0f));
	m_ppObjects[2] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppObjects[2]->SetPosition(XMFLOAT3(+30.0f, 0.0f, 20.0f));
	m_ppObjects[3] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppObjects[3]->SetPosition(XMFLOAT3(+40.0f, 10.0f, 50.0f));
	m_ppObjects[4] = new CGunshipHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppObjects[4]->SetPosition(XMFLOAT3(+40.0f, -10.0f, 30.0f));
#endif
#ifdef _WITH_APACHE_MODEL
	m_ppObjects[0] = new CApacheHellicopter(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppObjects[0]->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_ppObjects[0]->Rotate(0.0f, 90.0f, 0.0f);
#endif*/

	XMFLOAT3 xmf3Scale(8.0f, 2.f, 8.0f);
	XMFLOAT4 xmf4Color(1.0f, 1.0f, 1.0f, 0.0f);
#ifdef _WITH_
	_PARTITION
	//m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/HeightMap.raw"), 513, 513, 17, 17, xmf3Scale, xmf4Color);
#else
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("../Assets/Image/Terrain/terrain11.raw"), 513, 513, 513, 513, xmf3Scale, xmf4Color);
#endif


	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_pBuildings = new CShader;

	CObjectsShader *pBuildingShader = new CObjectsShader();
	pBuildingShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pBuildingShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	m_pBuildings = pBuildingShader;

	m_nShaders = 5;
	m_ppShaders = new CShader*[m_nShaders];

	CRedDotShader *pTreeShader = new CRedDotShader();
	pTreeShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pTreeShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CTreeShader *pFlowerShader = new CTreeShader();
	pFlowerShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pFlowerShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CBulletShader *pBulletShader = new CBulletShader();
	pBulletShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pBulletShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CParticleShader *pParticleShader = new CParticleShader();
	pParticleShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pParticleShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CObjectsShader *pObjectsShader = new CObjectsShader();
	pObjectsShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	m_ppShaders[0] = pTreeShader;
	m_ppShaders[1] = pFlowerShader;
	m_ppShaders[2] = pBulletShader;
	m_ppShaders[3] = pParticleShader;
	m_ppShaders[4] = pObjectsShader;

	// UI

	m_nUIShaders = 26;
	m_ppUIShaders = new CShader*[m_nUIShaders];

	CMiniMapShader *pMiniMapShader = new CMiniMapShader();
	pMiniMapShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pMiniMapShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CAlphaMapShader *pAlphaMapShader = new CAlphaMapShader();
	pAlphaMapShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pAlphaMapShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CHpBarShader *pHpBarShader = new CHpBarShader();
	pHpBarShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pHpBarShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CItemUIShader *pItemUIShader = new CItemUIShader();
	pItemUIShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pItemUIShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CItemUIShader_1 *pItemUIShader_1 = new CItemUIShader_1();
	pItemUIShader_1->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pItemUIShader_1->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CItemUIShader_2 *pItemUIShader_2 = new CItemUIShader_2();
	pItemUIShader_2->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pItemUIShader_2->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CItemUIShader_3 *pItemUIShader_3 = new CItemUIShader_3();
	pItemUIShader_3->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pItemUIShader_3->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CItemUIShader_4 *pItemUIShader_4 = new CItemUIShader_4();
	pItemUIShader_4->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pItemUIShader_4->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CItemEatUIShader *pItemEatUIShader = new CItemEatUIShader();
	pItemEatUIShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pItemEatUIShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CGunUIShader_1 *pGunUIShader_1 = new CGunUIShader_1();
	pGunUIShader_1->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pGunUIShader_1->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CGunUIShader_2 *pGunUIShader_2 = new CGunUIShader_2();
	pGunUIShader_2->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pGunUIShader_2->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	// 숫자 시작
	// 앞자리
	CNumShader_1 *pNumShader_1 = new CNumShader_1();
	pNumShader_1->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader_1->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader_2 *pNumShader_2 = new CNumShader_2();
	pNumShader_2->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader_2->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader_3 *pNumShader_3 = new CNumShader_3();
	pNumShader_3->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader_3->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader_4 *pNumShader_4 = new CNumShader_4();
	pNumShader_4->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader_4->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	// 뒷자리
	CNumShader0 *pNumShader0 = new CNumShader0();
	pNumShader0->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader0->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader1 *pNumShader1 = new CNumShader1();
	pNumShader1->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader1->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader2 *pNumShader2 = new CNumShader2();
	pNumShader2->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader2->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader3 *pNumShader3 = new CNumShader3();
	pNumShader3->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader3->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader4 *pNumShader4 = new CNumShader4();
	pNumShader4->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader4->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader5 *pNumShader5 = new CNumShader5();
	pNumShader5->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader5->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader6 *pNumShader6 = new CNumShader6();
	pNumShader6->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader6->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader7 *pNumShader7 = new CNumShader7();
	pNumShader7->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader7->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader8 *pNumShader8 = new CNumShader8();
	pNumShader8->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader8->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CNumShader9 *pNumShader9 = new CNumShader9();
	pNumShader9->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pNumShader9->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	m_ppUIShaders[0] = pMiniMapShader;
	m_ppUIShaders[1] = pTreeShader;
	m_ppUIShaders[2] = pHpBarShader;
	m_ppUIShaders[3] = pItemUIShader;
	m_ppUIShaders[4] = pItemUIShader_1;
	m_ppUIShaders[5] = pItemUIShader_2;
	m_ppUIShaders[6] = pItemUIShader_3;
	m_ppUIShaders[7] = pItemUIShader_4;
	m_ppUIShaders[8] = pItemEatUIShader;
	m_ppUIShaders[9] = pAlphaMapShader;
	m_ppUIShaders[10] = pGunUIShader_1;
	m_ppUIShaders[11] = pGunUIShader_2;
	m_ppUIShaders[12] = pNumShader_1;
	m_ppUIShaders[13] = pNumShader_2;
	m_ppUIShaders[14] = pNumShader_3;
	m_ppUIShaders[15] = pNumShader_4;
	m_ppUIShaders[16] = pNumShader0;
	m_ppUIShaders[17] = pNumShader1;
	m_ppUIShaders[18] = pNumShader2;
	m_ppUIShaders[19] = pNumShader3;
	m_ppUIShaders[20] = pNumShader4;
	m_ppUIShaders[21] = pNumShader5;
	m_ppUIShaders[22] = pNumShader6;
	m_ppUIShaders[23] = pNumShader7;
	m_ppUIShaders[24] = pNumShader8;
	m_ppUIShaders[25] = pNumShader9;

	// 메인화면
	m_nMainUIShaders = 4;
	m_ppMainUIShaders = new CShader*[m_nMainUIShaders];

	CMainScreenShader *pMainScreenShader = new CMainScreenShader();
	pMainScreenShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pMainScreenShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CMainScreenCheckShader *pMainScreenCheckShader = new CMainScreenCheckShader();
	pMainScreenCheckShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pMainScreenCheckShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CMainScreenCheck_1Shader *pMainScreenCheck_1Shader = new CMainScreenCheck_1Shader();
	pMainScreenCheck_1Shader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pMainScreenCheck_1Shader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	CGameOverShader *pGameOverShader = new CGameOverShader();
	pGameOverShader->CreateShader(pd3dDevice, m_pd3dGraphicsRootSignature);
	pGameOverShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);

	m_ppMainUIShaders[0] = pMainScreenShader;
	m_ppMainUIShaders[1] = pMainScreenCheckShader;
	m_ppMainUIShaders[2] = pMainScreenCheck_1Shader;
	m_ppMainUIShaders[3] = pGameOverShader;

	BuildLightsAndMaterials();

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pBuildings) m_pBuildings->Release();
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_ppObjects)
	{
		for (int i = 0; i < m_nObjects; i++) delete m_ppObjects[i];
		delete[] m_ppObjects;
	}

	ReleaseShaderVariables();

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;
	if (m_pLights) delete m_pLights;
	if (m_pMaterials) delete m_pMaterials;
	if (m_pBuildings) delete m_pBuildings;

	if (m_ppUIShaders)
	{
		for (int i = 0; i < m_nUIShaders; i++) delete m_ppUIShaders[i];
		delete[] m_ppUIShaders;
	}

	if (m_ppMainUIShaders)
	{
		for (int i = 0; i < m_nMainUIShaders; i++) delete m_ppMainUIShaders[i];
		delete[] m_ppMainUIShaders;
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pBuildings) m_pBuildings->ReleaseUploadBuffers();
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->ReleaseUploadBuffers();

	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();

	for (int i = 0; i < m_nUIShaders; i++) m_ppUIShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nMainUIShaders; i++) m_ppMainUIShaders[i]->ReleaseUploadBuffers();

	
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[6];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 2; //Game Objects
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 0; //Texture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 2; //t2: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 3; //t3: gtxtSkyBoxTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = 0;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 5; //t5: Texture[6]
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[10];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[1].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[0]; //Game Objects
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[3].Descriptor.ShaderRegister = 3; //Materials
	pd3dRootParameters[3].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[4].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[1]; //Texture
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[2];
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[3];
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[4];
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[5]; //Texture[6]
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);

	UINT ncbMaterialBytes = ((sizeof(MATERIALS) + 255) & ~255); //256의 배수
	m_pd3dcbMaterials = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbMaterialBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbMaterials->Map(0, NULL, (void **)&m_pcbMappedMaterials);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
	if (m_pd3dcbMaterials)
	{
		m_pd3dcbMaterials->Unmap(0, NULL);
		m_pd3dcbMaterials->Release();
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	/*if(CGameFramework::m_pCamera->GetMode == SPACESHIP_CAMERA)
		m_ppShaders[0]*/
	m_pBuildings->AnimateObjects(fTimeElapsed, pCamera);
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->AnimateObjects(fTimeElapsed, pCamera);

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fTimeElapsed);

	if (m_pLights)
	{
		for (int i = 0; i < 4; ++i) {
			m_pLights->m_pLights[1].m_xmf3Position = m_pPlayer[i]->GetPosition();
			m_pLights->m_pLights[1].m_xmf3Direction = m_pPlayer[i]->GetLookVector();
		}
	}

	for (int i = 0; i < MAX_PLAYER_SIZE; ++i) {
		//m_pPlayer[i]->SetScale(0.093, 0.093, 0.093);	// 캐릭터 크기 조정
		m_pPlayer[i]->SetScale(0.25, 0.25, 0.25);	// 캐릭터 크기 조정
	}
	for (int i = 0; i < NUM_OBJECT; ++i) {
		m_pObject[i]->SetScale(0.5f, 0.5f, 0.5f);
	}
	//for (int i = 0; i < m_nObjects; i++) m_ppUIShaders[i]->AnimateObjects(fTimeElapsed, pCamera);

	m_ppUIShaders[0]->AnimateObjects(fTimeElapsed, pCamera);
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress); //Materials

	
	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	
	if (m_pBuildings) m_pBuildings->Render(pd3dCommandList, pCamera);
	
	for (int i = 1; i < m_nShaders; i++) 
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->UpdateTransform(NULL);
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Render(pd3dCommandList, pCamera);
}

