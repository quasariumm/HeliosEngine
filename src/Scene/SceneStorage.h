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
        OBJECT
    };

    static void LoadFromFile(Scene* scene, const std::filesystem::path& fileName);
    static void SaveToFile(Scene* scene, const std::filesystem::path& fileName);

    static bool IsToken(const std::string& line, const std::string& token);
    static std::string TokenValue(const std::string& line);

    static std::string SaveTransform(SceneObject* object);
    static void LoadTransform(SceneObject* object, std::string line);
};
}
