#include "SceneStorage.h"

#include "Core/FileTools.h"

namespace Engine
{
void SceneLoader::LoadFromFile(Scene* scene, const std::filesystem::path& fileName)
{
    if (!ProjectHandler::ProjectLoaded())
    {
        DebugLog(LogSeverity::SEVERE, L"No project selected!");
        return;
    }

    std::wifstream file;
    file.open(fileName);
    if (!file.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Failed to open scene file");
        return;
    }

    // Clean the scene
    scene->ClearScene();

    std::wstring line;
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
            if (line == L"[Object]") loadType = OBJECT;
            break;
        case OBJECT:
            if (IsToken(line, L"UID"))
                newObject = scene->NewObject(stoul(TokenValue(line)));
            if (newObject == nullptr) continue;

            if (line == L"[Component]")
                loadType = COMPONENT;
            if (IsToken(line, L"Name"))
                newObject->SetName(TokenValue(line));
            if (IsToken(line, L"Transform"))
                LoadTransform(newObject, line.substr(line.find('=') + 2));
            if (IsToken(line, L"Parent"))
                sceneObjectParents.emplace(newObject, stoul(TokenValue(line)));
            break;
        case COMPONENT:
            if (IsToken(line, L"Type"))
                newComponent = newObject->AddComponentByName(TokenValue(line));
            if (IsToken(line, L"Property"))
            {
                std::wstring data = TokenValue(line);
                std::wstring name = data.substr(0, data.find(L" : "));
                std::wstring type = data.substr(data.find(L" : ") + 3, data.find(L" => ") - (data.find(L" : ") + 3));
                std::wstring value = data.substr(data.find(L" => ") + 4);

                if (type == STR_TO_WSTR(Demangle(typeid(bool).name())))
                    newComponent->SetPropertyValue(name, value != L"0");
                if (type == STR_TO_WSTR(Demangle(typeid(int).name())))
                    newComponent->SetPropertyValue(name, std::stoi(value));
                if (type == STR_TO_WSTR(Demangle(typeid(float).name())))
                    newComponent->SetPropertyValue(name, std::stof(value));
                if (type == STR_TO_WSTR(Demangle(typeid(vec2).name())))
                    newComponent->SetPropertyValue(name, ParseVec2(value));
                if (type == STR_TO_WSTR(Demangle(typeid(vec3).name())))
                    newComponent->SetPropertyValue(name, ParseVec3(value));
                if (type == STR_TO_WSTR(Demangle(typeid(std::string).name())))
                    newComponent->SetPropertyValue(name, value);
            }
            break;
        }
    }

    // Set object parents
    for (const std::pair<SceneObject*, uint32_t> pair : sceneObjectParents)
        pair.first->SetParent(scene->GetSceneObject(pair.second));

    DebugLog(LogSeverity::DONE, L"Scene was successfully loaded");
}

void SceneLoader::SaveToFile(Scene* scene, const std::filesystem::path& fileName)
{
    if (!ProjectHandler::ProjectLoaded())
    {
        DebugLog(LogSeverity::SEVERE, L"No project selected!");
        return;
    }

    std::wofstream file;
    file.open(fileName, std::ofstream::trunc);
    if (!file.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Failed to create or open scene file");
        return;
    }

    for (SceneObject* object : scene->GetSceneObjectList())
    {
        file << "[Object]" << std::endl;
        file << "UID = " << object->GetUID() << std::endl;
        file << "Name = " << object->GetName() << std::endl;
        file << "Transform = " << STR_TO_WSTR(SaveTransform(object)) << std::endl;
        if (object->GetParent() != nullptr)
            file << "Parent = " << object->GetParent()->GetUID() << std::endl;
        for (const std::unique_ptr<Component>& c : object->GetComponentList())
        {
            file << "[Component]" << std::endl;
            file << "Type = " << STR_TO_WSTR(Demangle(c->GetType().name())) << std::endl;
            for (const ComponentProperty& p : c->GetProperties())
            {
                file << "Property = " << p.name << " : " << p.type << " => ";
                if (p.rawType == typeid(bool)) file << *(bool*)p.value;
                if (p.rawType == typeid(int)) file << std::to_wstring(*(int*)p.value);
                if (p.rawType == typeid(float)) file << std::to_wstring(*(float*)p.value);
                if (p.rawType == typeid(vec2)) file << *(vec2*)p.value;
                if (p.rawType == typeid(vec3)) file << *(vec3*)p.value;
                if (p.rawType == typeid(std::string)) file << *(std::wstring*)p.value;
                file << std::endl;
            }
        }
        file << std::endl;
    }

    DebugLog(LogSeverity::DONE, L"Scene was successfully saved");
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

void SceneLoader::LoadTransform(SceneObject* object, const std::wstring& line)
{
    std::wstring posText = line.substr(line.find(L"P:") + 2, line.find(L"R:") - line.find(L"P:") - 2);
    std::wstring rotText = line.substr(line.find(L"R:") + 2, line.find(L"S:") - line.find(L"R:") - 2);
    std::wstring sclText = line.substr(line.find(L"S:") + 2);

    Transform t = Transform();
    t.position(ParseVec3(posText));
    t.rotation(ParseVec3(rotText));
    t.scale(ParseVec3(sclText));
    object->SetTransform(t);
}
}
