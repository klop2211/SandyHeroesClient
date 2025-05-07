#pragma once

class Component;

struct CollideType
{
	bool ground_check = false;	//Áö¸é Ã¼Å©°¡ ÇÊ¿äÇÑ°¡?	
	bool wall_check = false;	//º® Ã¼Å©°¡ ÇÊ¿äÇÑ°¡?
};

// Scene¿¡ µîÀåÇÏ´Â ¸ðµç ¿ÀºêÁ§Æ®ÀÇ Á¶»ó Å¬·¡½º
// ÀÚ½Ä°ú ÇüÁ¦ ³ëµå¸¦ °¡Áø Æ®¸®±¸Á¶

class Object
{
public:
	Object();
	Object(const std::string& name);
	virtual ~Object();

	Object(const Object& other);

	//getter
	XMFLOAT4X4 transform_matrix() const;
	XMFLOAT3 position_vector() const;
	XMFLOAT3 look_vector() const;
	XMFLOAT3 right_vector() const;
	XMFLOAT3 up_vector() const;

	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	XMFLOAT4X4 world_matrix() const;
	XMFLOAT3 world_position_vector() const;
	XMFLOAT3 world_look_vector() const;
	XMFLOAT3 world_right_vector() const;
	XMFLOAT3 world_up_vector() const;

	UINT id() const;
	std::string name() const;
	Object* child() const;
	Object* sibling() const;
	Object* parent() const;
	bool is_ground() const;	
	bool is_dead() const;	//Á×Àº ¿ÀºêÁ§Æ®ÀÎ°¡?

	CollideType collide_type() const;

	//setter
	void set_transform_matrix(const XMFLOAT4X4& value);
	void set_position_vector(const XMFLOAT3& value);
	void set_position_vector(float x, float y, float z);
	void set_look_vector(const XMFLOAT3& value);
	void set_right_vector(const XMFLOAT3& value);
	void set_up_vector(const XMFLOAT3& value);

	void set_name(const std::string& value);
	void set_is_dead(bool is_dead);

	void set_is_ground(bool on_ground);
	void set_collide_type(bool ground_check, bool wall_check);
	void set_collide_type(const CollideType& collide_type);


	void AddChild(Object* object);
	void AddSibling(Object* object);
	void AddComponent(Component* component);

	Object* FindFrame(const std::string& name);
	Object* GetHierarchyRoot();

	void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform); 

	virtual void Update(float elapsed_time);

	void Rotate(float pitch, float yaw, float roll);
	void Scale(float value);				

	// Applies the func to the object and all its descendants in the hierarchy.
	void EnableFuncInHeirachy(std::function<void(Object*, void*)> func, void* value);

	static Object* DeepCopy(Object* value, Object* parent = nullptr);

	template<class T>
	static T* GetComponent(Object* object)
	{
		for (auto& component : object->component_list_)
		{
			if (dynamic_cast<T*>(component.get()))
				return static_cast<T*>(component.get());
		}
		return nullptr;
	}

	template<class T>
	static std::list<T*> GetComponents(Object* object)
	{
		std::list<T*> r_value;
		for (auto& component : object->component_list_)
		{
			if (dynamic_cast<T*>(component.get()))
			{
				r_value.push_back(static_cast<T*>(component.get()));
			}
		}
		return r_value;
	}

	template<class T>
	static T* GetComponentInChildren(Object* object)
	{
		T* component = GetComponent<T>(object);
		if (component)
			return component;

		if (object->sibling_)
		{
			component = GetComponentInChildren<T>(object->sibling_);
			if (component)
				return component;
		}
		if (object->child_)
			return GetComponentInChildren<T>(object->child_);

		return nullptr;
	}

	template<class T>
	static std::list<T*> GetComponentsInChildren(Object* object)
	{
		std::list<T*> component_list = GetComponents<T>(object);
		std::list<T*> r_value;

		for (T* component : component_list)
		{
			r_value.push_back(component);
		}

		if (object->sibling_)
		{
			component_list = GetComponentsInChildren<T>(object->sibling_);
			for (T* component : component_list)
			{
				r_value.push_back(component);
			}
		}
		if (object->child_)
		{
			component_list = GetComponentsInChildren<T>(object->child_);
			for (T* component : component_list)
			{
				r_value.push_back(component);
			}
		}

		return r_value;
	}

protected:
	XMFLOAT4X4 transform_matrix_ = xmath_util_float4x4::Identity();

	Object* parent_ = nullptr;
	Object* child_ = nullptr;
	Object* sibling_ = nullptr;


	std::list<std::unique_ptr<Component>> component_list_;

	std::string name_ = "None";

	bool is_dead_ = false;	//Á×Àº ¿ÀºêÁ§Æ®ÀÎ°¡?	

	//¹°¸® °ü·Ã º¯¼öµé
	bool is_ground_ = false;	//Áö¸é¿¡ ´ê¾ÆÀÖ´Â°¡?

	//Ãæµ¹ Ã¼Å© °ü·Ã º¯¼ö
	CollideType collide_type_ = { false, false };	//Áö¸é Ã¼Å©, º® Ã¼Å©

private:
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();		

	static UINT kObjectNextId;
	UINT id_ = 0;
	

};

