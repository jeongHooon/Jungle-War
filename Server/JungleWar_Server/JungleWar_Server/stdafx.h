// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib,"ws2_32")

//#pragma comment(lib,"d3dcompiler.lib")
//#pragma comment(lib, "D3D12.lib")
//#pragma comment(lib, "dxgi.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <thread>
#include <Windows.h>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <d3d12.h>
#include <DirectXMath.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string> 
#include <wrl.h> 
#include <mutex>
#include <stdlib.h>
#include <DirectXCollision.h>
#include "protocol.h"


#define EPSILON 1.0e-8f
#define UPDATE_TIME 0.009f
#define OBB_SCALE_PLAYER_X		5.f
#define OBB_SCALE_PLAYER_Y		8.f
#define OBB_SCALE_PLAYER_Z		5.f

#define OBB_SCALE_BULLET_X		1.f
#define OBB_SCALE_BULLET_Y		1.f
#define OBB_SCALE_BULLET_Z		1.f

#define OBB_SCALE_BOX_X			10.f
#define OBB_SCALE_BOX_Y			10.f
#define OBB_SCALE_BOX_Z			10.f

#define OBB_SCALE_TREE_X			1.f
#define OBB_SCALE_TREE_Y			10.f
#define OBB_SCALE_TREE_Z			1.f

#define OBB_SCALE_STONE_X			13.f
#define OBB_SCALE_STONE_Y			8.f
#define OBB_SCALE_STONE_Z			13.f


#define AR_SHOOTER				0.2f	// AR 연사속도
#define AR_SPEED				1000.f	// AR 탄속
#define PLAYER_HEIGHT			10.f

#define ITEM_GEN_TIME			2.f

using namespace std::chrono;

using namespace DirectX;
using namespace std;
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

struct OverlappedExtensionSet {
	WSAOVERLAPPED wsa_over;
	bool is_recv;
	WSABUF wsabuf;

	// Move패킷 이용
	int shooter_player_id;
	int box_player_id;
	char command = 0;
	char io_buffer[MAX_BUFFER_SIZE];
	float elapsed_time;
};

struct Client {
	///////////////////////////////
	char userid[10];
	char chat[20];
	///////////////////////////////

	SOCKET s;
	bool in_use;

	float x, y, z;			// 1km * 1km
	bool is_ready;

	bool is_move_foward;
	bool is_move_backward;
	bool is_move_left;
	bool is_move_right;
	bool is_running;		// true : 달리기, false : 걷기

	bool is_left_click;
	bool is_right_click;

	bool is_q;

	bool is_jump;

	bool is_die;

	bool is_crouch;
	float hp;
	int box_count = 0;

	float elecX;
	float elecY;
	float elecZ;

	int CType;
	Team team;
	ARWeapons ar_weapons;
	char ar_mag = 0;		// 탄창
	SubWeapons sub_weapons;
	char sub_mag = 0;
	// Look Vector
	bool boat_parts[4];		// 0 : 하판, 1 : 상판, 2 : 엔진, 3 : 조향장치

	OverlappedExtensionSet overlapped_ex;
	int packet_size;
	int prev_packet_size;
	char prev_packet[MAX_PACKET_SIZE];

	int boxCount;

	XMFLOAT3 look_vec;
	mutex client_lock;

	BoundingOrientedBox bounding_box;
	void SetOOBBXXXX(XMFLOAT3 xmCenter, XMFLOAT3 xmExtents, XMFLOAT4 xmOrientation) {
	}
	void SetOOBB(XMFLOAT3 xmCenter, XMFLOAT3 xmExtents, XMFLOAT4 xmOrientation) {
		bounding_box = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
		//printf("테스트 : ,, %f ,,, %f ,,, %f\n", xmCenter.x, xmExtents.x, xmOrientation.x);
	}
};

struct Bullet {
	bool in_use = false;
	float x, y, z;

	// type 1 : AR
	// type 2 : 권총
	XMFLOAT3 look_vec;
	int type;
	bool is_bound = false;
	BoundingOrientedBox bounding_box;

	int shooter_id;

	void SetOOBB(XMFLOAT3 xmCenter, XMFLOAT3 xmExtents, XMFLOAT4 xmOrientation) {
		bounding_box = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
		//printf("테스트 : ,, %f ,,, %f ,,, %f\n", xmCenter.x, xmExtents.x, xmOrientation.x);
	}
};

struct Box {
	bool in_use = false;
	bool is_send = false;
	float x, y, z;

	int boxindex;


	XMFLOAT3 look_vec;
	int type;
	bool is_bound = false;
	float hp;
	BoundingOrientedBox bounding_box;

	void SetOOBB(XMFLOAT3 xmCenter, XMFLOAT3 xmExtents, XMFLOAT4 xmOrientation) {
		bounding_box = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
	}
};

struct Map_Object {

	bool in_use = true;
	bool is_send = false;
	float x, y, z;

	bool state;

	XMFLOAT3 look_vec;
	int type;
	int id;
	bool is_bound = false;
	float hp;
	BoundingOrientedBox bounding_box;

	void SetOOBB(XMFLOAT3 xmCenter, XMFLOAT3 xmExtents, XMFLOAT4 xmOrientation) {
		bounding_box = BoundingOrientedBox(xmCenter, xmExtents, xmOrientation);
	}
};

inline bool IsZero(float fValue) { return((fabsf(fValue) < EPSILON)); }


//3차원 벡터의 연산
namespace Vector3 {

	inline bool IsZero(XMFLOAT3& xmf3Vector)
	{
		if (::IsZero(xmf3Vector.x) && ::IsZero(xmf3Vector.y) && ::IsZero(xmf3Vector.z))
			return(true);
		return(false);
	}
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}
	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar); return(xmf3Result);
	}
	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}
	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}
	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}
	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}
	inline XMFLOAT3 Normalize(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}
	inline float Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}
	inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}
	//inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	//{
	//	return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	//}
	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result; XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}
	//inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	//{
	//	return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	//}
}

namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition, XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixLookAtLH(XMLoadFloat3(&xmf3EyePosition), XMLoadFloat3(&xmf3LookAtPosition), XMLoadFloat3(&xmf3UpDirection)));
		return(xmmtx4x4Result);
	}
}


void Rotating(XMFLOAT3* axis, float angle) {
	XMFLOAT4X4						m_xmf4x4ToParentTransform;

	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(axis), XMConvertToRadians(angle));
	m_xmf4x4ToParentTransform = Matrix4x4::Multiply(mtxRotate, m_xmf4x4ToParentTransform);

}



