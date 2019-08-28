//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"
#include "GameFramework.h"
#include "DDSTextureLoader12.h"

CPlayer* CGameFramework::m_pPlayer[];
CShadow* CGameFramework::m_pShadow[];
CPlayer* CGameFramework::m_pObject[];
CPlayer* CGameFramework::m_pObject2[];
int CGameFramework::my_client_id;
XMFLOAT3 CGameFramework::buildingPos[];
XMFLOAT3 CGameFramework::sendLook;
int CGameFramework::boxBound;


CShader::CShader()
{
	m_d3dSrvCPUDescriptorStartHandle.ptr = NULL;
	m_d3dSrvGPUDescriptorStartHandle.ptr = NULL;
}

CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

void CShader::SetPosition(int id, XMFLOAT3 input) {

}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}
D3D12_SHADER_BYTECODE CShader::CreateShadowPixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSShadow", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader::CreateTexturedPixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSINST", "ps_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}
D3D12_SHADER_BYTECODE CShader::CreateShadowMovePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSShadowPlayer", "ps_5_1", ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *errorBlob = NULL;
	HRESULT hresult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &errorBlob);

	if (errorBlob)
	{
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		errorBlob->Release();
	}

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = FALSE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}
D3D12_BLEND_DESC CShader::CreateShadowBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	/*if (isShadow) {	
		d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
		d3dPipelineStateDesc.PS = CreateShadowMovePixelShader(&pd3dPixelShaderBlob);
	}
	else
	{
		d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
		d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	}*/
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
	
	if (isShadow) {
		d3dPipelineStateDesc.RasterizerState.DepthBias = 50000;
		d3dPipelineStateDesc.RasterizerState.DepthBiasClamp = 0.0f;
		d3dPipelineStateDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		d3dPipelineStateDesc.NumRenderTargets = 0;
		d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
		d3dPipelineStateDesc.PS = CreateShadowMovePixelShader(&pd3dPixelShaderBlob);
		hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)& m_ppd3dPipelineStates[0]);
	}
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}
void CShader::CreateShadowShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateShadowVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreateShadowPixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateShadowBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}

void CShader::CreateTextureShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateTexturedVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreateTexturedPixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateShadowBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);

	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();

	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] d3dPipelineStateDesc.InputLayout.pInputElementDescs;

}
D3D12_SHADER_BYTECODE CShader::CreateShadowVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSShadow", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader::CreateTexturedVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSINST", "vs_5_1", ppd3dShaderBlob));

}
void CShader::CreateCbvAndSrvDescriptorHeaps(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

void CShader::CreateConstantBufferViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE d3dCbvCPUDescriptorHandle;
		d3dCbvCPUDescriptorHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, d3dCbvCPUDescriptorHandle);
	}
}

D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc(D3D12_RESOURCE_DESC d3dResourceDesc, UINT nTextureType)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc;
	d3dShaderResourceViewDesc.Format = d3dResourceDesc.Format;
	d3dShaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY:
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3dShaderResourceViewDesc.Texture2D.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2D.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3dShaderResourceViewDesc.Texture2DArray.MipLevels = -1;
		d3dShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.Texture2DArray.PlaneSlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3dShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
		d3dShaderResourceViewDesc.Texture2DArray.ArraySize = d3dResourceDesc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3dShaderResourceViewDesc.TextureCube.MipLevels = -1;
		d3dShaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
		d3dShaderResourceViewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3dShaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3dShaderResourceViewDesc.Buffer.FirstElement = 0;
		d3dShaderResourceViewDesc.Buffer.NumElements = 0;
		d3dShaderResourceViewDesc.Buffer.StructureByteStride = 0;
		d3dShaderResourceViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3dShaderResourceViewDesc);
}

void CShader::CreateShaderResourceViews(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement)
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dSrvCPUDescriptorHandle = m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE d3dSrvGPUDescriptorHandle = m_d3dSrvGPUDescriptorStartHandle;
	int nTextures = pTexture->GetTextureCount();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC d3dResourceDesc = pShaderResource->GetDesc();
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = GetShaderResourceViewDesc(d3dResourceDesc, nTextureType);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, d3dSrvCPUDescriptorHandle);
		d3dSrvCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, d3dSrvGPUDescriptorHandle);
		d3dSrvGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

void CShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CShader::UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT4X4 *pxmf4x4World)
{
}

void CShader::ReleaseShaderVariables()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}

void CShader::ReleaseUploadBuffers()
{
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_ppd3dPipelineStates) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
	pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	UpdateShaderVariables(pd3dCommandList);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	OnPrepareRender(pd3dCommandList);
}

void CShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, float hp)
{
	OnPrepareRender(pd3dCommandList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSPlayer", "vs_5_1", ppd3dShaderBlob));
	//return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTextured", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSPlayer", "ps_5_1", ppd3dShaderBlob));
	//return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTextured", "ps_5_1", ppd3dShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTexturedShader::CTexturedShader()
{
}

CTexturedShader::~CTexturedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTexturedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CTexturedShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = TRUE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_SHADER_BYTECODE CTexturedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTextured", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTextured", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTexturedShader::CreateShadowVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSShadow", "vs_5_1", ppd3dShaderBlob));
}

void CTexturedShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CIlluminatedTexturedShader::CIlluminatedTexturedShader()
{
}

CIlluminatedTexturedShader::~CIlluminatedTexturedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CIlluminatedTexturedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CIlluminatedTexturedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTexturedLighting", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CIlluminatedTexturedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTexturedLighting", "ps_5_1", ppd3dShaderBlob));
}

void CIlluminatedTexturedShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

D3D12_BLEND_DESC CIlluminatedTexturedShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CIlluminatedShader::CIlluminatedShader()
{
}

CIlluminatedShader::~CIlluminatedShader()
{
}

D3D12_INPUT_LAYOUT_DESC CIlluminatedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CIlluminatedShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSLighting", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CIlluminatedShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSLighting", "ps_5_1", ppd3dShaderBlob));
}

void CIlluminatedShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBigBoxShader::CBigBoxShader()
{
}

CBigBoxShader::~CBigBoxShader()
{
}

void CBigBoxShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CBigBoxShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nObjects; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
#ifdef _WITH_BATCH_MATERIAL
		if (m_pMaterial) pbMappedcbGameObject->m_nMaterial = m_pMaterial->m_nReflection;
#endif
	}
}

void CBigBoxShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CIlluminatedTexturedShader::ReleaseShaderVariables();
}

//#define _WITH_TERRAIN_PARTITION

void CBigBoxShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	int xObjects = 1, yObjects = 1, zObjects = 1, i = 0;

	m_nObjects = 1;

	CTexture *pTextures = new CTexture(6, RESOURCE_TEXTURE2DARRAY, 0);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/blueScreen.dds", 0);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/blueScreen.dds", 1);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/blueScreen.dds", 4);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/blueScreen.dds", 3);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/blueScreen.dds", 2);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/blueScreen.dds", 5);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTextures, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTextures);
	m_pMaterial->SetReflection(1);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
	pCubeMaterial->SetReflection(1);
#endif
	CCubeMeshIlluminatedTextured *pCubeMesh = new CCubeMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, 1000, 5000, 1000);

	m_ppObjects = new CGameObject*[m_nObjects];

	float fxPitch = 12.0f * 2.5f, fyPitch = 12.0f * 2.5f, fzPitch = 12.0f * 2.5f;

	CRotatingObject *pRotatingObject = NULL;
	pRotatingObject = new CRotatingObject(1);
	pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
	pRotatingObject->SetMaterial(pCubeMaterial);
#endif
	float xPosition = 1000, zPosition = 1000;
	float fHeight = pTerrain->GetHeight(xPosition, zPosition);
	pRotatingObject->SetPosition(XMFLOAT3(xPosition, fHeight, zPosition));
	pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
	pRotatingObject->SetRotationSpeed(10.0f * (i % 10));
	pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
	m_ppObjects[0] = pRotatingObject;
}

void CBigBoxShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CBigBoxShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CBigBoxShader::SetScale(float x, float y, float z)
{
	m_ppObjects[0]->SetScale(x, y, z);
}

void CBigBoxShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CBigBoxShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CIlluminatedTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	if (m_ppObjects[0]) m_ppObjects[0]->Render(pd3dCommandList, pCamera);
	m_ppObjects[0]->SetScale(0.9, 0.9, 0.9);
}

D3D12_BLEND_DESC CBigBoxShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

void CObjectsShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CObjectsShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nObjects; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
#ifdef _WITH_BATCH_MATERIAL
		if (m_pMaterial) pbMappedcbGameObject->m_nMaterial = m_pMaterial->m_nReflection;
#endif
	}
}

void CObjectsShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CIlluminatedTexturedShader::ReleaseShaderVariables();
}

//#define _WITH_TERRAIN_PARTITION

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	pTerrainCopy = pTerrain;
	int xObjects = MAX_BOX_SIZE, yObjects = MAX_PLAYER_SIZE, zObjects = 1, i = 0;

	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);

	CTexture *pTextures = new CTexture(6, RESOURCE_TEXTURE2DARRAY, 0);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 0);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 1);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 4);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 3);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 2);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 5);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTextures, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTextures);
	m_pMaterial->SetReflection(1);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
	pCubeMaterial->SetReflection(1);
#endif
	CCubeMeshIlluminatedTextured *pCubeMesh = new CCubeMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, 8, 8, 8);

	m_ppObjects = new CGameObject*[m_nObjects];

	float fxPitch = 12.0f * 2.5f, fyPitch = 12.0f * 2.5f, fzPitch = 12.0f * 2.5f;

	CRotatingObject *pRotatingObject = NULL;

	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject(1);
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = x * 30;
				float zPosition = 1000 + z * 30;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(0, -900, 0);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f * (i % 10));
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				pRotatingObject->SetScale(2.0, 2.0, 2.0);
				pRotatingObject->SetOOBB(pRotatingObject->GetPosition(), XMFLOAT3(4, 4, 4), XMFLOAT4(0, 0, 0, 1));
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
}

void CObjectsShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CObjectsShader::SetBoxPosition(int index, XMFLOAT3 input) {
	float fHeight = pTerrainCopy->GetHeight(input.x, input.z);
	m_ppObjects[index]->SetPosition(input.x, fHeight + 4, input.z);
	m_ppObjects[index]->SetOOBB(m_ppObjects[index]->GetPosition(), XMFLOAT3(4, 4, 4), XMFLOAT4(0, 0, 0, 1));
}


void CObjectsShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{

	for (int i = 0; i < MAX_BOX_SIZE * MAX_PLAYER_SIZE; ++i) {
		m_ppObjects[i]->Animate(fTimeElapsed);
	}
	bool check = false;
	//충돌체크
	for (int i = 0; i < MAX_PLAYER_SIZE * MAX_BOX_SIZE; ++i)	{
		ContainmentType containType = CGameFramework::m_pPlayer[CGameFramework::my_client_id]->bounding_box.Contains(m_ppObjects[i]->bounding_box);
		switch (containType)
		{
		case DISJOINT:
		{
			break;
		}
		case INTERSECTS:
		{
			if ((m_ppObjects[i]->GetPosition().x - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().x) * (m_ppObjects[i]->GetPosition().x - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().x)
				< (m_ppObjects[i]->GetPosition().z - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().z) * (m_ppObjects[i]->GetPosition().z - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().z)) {
				if (m_ppObjects[i]->GetPosition().z - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().z > 0) { m_ppObjects[i]->look = XMFLOAT3(0, 0, -1); }
				else { m_ppObjects[i]->look = XMFLOAT3(0, 0, 1); }
			}
			else {
				if (m_ppObjects[i]->GetPosition().x - CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().x > 0) { m_ppObjects[i]->look = XMFLOAT3(-1, 0, 0); }
				else { m_ppObjects[i]->look = XMFLOAT3(1, 0, 0); }
			}
			XMFLOAT3 xmf3Result;
			XMFLOAT3 xmf3Result_1;
			XMFLOAT3 xmf3Result_2;
			XMStoreFloat3(&xmf3Result_1, XMVector3Dot(XMLoadFloat3(&CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetLook()), XMLoadFloat3(&m_ppObjects[i]->look)));
			XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&m_ppObjects[i]->look), XMLoadFloat3(&xmf3Result_1)));
			xmf3Result_2 = XMFLOAT3(Vector3::Subtract(CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetLook(), xmf3Result));
			CGameFramework::sendLook = XMFLOAT3(2 * xmf3Result_2.x / 3, 2 * xmf3Result_2.y / 3, 2 * xmf3Result_2.z / 3);
			check = true;
			break;
		}
		case CONTAINS:

			break;
		}
	}
	if (check == true)
		CGameFramework::boxBound = 1;
}

void CObjectsShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < MAX_BOX_SIZE * MAX_PLAYER_SIZE; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CObjectsShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CIlluminatedTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < MAX_PLAYER_SIZE * MAX_BOX_SIZE; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	}
}

D3D12_BLEND_DESC CObjectsShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CObjectsShader::BoundCheck(XMFLOAT3 playerPosition, float playerSize) {

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CTerrainShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTerrain", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTerrain", "ps_5_1", ppd3dShaderBlob));
}

void CTerrainShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}



D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

//D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
//{
//	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSSkyBox", "vs_5_1", ppd3dShaderBlob));
//}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSSkyBox", "ps_5_1", ppd3dShaderBlob));
}

void CSkyBoxShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CFlowerShader::CFlowerShader() {

}
CFlowerShader::~CFlowerShader() {
}

D3D12_DEPTH_STENCIL_DESC CFlowerShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = TRUE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}


void CFlowerShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nTree = (xObjects * yObjects * zObjects);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/RedPoint.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 30.0f, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*[m_nTree];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CFlowerShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CFlowerShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CFlowerShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CFlowerShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nTree; j++)
	{
		if (m_ppTree[j]) m_ppTree[j]->Render(pd3dCommandList, pCamera);
	}
}

void CFlowerShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CFlowerShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	/*for (int j = 0; j < m_nTree; j++)
	{
	CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
	XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppTree[j]->m_xmf4x4World)));
	}*/
}

void CFlowerShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CFlowerShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CBulletShader::CBulletShader() {

}
CBulletShader::~CBulletShader() {
}

void CBulletShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 2;
	int yObjects = 1;
	int zObjects = 1;
	m_nBullet = 2;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/YellowDot.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nBullet, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nBullet, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 0.01f, 0.01f, 0.01f);

	m_ppBullet = new CBillboard*[m_nBullet];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 510 + x * 10;
				float zPosition = 1510 + z * 5;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, -100, zPosition);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppBullet[i++] = pRotatingObject;
			}
		}
	}
}

void CBulletShader::ReleaseObjects()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) delete m_ppBullet[j];
		delete[] m_ppBullet;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CBulletShader::SetPosition(int id, XMFLOAT3 input) {
	m_ppBullet[id]->SetPosition(input);
}

void CBulletShader::SetLook(int input, float x, float y, float z) {
	m_ppBullet[input]->SetLookAt(XMFLOAT3(x, y, z));
}

void CBulletShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nBullet; j++)
	{
		m_ppBullet[j]->Animate(fTimeElapsed, pCamera);
	}

}

void CBulletShader::ReleaseUploadBuffers()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) m_ppBullet[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CBulletShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nBullet; j++)
	{
		if (m_ppBullet[j]) m_ppBullet[j]->Render(pd3dCommandList, pCamera);
	}
}

void CBulletShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nBullet, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CBulletShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nBullet; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppBullet[j]->m_xmf4x4World)));
	}
}

void CBulletShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CBulletShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CTeamTriShader::CTeamTriShader() {

}
CTeamTriShader::~CTeamTriShader() {
}

void CTeamTriShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nBullet = 1;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/TeamTri.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nBullet, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nBullet, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 1.f, 1.f, 1.f);

	m_ppBullet = new CBillboard*[m_nBullet];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 510 + x * 10;
				float zPosition = 1510 + z * 5;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, -100, zPosition);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppBullet[i++] = pRotatingObject;
			}
		}
	}
}

void CTeamTriShader::ReleaseObjects()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) delete m_ppBullet[j];
		delete[] m_ppBullet;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTeamTriShader::SetPosition(int id, XMFLOAT3 input) {
	m_ppBullet[id]->SetPosition(input);
}

void CTeamTriShader::SetLook(int input, float x, float y, float z) {
	m_ppBullet[input]->SetLookAt(XMFLOAT3(x, y, z));
}

void CTeamTriShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nBullet; j++)
	{
		m_ppBullet[j]->Animate(fTimeElapsed, pCamera);
	}

}

void CTeamTriShader::ReleaseUploadBuffers()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) m_ppBullet[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTeamTriShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nBullet; j++)
	{
		if (m_ppBullet[j]) m_ppBullet[j]->Render(pd3dCommandList, pCamera);
	}
}

void CTeamTriShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nBullet, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTeamTriShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nBullet; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppBullet[j]->m_xmf4x4World)));
	}
}

void CTeamTriShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTeamTriShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CSkillShader_1::CSkillShader_1() {

}
CSkillShader_1::~CSkillShader_1() {
}

void CSkillShader_1::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = MAX_SKILLS;
	int yObjects = 1;
	int zObjects = 1;
	m_nBullet = MAX_SKILLS;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/Armor.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nBullet, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nBullet, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 3.f, 3.f, 3.f);

	m_ppBullet = new CBillboard*[m_nBullet];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 510 + x * 10;
				float zPosition = 1510 + z * 5;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, -100, zPosition);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppBullet[i++] = pRotatingObject;
			}
		}
	}
}

void CSkillShader_1::ReleaseObjects()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) delete m_ppBullet[j];
		delete[] m_ppBullet;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CSkillShader_1::SetPosition(int id, XMFLOAT3 input) {
	m_ppBullet[id]->SetPosition(input);
}

void CSkillShader_1::SetLook(int input, float x, float y, float z) {
	m_ppBullet[input]->SetLookAt(XMFLOAT3(x, y, z));
}

void CSkillShader_1::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nBullet; j++)
	{
		m_ppBullet[j]->Animate(fTimeElapsed, pCamera);
	}

}

void CSkillShader_1::ReleaseUploadBuffers()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) m_ppBullet[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CSkillShader_1::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nBullet; j++)
	{
		if (m_ppBullet[j]) m_ppBullet[j]->Render(pd3dCommandList, pCamera);
	}
}

void CSkillShader_1::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nBullet, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CSkillShader_1::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nBullet; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppBullet[j]->m_xmf4x4World)));
	}
}

void CSkillShader_1::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CSkillShader_1::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CSkillShader_1::ItemDrop(int id, int count, bool check) {
	if(count < 7)
		m_ppBullet[id]->SetPosition(XMFLOAT3(m_ppBullet[id]->GetPosition().x, m_ppBullet[id]->GetPosition().y + 0.2, m_ppBullet[id]->GetPosition().z));
	else if (count < 15)
		m_ppBullet[id]->SetPosition(XMFLOAT3(m_ppBullet[id]->GetPosition().x, m_ppBullet[id]->GetPosition().y + 0.1, m_ppBullet[id]->GetPosition().z));
	else if (count < 22)
		m_ppBullet[id]->SetPosition(XMFLOAT3(m_ppBullet[id]->GetPosition().x, m_ppBullet[id]->GetPosition().y - 0.1, m_ppBullet[id]->GetPosition().z));
	else if (count < 30)
		m_ppBullet[id]->SetPosition(XMFLOAT3(m_ppBullet[id]->GetPosition().x, m_ppBullet[id]->GetPosition().y - 0.2, m_ppBullet[id]->GetPosition().z));

}
///////////////////////////////
CSkillShader_2::CSkillShader_2() {

}
CSkillShader_2::~CSkillShader_2() {
}

void CSkillShader_2::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = MAX_SKILLS;
	int yObjects = 1;
	int zObjects = 1;
	m_nBullet = MAX_SKILLS;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/TeamTri.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nBullet, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nBullet, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 1.f, 1.f, 1.f);

	m_ppBullet = new CBillboard*[m_nBullet];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 510 + x * 10;
				float zPosition = 1510 + z * 5;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, -100, zPosition);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppBullet[i++] = pRotatingObject;
			}
		}
	}
}

void CSkillShader_2::ReleaseObjects()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) delete m_ppBullet[j];
		delete[] m_ppBullet;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CSkillShader_2::SetPosition(int id, XMFLOAT3 input) {
	m_ppBullet[id]->SetPosition(input);
}

void CSkillShader_2::SetLook(int input, float x, float y, float z) {
	m_ppBullet[input]->SetLookAt(XMFLOAT3(x, y, z));
}

void CSkillShader_2::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nBullet; j++)
	{
		m_ppBullet[j]->Animate(fTimeElapsed, pCamera);
	}

}

void CSkillShader_2::ReleaseUploadBuffers()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) m_ppBullet[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CSkillShader_2::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nBullet; j++)
	{
		if (m_ppBullet[j]) m_ppBullet[j]->Render(pd3dCommandList, pCamera);
	}
}

void CSkillShader_2::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nBullet, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CSkillShader_2::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nBullet; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppBullet[j]->m_xmf4x4World)));
	}
}

void CSkillShader_2::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CSkillShader_2::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

///////////////////////////////
CSkillShader_3::CSkillShader_3() {

}
CSkillShader_3::~CSkillShader_3() {
}

void CSkillShader_3::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = MAX_SKILLS;
	int yObjects = 1;
	int zObjects = 1;
	m_nBullet = MAX_SKILLS;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/TeamTri.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nBullet, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nBullet, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 1.f, 1.f, 1.f);

	m_ppBullet = new CBillboard*[m_nBullet];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 510 + x * 10;
				float zPosition = 1510 + z * 5;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, -100, zPosition);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppBullet[i++] = pRotatingObject;
			}
		}
	}
}

void CSkillShader_3::ReleaseObjects()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) delete m_ppBullet[j];
		delete[] m_ppBullet;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CSkillShader_3::SetPosition(int id, XMFLOAT3 input) {
	m_ppBullet[id]->SetPosition(input);
}

void CSkillShader_3::SetLook(int input, float x, float y, float z) {
	m_ppBullet[input]->SetLookAt(XMFLOAT3(x, y, z));
}

void CSkillShader_3::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nBullet; j++)
	{
		m_ppBullet[j]->Animate(fTimeElapsed, pCamera);
	}

}

void CSkillShader_3::ReleaseUploadBuffers()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) m_ppBullet[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CSkillShader_3::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nBullet; j++)
	{
		if (m_ppBullet[j]) m_ppBullet[j]->Render(pd3dCommandList, pCamera);
	}
}

void CSkillShader_3::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nBullet, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CSkillShader_3::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nBullet; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppBullet[j]->m_xmf4x4World)));
	}
}

void CSkillShader_3::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CSkillShader_3::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

////////////////////////////////////////

CSkillShader_4::CSkillShader_4() {

}
CSkillShader_4::~CSkillShader_4() {
}

void CSkillShader_4::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = MAX_SKILLS;
	int yObjects = 1;
	int zObjects = 1;
	m_nBullet = MAX_SKILLS;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/TeamTri.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nBullet, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nBullet, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 1.f, 1.f, 1.f);

	m_ppBullet = new CBillboard*[m_nBullet];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 510 + x * 10;
				float zPosition = 1510 + z * 5;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, -100, zPosition);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppBullet[i++] = pRotatingObject;
			}
		}
	}
}

void CSkillShader_4::ReleaseObjects()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) delete m_ppBullet[j];
		delete[] m_ppBullet;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CSkillShader_4::SetPosition(int id, XMFLOAT3 input) {
	m_ppBullet[id]->SetPosition(input);
}

void CSkillShader_4::SetLook(int input, float x, float y, float z) {
	m_ppBullet[input]->SetLookAt(XMFLOAT3(x, y, z));
}

void CSkillShader_4::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nBullet; j++)
	{
		m_ppBullet[j]->Animate(fTimeElapsed, pCamera);
	}

}

void CSkillShader_4::ReleaseUploadBuffers()
{
	if (m_ppBullet)
	{
		for (int j = 0; j < m_nBullet; j++) if (m_ppBullet[j]) m_ppBullet[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CSkillShader_4::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nBullet; j++)
	{
		if (m_ppBullet[j]) m_ppBullet[j]->Render(pd3dCommandList, pCamera);
	}
}

void CSkillShader_4::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nBullet, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CSkillShader_4::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nBullet; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppBullet[j]->m_xmf4x4World)));
	}
}

void CSkillShader_4::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CSkillShader_4::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


///////////////////

CParticleShader::CParticleShader() {

}
CParticleShader::~CParticleShader() {
}

void CParticleShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nParticle = 1;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Blood/Blood.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nParticle, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nParticle, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh *pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 20.0, 20.0, 10.0);

	m_ppParticle = new CBillboard*[m_nParticle];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 500;
				float zPosition = 500;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(XMFLOAT3(xPosition, fHeight, zPosition));
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppParticle[i++] = pRotatingObject;
			}
		}
	}
}

void CParticleShader::ReleaseObjects()
{
	if (m_ppParticle)
	{
		for (int j = 0; j < m_nParticle; j++) if (m_ppParticle[j]) delete m_ppParticle[j];
		delete[] m_ppParticle;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CParticleShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nParticle; j++)
	{
		m_ppParticle[j]->SetPosition(changePosition);
		m_ppParticle[j]->Animate(fTimeElapsed, pCamera);
	}

	//if (shootBullet == 1)
	//{
	//	m_ppParticle[BulletCount]->render = 1;
	//	m_ppParticle[BulletCount]->SetPosition(XMFLOAT3(0,0,0));
	//	//m_ppParticle[BulletCount]->SetPlayerLook(CGameFramework::m_pPlayer->GetLook());
	//	++BulletCount;
	//	if (BulletCount > 99)
	//		BulletCount = 0;
	//	shootBullet = 2;
	//}
}

void CParticleShader::SetPosition(int id, XMFLOAT3 input) {
	changePosition = XMFLOAT3(input.x, input.y - 60.0, input.z);
	//printf("%f %f %f", m_ppParticle[0]->GetPosition().x, m_ppParticle[0]->GetPosition().y - 150, m_ppParticle[0]->GetPosition().z);
}

void CParticleShader::ReleaseUploadBuffers()
{
	if (m_ppParticle)
	{
		for (int j = 0; j < m_nParticle; j++) if (m_ppParticle[j]) m_ppParticle[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CParticleShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	for (int j = 0; j < m_nParticle; j++)
		m_ppParticle[j]->Render(pd3dCommandList, pCamera);
}

void CParticleShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nParticle, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CParticleShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nParticle; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppParticle[j]->m_xmf4x4World)));
	}
}

void CParticleShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CParticleShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

// 나무 부수면
CDropItemShader::CDropItemShader() {

}
CDropItemShader::~CDropItemShader() {
}

void CDropItemShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pContext)
{
	CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nParticle = 1;

	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Blood/Blood.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nParticle, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nParticle, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial* pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CBillboardMesh* pCubeMesh = new CBillboardMesh(pd3dDevice, pd3dCommandList, 10.0, 10.0, 10.0);

	m_ppParticle = new CBillboard * [m_nParticle];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CBillboard* pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CBillboard();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 510 + x * 10;
				float zPosition = 1510 + z * 5;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(XMFLOAT3(xPosition, fHeight, zPosition));
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
					float fAngle = acos(Vector3::DotProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal));
					pRotatingObject->Rotate(&xmf3RotateAxis, XMConvertToDegrees(fAngle));
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppParticle[i++] = pRotatingObject;
			}
		}
	}
}

void CDropItemShader::ReleaseObjects()
{
	if (m_ppParticle)
	{
		for (int j = 0; j < m_nParticle; j++) if (m_ppParticle[j]) delete m_ppParticle[j];
		delete[] m_ppParticle;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CDropItemShader::AnimateObjects(float fTimeElapsed, CCamera* pCamera)
{
	for (int j = 0; j < m_nParticle; j++)
	{
		m_ppParticle[j]->SetPosition(changePosition);
		m_ppParticle[j]->Animate(fTimeElapsed, pCamera, 1);
	}

	//if (shootBullet == 1)
	//{
	//	m_ppParticle[BulletCount]->render = 1;
	//	m_ppParticle[BulletCount]->SetPosition(XMFLOAT3(0,0,0));
	//	//m_ppParticle[BulletCount]->SetPlayerLook(CGameFramework::m_pPlayer->GetLook());
	//	++BulletCount;
	//	if (BulletCount > 99)
	//		BulletCount = 0;
	//	shootBullet = 2;
	//}
}

void CDropItemShader::SetPosition(int id, XMFLOAT3 input) {
	changePosition = XMFLOAT3(input.x, input.y - 60.0, input.z);
	//printf("%f %f %f", m_ppParticle[0]->GetPosition().x, m_ppParticle[0]->GetPosition().y - 150, m_ppParticle[0]->GetPosition().z);
}

void CDropItemShader::ReleaseUploadBuffers()
{
	if (m_ppParticle)
	{
		for (int j = 0; j < m_nParticle; j++) if (m_ppParticle[j]) m_ppParticle[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CDropItemShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	for (int j = 0; j < m_nParticle; j++)
		m_ppParticle[j]->Render(pd3dCommandList, pCamera);
}

void CDropItemShader::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nParticle, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)& m_pcbMappedGameObjects);
}

void CDropItemShader::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nParticle; j++)
	{
		CB_GAMEOBJECT_INFO* pbMappedcbGameObject = (CB_GAMEOBJECT_INFO*)((UINT8*)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppParticle[j]->m_xmf4x4World)));
	}
}

void CDropItemShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CDropItemShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 도트


CRedDotShader::CRedDotShader() {
}
CRedDotShader::~CRedDotShader() {
}

D3D12_DEPTH_STENCIL_DESC CRedDotShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CRedDotShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CRedDotShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CRedDotShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nTree = (xObjects * yObjects * zObjects);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/Scope.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CDotMesh *pCubeMesh = new CDotMesh(pd3dDevice, pd3dCommandList, 30.0f, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*[m_nTree];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CRedDotShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CRedDotShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
	/*m_ppTree[0]->SetPosition(XMFLOAT3(CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().x + 15 * CGameFramework::m_pCamera->GetLookVector().x,
	CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().y + 15 + 15 * CGameFramework::m_pCamera->GetLookVector().y,
	CGameFramework::m_pPlayer[CGameFramework::my_client_id]->GetPosition().z + 15 * CGameFramework::m_pCamera->GetLookVector().z));
	*/
	/*m_ppTree[0]->SetPosition(XMFLOAT3(-1.0, -1.0, 1.0));
	m_ppTree[0]->SetScale(0.1, 0.1, 0.1);*/
}

void CRedDotShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CRedDotShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nTree; j++)
	{
		if (m_ppTree[j]) m_ppTree[j]->Render(pd3dCommandList, pCamera);
	}
}

void CRedDotShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CRedDotShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	/*for (int j = 0; j < m_nTree; j++)
	{
	CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
	XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppTree[j]->m_xmf4x4World)));
	}*/
}

void CRedDotShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CRedDotShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 미니맵

CMiniMapShader::CMiniMapShader() {
}
CMiniMapShader::~CMiniMapShader() {
}

D3D12_DEPTH_STENCIL_DESC CMiniMapShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CMiniMapShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CMiniMapShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CMiniMapShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nTree = (xObjects * yObjects * zObjects);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/Wood.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMiniMapMesh *pCubeMesh = new CMiniMapMesh(pd3dDevice, pd3dCommandList, 30.0f, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*[m_nTree];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}


void CMiniMapShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->SetPosition(XMFLOAT3(m_ppTree[j]->GetPosition().x + 10000, m_ppTree[j]->GetPosition().y, m_ppTree[j]->GetPosition().z));
	}
}

void CMiniMapShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}


void CMiniMapShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CMiniMapShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nTree; j++)
	{
		if (m_ppTree[j]) m_ppTree[j]->Render(pd3dCommandList, pCamera);
	}
}

void CMiniMapShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CMiniMapShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CMiniMapShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CMiniMapShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 알파 맵

CAlphaMapShader::CAlphaMapShader() {
}
CAlphaMapShader::~CAlphaMapShader() {
}

D3D12_DEPTH_STENCIL_DESC CAlphaMapShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CAlphaMapShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CAlphaMapShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CAlphaMapShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nTree = (xObjects * yObjects * zObjects);

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Terrain/MapFinal.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenMesh *pCubeMesh = new CMainScreenMesh(pd3dDevice, pd3dCommandList, 30.0f, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*[m_nTree];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < xObjects; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CAlphaMapShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CAlphaMapShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CAlphaMapShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CAlphaMapShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nTree; j++)
	{
		if (m_ppTree[j]) m_ppTree[j]->Render(pd3dCommandList, pCamera);
	}
}

void CAlphaMapShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CAlphaMapShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CAlphaMapShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CAlphaMapShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Hp바

CHpBarShader::CHpBarShader() {
}
CHpBarShader::~CHpBarShader() {
}

D3D12_DEPTH_STENCIL_DESC CHpBarShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CHpBarShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CHpBarShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CHpBarShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nTree = 20;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/HpBar/HpBar.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CHpBarMesh *pCubeMesh[20];
	
	for(int i=0;i<20;++i)
		pCubeMesh[i] = new CHpBarMesh(pd3dDevice, pd3dCommandList, i, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*[20];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < 20; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CHpBarShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CHpBarShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
	testHp--;
}

void CHpBarShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CHpBarShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, float hp)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int j = 0; j < hp/5; j++)
	{
		if (m_ppTree[j]) m_ppTree[j]->Render(pd3dCommandList, pCamera);
	}
}

void CHpBarShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CHpBarShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CHpBarShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CHpBarShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// 채팅 레디

CChatReadyShader::CChatReadyShader() {
}
CChatReadyShader::~CChatReadyShader() {
}

D3D12_DEPTH_STENCIL_DESC CChatReadyShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CChatReadyShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CChatReadyShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CChatReadyShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/ready.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CChatReadyMesh *pCubeMesh[20];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CChatReadyMesh(pd3dDevice, pd3dCommandList, i, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*[20];

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < 20; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CChatReadyShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CChatReadyShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
	testHp--;
}

void CChatReadyShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CChatReadyShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, bool playerready[4])
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	for (int j = 0; j < 4; j++)
	{
		if (playerready[j] == true) m_ppTree[j]->Render(pd3dCommandList, pCamera);
	}
}

void CChatReadyShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CChatReadyShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CChatReadyShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CChatReadyShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 아이템 표시

CItemUIShader::CItemUIShader() {
}
CItemUIShader::~CItemUIShader() {
}

D3D12_DEPTH_STENCIL_DESC CItemUIShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CItemUIShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CItemUIShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CItemUIShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/Items_Black.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CItemBlackUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CItemBlackUIMesh(pd3dDevice, pd3dCommandList, i, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CItemUIShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CItemUIShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CItemUIShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CItemUIShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CItemUIShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CItemUIShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CItemUIShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CItemUIShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CItemUIShader_1::CItemUIShader_1() {
}
CItemUIShader_1::~CItemUIShader_1() {
}

D3D12_DEPTH_STENCIL_DESC CItemUIShader_1::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CItemUIShader_1::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CItemUIShader_1::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CItemUIShader_1::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/Item_Spanner.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CItemUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CItemUIMesh(pd3dDevice, pd3dCommandList, 1, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CItemUIShader_1::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CItemUIShader_1::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CItemUIShader_1::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CItemUIShader_1::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CItemUIShader_1::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CItemUIShader_1::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CItemUIShader_1::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CItemUIShader_1::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

////////////////////////

CItemUIShader_2::CItemUIShader_2() {
}
CItemUIShader_2::~CItemUIShader_2() {
}

D3D12_DEPTH_STENCIL_DESC CItemUIShader_2::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CItemUIShader_2::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CItemUIShader_2::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CItemUIShader_2::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/Armor.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CItemUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CItemUIMesh(pd3dDevice, pd3dCommandList, 1, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CItemUIShader_2::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CItemUIShader_2::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CItemUIShader_2::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CItemUIShader_2::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CItemUIShader_2::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CItemUIShader_2::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CItemUIShader_2::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CItemUIShader_2::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

///////////////////////////

CItemUIShader_3::CItemUIShader_3() {
}
CItemUIShader_3::~CItemUIShader_3() {
}

D3D12_DEPTH_STENCIL_DESC CItemUIShader_3::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CItemUIShader_3::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CItemUIShader_3::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CItemUIShader_3::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/Item_Fuel.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CItemUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CItemUIMesh(pd3dDevice, pd3dCommandList, 1, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CItemUIShader_3::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CItemUIShader_3::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CItemUIShader_3::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CItemUIShader_3::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CItemUIShader_3::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CItemUIShader_3::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CItemUIShader_3::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CItemUIShader_3::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

///////////////////////////

CItemUIShader_4::CItemUIShader_4() {
}
CItemUIShader_4::~CItemUIShader_4() {
}

D3D12_DEPTH_STENCIL_DESC CItemUIShader_4::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CItemUIShader_4::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CItemUIShader_4::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CItemUIShader_4::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/Item_Battery.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CItemUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CItemUIMesh(pd3dDevice, pd3dCommandList, 1, 30.0f, 1.0f);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CItemUIShader_4::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CItemUIShader_4::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CItemUIShader_4::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CItemUIShader_4::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CItemUIShader_4::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CItemUIShader_4::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CItemUIShader_4::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CItemUIShader_4::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


////////////////////////////////////////////////////

CItemEatUIShader::CItemEatUIShader() {
}
CItemEatUIShader::~CItemEatUIShader() {
}

D3D12_DEPTH_STENCIL_DESC CItemEatUIShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CItemEatUIShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CItemEatUIShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CItemEatUIShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/F_.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CUIMesh(pd3dDevice, pd3dCommandList, 0.3, 0.4, -0.15, 0);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CItemEatUIShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CItemEatUIShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CItemEatUIShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CItemEatUIShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CItemEatUIShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CItemEatUIShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CItemEatUIShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CItemEatUIShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

//////////////////////////////////////////////////////////////////////

CGunUIShader_1::CGunUIShader_1() {
}
CGunUIShader_1::~CGunUIShader_1() {
}

D3D12_DEPTH_STENCIL_DESC CGunUIShader_1::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CGunUIShader_1::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CGunUIShader_1::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CGunUIShader_1::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/M4A1.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CGunUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CGunUIMesh(pd3dDevice, pd3dCommandList, 0, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CGunUIShader_1::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CGunUIShader_1::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CGunUIShader_1::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CGunUIShader_1::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CGunUIShader_1::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CGunUIShader_1::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CGunUIShader_1::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CGunUIShader_1::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}



CGunUIShader_2::CGunUIShader_2() {
}
CGunUIShader_2::~CGunUIShader_2() {
}

D3D12_DEPTH_STENCIL_DESC CGunUIShader_2::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CGunUIShader_2::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CGunUIShader_2::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CGunUIShader_2::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Items/M4A1.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CGunUIMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CGunUIMesh(pd3dDevice, pd3dCommandList, 1,  0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CGunUIShader_2::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CGunUIShader_2::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CGunUIShader_2::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CGunUIShader_2::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CGunUIShader_2::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CGunUIShader_2::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CGunUIShader_2::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CGunUIShader_2::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// 총알 숫자 띄우기
////////////////////////////////////////////////////////////

/// 앞숫자 1
CNumShader_1::CNumShader_1() {
}
CNumShader_1::~CNumShader_1() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader_1::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader_1::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader_1::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader_1::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num_1.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 0, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader_1::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader_1::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader_1::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader_1::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader_1::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader_1::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader_1::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader_1::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

// 앞숫자2
CNumShader_2::CNumShader_2() {
}
CNumShader_2::~CNumShader_2() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader_2::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader_2::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader_2::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader_2::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num_2.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 0, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader_2::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader_2::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader_2::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader_2::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader_2::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader_2::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader_2::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader_2::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 앞숫자3
CNumShader_3::CNumShader_3() {
}
CNumShader_3::~CNumShader_3() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader_3::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader_3::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader_3::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader_3::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num_3.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 0, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader_3::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader_3::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader_3::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader_3::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader_3::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader_3::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader_3::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader_3::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 앞숫자4
CNumShader_4::CNumShader_4() {
}
CNumShader_4::~CNumShader_4() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader_4::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader_4::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader_4::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader_4::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num_4.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 0, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader_4::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader_4::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader_4::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader_4::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader_4::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader_4::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader_4::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader_4::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 뒷자리 숫자


CNumShader0::CNumShader0() {
}
CNumShader0::~CNumShader0() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader0::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader0::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader0::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader0::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num0.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader0::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader0::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader0::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader0::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader0::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader0::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader0::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader0::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CNumShader1::CNumShader1() {
}
CNumShader1::~CNumShader1() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader1::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader1::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader1::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader1::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num1.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader1::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader1::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader1::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader1::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader1::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader1::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader1::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader1::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CNumShader2::CNumShader2() {
}
CNumShader2::~CNumShader2() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader2::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader2::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader2::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader2::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num2.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader2::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader2::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader2::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader2::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader2::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader2::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader2::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader2::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자3
CNumShader3::CNumShader3() {
}
CNumShader3::~CNumShader3() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader3::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader3::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader3::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader3::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num3.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader3::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader3::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader3::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader3::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader3::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader3::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader3::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader3::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자4
CNumShader4::CNumShader4() {
}
CNumShader4::~CNumShader4() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader4::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader4::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader4::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader4::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num4.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader4::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader4::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader4::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader4::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader4::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader4::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader4::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader4::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CNumShader5::CNumShader5() {
}
CNumShader5::~CNumShader5() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader5::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader5::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader5::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader5::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num5.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader5::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader5::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader5::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader5::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader5::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader5::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader5::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader5::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CNumShader6::CNumShader6() {
}
CNumShader6::~CNumShader6() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader6::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader6::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader6::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader6::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num6.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader6::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader6::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader6::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader6::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader6::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader6::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader6::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader6::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CNumShader7::CNumShader7() {
}
CNumShader7::~CNumShader7() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader7::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader7::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader7::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader7::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num7.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader7::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader7::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader7::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader7::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader7::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader7::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader7::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader7::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자3
CNumShader8::CNumShader8() {
}
CNumShader8::~CNumShader8() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader8::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader8::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader8::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CNumShader8::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num8.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader8::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader8::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader8::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader8::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader8::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader8::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader8::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader8::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자4
CNumShader9::CNumShader9() {
}
CNumShader9::~CNumShader9() {
}

D3D12_DEPTH_STENCIL_DESC CNumShader9::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CNumShader9::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CNumShader9::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CNumShader9::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num9.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CNumShader9::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CNumShader9::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CNumShader9::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CNumShader9::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CNumShader9::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CNumShader9::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CNumShader9::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CNumShader9::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

///////////////////////////////////////////////////// 나무숫자
/////////////////////////////////////////////////////////////////////////////////////////////////////
// 뒷자리 숫자


CTreeNumShader0::CTreeNumShader0() {
}
CTreeNumShader0::~CTreeNumShader0() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader0::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader0::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader0::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader0::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num0.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader0::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader0::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader0::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader0::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader0::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader0::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader0::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader0::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CTreeNumShader1::CTreeNumShader1() {
}
CTreeNumShader1::~CTreeNumShader1() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader1::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader1::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader1::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader1::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num1.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader1::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader1::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader1::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader1::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader1::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader1::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader1::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader1::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CTreeNumShader2::CTreeNumShader2() {
}
CTreeNumShader2::~CTreeNumShader2() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader2::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader2::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader2::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader2::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num2.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader2::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader2::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader2::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader2::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader2::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader2::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader2::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader2::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자3
CTreeNumShader3::CTreeNumShader3() {
}
CTreeNumShader3::~CTreeNumShader3() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader3::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader3::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader3::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader3::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num3.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader3::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader3::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader3::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader3::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader3::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader3::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader3::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader3::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자4
CTreeNumShader4::CTreeNumShader4() {
}
CTreeNumShader4::~CTreeNumShader4() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader4::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader4::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader4::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader4::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num4.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader4::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader4::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader4::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader4::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader4::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader4::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader4::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader4::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CTreeNumShader5::CTreeNumShader5() {
}
CTreeNumShader5::~CTreeNumShader5() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader5::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader5::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader5::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader5::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num5.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader5::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader5::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader5::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader5::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader5::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader5::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader5::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader5::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CTreeNumShader6::CTreeNumShader6() {
}
CTreeNumShader6::~CTreeNumShader6() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader6::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader6::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader6::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader6::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num6.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader6::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader6::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader6::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader6::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader6::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader6::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader6::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader6::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CTreeNumShader7::CTreeNumShader7() {
}
CTreeNumShader7::~CTreeNumShader7() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader7::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader7::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader7::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader7::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num7.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader7::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader7::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader7::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader7::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader7::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader7::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader7::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader7::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자3
CTreeNumShader8::CTreeNumShader8() {
}
CTreeNumShader8::~CTreeNumShader8() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader8::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader8::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader8::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}


void CTreeNumShader8::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num8.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader8::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader8::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader8::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader8::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader8::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader8::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader8::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader8::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


// 뒷숫자4
CTreeNumShader9::CTreeNumShader9() {
}
CTreeNumShader9::~CTreeNumShader9() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader9::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader9::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader9::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CTreeNumShader9::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num9.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 35, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader9::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader9::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader9::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader9::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader9::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader9::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader9::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader9::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

CTreeNumShader10::CTreeNumShader10() {
}
CTreeNumShader10::~CTreeNumShader10() {
}

D3D12_DEPTH_STENCIL_DESC CTreeNumShader10::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CTreeNumShader10::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CTreeNumShader10::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CTreeNumShader10::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Numbers/Num1.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CNumMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CNumMesh(pd3dDevice, pd3dCommandList, 34, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeNumShader10::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeNumShader10::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CTreeNumShader10::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeNumShader10::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CTreeNumShader10::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeNumShader10::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CTreeNumShader10::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CTreeNumShader10::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

///////////////////////////////////////////////////////////// 메인화면

CMainScreenShader::CMainScreenShader() {
}
CMainScreenShader::~CMainScreenShader() {
}

D3D12_DEPTH_STENCIL_DESC CMainScreenShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CMainScreenShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CMainScreenShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CMainScreenShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/MainScreen.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CMainScreenShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CMainScreenShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CMainScreenShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CMainScreenShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CMainScreenShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CMainScreenShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CMainScreenShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CMainScreenShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}


CMainScreenCheckShader::CMainScreenCheckShader() {
}
CMainScreenCheckShader::~CMainScreenCheckShader() {
}

D3D12_DEPTH_STENCIL_DESC CMainScreenCheckShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CMainScreenCheckShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CMainScreenCheckShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CMainScreenCheckShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/SelectIcon.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenCheckMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenCheckMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CMainScreenCheckShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CMainScreenCheckShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CMainScreenCheckShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CMainScreenCheckShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CMainScreenCheckShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CMainScreenCheckShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CMainScreenCheckShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CMainScreenCheckShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

CMainScreenCheck_1Shader::CMainScreenCheck_1Shader() {
}
CMainScreenCheck_1Shader::~CMainScreenCheck_1Shader() {
}

D3D12_DEPTH_STENCIL_DESC CMainScreenCheck_1Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CMainScreenCheck_1Shader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CMainScreenCheck_1Shader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CMainScreenCheck_1Shader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/SelectIcon.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenCheck_1Mesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenCheck_1Mesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CMainScreenCheck_1Shader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}
//
void CMainScreenCheck_1Shader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CMainScreenCheck_1Shader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CMainScreenCheck_1Shader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CMainScreenCheck_1Shader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CMainScreenCheck_1Shader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CMainScreenCheck_1Shader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CMainScreenCheck_1Shader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

/////// 플레이어 아이디 입력

CPlayerNameShader::CPlayerNameShader() {
}
CPlayerNameShader::~CPlayerNameShader() {
}

D3D12_DEPTH_STENCIL_DESC CPlayerNameShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CPlayerNameShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerNameShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CPlayerNameShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/jungleName.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CPlayerNameShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CPlayerNameShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CPlayerNameShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CPlayerNameShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CPlayerNameShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CPlayerNameShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CPlayerNameShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CPlayerNameShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

CGameOverShader::CGameOverShader() {
}
CGameOverShader::~CGameOverShader() {
}

D3D12_DEPTH_STENCIL_DESC CGameOverShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CGameOverShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CGameOverShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CGameOverShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/GameOver.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CGameOverShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CGameOverShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CGameOverShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CGameOverShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CGameOverShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CGameOverShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CGameOverShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CGameOverShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


////////////////////////////////////////////////////////////////////////////////////////
CChatScreen::CChatScreen() {
}
CChatScreen::~CChatScreen() {
}

D3D12_DEPTH_STENCIL_DESC CChatScreen::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CChatScreen::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CChatScreen::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CChatScreen::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/ChatScreen.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CChatScreen::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CChatScreen::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CChatScreen::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CChatScreen::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CChatScreen::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CChatScreen::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CChatScreen::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CChatScreen::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

////////////////
CChatScreenReady::CChatScreenReady() {
}
CChatScreenReady::~CChatScreenReady() {
}

D3D12_DEPTH_STENCIL_DESC CChatScreenReady::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CChatScreenReady::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CChatScreenReady::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CChatScreenReady::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/ChatScreenReady.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CChatScreenReady::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CChatScreenReady::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CChatScreenReady::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CChatScreenReady::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CChatScreenReady::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CChatScreenReady::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CChatScreenReady::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CChatScreenReady::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}
///////////////////////////

CBlueScreenShader::CBlueScreenShader() {
}
CBlueScreenShader::~CBlueScreenShader() {
}

D3D12_DEPTH_STENCIL_DESC CBlueScreenShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CBlueScreenShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VS_UI", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CBlueScreenShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PS_UI", "ps_5_1", ppd3dShaderBlob));
}

void CBlueScreenShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	float fxPitch = 12.0f * 3.5f;
	float fyPitch = 12.0f * 3.5f;
	float fzPitch = 12.0f * 3.5f;

	float fTerrainWidth = pTerrain->GetWidth();
	float fTerrainLength = pTerrain->GetLength();

	int xObjects = 1;
	int yObjects = 1;
	int zObjects = 1;
	//m_nTree = 4;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D_ARRAY, 0);
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/UI/blue.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nTree, 6);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTree, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 5, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CMainScreenMesh *pCubeMesh[1];

	for (int i = 0; i<m_nTree; ++i)
		pCubeMesh[i] = new CMainScreenMesh(pd3dDevice, pd3dCommandList, 1, 0.3, 0.4);

	m_ppTree = new CRotatingObject*;

	XMFLOAT3 xmf3RotateAxis, xmf3SurfaceNormal;
	CRotatingObject *pRotatingObject = NULL;
	for (int i = 0, x = 0; x < m_nTree; x++)
	{
		for (int z = 0; z < zObjects; z++)
		{
			for (int y = 0; y < yObjects; y++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(0, pCubeMesh[i]);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition = 800;
				float zPosition = 800;
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				//pRotatingObject->SetPosition(800, 0, 1000);
				if (y == 0)
				{
					xmf3SurfaceNormal = pTerrain->GetNormal(xPosition, zPosition);
					xmf3RotateAxis = Vector3::CrossProduct(XMFLOAT3(0.0f, 1.0f, 0.0f), xmf3SurfaceNormal);
					if (Vector3::IsZero(xmf3RotateAxis)) xmf3RotateAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
				}
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				m_ppTree[i++] = pRotatingObject;
			}
		}
	}
}

void CBlueScreenShader::ReleaseObjects()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) delete m_ppTree[j];
		delete[] m_ppTree;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CBlueScreenShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{
	for (int j = 0; j < m_nTree; j++)
	{
		m_ppTree[j]->Animate(fTimeElapsed);
	}
}

void CBlueScreenShader::ReleaseUploadBuffers()
{
	if (m_ppTree)
	{
		for (int j = 0; j < m_nTree; j++) if (m_ppTree[j]) m_ppTree[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CBlueScreenShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif
	//printf("%f\n", hp);
	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nTree; ++i)
	{
		if (m_ppTree[i]) m_ppTree[i]->Render(pd3dCommandList, pCamera);
	}
}

void CBlueScreenShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nTree, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CBlueScreenShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
}

void CBlueScreenShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}
	CTexturedShader::ReleaseShaderVariables();
}

D3D12_BLEND_DESC CBlueScreenShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

////////////////////////////

CTreeShader::CTreeShader()
{
}

CTreeShader::~CTreeShader()
{
}

void CTreeShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes * m_nObjects, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CTreeShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	for (int j = 0; j < m_nObjects; j++)
	{
		CB_GAMEOBJECT_INFO *pbMappedcbGameObject = (CB_GAMEOBJECT_INFO *)((UINT8 *)m_pcbMappedGameObjects + (j * ncbElementBytes));
		XMStoreFloat4x4(&pbMappedcbGameObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->m_xmf4x4World)));
#ifdef _WITH_BATCH_MATERIAL
		if (m_pMaterial) pbMappedcbGameObject->m_nMaterial = m_pMaterial->m_nReflection;
#endif
	}
}

void CTreeShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CIlluminatedTexturedShader::ReleaseShaderVariables();
}

//#define _WITH_TERRAIN_PARTITION

void CTreeShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	pTerrainCopy = pTerrain;
	int xObjects = MAX_BOX_SIZE, yObjects = 3, zObjects = 1, i = 0;

	m_nObjects = (xObjects ) * (yObjects ) * (zObjects );

	CMesh *soldier = NULL;
	ID3D12RootSignature *a = NULL;
	Model3D SoldierModel;
	std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC*> meshSRV;
	std::vector<std::wstring> textureNameArray1;
	//
	LoadMD5Model(pd3dDevice, pd3dCommandList, a, L"../Assets/Model/Soldier_Mesh.MD5MESH", SoldierModel, meshSRV, textureNameArray1, soldier);
	//CGameObject *TreeObject = NULL;
	m_pMaterial = new CMaterial();
	CTexture *pTextures = new CTexture(6, RESOURCE_TEXTURE2DARRAY, 0);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 0);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 1);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 4);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 3);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 2);
	pTextures->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"../Assets/Image/Building/box.dds", 5);

	m_pMaterial->SetTexture(pTextures);


	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	//CGameObject *A;
	//ID3D12Resource *pd3dcbResource = A->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTextures, 5, false);

//#ifdef _WITH_BATCH_MATERIAL
//	m_pMaterial = new CMaterial();
//	m_pMaterial->SetTexture(pTextures);
//	m_pMaterial->SetReflection(1);
//#else
//	CMaterial *pCubeMaterial = new CMaterial();
//	pCubeMaterial->SetTexture(pTexture);
//	pCubeMaterial->SetReflection(1);
//#endif

	////
	CCubeMeshIlluminatedTextured *pCubeMesh = new CCubeMeshIlluminatedTextured(pd3dDevice, pd3dCommandList, 10, 10, 10);
	
	m_ppObjects = new CGameObject*[m_nObjects];
	
	CGameObject *pRotatingObject = NULL;
	for (int x = 0; x < xObjects; x++)
	{
		for (int y = 0; y < yObjects; y++)
		{
			for (int z = 0; z < zObjects; z++)
			{
				pRotatingObject = new CGameObject();
				pRotatingObject->SetMesh(0, pCubeMesh);
#ifndef _WITH_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				float xPosition;
				float zPosition;
				if (i == 0) xPosition = 680, zPosition = 925;
				else if (i == 1) xPosition = 715, zPosition = 853;
				else if (i == 2) xPosition = 641, zPosition = 858;
				else if (i == 3) xPosition = 641, zPosition = 858;
				else if (i == 4) xPosition = 639, zPosition = 794;
				else if (i == 5) xPosition = 577, zPosition = 800;
				else if (i == 6) xPosition = 516, zPosition = 822;
				else if (i == 7) xPosition = 463, zPosition = 875;
				else if (i == 8) xPosition = 487, zPosition = 951;
				else if (i == 9) xPosition = 538, zPosition = 969;
				else if (i == 10) xPosition = 465, zPosition = 766;
				else if (i == 11) xPosition = 518, zPosition = 757;
				else if (i == 12) xPosition = 436, zPosition = 795;
				else if (i == 13) xPosition = 426, zPosition = 729;
				else if (i == 14) xPosition = 385, zPosition = 876;
				else if (i == 15) xPosition = 396, zPosition = 968;
				else if (i == 16) xPosition = 338, zPosition = 970;
				else if (i == 17) xPosition = 390, zPosition = 1048;
				else if (i == 18) xPosition = 469, zPosition = 1106;
				else if (i == 19) xPosition = 563, zPosition = 1078;
				else if (i == 20) xPosition = 653, zPosition = 1029;
				else if (i == 21) xPosition = 706, zPosition = 1080;
				else if (i == 22) xPosition = 737, zPosition = 1036;
				else if (i == 23) xPosition = 778, zPosition = 957;
				else if (i == 24) xPosition = 667, zPosition = 1257;
				else if (i == 25) xPosition = 574, zPosition = 1272;
				else if (i == 26) xPosition = 484, zPosition = 1269;
				else if (i == 27) xPosition = 504, zPosition = 1139;
				else if (i == 28) xPosition = 602, zPosition = 1122;
				else if (i == 29) xPosition = 851, zPosition = 984;
				
				float fHeight = pTerrain->GetHeight(xPosition, zPosition);
				pRotatingObject->SetPosition(xPosition, fHeight, zPosition);
				//pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				//pRotatingObject->SetRotationSpeed(10.0f * (i % 10)+3.0f);
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * i));
				pRotatingObject->SetScale(2.0, 15.0, 2.0);
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
}

void CTreeShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) delete m_ppObjects[j];
		delete[] m_ppObjects;
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) delete m_pMaterial;
#endif
}

void CTreeShader::SetBoxPosition(int index, XMFLOAT3 input) {
	float fHeight = pTerrainCopy->GetHeight(input.x, input.z);
	m_ppObjects[index]->SetPosition(input.x, fHeight + 4, input.z);

}


void CTreeShader::AnimateObjects(float fTimeElapsed, CCamera *pCamera)
{

	for (int i = 0; i < 10; ++i) {
		m_ppObjects[i]->Animate(fTimeElapsed);
	}
}

void CTreeShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->ReleaseUploadBuffers();
	}

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CTreeShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CIlluminatedTexturedShader::Render(pd3dCommandList, pCamera);

#ifdef _WITH_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
#endif

	for (int j = 0; j < m_nObjects; j++)
	{
		;//if (m_ppObjects[j]) m_ppObjects[j]->Render(pd3dCommandList, pCamera);
	}
}

// 인스턴싱
CInstancingShader::CInstancingShader()
{

}
CInstancingShader::~CInstancingShader()
{

}

D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 3;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	//정점 정보를 위한 입력 원소이다. 
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	//UINT nInputElementDescs = 7;
	//D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
	//	D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	////정점 정보를 위한 입력 원소이다.
	//pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	//D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	//pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
	//D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	////인스턴싱 정보를 위한 입력 원소이다.
	//pd3dInputElementDescs[2] = { "WORLDMATRIX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0,
	//D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	//pd3dInputElementDescs[3] = { "WORLDMATRIX", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
	//D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	//pd3dInputElementDescs[4] = { "WORLDMATRIX", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32,
	//D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	//pd3dInputElementDescs[5] = { "WORLDMATRIX", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48,
	//D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	//pd3dInputElementDescs[6] = { "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
	//64, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(TEXT("Shaders.hlsl"), "VSInstancing", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSInstancing", "ps_5_1", ppd3dShaderBlob));
}

void CInstancingShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateTexturedShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateTextureShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShadowShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShadowShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	//인스턴스 정보를 저장할 정점 버퍼를 업로드 힙 유형으로 생성한다. 
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, sizeof(CB_INSTANCE_INFO) * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	//D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
	//D3D12_RESOURCE_STATE_GENERIC_READ
	//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다.
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&cbMappedGameObjects);
}

void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->SetGraphicsRootShaderResourceView(9, m_pd3dcbGameObjects->GetGPUVirtualAddress());

	for (int i = 0; i < m_GameObjects.size(); ++i) {
		XMStoreFloat4x4(&cbMappedGameObjects[i].f4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_GameObjects[i]->GetWMatrix())));
	}
}

void CInstancingShader::ReleaseShaderVariables()
{
	if (m_pd3dcbGameObjects)
	{
		m_pd3dcbGameObjects->Unmap(0, NULL);
		m_pd3dcbGameObjects->Release();
	}

	CTexturedShader::ReleaseShaderVariables();
}

void CInstancingShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pContext)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0); //6개의 텍스쳐를 사용
	pTexture->LoadTextureFromFile(pd3dDevice, pd3dCommandList, L"Assets/Image/Trees/Tree01.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_INSTANCE_INFO) + 255) & ~255);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	//CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nTrees, m_pd3dcbGameObjects, ncbElementBytes);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, pTexture, 8, false);

#ifdef _WITH_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pTreeMaterial = new CMaterial();
	pTreeMaterial->SetTexture(pTexture);
#endif

	CTexturedRectMesh *pTreeMesh = new CTexturedRectMesh(pd3dDevice, pd3dCommandList, 60.0f, 60.0f, 0.0f);

	//m_ppTrees = new CGameObject*[m_nTrees];

	//BillboardObject *pRotatingObject = NULL;

}

void CInstancingShader::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CMesh*& pMesh)
{
	m_nObjects = 400;
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Static/Tree/Trees3x3Map.dds"), 0);

	UINT ncbElementBytes = ((sizeof(CB_INSTANCE_INFO) + 255) & ~255);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(tex);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 8, true);

	CGameObject* tempObj = NULL;
	int i = 0;
	//for (int y = 0; y < MAPSIZE; ++y) {
	//	for (int x = 0; x < MAPSIZE; ++x) {
	//		//for (int i = 0; i < m_GameObjects.capacity(); ++i) {
	//		if (CMapData::GET_SINGLE()->Stage1[y][x] == READ_DATA::TREE) {
	//			tempObj = new CTree();
	//			tempObj->SetWPosition(30 * x, 0, -30 * y);
	//			tempObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize *i));
	//			m_GameObjects.push_back(tempObj);
	//			++i;
	//		}
	//		//}
	//	}

	//}

	if (m_GameObjects.size() > 0) {
		m_GameObjects[0]->SetMaterial(m_pMaterial);
		m_GameObjects[0]->SetMesh(0, pMesh);
	}
}

void CInstancingShader::InitializeStone(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CMesh*& pMesh)
{
	m_nObjects = 400;
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Static/Stone/stone.dds"), 0);

	UINT ncbElementBytes = ((sizeof(CB_INSTANCE_INFO) + 255) & ~255);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(tex);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 8, true);

	CGameObject* tempObj = NULL;
	int i = 0;
	//for (int y = 0; y < MAPSIZE; ++y) {
	//	for (int x = 0; x < MAPSIZE; ++x) {
	//		//for (int i = 0; i < m_GameObjects.capacity(); ++i) {
	//		if (CMapData::GET_SINGLE()->Stage1[y][x] != READ_DATA::TREE) {
	//			tempObj = new CStone();
	//			tempObj->SetWPosition(30 * x - 15, 0, -30 * y + 10);
	//			tempObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize *i));
	//			m_GameObjects.push_back(tempObj);
	//			++i;
	//		}
	//		//}
	//	}

	//}

	if (m_GameObjects.size() > 0) {
		m_GameObjects[0]->SetMaterial(m_pMaterial);
		m_GameObjects[0]->SetMesh(0, pMesh);
	}
}

void CInstancingShader::InitializeStone2(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CMesh *& pMesh)
{
	m_nObjects = 400;
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Static/Stone/stone.dds"), 0);

	UINT ncbElementBytes = ((sizeof(CB_INSTANCE_INFO) + 255) & ~255);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(tex);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 8, true);

	CGameObject* tempObj = NULL;
	int i = 0;
	//for (int y = 0; y < MAPSIZE; ++y) {
	//	for (int x = 0; x < MAPSIZE; ++x) {
	//		//for (int i = 0; i < m_GameObjects.capacity(); ++i) {
	//		if (CMapData::GET_SINGLE()->Stage1[y][x] != READ_DATA::TREE) {
	//			tempObj = new CStone();
	//			tempObj->SetWPosition(30 * x - 10, 0, -30 * y + 5);
	//			tempObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize *i));
	//			m_GameObjects.push_back(tempObj);
	//			++i;
	//		}
	//		//}
	//	}

	//}

	if (m_GameObjects.size() > 0) {
		m_GameObjects[0]->SetMaterial(m_pMaterial);
		m_GameObjects[0]->SetMesh(0, pMesh);
	}
}

void CInstancingShader::InitializeStone3(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CMesh *& pMesh)
{
	m_nObjects = 400;
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Static/Stone/stone.dds"), 0);

	UINT ncbElementBytes = ((sizeof(CB_INSTANCE_INFO) + 255) & ~255);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(tex);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 8, true);

	//CGameObject* tempObj = NULL;
	//int i = 0;
	//for (int y = 0; y < MAPSIZE; ++y) {
	//	for (int x = 0; x < MAPSIZE; ++x) {
	//		//for (int i = 0; i < m_GameObjects.capacity(); ++i) {
	//		if (CMapData::GET_SINGLE()->Stage1[y][x] != READ_DATA::TREE) {
	//			tempObj = new CStone();
	//			tempObj->SetWPosition(30 * x + 10, 0, -30 * y + 7);
	//			tempObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize *i));
	//			m_GameObjects.push_back(tempObj);
	//			++i;
	//		}
	//		//}
	//	}

	//}

	if (m_GameObjects.size() > 0) {
		m_GameObjects[0]->SetMaterial(m_pMaterial);
		m_GameObjects[0]->SetMesh(0, pMesh);
	}
}

void CInstancingShader::InitializeMush(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CMesh*& pMesh)
{
	m_nObjects = 400;
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Static/Mushroom/Mushroom3.dds"), 0);

	UINT ncbElementBytes = ((sizeof(CB_INSTANCE_INFO) + 255) & ~255);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(tex);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 8, true);

	CGameObject* tempObj = NULL;
	int i = 0;
	//for (int y = 0; y < MAPSIZE; ++y) {
	//	for (int x = 0; x < MAPSIZE; ++x) {
	//		//for (int i = 0; i < m_GameObjects.capacity(); ++i) {
	//		if (CMapData::GET_SINGLE()->Stage1[y][x] != READ_DATA::TREE) {
	//			tempObj = new CMushroom();
	//			tempObj->SetWPosition(30 * x - 15, 0, -30 * y - 5);
	//			tempObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize *i));
	//			m_GameObjects.push_back(tempObj);
	//			++i;
	//		}
	//		//}
	//	}

	//}

	if (m_GameObjects.size() > 0) {
		m_GameObjects[0]->SetMaterial(m_pMaterial);
		m_GameObjects[0]->SetMesh(0, pMesh);
	}
}

void CInstancingShader::InitializeShadow(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, CMesh*& pMesh)
{
	m_nObjects = 400;

	XMVECTOR shadowPlane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz평면
	XMVECTOR toMainLight = -XMLoadFloat3(new XMFLOAT3(-0.5f, -1.0f, 0.0f));
	XMMATRIX S = XMMatrixShadow(shadowPlane, toMainLight);
	XMMATRIX shadowOffSetY = XMMatrixTranslation(0.0f, 0.001f, 0.0f);

	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Static/Tree/Trees3x3Map.dds"), 0);

	UINT ncbElementBytes = ((sizeof(CB_INSTANCE_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 8, true);

	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(tex);

	CGameObject* tempObj = NULL;
	int i = 0;
	//for (int y = 0; y < MAPSIZE; ++y) {
	//	for (int x = 0; x < MAPSIZE; ++x) {
	//		//for (int i = 0; i < m_GameObjects.capacity(); ++i) {
	//		if (CMapData::GET_SINGLE()->Stage1[y][x] == READ_DATA::TREE) {
	//			tempObj = new CTree();
	//			tempObj->SetWPosition(30 * x, 0, -30 * y);
	//			tempObj->SetCbvGPUDescriptorHandlePtr(m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize *i));
	//			XMFLOAT4X4 TempShadow = tempObj->GetWMatrix();
	//			tempObj->SetWMatrix(Matrix4x4::Multiply(TempShadow, S*shadowOffSetY));
	//			m_GameObjects.push_back(tempObj);
	//			++i;
	//		}
	//		//}
	//	}

	//}
	if (m_GameObjects.size() > 0) {
		m_GameObjects[0]->SetMaterial(m_pMaterial);
		m_GameObjects[0]->SetMesh(0, pMesh);
	}
}
void CInstancingShader::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	CTexturedShader::Render(pd3dCommandList, pCamera);

	if (m_pMaterial) m_pMaterial->UpdateShaderVariables(pd3dCommandList);
	UpdateShaderVariables(pd3dCommandList);

	m_GameObjects[0]->Render(pd3dCommandList, pCamera, m_GameObjects.size());
}