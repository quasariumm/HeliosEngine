#pragma once
#include <memory>
#include <string>

#include "Component.h"
#include "Objects/ObjectRenderer.h"

namespace Engine
{

class Sphere : public Component
{
public:
    Sphere() : Component(this, STR_TO_WSTR(ICON_SPHERE) + L" Sphere")
    {
        AssignProperty(L"Radius", &m_radius);

        AssignProperty(L"Test1", &m_testBool);
        AssignProperty(L"Test2", &m_testInt);
        AssignProperty(L"Test3", &m_testFloat);
        AssignProperty(L"Test4", &m_testVec2);
        AssignProperty(L"Test5", &m_testVec3);
        AssignProperty(L"Test6", &m_testText);
    }

    void Init() override
    {
        ObjectRenderer::RegisterSphere(m_attachedObject->GetTransform(), &m_radius);
        ObjectRenderer::SendObjectData();
    }

    bool m_testBool = false;
    int m_testInt = 0;
    float m_testFloat = 1.0f;
    vec2 m_testVec2 = {};
    vec3 m_testVec3 = {};
    std::wstring m_testText = L"test";

    float m_radius = 1.0f;
};

REGISTER_COMPONENT(Sphere);

}
