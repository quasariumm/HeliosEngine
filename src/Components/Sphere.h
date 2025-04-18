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
};

REGISTER_COMPONENT(Sphere);

}
