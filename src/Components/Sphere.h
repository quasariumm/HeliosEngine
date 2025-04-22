#pragma once
#include <memory>
#include <string>

#include "Component.h"

namespace Engine
{

class Sphere : public Component
{
public:
    Sphere() : Component(this, ICON_SPHERE" Sphere")
    {
        AssignProperty("Radius", &m_radius);

        AssignProperty("Test1", &m_testBool);
        AssignProperty("Test2", &m_testInt);
        AssignProperty("Test3", &m_testFloat);
        AssignProperty("Test4", &m_testVec2);
        AssignProperty("Test5", &m_testVec3);
        AssignProperty("Test6", &m_testText);
    }

    bool m_testBool = false;
    int m_testInt = 0;
    float m_testFloat = 1.0f;
    vec2 m_testVec2 = {};
    vec3 m_testVec3 = {};
    std::string m_testText = "test";

    float m_radius = 1.0f;
};

REGISTER_COMPONENT(Sphere);

}
