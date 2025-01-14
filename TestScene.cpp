#include "stdafx.h"
#include "TestScene.h"
#include "Object.h"
#include "ColorShader.h"

void TestScene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, ID3D12RootSignature* root_signature)
{
	BuildShader(device, root_signature);

}

void TestScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	//씬에서 사용하는 쉐이더 개수만큼 예약
	int shader_count = 1;
	shaders_.reserve(shader_count);

	for (int i = 0; i < shader_count; ++i)
	{
		shaders_.push_back(std::make_unique<ColorShader>());
		shaders_[i]->CreateShader(device, root_signature);
	}
	
}

void TestScene::BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
}

void TestScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
}

void TestScene::BuildFrameResources(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
}

void TestScene::BuildDescriptorHeap(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
}
