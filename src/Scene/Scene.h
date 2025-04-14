#pragma once
#include "SceneObjects.h"

namespace Engine
{
    class Scene
    {
    public:
        /// Generate a new random UID.
        int NewUID()
        {
            int i = 0;

            return (int)m_sceneObjects.size() + 1;

            // while (i < 100)
            // {
            //     uint newUID = Rand();
            //     bool failed = false;
            //     for (SceneObject* object : m_sceneObjects)
            //         if (object->GetUID() == newUID && NewUID() != 0)
            //             failed = true;
            //
            //     if (!failed)
            //         return newUID;
            //     i++;
            // }
            //
            // std::cout << ANSI_ERR <<
            //     "Creation of new UID took too long, Try again or remove some objects.\n";

            // return 0;
        }

        /// Create and empty object for the scene. Returns the newly created object.
        SceneObject* NewObject()
        {
            auto object = new SceneObject(NewUID());

            // Check for valid UID
            if (object->GetUID() == 0)
            {
                delete object;
                return nullptr;
            }

            m_sceneObjects.push_back(object);
            return object;
        }

        /// Delete an object from the scene, also deletes any child objects.
        void DeleteObject(int UID)
        {
            for (int i = 0; i < (int)m_sceneObjects.size(); i++)
                if (m_sceneObjects[i]->GetUID() == UID)
                {
                    delete m_sceneObjects[i];
                    m_sceneObjects.erase(m_sceneObjects.begin() + i);
                }
        }

        /// Delete an object from the scene, also deletes any child objects.
        void DeleteObject(SceneObject* object) { DeleteObject(object->GetUID()); }

        /// Find object by UID.
        [[nodiscard]] SceneObject* GetSceneObject(int UID) const
        {
            for (SceneObject* object : m_sceneObjects)
                if (object->GetUID() == UID)
                    return object;
            return nullptr;
        }

        /// Find object by name.
        [[nodiscard]] SceneObject* GetSceneObject(const std::string& name) const
        {
            for (SceneObject* object : m_sceneObjects)
                if (object->GetName() == name)
                    return object;
            return nullptr;
        }

        std::vector<SceneObject*>& GetSceneObjectList() { return m_sceneObjects; }

    private:
        std::vector<SceneObject*> m_sceneObjects = {};
    };
}
