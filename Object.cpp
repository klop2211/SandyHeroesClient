#include "stdafx.h"
#include "Object.h"
#include "Component.h"

UINT Object::kObjectNextId = 1;

Object::Object()
{
	id_ = kObjectNextId;
	++kObjectNextId;
}

Object::Object(const std::string& name) : Object()
{
	name_ = name;
}

Object::~Object()
{
	if (sibling_)
		delete sibling_;
	if (child_)
		delete child_;
}

Object::Object(const Object& other) : 
	transform_matrix_(other.transform_matrix_), 
	world_matrix_(other.world_matrix_), 
	name_(other.name_)
{
	id_ = kObjectNextId;
	++kObjectNextId;

	child_ = nullptr;
	sibling_ = nullptr;

	//복사 대상 오브젝트의 컴포넌트들을 가져오고 이 오브젝트로 owner를 재설정한다.
	for (const std::unique_ptr<Component>& component : other.component_list_)
	{
		component_list_.emplace_back();
		component_list_.back().reset(component->GetCopy());
		component_list_.back()->set_owner(this);
	}
}

XMFLOAT4X4 Object::transform_matrix() const
{
	return transform_matrix_;
}

XMFLOAT3 Object::position_vector() const
{
	return XMFLOAT3(transform_matrix_._41, transform_matrix_._42, transform_matrix_._43);
}

XMFLOAT3 Object::look_vector() const
{
	return XMFLOAT3(transform_matrix_._31, transform_matrix_._32, transform_matrix_._33);
}

XMFLOAT3 Object::right_vector() const
{
	return XMFLOAT3(transform_matrix_._11, transform_matrix_._12, transform_matrix_._13);
}

XMFLOAT3 Object::up_vector() const
{
	return XMFLOAT3(transform_matrix_._21, transform_matrix_._22, transform_matrix_._23);
}

XMFLOAT4X4 Object::world_matrix() const
{
	return world_matrix_;
}

XMFLOAT3 Object::world_position_vector() const
{
	return XMFLOAT3(world_matrix_._41, world_matrix_._42, world_matrix_._43);
}

XMFLOAT3 Object::world_look_vector() const
{
	return XMFLOAT3(world_matrix_._31, world_matrix_._32, world_matrix_._33);
}

XMFLOAT3 Object::world_right_vector() const
{
	return XMFLOAT3(world_matrix_._11, world_matrix_._12, world_matrix_._13);
}

XMFLOAT3 Object::world_up_vector() const
{
	return XMFLOAT3(world_matrix_._21, world_matrix_._22, world_matrix_._23);
}

UINT Object::id() const
{
	return id_;
}

std::string Object::name() const
{
	return name_;
}

XMFLOAT3 Object::velocity() const
{
	return velocity_;
}

void Object::set_transform_matrix(const XMFLOAT4X4& value)
{
	transform_matrix_ = value;
}

void Object::set_position_vector(const XMFLOAT3& value)
{
	transform_matrix_._41 = value.x;
	transform_matrix_._42 = value.y;
	transform_matrix_._43 = value.z;
}

void Object::set_look_vector(const XMFLOAT3& value)
{
	transform_matrix_._31 = value.x;
	transform_matrix_._32 = value.y;
	transform_matrix_._33 = value.z;
}

void Object::set_right_vector(const XMFLOAT3& value)
{
	transform_matrix_._11 = value.x;
	transform_matrix_._12 = value.y;
	transform_matrix_._13 = value.z;
}

void Object::set_up_vector(const XMFLOAT3& value)
{
	transform_matrix_._21 = value.x;
	transform_matrix_._22 = value.y;
	transform_matrix_._23 = value.z;
}

void Object::set_name(const std::string& value)
{
	name_ = value;
}

void Object::set_velocity(const XMFLOAT3& value)
{
	velocity_ = value;
}

void Object::AddChild(Object* object)
{
	if (child_)
		child_->AddSibling(object);
	else
		child_ = object;
}

void Object::AddSibling(Object* object)
{
	if (sibling_)
		sibling_->AddSibling(object);
	else
		sibling_ = object;
}

void Object::AddComponent(Component* component)
{
	component_list_.emplace_back();
	component_list_.back().reset(component);
}

Object* Object::DeepCopyObject(Object* parent)
{
	Object* r_value = new Object(*this);

	if (child_)
		AddChild(child_->DeepCopyObject(r_value));
	if (sibling_)
		AddSibling(sibling_->DeepCopyObject(parent));

	return r_value;
}

void Object::UpdateWorldMatrix(const XMFLOAT4X4* const parent_world)
{
	world_matrix_ = parent_world ? transform_matrix_ * (*parent_world) : transform_matrix_;

	if (sibling_)
		sibling_->UpdateWorldMatrix(parent_world);
	if (child_)
		child_->UpdateWorldMatrix(&world_matrix_);
}

void Object::Update(float elapsed_time)
{
	for (const std::unique_ptr<Component>& component : component_list_)
	{
		component->Update(elapsed_time);
	}

	set_position_vector(position_vector() + (velocity_ * elapsed_time));
}
