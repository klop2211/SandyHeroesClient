// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

// STL 헤더 파일
#include <vector>
#include <list>
#include <array>
#include <memory>

// DirectX 관련 헤더 파일 및 선언문
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include "d3dx12.h"
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

// 상수값
const int kDefaultFrmaeBufferWidth = 1366;
const int kDefaultFrameBufferHeight = 768;
const UINT kDefaultRefreshRate = 60;

// 유틸 함수
namespace d3d_util
{
	// 업로드 버퍼를 이용하여 디폴트버퍼 생성
	// 주의: 이 함수가 실행되고 gpu 명령이 전부 실행된 뒤 upload_buffer를 해제해야한다.
	ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,						
		ID3D12GraphicsCommandList* command_list,
		const void* init_data,
		UINT64 byte_size,
		ComPtr<ID3D12Resource>& upload_buffer);

}

namespace xmath_util_float3
{
	inline XMFLOAT3 Add(const XMFLOAT3& vector1, const XMFLOAT3& vector2) 
	{ 
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMLoadFloat3(&vector1) + XMLoadFloat3(&vector2));
		return r_value;
	}

	inline XMFLOAT3 Subtract(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMLoadFloat3(&vector1) - XMLoadFloat3(&vector2));
		return r_value;
	}

	inline XMFLOAT3 ScalarProduct(const XMFLOAT3& vector, float scalar)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMLoadFloat3(&vector) * scalar);
		return r_value;
	}

	inline float DotProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3Dot(XMLoadFloat3(&vector1), XMLoadFloat3(&vector1)));
		return r_value.x;
	}

	inline XMFLOAT3 CrossProduct(const XMFLOAT3& vector1, const XMFLOAT3& vector2)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3Cross(XMLoadFloat3(&vector1), XMLoadFloat3(&vector1)));
		return r_value;
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& vector)
	{
		XMFLOAT3 r_value;
		XMStoreFloat3(&r_value, XMVector3Normalize(XMLoadFloat3(&vector)));
		return r_value;
	}

}

namespace xmath_util_float4
{
	inline XMFLOAT4 Add(const XMFLOAT4& vector1, const XMFLOAT4& vector2)
	{
		XMFLOAT4 r_value;
		XMStoreFloat4(&r_value, XMLoadFloat4(&vector1) + XMLoadFloat4(&vector2));
		return r_value;
	}

	inline XMFLOAT4 Subtract(const XMFLOAT4& vector1, const XMFLOAT4& vector2)
	{
		XMFLOAT4 r_value;
		XMStoreFloat4(&r_value, XMLoadFloat4(&vector1) - XMLoadFloat4(&vector2));
		return r_value;
	}

	inline XMFLOAT4 ScalarProduct(const XMFLOAT4& vector, float scalar)
	{
		XMFLOAT4 r_value;
		XMStoreFloat4(&r_value, XMLoadFloat4(&vector) * scalar);
		return r_value;
	}

}

namespace xmath_util_float4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMMatrixIdentity());
		return r_value;
	}

	inline XMFLOAT4X4 Multiply(const XMFLOAT4X4& matrix1, const XMFLOAT4X4& matrix2)
	{
		XMFLOAT4X4 r_value;
		XMStoreFloat4x4(&r_value, XMLoadFloat4x4(&matrix1) * XMLoadFloat4x4(&matrix2));
		return r_value;
	}
}

// xmf 관련 연산자 오버로딩
inline XMFLOAT3 operator+(const XMFLOAT3& lhs, const XMFLOAT3& rhs) { return xmath_util_float3::Add(lhs, rhs); }
inline void operator+=(XMFLOAT3& lhs, const XMFLOAT3& rhs) { lhs = xmath_util_float3::Add(lhs, rhs); }
inline XMFLOAT3 operator-(const XMFLOAT3& lhs, const XMFLOAT3& rhs) { return xmath_util_float3::Subtract(lhs, rhs); }
inline XMFLOAT3 operator*(const XMFLOAT3& lhs, const float& rhs) { return xmath_util_float3::ScalarProduct(lhs, rhs); }
inline XMFLOAT4X4 operator*(const XMFLOAT4X4& lhs, const XMFLOAT4X4& rhs) { return xmath_util_float4x4::Multiply(lhs, rhs); }
