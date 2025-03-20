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
	//������ ����ϴ� ���̴� ������ŭ ����
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

	model_infos_.reserve(3);
	model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/Golem_Earth.bin", meshes_, materials_));

	BuildScene();

	for (const std::unique_ptr<Mesh>& mesh : meshes_)
	{
		mesh->CreateShaderVariables(device, command_list);
	}
}

void TestScene::BuildObject(ID3D12Device* device, ID3D12GraphicsCommandList* command_list)
{
	//TODO: �� �޽��� ������Ʈ ���� ������ �ľ��ϸ� �Ʒ� ��ġ�� �������ϰ� ������ �� ������ ����..
	cb_object_capacity_ = 1000;
	cb_skinned_mesh_object_capacity_ = 100;


	////������Ʈ�� �����ϰ�
	//Object* cube_object = new Object();
	//Mesh* green_cube = Scene::FindMesh("green_cube", meshes_);
	////�޽�������Ʈ�� ���ϴ� ������Ʈ�� �޽��� �־� �����Ѵ�.
	//MeshComponent* cube_component = new MeshComponent(cube_object, green_cube);
	//cube_object->AddComponent(cube_component);
	////�̸� ���� ������Ʈ����Ʈ�� �߰�
	//object_list_.emplace_back();
	//object_list_.back().reset(cube_object);

	Object* temp = model_infos_[0]->GetInstance();
	temp->set_position_vector(XMFLOAT3{ 0, 0, 0 });
	object_list_.emplace_back();
	object_list_.back().reset(temp);

	//Object* temp1 = Object::DeepCopy(temp);
	//temp1->set_position_vector(XMFLOAT3{ 0.6, 0, 0 });
	//object_list_.emplace_back();
	//object_list_.back().reset(temp1);


	Object* camera_object = new Object();
	camera_object->set_name("CAMERA_1");
	CameraComponent* camera_component = 
		new CameraComponent(camera_object, 0.3, 10000, 
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	camera_object->AddComponent(camera_component);
	main_camera_ = camera_component;

	//��ǲ ó�� ��Ʈ�ѷ� ����
	TestControllerComponent* controller = new TestControllerComponent(camera_object);
	//���� ��Ʈ�ѷ��� ����
	main_input_controller_ = controller;
	camera_object->AddComponent(controller);
	camera_object->set_position_vector(XMFLOAT3(0, 1.7, -1));

	object_list_.emplace_back();
	object_list_.back().reset(camera_object);

	camera_object = new Object;
	camera_object->set_name("CAMERA_2");
	controller = new TestControllerComponent(camera_object);
	camera_component =
		new CameraComponent(camera_object, 0.3, 10000,
			(float)kDefaultFrameBufferWidth / (float)kDefaultFrameBufferHeight, 58);
	camera_object->AddComponent(controller);
	camera_object->AddComponent(camera_component);
	object_list_.emplace_back();
	object_list_.back().reset(camera_object);



}

void TestScene::BuildConstantBufferViews(ID3D12Device* device)
{
	//25.02.23 ����
	//��Ʈ ��ũ���� ���̺��� ��Ʈ CBV�� �����Կ� ���� 
	//���̻� �� �Լ��� ���� CBV�� �������� ����

	//UINT object_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBObject));

	//UINT bone_transform_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBBoneTransform));
	//UINT bone_transform_offset = descriptor_manager_->cbv_bone_transform_offset();

	//UINT pass_cb_size = d3d_util::CalculateConstantBufferSize(sizeof(CBPass));

	////�� �����Ӹ��ҽ����� cbv�� �ʿ��ϴ�
	//for (int frame_index = 0; frame_index < FrameResourceManager::kFrameCount; ++frame_index)
	//{
	//	//�����Ӹ��ҽ����� ���ϴ� ��������� ���ҽ��� �����´�.
	//	ID3D12Resource* object_cb =
	//		frame_resource_manager_->GetResource(frame_index)->cb_object->Resource();

	//	D3D12_GPU_VIRTUAL_ADDRESS cb_adress = object_cb->GetGPUVirtualAddress();
	//	//��� ������Ʈ�� ���� cbv�� �ʿ��ϴ�.
	//	for (UINT i = 0; i < cb_object_capacity_; ++i)
	//	{
	//		// ��ũ���� �� ������
	//		int heap_index = cb_object_capacity_ * frame_index + i;
	//		D3D12_CPU_DESCRIPTOR_HANDLE handle = descriptor_manager_->GetCpuHandle(heap_index);

	//		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
	//		cbv_desc.BufferLocation = cb_adress;
	//		cbv_desc.SizeInBytes = object_cb_size;

	//		device->CreateConstantBufferView(&cbv_desc, handle);

	//		// ���� ������Ʈ�� ������� ��巹��
	//		cb_adress += object_cb_size;
	//	}

	//	// �� ������Ʈ�� ������ ������� ������ش�.
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

using namespace file_load_util;
void TestScene::BuildScene()
{
	std::ifstream scene_file{ "./Resource/Model/Scene.bin", std::ios::binary };

	int root_object_count = ReadFromFile<int>(scene_file);

	std::string load_token; 

	for (int i = 0; i < root_object_count; ++i)
	{
		ReadStringFromFile(scene_file, load_token);
		if (load_token[0] == '@')
		{
			load_token.erase(0, 1);
			object_list_.emplace_back();
			object_list_.back().reset(FindModelInfo(load_token)->GetInstance());

			ReadStringFromFile(scene_file, load_token);
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
			object_list_.back()->set_transform_matrix(transfrom);
		}
		else
		{
			model_infos_.push_back(std::make_unique<ModelInfo>("./Resource/Model/" + load_token + ".bin", meshes_, materials_));

			object_list_.emplace_back();
			object_list_.back().reset(model_infos_.back()->GetInstance());

			ReadStringFromFile(scene_file, load_token); // <Transfrom>
			XMFLOAT4X4 transfrom = ReadFromFile<XMFLOAT4X4>(scene_file);
			object_list_.back()->set_transform_matrix(transfrom);

		}
	}

}

void TestScene::Render(ID3D12GraphicsCommandList* command_list)
{
	main_camera_->UpdateCameraInfo();

	CBPass cb_pass{};
	cb_pass.view_matrix = xmath_util_float4x4::TransPose(main_camera_->view_matrix());
	cb_pass.proj_matrix = xmath_util_float4x4::TransPose(main_camera_->projection_matrix());
	cb_pass.camera_position = main_camera_->world_position();

	//TODO: ���� ���� Ŭ������ ������ �װ��� ����Ͽ� �Ʒ� ���� ������Ʈ(����� �׽�Ʈ�� �ϵ��ڵ�)
	cb_pass.ambient_light = XMFLOAT4{ 0.1,0.1,0.1, 1 };
	cb_pass.lights[0].strength = XMFLOAT3{ 1,1,1 };
	cb_pass.lights[0].direction = XMFLOAT3{ 1,-1, -0.5 };
	cb_pass.lights[0].enable = true;
	cb_pass.lights[0].type = 0;

	for (int i = 1; i < 16; ++i)
		cb_pass.lights[i].enable = false;

	frame_resource_manager_->curr_frame_resource()->cb_pass.get()->CopyData(0, cb_pass);

	//25.02.23 ����
	//���� ��Ʈ ��ũ���� ���̺��� ��Ʈ CBV�� ����
	D3D12_GPU_VIRTUAL_ADDRESS cb_pass_address =
		frame_resource_manager_->curr_frame_resource()->cb_pass.get()->Resource()->GetGPUVirtualAddress();

	command_list->SetGraphicsRootConstantBufferView((int)RootParameterIndex::kRenderPass, cb_pass_address);

	Mesh::ResetCBObjectCurrentIndex();
	SkinnedMesh::ResetCBSkinnedMeshObjectCurrentIndex();

	// �ܼ��� ��ġ ó�� 
	// ������ ����ϴ� ���̴��� n���̸� SetPipelineState�� n�� ȣ��ȴ�
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
	case WM_KEYDOWN:
		// ī�޶� ��ȯ �׽�Ʈ
		if (w_param == 'K')
		{
			//�ٲ� ī�޶� ������Ʈ�� ã��
			Object* camera = FindObject("CAMERA_2");

			//�� ������Ʈ�� ī�޶�� ��Ʈ�ѷ��� ������ �����´�
			CameraComponent* new_camera = Object::GetComponent<CameraComponent>(camera);
			if (new_camera) // nullptr ����
			{
				main_camera_ = new_camera;
			}
			main_input_controller_ = Object::GetComponent<InputControllerComponent>(camera);
		}
		if (w_param == 'L')
		{
			//�ٲ� ī�޶� ������Ʈ�� ã��
			Object* camera = FindObject("CAMERA_1");
			//�� ������Ʈ�� ī�޶�� ��Ʈ�ѷ��� ������ �����´�
			main_camera_ = Object::GetComponent<CameraComponent>(camera);
			main_input_controller_ = Object::GetComponent<TestControllerComponent>(camera);
		}
		break;

	default:
		return false;
		break;
	}
}

void TestScene::Update(float elapsed_time)
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->Update(elapsed_time);
	}
}
