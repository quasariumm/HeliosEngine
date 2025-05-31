#pragma once

#include "Component.h"
#include "Objects/ObjectRenderer.h"

namespace Engine
{

class Sphere final : public Component
{
public:
    Sphere() : Component(this)
    {
        AssignProperty(L"Radius", &m_radius);

        AssignProperty(L"Material Index", &m_materialIdx);
    }

    void Init() override
    {
        ObjectRenderer::RegisterSphere(m_attachedObject->GetTransform(), &m_radius, &m_materialIdx);
        ObjectRenderer::SendObjectData();
    }

    int m_materialIdx = 0;

    float m_radius = 1.0f;
};

REGISTER_COMPONENT(Sphere, STR_TO_WSTR(ICON_SPHERE) + L" Sphere");

}
