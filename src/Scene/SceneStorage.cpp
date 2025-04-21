#include "SceneStorage.h"

#include "Core/FileTools.h"

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
    scene->ClearScene();

    std::string line;
    LoadType loadType = NONE;

    SceneObject* newObject = nullptr;
    Component* newComponent = nullptr;

    std::unordered_map<SceneObject*, uint32_t> sceneObjectParents;

    while (std::getline(file, line))
    {
        if (line.empty()) loadType = NONE, newObject = nullptr, newComponent = nullptr;

        switch (loadType)
        {
        case NONE:
            if (line == "[Object]") loadType = OBJECT;
            break;
        case OBJECT:
            if (IsToken(line, "UID"))
                newObject = scene->NewObject(stoul(TokenValue(line)));
            if (newObject == nullptr) continue;

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
                newComponent = newObject->AddComponentByName(TokenValue(line));
            if (IsToken(line, "Property"))
            {
                std::string data = TokenValue(line);
                std::string name = data.substr(0, data.find(" : "));
                std::string type = data.substr(data.find(" : ") + 3, data.find(" => ") - (data.find(" : ") + 3));
                std::string value = data.substr(data.find(" => ") + 4);

                if (type == Demangle(typeid(bool).name()))
                    newComponent->SetPropertyValue(name, value != "0");
                if (type == Demangle(typeid(int).name()))
                    newComponent->SetPropertyValue(name, std::stoi(value));
                if (type == Demangle(typeid(float).name()))
                    newComponent->SetPropertyValue(name, std::stof(value));
                if (type == Demangle(typeid(vec2).name()))
                    newComponent->SetPropertyValue(name, ParseVec2(value));
                if (type == Demangle(typeid(vec3).name()))
                    newComponent->SetPropertyValue(name, ParseVec3(value));
                if (type == Demangle(typeid(std::string).name()))
                    newComponent->SetPropertyValue(name, value);
            }
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
            for (const ComponentProperty& p : c->GetProperties())
            {
                file << "Property = " << p.name << " : " << p.type << " => ";
                if (p.rawType == typeid(bool)) file << *(bool*)p.value;
                if (p.rawType == typeid(int)) file << std::to_string(*(int*)p.value);
                if (p.rawType == typeid(float)) file << std::to_string(*(float*)p.value);
                if (p.rawType == typeid(vec2)) file << *(vec2*)p.value;
                if (p.rawType == typeid(vec3)) file << *(vec3*)p.value;
                if (p.rawType == typeid(std::string)) file << *(std::string*)p.value;
                file << std::endl;
            }
        }
        file << std::endl;
    }

    DebugLog(LogSeverity::DONE, "Scene was successfully saved");
}

std::string SceneLoader::SaveTransform(SceneObject* object)
{
    std::stringstream ss;
    Transform t = *object->GetTransform();
    ss << "P:" << t.position();
    ss << "R:" << t.rotation();
    ss << "S:" << t.scale();
    return ss.str();
}

void SceneLoader::LoadTransform(SceneObject* object, const std::string& line)
{
    std::string posText = line.substr(line.find("P:") + 2, line.find("R:") - line.find("P:") - 2);
    std::string rotText = line.substr(line.find("R:") + 2, line.find("S:") - line.find("R:") - 2);
    std::string sclText = line.substr(line.find("S:") + 2);

    Transform t = Transform();
    t.position(ParseVec3(posText));
    t.rotation(ParseVec3(rotText));
    t.scale(ParseVec3(sclText));
    object->SetTransform(t);
}
}
