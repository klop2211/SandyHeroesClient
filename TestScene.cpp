#include "stdafx.h"
#include "TestScene.h"
#include "Object.h"
#include "ColorShader.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "Mesh.h"
#include "CubeMesh.h"
#include "MeshComponent.h"
#include "CameraComponent.h"
#include "InputControllerComponent.h"
#include "TestControllerComponent.h"
#include "InputManager.h"
#include "ModelInfo.h"
#include "SkinnedMeshShader.h"
#include "SkinnedMesh.h"
#include "Material.h"
#include "StandardMeshShader.h"
#include "StandardSkinnedMeshShader.h"
#include "AnimationSet.h"

void TestScene::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* command_list, 
	ID3D12RootSignature* root_signature, FrameResourceManager* frame_resource_manager,
	DescriptorManager* descriptor_manager)
{
	frame_resource_manager_ = frame_resource_manager;
	descriptor_manager_ = descriptor_manager;

	BuildShader(device, root_signature);
	BuildMesh(device, command_list);
	BuildMaterial(device, command_list);
	BuildObject(device, command_list);
	BuildFrameResources(device);
	BuildDescriptorHeap(device);
	BuildConstantBufferViews(device);
	BuildShaderResourceViews(device);
}

void TestScene::BuildShader(ID3D12Device* device, ID3D12RootSignature* root_signature)
{
	//씬에서 사용하는 쉐이더 개수만큼 예약
	int shader_count = 2;
	shaders_.reserve(shader_count);
	shaders_.push_back(std::make_unique<StandardMeshShader>());
	shaders_.push_back(std::make_unique<StandardSkinnedMeshShader>());

	for (int i = 0; i < shader_count; ++i)
	{
		shaders_[i]->CreateShader(device, root_signature);
	}
	
}

void TestScene::BuildMesh(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	meshes_.reserve(2);
	meshes_.push_back(std::make_unique<CubeMesh>(XMFLOAT4(0, 1, 0, 1)));
	meshes_[0].get()->set_name("green_cube");

	model_infos_.reserve(1);
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Ellen.bin", meshes_, materials_));

	for (const std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->CreateShaderVariables(device, command_list);
	}
}

void TestScene::BuildMaterial(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	int frame_resource_index = 0;
	for (std::unique_ptr<Material>& material : materials_)
	{
		material->CreateShaderVariables(device, command_list);
		material->set_frame_resource_index(frame_resource_index);
		++frame_resource_index;
	}
}

void TestScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	//TODO: 각 메쉬의 컴포넌트 연결 개수를 파악하면 아래 수치를 디테일하게 설정할 수 있을것 같다..
	cb_object_capacity_ = 1000;
	cb_skinned_mesh_object_capacity_ = 100;


	////오브젝트를 생성하고
	//Object* cube_object = new Object();
	//Mesh* green_cube = Scene::FindMesh("green_cube", meshes_);
	////메쉬컴포넌트를 원하는 오브젝트와 메쉬를 넣어 생성한다.
	//MeshComponent* cube_component = new MeshComponent(cube_object, green_cube);
	//cube_object->AddComponent(cube_component);
	////이를 씬의 오브젝트리스트에 추가
	//object_list_.emplace_back();
	//object_list_.back().reset(cube_object);

	Object* temp = model_infos_[0]->GetInstance();
	temp->set_position_vector(XMFLOAT3{ 0, 0, 0 });
	object_list_.emplace_back();
	object_list_.back().reset(temp);

	Object* temp1 = Object::DeepCopy(temp);
	temp1->set_position_vector(XMFLOAT3{ 0.6, 0, 0 });
	object_list_.emplace_back();
	object_list_.back().reset(temp1);

	//인풋 처리 컨트롤러 생성
	TestControllerComponent* controller = new TestControllerComponent(temp);
	//메인 컨트롤러로 설정
	main_input_controller_ = controller;
	temp->AddComponent(controller);


	Object* camera_object = new Object();
	CameraComponent* camera_component = 
		new CameraComponent(camera_object, 0.3, 10000, 
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);

	camera_object->set_position_vector(XMFLOAT3(0, 1.7, -1));

	object_list_.emplace_back();
	object_list_.back().reset(camera_object);
	
	main_camera_ = camera_component;

}

void TestScene::BuildFrameResources(ID3D12Device* device)
{
	frame_resource_manager_->ResetFrameResources(device, 1,
		cb_object_capacity_, 
		cb_skinned_mesh_object_capacity_, materials_.size());
}

void TestScene::BuildDescriptorHeap(ID3D12Device* device)
{
	descriptor_manager_->ResetDescriptorHeap(device, 
		Material::GetTextureCount());
}

void TestScene::BuildConstantBufferViews(ID3D12Device* device)
{
	//25.02.23 수정
	//루트 디스크립터 테이블에서 루트 CBV로 변경함에 따라 
	//더이상 이 함수를 통해 CBV를 생성하지 않음

	//UINT object_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBObject));

	//UINT bone_transform_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBBoneTransform));
	//UINT bone_transform_offset = descriptor_manager_->cbv_bone_transform_offset();

	//UINT pass_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBPass));

	////각 프레임리소스별로 cbv가 필요하다
	//for (int frame_index = 0; frame_index < FrameResourceManager::kFrameCount; ++frame_index)
	//{
	//	//프레임리소스에서 원하는 상수버퍼의 리소스를 가져온다.
	//	ID3D12Resource* object_cb =
	//		frame_resource_manager_->GetResource(frame_index)->cb_object->Resource();

	//	D3D12_GPU_VIRTUAL_ADDRESS cb_adress = object_cb->GetGPUVirtualAddress();
	//	//모든 오브젝트에 대해 cbv가 필요하다.
	//	for (UINT i = 0; i < cb_object_capacity_; ++i)
	//	{
	//		// 디스크립터 힙 오프셋
	//		int heap_index = cb_object_capacity_ * frame_index + i;
	//		D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptor_manager_->GetCpuHandle(heap_index);

	//		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
	//		cbv_desc.BufferLocation = cb_adress;
	//		cbv_desc.SizeInBytes = object_cb_size;

	//		device->CreateConstantBufferView(&cbv_desc, handle);

	//		// 다음 오브젝트의 상수버퍼 어드레스
	//		cb_adress += object_cb_size;
	//	}

	//	// 위 오브젝트와 동일한 방법으로 만들어준다.
	//	ID3D12Resource* bone_transform_cb =
	//		frame_resource_manager_->GetResource(frame_index)->cb_bone_transform->Resource();

	//	cb_adress = bone_transform_cb->GetGPUVirtualAddress();
	//	for (UINT i = 0; i < cb_skinned_mesh_object_capacity_; ++i)
	//	{
	//		int heap_index = descriptor_manager_->cbv_bone_transform_offset() +
	//			cb_skinned_mesh_object_capacity_ * frame_index + i;
	//		D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptor_manager_->GetCpuHandle(heap_index);

	//		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
	//		cbv_desc.BufferLocation = cb_adress;
	//		cbv_desc.SizeInBytes = bone_transform_cb_size;

	//		device->CreateConstantBufferView(&cbv_desc, handle);

	//		cb_adress += bone_transform_cb_size;
	//	}

	//	ID3D12Resource* pass_cb =
	//		frame_resource_manager_->GetResource(frame_index)->cb_pass->Resource();

	//	cb_adress = pass_cb->GetGPUVirtualAddress();

	//	int heap_index = descriptor_manager_->cbv_pass_offset() + frame_index;
	//	D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptor_manager_->GetCpuHandle(heap_index);

	//	D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
	//	cbv_desc.BufferLocation = cb_adress;
	//	cbv_desc.SizeInBytes = pass_cb_size;

	//	device->CreateConstantBufferView(&cbv_desc, handle);

	//}

}

void TestScene::BuildShaderResourceViews(ID3D12Device* device)
{
	int heap_index = descriptor_manager_->srv_offset();
	for (std::unique_ptr<Material>& material : materials_)
	{
		heap_index = material->CreateShaderResourceViews(device, descriptor_manager_, heap_index);
	}
}

void TestScene::Render(ID3D12GraphicsCommandList* command_list)
{
	main_camera_->UpdateCameraInfo();

	CBPass cb_pass{};
	cb_pass.view_matrix = xmath_util_float4x4::TransPose(main_camera_->view_matrix());
	cb_pass.proj_matrix = xmath_util_float4x4::TransPose(main_camera_->projection_matrix());
	cb_pass.camera_position = main_camera_->world_position();

	//TODO: 조명 관련 클래스를 생성후 그것을 사용하여 아래 정보 업데이트(현재는 테스트용 하드코딩)
	cb_pass.ambient_light = XMFLOAT4{ 0.1,0.1,0.1, 1 };
	cb_pass.lights[0].strength = XMFLOAT3{ 1,1,1 };
	cb_pass.lights[0].direction = XMFLOAT3{ 1,-1, -0.5 };
	cb_pass.lights[0].enable = true;
	cb_pass.lights[0].type = 0;

	for (int i = 1; i < 16; ++i)
		cb_pass.lights[i].enable = false;

	frame_resource_manager_->curr_frame_resource()->cb_pass.get()->CopyData(0, cb_pass);

	//25.02.23 수정
	//기존 루트 디스크립터 테이블에서 루트 CBV로 변경
	D3D12_GPU_VIRTUAL_ADDRESS cb_pass_address =
		frame_resource_manager_->curr_frame_resource()->cb_pass.get()->Resource()->GetGPUVirtualAddress();

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kRenderPass, cb_pass_address);

	Mesh::ResetCBObjectCurrentIndex();
	SkinnedMesh::ResetCBSkinnedMeshObjectCurrentIndex();

	// 단순한 배치 처리 
	// 씬에서 사용하는 쉐이더가 n개이면 SetPipelineState가 n번 호출된다
	for (const std::unique_ptr<Shader>& shader : shaders_)
	{
		command_list->SetPipelineState(shader->GetPipelineState());

		for (const std::unique_ptr<Mesh>& mesh : meshes_)
		{
			if (mesh->shader_type() == (int)shader->shader_type())
			{
				mesh->Render(command_list, frame_resource_manager_, descriptor_manager_);
			}
		}

	}
}

bool TestScene::ProcessInput(UINT id, WPARAM w_param, LPARAM l_param, float time)
{
	if (main_input_controller_)
	{
		if (main_input_controller_->ProcessInput(id, w_param, l_param, time))
			return true;
	}
	switch (id)
	{
	default:
		return false;
		break;
	}
	return true;
}

void TestScene::Update(float elapsed_time)
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->Update(elapsed_time);
	}
}
