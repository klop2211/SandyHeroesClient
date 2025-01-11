#include "stdafx.h"
#include "Object.h"

Object::~Object()
{
	if (sibling_)
		delete sibling_;
	if (child_)
		delete child_;
}

Object::Object(const Object& other) : 
	transform_matrix_(other.transform_matrix_), world_matrix_(other.world_matrix_)
{
	child_ = nullptr;
	sibling_ = nullptr;

	//TODO: 컴포넌트 복사 코드
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
