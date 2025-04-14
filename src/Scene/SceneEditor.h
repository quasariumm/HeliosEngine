#pragma once
#include <cstdint>

#include "Scene.h"
#include "Editor/EditorInterface.h"

namespace Engine
{
    enum TreeSelectMode
    {
        SELECT,
        PARENT
    };

    class SceneEditor : public EditorInterface
    {
    public:
        SceneEditor() = delete;
        explicit SceneEditor(Scene* scene) : m_targetScene(scene) {}

        void DrawInterface() override;

        void TreeEditor();
        void ObjectEditor();
        void DisplayObjectTree(SceneObject* object);

    private:
        Scene* m_targetScene;

        uint8_t m_selectedObject = 0;
        uint8_t m_prvSelectedObject = 0;
        TreeSelectMode m_selectMode = SELECT;
    };

}
