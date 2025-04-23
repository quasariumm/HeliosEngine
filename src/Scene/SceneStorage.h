#pragma once

#include "Scene.h"

namespace Engine
{
class SceneLoader
{
public:
    enum LoadType
    {
        NONE,
        OBJECT,
        COMPONENT
    };

    static void LoadFromFile(Scene* scene, const std::filesystem::path& fileName);
    static void SaveToFile(Scene* scene, const std::filesystem::path& fileName);

    static std::string SaveTransform(SceneObject* object);
    static void LoadTransform(SceneObject* object, const std::wstring& line);
};
}
