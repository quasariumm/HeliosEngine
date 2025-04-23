#include "Scene.h"

namespace Engine
{
uint32_t Scene::NewUID()
{
    for (int i = 0; i < 100; ++i)
    {
        uint32_t seed = RandomSeed(m_sceneObjects.size());
        uint32_t newUID = RandomUInt(seed);

        if (newUID == 0)
            continue;

        if (!std::ranges::any_of(m_sceneObjects, [&](const SceneObject* o){ return o->GetUID() == newUID; }))
            return newUID;
    }

    DebugLog(LogSeverity::ERROR, L"Creating UID for object took too long");

    return 0;
}

bool Scene::ValidUID(const uint32_t UID) const
{
    if (UID == 0)
        return false;

	return std::ranges::all_of(m_sceneObjects, [&](const SceneObject* o){ return o->GetUID() != UID; });
}

SceneObject* Scene::NewObject(uint32_t UID)
{
    if (!ValidUID(UID))
        UID = NewUID();

    const auto object = new SceneObject(UID);

    // Check for valid UID
    if (object->GetUID() == 0)
    {
        delete object;
        DebugLog(LogSeverity::ERROR, L"Creating new object failed");
        return nullptr;
    }

    m_sceneObjects.push_back(object);
    return object;
}

void Scene::ClearScene()
{
    for (const SceneObject* object : m_sceneObjects)
        delete object;
    m_sceneObjects.clear();
}

SceneObject* Scene::GetSceneObject(const uint32_t UID) const
{
    for (SceneObject* object : m_sceneObjects)
        if (object->GetUID() == UID)
            return object;
    DebugLog(LogSeverity::ERROR, L"Object '" + std::to_wstring(UID) + L"' not found");
    return nullptr;
}

SceneObject* Scene::GetSceneObject(const std::wstring& name) const
{
    for (SceneObject* object : m_sceneObjects)
        if (object->GetName() == name)
            return object;
    DebugLog(LogSeverity::ERROR, L"Object '" + name + L"' not found");
    return nullptr;
}
}
