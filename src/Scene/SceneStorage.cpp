#include "SceneStorage.h"

#include "Core/FileTools.h"
#include "Objects/ObjectRenderer.h"


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
                else if (type == STR_TO_WSTR(Demangle(typeid(int).name())))
                    newComponent->SetPropertyValue(name, std::stoi(value));
                else if (type == STR_TO_WSTR(Demangle(typeid(float).name())))
                    newComponent->SetPropertyValue(name, std::stof(value));
                else if (type == STR_TO_WSTR(Demangle(typeid(vec2).name())))
                    newComponent->SetPropertyValue(name, ParseVec2(value));
                else if (type == STR_TO_WSTR(Demangle(typeid(vec3).name())))
                    newComponent->SetPropertyValue(name, ParseVec3(value));
                else if (type == STR_TO_WSTR(Demangle(typeid(std::string).name())))
                    newComponent->SetPropertyValue(name, value);
                else if (type == STR_TO_WSTR(Demangle(typeid(std::wstring).name())))
                    newComponent->SetPropertyValue(name, value);
            	else
            	{
            		// Trim the value of any whitespace
            		value.erase(std::ranges::remove_if(value, isspace).begin(), value.end());
            		const wchar_t* cstr = value.c_str();
            		size_t toCopyByteSize = value.length();
            		char* rawData = (char*)malloc(toCopyByteSize);
		            for (int i = 0; i < toCopyByteSize; ++i)
		            {
		            	// Load one byte (left 4 bits from the first character, right 4 from the next character)
			            rawData[i] = (char)((cstr[2 * i] & 0xf0) + ((cstr[2 * i + 1] & 0xf0) >> 4));
		            }
            		newComponent->SetPropertyValue(name, rawData, value.length());
            		free(rawData);
            	}
            }
            break;
        }
    }

    // Set object parents
    for (const std::pair<SceneObject*, uint32_t> pair : sceneObjectParents)
        pair.first->SetParent(scene->GetSceneObject(pair.second));

	// Update material SSBO
	ObjectRenderer::Instance().UpdateMaterialSSBO();

    DebugLog(LogSeverity::DONE, L"Scene was successfully loaded");
}

uint8_t GetCharOffset(const uint8_t c)
{
	// Because some characters are control characters, they might cut off the file at a certain point.
	// Hence, we need to set some offsets to stop this from happening
	switch (c)
	{
	case 0x00:
	case 0xa0:
		return 5;
	case 0x10:
	case 0x20:
	case 0x90:
		return 1;
	case 0x80:
		return 8;
	default:
		return 0;
	}
}

void SceneLoader::SaveToFile(Scene* scene, const std::filesystem::path& fileName)
{
    if (!ProjectHandler::ProjectLoaded())
    {
        DebugLog(LogSeverity::SEVERE, L"No project selected!");
        return;
    }

    std::wofstream file;
    file.open(fileName, std::ios::trunc);
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
                else if (p.rawType == typeid(int)) file << std::to_wstring(*(int*)p.value);
                else if (p.rawType == typeid(float)) file << std::to_wstring(*(float*)p.value);
                else if (p.rawType == typeid(vec2)) file << *(vec2*)p.value;
                else if (p.rawType == typeid(vec3)) file << *(vec3*)p.value;
                else if (p.rawType == typeid(std::string)) file << *(std::wstring*)p.value;
                else if (p.rawType == typeid(std::wstring)) file << *(std::wstring*)p.value;
            	else
            	{
            		// Copy raw data
            		const auto* valueData = static_cast<const unsigned char*>(p.value);
            		std::wstring out = L"";
		            for (int i = 0; i < p.typeSize; ++i)
		            {
		            	// Because of the control characters, we store on byte of value data in two.
		            	// We store the left 4 bits in one byte, the right 4 in another.
		            	uint8_t BE = (uint8_t)(valueData[i] & 0xf0);
		            	uint8_t BEOffset = GetCharOffset(BE);
		            	uint8_t LE = (uint8_t)(((valueData[i] & 0x0f) << 4) & 0xff);
		            	uint8_t LEOffset = GetCharOffset(LE);
			            out += static_cast<wchar_t>( BE + BEOffset );
			            out += static_cast<wchar_t>( LE + LEOffset );
		            }
            		file << out;
            	}
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
