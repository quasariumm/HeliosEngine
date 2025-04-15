#include "SceneStorage.h"

#include <fstream>

namespace Engine
{
void SceneLoader::LoadFromFile(Scene* scene, const std::string& fileName)
{
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

    while (std::getline(file, line))
    {
        switch (loadType)
        {
        case NONE:
            if (line.empty()) loadType = NONE;
            if (line == "[Object]") loadType = OBJECT;
            break;
        case OBJECT:
            if (line.find(">UID") != std::string::npos)
                newObject = scene->NewObject(stoul(line.substr(line.find('=') + 2)));
            if (newObject == nullptr) continue;

            if (line.find(">Name") != std::string::npos)
                newObject->SetName(line.substr(line.find('=') + 2));
            if (line.find(">Transform") != std::string::npos)
                LoadTransform(newObject, line.substr(line.find('=') + 2));
            break;
        }



    }

    DebugLog(LogSeverity::DONE, "Scene was successfully loaded");

}

void SceneLoader::SaveToFile(Scene* scene, const std::string& fileName)
{
    std::ofstream file;
    file.open(fileName);
    if (!file.is_open())
    {
        DebugLog(LogSeverity::ERROR, "Failed to open scene file");
        return;
    }

    for (SceneObject* object : scene->GetSceneObjectList())
    {
        file << "[Object]" << std::endl;
        file << ">UID = " << object->GetUID() << std::endl;
        file << ">Name = " << object->GetName() << std::endl;
        file << ">Transform = " << SaveTransform(object) << std::endl;
        file << std::endl;
    }

    DebugLog(LogSeverity::DONE, "Scene was successfully saved");

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
    while ((pos = line.find('!')) != std::string::npos) {
        std::string token = line.substr(0, pos);
        line.erase(0, pos + 1);

        if (token == "P" || token == "R" || token == "S")
            continue;
        v.push_back(atof(token.c_str()));
        i++;
    }

    Transform t = Transform();
    t.position({v[0], v[1], v[2]} );
    t.rotation({v[3], v[4], v[5]} );
    t.scale({v[6], v[7], v[8]} );
    object->SetTransform(t);
}
}
