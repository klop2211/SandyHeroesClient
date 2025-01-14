#pragma once

class Component;

// Scene�� �����ϴ� ��� ������Ʈ�� ���� Ŭ����
// �ڽİ� ���� ��带 ���� Ʈ������
class Object
{
public:
	Object();
	virtual ~Object();

	//���� ������(child, sibling�� �����ʹ� �������� ����)
	Object(const Object& other);

	//getter
	// ��ȯ��� �� �� ����
	XMFLOAT4X4 transform_matrix() const;
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 right_vector() const;
	XMFLOAT3 up_vector() const;

	// ������� �� �� ����
	XMFLOAT4X4 world_matrix() const;
	XMFLOAT3 world_position_vector() const;
	XMFLOAT3 world_look_vector() const;
	XMFLOAT3 world_right_vector() const;
	XMFLOAT3 world_up_vector() const;

	UINT id() const;

	std::string name() const;

	//setter
	// ��ȯ��� �� �� ����
	void set_transform_matrix(const XMFLOAT4X4& value);
	void set_position_vector(const XMFLOAT3& value);
	void set_look_vector(const XMFLOAT3& value);
	void set_right_vector(const XMFLOAT3& value);
	void set_up_vector(const XMFLOAT3& value);

	// ��������� setter�� �������� �ʴ´�.(��������� ���� ������Ʈ �Ǳ� ����)

	void set_name(const std::string& value);

	void AddChild(Object* object);
	void AddSibling(Object* object);

	Object* DeepCopyObject(Object* parent);

	// ��带 ��ȸ�ϸ� world_matrix�� ������Ʈ�Ѵ�.(�ֻ��� ����� ��� ���ڿ� nullptr�� ������ �ȴ�)
	void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform); 


	// �� Ŭ������ ��ӹ��� Ŭ�������� ��������!
	virtual void Update(float elased_time) {};

protected:
	// ������Ʈ�� ��ȯ���
	XMFLOAT4X4 transform_matrix_ = xmath_util_float4x4::Identity();

	Object* child_ = nullptr;
	Object* sibling_ = nullptr;

	// ������Ʈ�� �߰��� ��� ������Ʈ�� ����Ʈ
	// ������ ������Ʈ�� ����ϴ� �뵵���ٴ� ���� �� ������ ���� ���� �ִ� ����
	// ���� �̸� ���� ��뵵 �������ϴ�.
	std::list<Component*> component_list_;

	std::string name_ = "None";

private:
	// ������Ʈ�� ���� ���� ���(��, ��������� ��ȯ�� ���� �����)
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();		

	// ���� ������ ������Ʈ�� id, 1���� �����Ѵ�. ��, id 0���� ���� �� ����
	static UINT kObjectNextId;
	UINT id_ = 0;

};

