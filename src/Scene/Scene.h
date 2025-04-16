#pragma once
#include "SceneObjects.h"
#include "Debugger/Debugger.h"

namespace Engine
{
    class Scene
    {
    public:
        /// Generate a new random UID.
        [[nodiscard]]
        uint32_t NewUID()
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

        /// Check if the given UID is allowed
        [[nodiscard]]
        bool ValidUID(const uint32_t UID) const
        {
            if (UID == 0)
                return false;

            for (const SceneObject* object : m_sceneObjects)
                if (object->GetUID() == UID)
                    return false;
            return true;
        }

        /// Create and empty object for the scene. Returns the newly created object.
        SceneObject* NewObject(uint32_t UID = 0)
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

        /// Delete an object from the scene, also deletes any child objects.
        void DeleteObject(const uint32_t UID)
        {
            for (int i = 0; i < (int)m_sceneObjects.size(); i++)
                if (m_sceneObjects[i]->GetUID() == UID)
                {
                    delete m_sceneObjects[i];
                    m_sceneObjects.erase(m_sceneObjects.begin() + i);
                }
        }

        /// Delete an object from the scene, also deletes any child objects.
        void DeleteObject(const SceneObject* object) { DeleteObject(object->GetUID()); }

        /// Delete all objects from the scene
        void ClearObjects()
        {
            for (SceneObject* object : m_sceneObjects)
                delete object;
            m_sceneObjects.clear();
        }

        /// Find object by UID.
        [[nodiscard]] SceneObject* GetSceneObject(uint32_t UID) const
        {
            for (SceneObject* object : m_sceneObjects)
                if (object->GetUID() == UID)
                    return object;
            DebugLog(LogSeverity::ERROR, "Object '" + std::to_string(UID) + "' not found");
            return nullptr;
        }

        /// Find object by name.
        [[nodiscard]] SceneObject* GetSceneObject(const std::string& name) const
        {
            for (SceneObject* object : m_sceneObjects)
                if (object->GetName() == name)
                    return object;
            DebugLog(LogSeverity::ERROR, "Object '" + name + "' not found");
            return nullptr;
        }

        std::vector<SceneObject*>& GetSceneObjectList() { return m_sceneObjects; }

    private:
        std::vector<SceneObject*> m_sceneObjects = {};
    };
}
