#include "stdafx.h"

#include "modelclass.h"


ModelClass::ModelClass()
{
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename)
{
	// 정점 및 인덱스 버퍼를 초기화합니다.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// 이 모델의 텍스처를 로드합니다.
	return LoadTexture(device, textureFilename);
}


void ModelClass::Shutdown()
{
	// 모델 텍스쳐를 반환합니다.
	ReleaseTexture();

	// 버텍스 및 인덱스 버퍼를 종료합니다.
	ShutdownBuffers();
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
	RenderBuffers(deviceContext);
}


int ModelClass::GetVertexCount()
{
	return m_vertexCount;
}


int ModelClass::GetInstanceCount()
{
	return m_instanceCount;
}


ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	// 정점 배열의 정점 수를 설정합니다.
	m_vertexCount = 3;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 정점 배열에 값을 설정합니다.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

}
