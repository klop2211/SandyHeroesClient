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
	name_(other.name_),
	is_ground_(other.is_ground_),
	collide_type_(other.collide_type_),
	tag_(other.tag_),
	is_movable_(other.is_movable_)
{
	id_ = kObjectNextId;
	++kObjectNextId;
	
	child_ = nullptr;
	sibling_ = nullptr;

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

std::string Object::tag() const
{
	return tag_;
}

Object* Object::child() const
{
	return child_;
}

Object* Object::sibling() const
{
	return sibling_;
}

Object* Object::parent() const
{
	return parent_;
}

bool Object::is_ground() const
{
	return is_ground_;
}

bool Object::is_dead() const
{
	return is_dead_;
}

CollideType Object::collide_type() const
{
	return collide_type_;
}

void Object::set_transform_matrix(const XMFLOAT4X4& value)
{
	transform_matrix_ = value;
	ResetSRTFromTransformMatrix();
}

void Object::set_position_vector(const XMFLOAT3& value)
{
	transform_matrix_._41 = value.x;
	transform_matrix_._42 = value.y;
	transform_matrix_._43 = value.z;

	local_position_ = value;
}

void Object::set_position_vector(float x, float y, float z)
{
	set_position_vector(XMFLOAT3{ x,y,z });
}

void Object::set_look_vector(const XMFLOAT3& value)
{
	transform_matrix_._31 = value.x;
	transform_matrix_._32 = value.y;
	transform_matrix_._33 = value.z;
	ResetSRTFromTransformMatrix();
}

void Object::set_right_vector(const XMFLOAT3& value)
{
	transform_matrix_._11 = value.x;
	transform_matrix_._12 = value.y;
	transform_matrix_._13 = value.z;
	ResetSRTFromTransformMatrix();
}

void Object::set_up_vector(const XMFLOAT3& value)
{
	transform_matrix_._21 = value.x;
	transform_matrix_._22 = value.y;
	transform_matrix_._23 = value.z;
	ResetSRTFromTransformMatrix();
}

void Object::set_name(const std::string& value)
{
	name_ = value;
}

void Object::set_tag(const std::string& value)
{
	tag_ = value;
}

void Object::set_is_dead(bool is_dead)
{
	is_dead_ = is_dead;
}

void Object::set_is_ground(bool is_ground)
{
	is_ground_ = is_ground;
}

void Object::set_collide_type(bool ground_check, bool wall_check)
{
	collide_type_.ground_check = ground_check;
	collide_type_.wall_check = wall_check;
}

void Object::set_collide_type(const CollideType& collide_type)
{
	collide_type_ = collide_type;
}

void Object::set_local_scale(const XMFLOAT3& value)
{
	local_scale_ = value;

	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&transform_matrix_));
	s = XMLoadFloat3(&local_scale_);
	XMStoreFloat4x4(&transform_matrix_, XMMatrixAffineTransformation(s, XMVectorZero(), r, t));
}

void Object::set_local_rotation(const XMFLOAT3& value)
{
	local_rotation_ = value;
	XMVECTOR rotation_quet = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(value.x), 
		XMConvertToRadians(value.y),
		XMConvertToRadians(value.z));
	XMVECTOR s, r, t;
	if (XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&transform_matrix_)))
	{
		XMStoreFloat4x4(&transform_matrix_, XMMatrixAffineTransformation(s, XMVectorZero(), rotation_quet, t));
	}
}

void Object::set_local_position(const XMFLOAT3& value)
{
	local_position_ = value;

	set_position_vector(value);
}

void Object::set_is_movable(bool value)
{
	is_movable_ = value;
}

void Object::ResetSRTFromTransformMatrix()
{
	XMVECTOR s, r, t;
	XMFLOAT4 rot_quaternion;
	if (XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&transform_matrix_)))
	{
		XMStoreFloat3(&local_scale_, s);

		XMStoreFloat4(&rot_quaternion, r);
		float w = rot_quaternion.w;
		float x = rot_quaternion.x;
		float y = rot_quaternion.y;
		float z = rot_quaternion.z;
		float sinr_cosp = 2 * (w * x + y * z);
		float cosr_cosp = 1 - 2 * (x * x + y * y);
		float roll = atan2(sinr_cosp, cosr_cosp);  // Z축 회전

		float sinp = 2 * (w * y - z * x);
		float pitch;
		if (abs(sinp) >= 1)
			pitch = copysign(XM_PI / 2, sinp);      // Gimbal lock
		else
			pitch = asin(sinp);                    // X축 회전

		float siny_cosp = 2 * (w * z + x * y);
		float cosy_cosp = 1 - 2 * (y * y + z * z);
		float yaw = atan2(siny_cosp, cosy_cosp);   // Y축 회전

		local_rotation_.x = XMConvertToDegrees(pitch);
		local_rotation_.y = XMConvertToDegrees(yaw);
		local_rotation_.z = XMConvertToDegrees(roll);

		XMStoreFloat3(&local_position_, t);
	}
}

void Object::AddChild(Object* object)
{
	object->parent_ = this;
	if (child_)
		child_->AddSibling(object);
	else
		child_ = object;
}

void Object::AddSibling(Object* object)
{
	object->parent_ = parent_;
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

Object* Object::FindFrame(const std::string& name)
{
	if(name_ == name)
		return this;

	if (child_)
	{
		Object* found = child_->FindFrame(name);
		if (found)
			return found;
	}
		
	if (sibling_)
		return sibling_->FindFrame(name);

	return nullptr;
}

Object* Object::GetHierarchyRoot()
{
	if (parent_)
		return parent_->GetHierarchyRoot();
	return this;
}

void Object::DeleteChild(const std::string& name)
{
	if (child_ && child_->name_ == name)
	{
		delete child_;
		child_ = nullptr;
		return;
	}
	if (child_)
	{
		child_->DeleteChild(name);
	}
	if (sibling_)
	{
		sibling_->DeleteChild(name);
	}
}

void Object::KillChild(const std::string& name)
{
	if (child_ && child_->name_ == name)
	{
		child_->set_is_dead(true);
		return;
	}
	if (child_)
	{
		child_->KillChild(name);
	}
	if (sibling_)
	{
		sibling_->KillChild(name);
	}
}

Object* Object::PopDeadChild()
{
	if (child_ && child_->is_dead_)
	{
		Object* new_child = child_->sibling_;
		child_->sibling_ = nullptr;
		Object* dead_child = child_;
		child_ = new_child;
		return dead_child;
	}
	if (child_)
	{
		Object* dead_child = child_->PopDeadChild();
		if (dead_child)
			return dead_child;
	}
	if (sibling_)
	{
		return sibling_->PopDeadChild();
	}
	return nullptr;
}

void Object::ChangeChild(Object* src, const std::string& dst_name, bool is_delete)
{
	if (!src)
		return;
	if (child_ && child_->name_ == dst_name)
	{
		if (is_delete)
		{
			delete child_;
			child_ = src;
		}
		else
		{
			child_->set_is_dead(true);
			child_->AddSibling(src);
		}
		return;
	}
	if (child_)
	{
		child_->ChangeChild(src, dst_name, is_delete);
	}
	if (sibling_)
	{
		sibling_->ChangeChild(src, dst_name, is_delete);
	}
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
	if (is_dead_)
		return;
	for (const std::unique_ptr<Component>& component : component_list_)
	{
		component->Update(elapsed_time);
	}

	if (child_)
	{
		child_->Update(elapsed_time);
	}
	if (sibling_)
	{
		sibling_->Update(elapsed_time);
	}

}

void Object::Rotate(float pitch, float yaw, float roll)
{
	local_rotation_.x += pitch;
	local_rotation_.y += yaw;
	local_rotation_.z += roll;

	XMVECTOR rotation_quet = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(local_rotation_.x), 
		XMConvertToRadians(local_rotation_.y), 
		XMConvertToRadians(local_rotation_.z));
	XMVECTOR s, r, t;
	if (XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&transform_matrix_)))
	{
		XMStoreFloat4x4(&transform_matrix_, XMMatrixAffineTransformation(s, XMVectorZero(), rotation_quet, t));
	}
}

void Object::Scale(float value)
{
	local_scale_.x *= value;
	local_scale_.y *= value;
	local_scale_.z *= value;
	XMVECTOR s, r, t;
	if (XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&transform_matrix_)))
	{
		XMStoreFloat4x4(&transform_matrix_, XMMatrixAffineTransformation(XMLoadFloat3(&local_scale_), XMVectorZero(), r, t));
	}
}

void Object::EnableFuncInHeirachy(std::function<void(Object*, void*)> func, void* value)
{
	func(this, value);

	if (child_)
	{
		child_->EnableFuncInHeirachy(func, value);
	}
	if (sibling_)
	{
		sibling_->EnableFuncInHeirachy(func, value);
	}
}

void Object::OnDestroy(std::function<void(Object*)> func)
{
	on_destroy_func_ = func;
}

void Object::Destroy()
{
	if (on_destroy_func_)
	{
		on_destroy_func_(this);
	}
}

void Object::AddDeadFrameCount(UINT frame_count)
{
	dead_frame_count_ += frame_count;
}

Object* Object::DeepCopy(Object* value, Object* parent)
{
	if (!value)
		return nullptr;

	Object* copy = new Object(*value);
	copy->parent_ = parent;

	copy->child_ = DeepCopy(value->child_, copy);
	copy->sibling_ = DeepCopy(value->sibling_, parent);

	return copy;
}
