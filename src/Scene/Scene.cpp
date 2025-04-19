#include "Scene.h"

namespace Engine
{
uint32_t Scene::NewUID()
{
    int i = 0;
    while (i < 100)
    {
        uint32_t seed = RandomSeed(m_sceneObjects.size());
        uint32_t newUID = RandomUInt(seed);

        if (newUID == 0)
        {
            i++;
            continue;
        }

        const bool failed = std::ranges::any_of(m_sceneObjects, [&](const SceneObject* o)
                                                {return o->GetUID() == newUID; });

        if (!failed)
            return newUID;
        i++;
    }

    DebugLog(LogSeverity::ERROR, "Creating UID for object took too long");

    return 0;
}

bool Scene::ValidUID(const uint32_t UID) const
{
    if (UID == 0)
        return false;

    for (const SceneObject* object : m_sceneObjects)
        if (object->GetUID() == UID)
            return false;
    return true;
}

SceneObject* Scene::NewObject(uint32_t UID)
{
    if (!ValidUID(UID))
        UID = NewUID();

    auto object = new SceneObject(UID);

    // Check for valid UID
    if (object->GetUID() == 0)
    {
        delete object;
        DebugLog(LogSeverity::ERROR, "Creating new object failed");
        return nullptr;
    }

    m_sceneObjects.push_back(object);
    return object;
}

void Scene::ClearScene()
{
    for (SceneObject* object : m_sceneObjects)
        delete object;
    m_sceneObjects.clear();
}

SceneObject* Scene::GetSceneObject(uint32_t UID) const
{
    for (SceneObject* object : m_sceneObjects)
        if (object->GetUID() == UID)
            return object;
    DebugLog(LogSeverity::ERROR, "Object '" + std::to_string(UID) + "' not found");
    return nullptr;
}

SceneObject* Scene::GetSceneObject(const std::string& name) const
{
    for (SceneObject* object : m_sceneObjects)
        if (object->GetName() == name)
            return object;
    DebugLog(LogSeverity::ERROR, "Object '" + name + "' not found");
    return nullptr;
}
}
