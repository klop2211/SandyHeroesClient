#pragma once
#include "stdafx.h"
#include "Component.h"
#include "UploadBuffer.h"
#include "Material.h"
#include "Object.h"
#include "Scene.h"

struct FrameResource;

class ParticleComponent :
	public Component
{
private:
	struct ParticleData {
		XMVECTOR velocity_;
		float speed_;
		float life_time_;
		float max_life_time_;

		bool IsActive() const { return life_time_ > 0.0f; }
	};
public:
	struct Particle {
		XMFLOAT3 position_;
		//float life_factor_;
		XMFLOAT2 size_;
	};

	enum eShape {
		Sphere,
		Cone,
		Circle,
		Count
	};

	ParticleComponent(Object* owner);
	ParticleComponent(Object* owner, ID3D12Device* device, UINT particle_count, eShape shape, Material* material);

	ParticleComponent(const ParticleComponent& other);

	virtual Component* GetCopy() override;

	virtual void Update(float elapsed_time) override;
	virtual void Render(ID3D12GraphicsCommandList* command_list, FrameResource* curr_frame_resource);

	bool IsAlive(const int index) const;
	Particle particle(const int index) const;
	UINT capacity() const;
	Material* material() const;

	void set_scene(Scene* value);

private:
	Scene* scene_ = nullptr;

	UINT capacity_ = 50;
	UINT alive_count_ = 0;
	std::vector<Particle> particles_ = {};
	std::vector<ParticleData> particle_data_ = {};

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_ = {};
	std::unique_ptr<UploadBuffer<Particle>> particle_buffer_ = nullptr;

	Material* material_ = nullptr;

	eShape shape_ = eShape::Sphere;
};