#include "SceneStorage.h"

#if defined(__GNUG__)
#include <cxxabi.h>
#include <cstdlib>
#endif

inline std::string Demangle(const char* name)
{
#if defined(__GNUG__)
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result = (status == 0 && demangled != nullptr) ? demangled : name;
    free(demangled);
#else
    // MSVC or unsupported compiler — just return the raw name
    return name;
#endif

    // Strip namespaces: find last "::" and return what's after
    size_t pos = result.rfind("::");
    if (pos != std::string::npos)
    {
        return result.substr(pos + 2);
    }
    return result;
}

namespace Engine
{
void SceneLoader::LoadFromFile(Scene* scene, const std::filesystem::path& fileName)
{
    if (!ProjectLoaded())
    {
        DebugLog(LogSeverity::ERROR, "No project selected!");
        return;
    }

    std::ifstream file;
    file.open(fileName);
    if (!file.is_open())
    {
        DebugLog(LogSeverity::ERROR, "Failed to open scene file");
        return;
    }

    // Clean the scene
    scene->ClearObjects();

    std::string line;
    LoadType loadType = NONE;

    SceneObject* newObject = nullptr;

    std::unordered_map<SceneObject*, uint32_t> sceneObjectParents;

    while (std::getline(file, line))
    {
        if (line.empty()) loadType = NONE, newObject = nullptr;

        switch (loadType)
        {
        case NONE:
            if (line == "[Object]") loadType = OBJECT;
            break;
        case OBJECT:
            if (IsToken(line, "UID"))
                newObject = scene->NewObject(stoul(TokenValue(line)));
        // ReSharper disable once CppDFAConstantConditions
            if (newObject == nullptr) continue;

        // ReSharper disable once CppDFAUnreachableCode
            if (line == "[Component]")
                loadType = COMPONENT;
            if (IsToken(line, "Name"))
                newObject->SetName(TokenValue(line));
            if (IsToken(line, "Transform"))
                LoadTransform(newObject, line.substr(line.find('=') + 2));
            if (IsToken(line, "Parent"))
                sceneObjectParents.emplace(newObject, stoul(TokenValue(line)));
            break;
        case COMPONENT:
            if (IsToken(line, "Type"))
                newObject->AddComponentByName(TokenValue(line));
            if (IsToken(line, "Property"))
                newObject->AddComponentByName(TokenValue(line));
            break;
        }
    }

    // Set object parents
    for (const std::pair<SceneObject*, uint32_t> pair : sceneObjectParents)
        pair.first->SetParent(scene->GetSceneObject(pair.second));

    DebugLog(LogSeverity::DONE, "Scene was successfully loaded");
}

void SceneLoader::SaveToFile(Scene* scene, const std::filesystem::path& fileName)
{
    if (!ProjectLoaded())
    {
        DebugLog(LogSeverity::ERROR, "No project selected!");
        return;
    }

    std::ofstream file;
    file.open(fileName, std::ofstream::trunc);
    if (!file.is_open())
    {
        DebugLog(LogSeverity::ERROR, "Failed to create or open scene file");
        return;
    }

    for (SceneObject* object : scene->GetSceneObjectList())
    {
        file << "[Object]" << std::endl;
        file << "UID = " << object->GetUID() << std::endl;
        file << "Name = " << object->GetName() << std::endl;
        file << "Transform = " << SaveTransform(object) << std::endl;
        if (object->GetParent() != nullptr)
            file << "Parent = " << object->GetParent()->GetUID() << std::endl;
        for (const std::unique_ptr<Component>& c : object->GetComponentList())
        {
            file << "[Component]" << std::endl;
            file << "Type = " << Demangle(c->GetType().name()) << std::endl;
        }
        file << std::endl;
    }

    DebugLog(LogSeverity::DONE, "Scene was successfully saved");
}

bool SceneLoader::IsToken(const std::string& line, const std::string& token)
{
    return line.substr(0, line.find(" = ")) == token;
}

std::string SceneLoader::TokenValue(const std::string& line)
{
    std::string token = line;
    return token.erase(0, line.find(" = ") + 3);
}

std::string SceneLoader::SaveTransform(SceneObject* object)
{
    std::stringstream ss;
    Transform t = *object->GetTransform();
    ss << "P!" << t.position().x << "!" << t.position().y << "!" << t.position().z << "!";
    ss << "R!" << t.rotation().x << "!" << t.rotation().y << "!" << t.rotation().z << "!";
    ss << "S!" << t.scale().x << "!" << t.scale().y << "!" << t.scale().z << "!";
    return ss.str();
}

void SceneLoader::LoadTransform(SceneObject* object, std::string line)
{
    std::vector<float> v;
    int i = 0;
    size_t pos = 0;
    while ((pos = line.find('!')) != std::string::npos)
    {
        std::string token = line.substr(0, pos);
        line.erase(0, pos + 1);

        if (token == "P" || token == "R" || token == "S")
            continue;
        v.push_back(atof(token.c_str()));
        i++;
    }

    Transform t = Transform();
    t.position({v[0], v[1], v[2]});
    t.rotation({v[3], v[4], v[5]});
    t.scale({v[6], v[7], v[8]});
    object->SetTransform(t);
}
}
