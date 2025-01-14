#pragma once

class Component;

// Scene에 등장하는 모든 오브젝트의 조상 클래스
// 자식과 형제 노드를 가진 트리구조
class Object
{
public:
	Object();
	virtual ~Object();

	//복사 생성자(child, sibling의 포인터는 가져오지 않음)
	Object(const Object& other);

	//getter
	// 변환행렬 및 각 벡터
	XMFLOAT4X4 transform_matrix() const;
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 right_vector() const;
	XMFLOAT3 up_vector() const;

	// 월드행렬 및 각 벡터
	XMFLOAT4X4 world_matrix() const;
	XMFLOAT3 world_position_vector() const;
	XMFLOAT3 world_look_vector() const;
	XMFLOAT3 world_right_vector() const;
	XMFLOAT3 world_up_vector() const;

	UINT id() const;

	std::string name() const;

	//setter
	// 변환행렬 및 각 벡터
	void set_transform_matrix(const XMFLOAT4X4& value);
	void set_position_vector(const XMFLOAT3& value);
	void set_look_vector(const XMFLOAT3& value);
	void set_right_vector(const XMFLOAT3& value);
	void set_up_vector(const XMFLOAT3& value);

	// 월드행렬의 setter는 지원하지 않는다.(상위노드의 의해 업데이트 되기 때문)

	void set_name(const std::string& value);

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

	// 오브젝트에 추가된 모든 컴포넌트의 리스트
	// 실제로 컴포넌트를 사용하는 용도보다는 삭제 및 복사의 편리를 위해 있는 변수
	// 물론 이를 통해 사용도 가능은하다.
	std::list<Component*> component_list_;

	std::string name_ = "None";

private:
	// 오브젝트의 실제 월드 행렬(즉, 상위노드의 변환이 전부 적용된)
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();		

	// 다음 생성될 오브젝트의 id, 1부터 시작한다. 즉, id 0번은 나올 수 없음
	static UINT kObjectNextId;
	UINT id_ = 0;

};

