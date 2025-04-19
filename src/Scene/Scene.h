#pragma once
#include "SceneObjects.h"

namespace Engine
{
    class Scene
    {
    public:
        /**
         * @brief Generates a new UID for a scene object specific to the scene. Only use in special cases
         * @return A new UID for a scene object
         */
        [[nodiscard]]
        uint32_t NewUID();

        /**
         * @brief Check if the given UID is valid
         * @param UID The UID that needs to be validated
         * @return True if the UId is not 0 and doesn't exist yet
         */
        [[nodiscard]]
        bool ValidUID(const uint32_t UID) const;

        /**
         * @brief Creates a new object in the scene
         * @param UID (OPTIONAL) A specific UID. Used for loading in objects from disk. By default, creates a new UID
         * @return A pointer to the scene object
         */
        SceneObject* NewObject(uint32_t UID = 0);

        /**
         * @brief Creates a new object in the scene. Also deletes any child objects
         * @param UID The UID of the object you want to delete
         */
        void DeleteObject(const uint32_t UID)
        {
            for (int i = 0; i < (int)m_sceneObjects.size(); i++)
                if (m_sceneObjects[i]->GetUID() == UID)
                {
                    delete m_sceneObjects[i];
                    m_sceneObjects.erase(m_sceneObjects.begin() + i);
                }
        }

        /**
         * @brief Creates a new object in the scene. Also deletes any child objects
         * @param object A pointer to the object you want to delete
         */
        void DeleteObject(const SceneObject* object) { DeleteObject(object->GetUID()); }

        /**
         * @brief Delete all the objects in the scene
         */
        void ClearScene();

        /**
         * @brief Get an object from the scene
         * @param UID The UID of the object you want to find
         * @return A pointer to the requested object, or nullptr if it wasn't found
         */
        [[nodiscard]] SceneObject* GetSceneObject(uint32_t UID) const;

        /**
         * @brief Get an object from the scene
         * @param name The display name of the object you want to find
         * @return A pointer to the requested object, or nullptr if it wasn't found
         */
        [[nodiscard]] SceneObject* GetSceneObject(const std::string& name) const;

        /**
         * @brief Get the full list of scene objects
         * @return A reference to the list of all objects in the scene
         */
        std::vector<SceneObject*>& GetSceneObjectList() { return m_sceneObjects; }

    private:
        std::vector<SceneObject*> m_sceneObjects = {};
    };
}
