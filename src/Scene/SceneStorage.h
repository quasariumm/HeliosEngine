#pragma once
#include <vector>

#include "Scene.h"

namespace Engine
{
class SceneLoader
{
public:
    enum LoadType
    {
        NONE,
        OBJECT
    };

    static void LoadFromFile(Scene* scene, const std::string& fileName);
    static void SaveToFile(Scene* scene, const std::string& fileName);

    static std::string SaveTransform(SceneObject* object);
    static void LoadTransform(SceneObject* object, std::string line);
};
}
