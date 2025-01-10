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

using Microsoft::WRL::ComPtr;

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
