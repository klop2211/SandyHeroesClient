#pragma once

class Component;

// Scene에 등장하는 모든 오브젝트의 조상 클래스
// 자식과 형제 노드를 가진 트리구조
class Object
{
public:
	Object() {}
	~Object();

	//복사 생성자(child, sibling의 포인터는 가져오지 않음)
	Object(const Object& other);

	//getter
	// 변환행렬 및 각 벡터
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 right_vector() const;
	XMFLOAT3 up_vector() const;

	// 월드행렬 및 각 벡터
	XMFLOAT3 world_position_vector() const;
	XMFLOAT3 world_look_vector() const;
	XMFLOAT3 world_right_vector() const;
	XMFLOAT3 world_up_vector() const;

	//setter
	// 변환행렬 및 각 벡터
	void set_position_vector(const XMFLOAT3& value);
	void set_look_vector(const XMFLOAT3& value);
	void set_right_vector(const XMFLOAT3& value);
	void set_up_vector(const XMFLOAT3& value);

	// 월드행렬의 setter는 지원하지 않는다.(상위노드의 의해 업데이트 되기 때문)

	void AddChild(Object* object);
	void AddSibling(Object* object);

	Object* DeepCopyObject(Object* parent);

	// 노드를 순회하며 world_matrix를 업데이트한다.(최상위 노드의 경우 인자에 nullptr을 넣으면 된다)
	void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform); 


	// 이 클래스를 상속받은 클래스에서 만들어야함!
	virtual void Update(float elased_time) {};

protected:
	// 오브젝트의 변환행렬
	XMFLOAT4X4 transform_matrix_ = xmath_util_float4x4::Identity();

	Object* child_ = nullptr;
	Object* sibling_ = nullptr;

	std::list<Component*> component_list_;

private:
	// 오브젝트의 실제 월드 행렬(즉, 상위노드의 변환이 전부 적용된)
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();		

};

