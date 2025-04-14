#pragma once
#include <cstdint>

#include "Scene.h"

namespace Engine
{
    enum TreeSelectMode
    {
        SELECT,
        PARENT
    };

    static uint8_t gSelectedObject = 0;
    static uint8_t gPreviousSelectedObject = 0;
    static TreeSelectMode gSelectMode = SELECT;

    static void DisplayObjectTree(SceneObject* object);

    class SceneEditor
    {
    public:
        SceneEditor() = delete;
        explicit SceneEditor(Scene* scene) : mTargetScene(scene) {}
        
        void SceneTreeEditor() const;
        void ObjectEditor() const;

    private:
        Scene* mTargetScene;
    };

}
