#include "stdafx.h"
#include "Component.h"
#include "Object.h"

Component::Component(Object* owner) : owner_(owner)
{
}

Component::Component(const Component& other) : owner_(other.owner_)
{
}
