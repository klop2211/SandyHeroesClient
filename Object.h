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

	//ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½(child, siblingï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ï´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½)
	Object(const Object& other);

	//getter
	// ï¿½ï¿½È¯ï¿½ï¿½ï¿?ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
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

	CollideType collide_type() const;

	//setter
	// ï¿½ï¿½È¯ï¿½ï¿½ï¿?ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	void set_transform_matrix(const XMFLOAT4X4& value);
	void set_position_vector(const XMFLOAT3& value);
	void set_position_vector(float x, float y, float z);
	void set_look_vector(const XMFLOAT3& value);
	void set_right_vector(const XMFLOAT3& value);
	void set_up_vector(const XMFLOAT3& value);


	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?setterï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ê´Â´ï¿½.(ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ® ï¿½Ç±ï¿½ ï¿½ï¿½ï¿½ï¿½)

	void set_name(const std::string& value);

	void set_is_ground(bool on_ground);
	void set_collide_type(bool ground_check, bool wall_check);
	void set_collide_type(const CollideType& collide_type);

	void AddChild(Object* object);
	void AddSibling(Object* object);
	void AddComponent(Component* component);

	Object* FindFrame(const std::string& name);
	Object* GetHierarchyRoot();

	// ï¿½ï¿½å¸?ï¿½ï¿½È¸ï¿½Ï¸ï¿½ world_matrixï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®ï¿½Ñ´ï¿½.(ï¿½Ö»ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½Ú¿ï¿½ nullptrï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½È´ï¿½)
	void UpdateWorldMatrix(const XMFLOAT4X4* const parent_transform); 

	virtual void Update(float elapsed_time);

	void Rotate(float pitch, float yaw, float roll);
	void Scale(float value);				// transform ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ valueï¿½ï¿½ ï¿½Õµï¿½ ï¿½ï¿½È¯ï¿½Ñ´ï¿½.

	// func ï¿½Ô¼ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?ï¿½ï¿½å¿?ï¿½ï¿½ï¿½ï¿½ï¿½Ï¶ï¿½.
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
	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½È¯ï¿½ï¿½ï¿?
	XMFLOAT4X4 transform_matrix_ = xmath_util_float4x4::Identity();

	Object* parent_ = nullptr;
	Object* child_ = nullptr;
	Object* sibling_ = nullptr;


	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ß°ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½Æ®
	std::list<std::unique_ptr<Component>> component_list_;

	std::string name_ = "None";


	//¹°¸® °ü·Ã º¯¼öµé
	bool is_ground_ = false;	//Áö¸é¿¡ ´ê¾ÆÀÖ´Â°¡?

	//Ãæµ¹ Ã¼Å© °ü·Ã º¯¼ö
	CollideType collide_type_ = { false, false };	//Áö¸é Ã¼Å©, º® Ã¼Å©

private:
	// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿?ï¿½ï¿½, ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½ï¿½È¯ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿?
	XMFLOAT4X4 world_matrix_ = xmath_util_float4x4::Identity();		

	// ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Æ®ï¿½ï¿½ id, 1ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½Ñ´ï¿½. ï¿½ï¿½, id 0ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
	static UINT kObjectNextId;
	UINT id_ = 0;
	

};

