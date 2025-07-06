#include "stdafx.h"
#include "ParticleComponent.h"
#include "FPSControllerComponent.h"
#include "BaseScene.h"
#include "Scene.h"

ParticleComponent::ParticleComponent(Object* owner) : Component(owner)
{
}

ParticleComponent::ParticleComponent(Object* owner, ID3D12Device* device, UINT particle_count, eShape shape, Material* material) : Component(owner)
{
	capacity_ = particle_count;
	shape_ = shape;
	material_ = material;
	particles_.reserve(capacity_);
	particle_data_.reserve(capacity_);

	particle_buffer_ = std::make_unique<UploadBuffer<Particle>>(device, particle_count, false);


	for (int i = 0; i < capacity_; ++i)
	{
		XMFLOAT3 position = {};
		XMVECTOR direction = {};

		if (shape_ == Sphere)
		{
			position = XMFLOAT3(0.0f, 5.0f, 0.0f);
			direction = RANDOM::InsideUnitSphere();
		}
		else if (shape_ == Cone)
		{
			position = owner->world_position_vector();
			XMFLOAT3 owner_forward = owner->world_look_vector();
			XMVECTOR forward = XMLoadFloat3(&owner_forward); // ���� ����
			constexpr float coneAngle = XMConvertToRadians(45.0f); // ���� �ݰ���
			//constexpr float radius = 5.0f;
			constexpr float radius = 0.0f;
			direction = RANDOM::DirectionInCone(forward, coneAngle);
			XMVECTOR randomPos = direction * radius;
			randomPos += XMVectorSet(position.x, position.y, position.z, 0);
			XMStoreFloat3(&position, randomPos);
		}
		else if (shape_ == Circle)
		{
			XMVECTOR axis = XMVectorSet(1, 0, 0, 0); // ���� ����
			XMStoreFloat3(&position, RANDOM::CircleEdgePoint(axis, 5.0f, &direction) + XMVectorSet(0, 10, 0, 0));
		}
		else
		{
			assert(false);
		}

		//particles_.emplace_back(Particle(position, XMFLOAT2(1.0f, 1.0f))); // �ؽ��� ������
		particles_.emplace_back(Particle(position, XMFLOAT2(0.05f, 0.05f))); // cone �ؽ��� ������
		//particles_.emplace_back(Particle(position, XMFLOAT2(0.5f, 0.5f))); // sphere �ؽ��� ������

		//particle_data_.emplace_back(ParticleData(direction, 10.0f, 0.1f, 0.1f));
		particle_data_.emplace_back(ParticleData(direction, 2.0f, 0.1f, 0.1f));	//cone
	}

	vertex_buffer_view_.BufferLocation = particle_buffer_->Resource()->GetGPUVirtualAddress();
	vertex_buffer_view_.SizeInBytes = capacity_ * sizeof(Particle);
	vertex_buffer_view_.StrideInBytes = sizeof(Particle);
}

ParticleComponent::ParticleComponent(const ParticleComponent& other) : Component(other)
{
}

Component* ParticleComponent::GetCopy()
{
    return new ParticleComponent(*this);
}

void ParticleComponent::Update(float elapsed_time)
{
	int particleIndex = 0;

	auto base_scene = dynamic_cast<BaseScene*>(scene_);
	if (!base_scene) return;

	Object* player = base_scene->player();  // BaseScene�� player_
	auto controller = Object::GetComponentInChildren<FPSControllerComponent>(player);

	for (int i = 0; i < capacity_; ++i)
	{
		//TODO: Particle Movement Update.
		XMVECTOR vPosition = XMLoadFloat3(&particles_[i].position_);
		vPosition += particle_data_[i].velocity_ * particle_data_[i].speed_ * elapsed_time;
		XMStoreFloat3(&particles_[i].position_, vPosition);

		particle_data_[i].life_time_ -= elapsed_time;
		// TODO : life_factor ���� �߰��ϱ�
		//particles_[i].life_factor_ = 0.7f + particle_data_[i].life_time_ / particle_data_[i].max_life_time_ * (1.0f - 0.7f);

		//Alive�� Particle�� ��ƾ���.
		if (particle_data_[i].IsActive())
		{
			if (!(shape_ == Cone && controller->is_firekey_down()))
				continue;
			particle_buffer_->CopyData(particleIndex++, particles_[i]);
		}
		else
		{
			XMFLOAT3 position = {};
			XMVECTOR direction = {};

			if (shape_ == Sphere)
			{
				//position = XMFLOAT3(0.0f, 15.0f, 0.0f);
				position = XMFLOAT3(0.0f, 5.0f, 0.0f);
				direction = RANDOM::InsideUnitSphere();
			}
			else if (shape_ == Cone && controller->is_firekey_down())
			{
				position = owner_->world_position_vector();
				XMFLOAT3 owner_forward = owner_->world_look_vector();
				XMVECTOR forward = XMLoadFloat3(&owner_forward); // ���� ����
				constexpr float coneAngle = XMConvertToRadians(18.0f); // ���� �ݰ��� (23 �⺻)
				constexpr float radius = 0.2f; // (�⺻ 5)
				direction = RANDOM::DirectionInCone(forward, coneAngle);
				XMVECTOR randomPos = direction * radius;
				//randomPos += XMVectorSet(position.x, position.y, position.z, 0);
				XMStoreFloat3(&position, randomPos);
			}
			else if (shape_ == Circle)
			{
				XMVECTOR axis = XMVectorSet(1, 0, 0, 0); // ���� ����
				XMStoreFloat3(&position, RANDOM::CircleEdgePoint(axis, 5.0f, &direction) + XMVectorSet(0, 15, 0, 0));
			}
			else
			{
				assert(false);
			}

			particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.01f, 0.1f);	// cone
			//particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(0.75, 0.75f);	//sphere
			//particle_data_[i].max_life_time_ = RANDOM::GetRandomValue(1.5f, 1.5f); // �⺻
			particle_data_[i].life_time_ = particle_data_[i].max_life_time_;
			particle_data_[i].velocity_ = direction;

			XMFLOAT3 pivot_position = owner_->world_position_vector();

			XMVECTOR rPosition = XMLoadFloat3(&pivot_position) + XMLoadFloat3(&position);

			//particles_[i].position_ = position;
			
			XMStoreFloat3(&particles_[i].position_, rPosition);
		}
	}

	alive_count_ = particleIndex;
}

void ParticleComponent::Render(ID3D12GraphicsCommandList* command_list,
	FrameResource* curr_frame_resource)
{
	command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view_);
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	command_list->DrawInstanced(alive_count_, 1, 0, 0); // aliveCount�� ����ִ� ��ƼŬ ��
}

bool ParticleComponent::IsAlive(const int index) const
{
	return particle_data_[index].IsActive();
}

ParticleComponent::Particle ParticleComponent::particle(const int index) const
{
	return particles_[index];
}

UINT ParticleComponent::capacity() const
{
	return capacity_;
}

Material* ParticleComponent::material() const
{
	return material_;
}

void ParticleComponent::set_scene(Scene* value)
{
	scene_ = value;
}