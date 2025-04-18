#pragma once
#include <memory>
#include <string>

#include "Component.h"
#include "Scene/SceneObjects.h"

namespace Engine
{

class Sphere : public Component
{
public:
    Sphere() : Component(this, ICON_SPHERE" Sphere")
    {
        AssignProperty("Radius", m_radius);
    }

    float m_radius = 1.0f;
    vec3* m_position = m_attachedObject->GetTransform()->positionRef();
};

REGISTER_COMPONENT(Sphere);

}
