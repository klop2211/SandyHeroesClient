#include "stdafx.h"
#include "Scene.h"
#include "Object.h"
#include "Shader.h"
#include "FrameResourceManager.h"
#include "DescriptorManager.h"
#include "Mesh.h"
#include "CameraComponent.h"
#include "InputManager.h"
#include "InputControllerComponent.h"

void Scene::UpdateObjectWorldMatrix()
{
	for (const std::unique_ptr<Object>& object : object_list_)
	{
		object->UpdateWorldMatrix(nullptr);
	}
}

void Scene::SetInputController(InputControllerComponent* controller)
{
	input_manager_->set_main_controller(controller);
}
