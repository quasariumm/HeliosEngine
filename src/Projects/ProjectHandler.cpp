#include "ProjectHandler.h"
#include "imgui.h"
#include "Debugger/Debugger.h"

#include <nfd.hpp>

#include "Assets/AssetManager.h"
#include "Core/FileTools.h"
#include <windows.h>

namespace Engine
{

bool ProjectHandler::m_selectorOpen = false;
bool ProjectHandler::m_creatorOpen = false;
bool ProjectHandler::m_lockOut = false;

HINSTANCE ProjectHandler::m_projectLibrary = nullptr;
ProjectData ProjectHandler::m_projectData = {};
EngineProject* ProjectHandler::m_project = nullptr;

std::future<bool> ProjectHandler::m_recompileResult = {};

void ProjectHandler::ProjectWindows()
{
    static std::filesystem::path path;

    if (ImGui::BeginPopupModal("Recompiling Project"))
    {
        if (!m_lockOut)
            ImGui::CloseCurrentPopup();

        ImGui::Separator();
        ImGui::Text("Do not close the editor!");
        // TODO: Show the console output here
        ImGui::Text("For detailed compilation log, check the console output in your IDE");
        ImGui::Separator();

        ImGui::EndPopup();
    }
    else if (m_lockOut)
        ImGui::OpenPopup("Recompiling Project");

    if (m_creatorOpen)
    {
        ImGui::Begin("Project Creator", nullptr, ImGuiWindowFlags_NoDocking);

        ProjectData newProjectData = {};

        if (ImGui::Button("Select folder"))
            ShowFileSelect(path);

        ImGui::Text(ICON_FOLDER);
        ImGui::SameLine(30);
        ImGui::Text( path.string().c_str());

        static char projectName[64];
        ImGui::Text(ICON_RENAME);
        ImGui::SameLine(30);
        ImGui::InputText("Project Name", projectName, 64);
        newProjectData.projectName = STR_TO_WSTR(projectName);

        if (ImGui::Button("Create"))
        {
            if (ValidateProjectFolder(path))
                DebugLog(LogSeverity::SEVERE, L"There is already a project in this folder");
            else if (!ValidateProjectData(newProjectData))
                DebugLog(LogSeverity::SEVERE, L"Incorrect project settings");
            else
            {
                if (CreateProject(path, newProjectData))
                {
                    LoadProject(path);
                    ShowProjectCreator(false);
                }
            }
        }

        if (ImGui::Button("Cancel"))
            ShowProjectCreator(false);

        ImGui::End();
    }
    else if (m_selectorOpen)
    {
        ImGui::Begin("Project Selector", nullptr, ImGuiWindowFlags_NoDocking);

        if (ImGui::Button("Select file"))
        {
            if (ShowFileSelect(path, true))
            {
                if (!ValidateProjectFolder(path))
                    DebugLog(LogSeverity::SEVERE, L"Project doesn't exist");
                else
                {
                    ShowProjectSelector(false);
                    LoadProject(path);
                    AddRecentProject(path, m_projectData.projectName);
                }
            }
        }

        if (ImGui::CollapsingHeader("Recent Projects"))
        {
            std::unordered_map<std::wstring, std::filesystem::path> recentProjects = ReadRecentProjects();
            for (std::pair<std::wstring, std::filesystem::path> recent : recentProjects)
            {
                if (ImGui::Selectable(WStringToUTF8(recent.first).c_str()))
                {
                    if (!ValidateProjectFolder(recent.second))
                        DebugLog(LogSeverity::SEVERE, L"Project doesn't exist");
                    else
                    {
                        ShowProjectSelector(false);
                        LoadProject(recent.second);
                    }
                }
            }
        }

        if (ImGui::Button("Cancel"))
            ShowProjectSelector(false);

        ImGui::End();
    }

}

bool ProjectHandler::ShowFileSelect(std::filesystem::path& path, bool projectOnly)
{
    NFD_Init();

    nfdu8char_t *outPath;
    nfdresult_t result;

    if (projectOnly)
    {
        constexpr nfdu8filteritem_t filters[1] = {"Projects", "gep" };
        result = NFD_OpenDialog(&outPath, filters, 1, nullptr);
    }
    else
    {
        result = NFD::PickFolder(outPath);
    }

    if (result != NFD_OKAY)
        return false;

    path = std::filesystem::path(outPath).make_preferred();

    NFD_Quit();

    return true;
}

bool ProjectHandler::CreateProject(std::filesystem::path& projectPath, const ProjectData& data)
{
    if (projectPath.has_extension())
        projectPath.remove_filename();

    projectPath.append(data.projectName);

    std::filesystem::path projectFilePath = projectPath;
    projectFilePath.append("Project.gep");

    create_directories(projectPath);

    // Creating the Project.gep file
    std::wofstream file(projectFilePath);
    if (!file.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Could not create Project.gep");
        return false;
    }

    // Setting all values for the project file
    file << "[Project]\n";
    file << "Name = " << data.projectName << "\n";

    file.close();

    // Adding a CMake file
    std::filesystem::path projectDataPath = projectPath;
    std::wofstream cmakeFile(projectDataPath.append("CMakeLists.txt"));
    if (!cmakeFile.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Could not create CMakeLists.txt");
        return false;
    }
    cmakeFile << DefaultCMakeFile(data.projectName);
    cmakeFile.close();

    // Creating a source directory with the basics
    std::filesystem::path sourceFolder = projectPath;
    create_directories(sourceFolder.append("Source"));

    std::filesystem::path headerPath = sourceFolder;
    std::wofstream headerFile(headerPath.append(data.projectName + STR_TO_WSTR(".h")));
    if (!headerFile.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Could not create header file");
        return false;
    }
    headerFile << DefaultSourceFile(data.projectName);
    headerFile.close();

    std::filesystem::path sourcePath = sourceFolder;
    std::wofstream sourceFile(sourcePath.append(data.projectName + STR_TO_WSTR(".cpp")));
    if (!sourceFile.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Could not create source file");
        return false;
    }
    sourceFile << "#include \"" << data.projectName << ".h" << "\"";
    sourceFile.close();

    if (!RecompileProject())
        return false;

    AddRecentProject(projectPath, data.projectName);

    DebugLog(LogSeverity::DONE, L"Project created successfully");
    return true;
}

typedef EngineProject* (__stdcall *ProjectCreateFunc)();

bool ProjectHandler::LoadProject(std::filesystem::path projectPath)
{
    if (projectPath.has_extension())
        projectPath.remove_filename();

    FreeLibrary(m_projectLibrary);
    std::filesystem::remove(GetProjectLib());

    m_projectData = ProjectData();
    m_projectData.projectPath = projectPath;

    std::wifstream file(std::filesystem::path(projectPath.wstring() + L"/Project.gep"));
    if (!file.is_open())
    {
        DebugLog(LogSeverity::SEVERE, L"Could not open project file");
        return false;
    }

    std::wstring line;

    while (std::getline(file, line))
    {
        if (IsToken(line, L"Name"))
            m_projectData.projectName = TokenValue(line);
    }

    file.close();

    AssetManager::UpdateAssetList();

    // Loading the code
    std::wstring libraryPath = m_projectData.projectPath.wstring();
    libraryPath += L"\\cmake-build-debug\\lib" + m_projectData.projectName + L".dll";

    ForceCopy(libraryPath, GetProjectLib());

    std::cout << WStringToUTF8(libraryPath) << std::endl;

    m_projectLibrary = LoadLibraryW(GetProjectLib().c_str());

    if (!m_projectLibrary) {
        std::cout << "Could not load the dynamic library" << std::endl;
        return false;
    }

    const auto function = reinterpret_cast<ProjectCreateFunc>(GetProcAddress(m_projectLibrary, "GenerateProject"));
    if (!function) {
        std::cout << "Could not locate the function" << std::endl;
        FreeLibrary(m_projectLibrary);
        std::filesystem::remove(GetProjectLib());
        return false;
    }

    m_project = function();

    DebugLog(LogSeverity::DONE, L"Project loaded successfully");
    return true;
}

void ProjectHandler::ReloadProjectAsync()
{
    m_lockOut = true;

    m_recompileResult = std::async(std::launch::async, ReloadProject);
}

bool ProjectHandler::ReloadProject()
{
    if (!RecompileProject())
        return false;

    ReloadLibrary();

    m_lockOut = false;
    DebugLog(LogSeverity::DONE, L"Reloading project completed");
    return true;
}

bool ProjectHandler::RecompileProject()
{
    int cmakeStatus = std::system(("cmake -S " + ProjectFolder().generic_string() +
    " -B " + ProjectFolder().generic_string() + "\\cmake-build-debug" + " -G \"Ninja\"").c_str());
    if (cmakeStatus != 0)
    {
        DebugLog(LogSeverity::SEVERE, L"Loading CMake project failed");
        return false;
    }

    int buildStatus = std::system(("cmake --build " + ProjectFolder().generic_string() + "\\cmake-build-debug").c_str());
    if (buildStatus != 0)
    {
        DebugLog(LogSeverity::SEVERE, L"Building project source failed");
        return false;
    }

    return true;
}

bool ProjectHandler::ReloadLibrary()
{
    FreeLibrary(m_projectLibrary);
    std::filesystem::remove(GetProjectLib());

    // Loading the code
    const std::filesystem::path libraryPath = m_projectData.projectPath.wstring() + L"\\cmake-build-debug\\lib" + m_projectData.projectName + L".dll";
    ForceCopy(libraryPath, GetProjectLib());
    m_projectLibrary = LoadLibraryW((GetProjectLib()).c_str());

    if (!m_projectLibrary) {
        std::cout << "Could not load the dynamic library" << std::endl;
        DebugLog(LogSeverity::SEVERE, L"Reloading project failed. Please restart the editor to prevent corruption");
        return false;
    }

    auto function = reinterpret_cast<ProjectCreateFunc>(GetProcAddress(m_projectLibrary, "GenerateProject"));
    if (!function) {
        std::cout << "Could not locate project creator function" << std::endl;
        DebugLog(LogSeverity::SEVERE, L"Reloading project failed. Please restart the editor to prevent corruption");
        FreeLibrary(m_projectLibrary);
        std::filesystem::remove(GetProjectLib());
        return false;
    }

    m_project = function();
    return true;
}

bool ProjectHandler::ValidateProjectFolder(std::filesystem::path& projectPath)
{
    if (projectPath.has_extension())
        projectPath.remove_filename();
    projectPath.make_preferred();

    return std::filesystem::exists(projectPath.string() + "/Project.gep");
}

bool ProjectHandler::ValidateProjectData(const ProjectData& data)
{
    if (data.projectName.empty()) return false;
    return true;
}

void ProjectHandler::AddRecentProject(const std::filesystem::path& projectPath, const std::wstring& name)
{
    std::wofstream file("RecentProjects.txt", std::ios_base::app);
    if (!file.is_open())
        return;
    file << name << " = " << projectPath.generic_wstring() << std::endl;
    file.close();
}

std::unordered_map<std::wstring, std::filesystem::path> ProjectHandler::ReadRecentProjects()
{
    std::wifstream file("RecentProjects.txt");

    std::wstring line;

    std::unordered_map<std::wstring, std::filesystem::path> recentProjects;

    while (std::getline(file, line))
    {
        std::wstring name = line.substr(0, line.find(L" = "));
        std::wstring path = line;
        path = path.erase(0, path.find(L" = ") + 3);
        recentProjects[name] = path;
    }

    return recentProjects;
}

// TODO: There is probably a much better option for this but my 1 AM brain thinks this is fine
std::wstring ProjectHandler::DefaultSourceFile(const std::wstring& projectName)
{
    return L"#pragma once\n"
    L"#include <Engine.h>\n\n"
    L"using namespace Engine;\n\n"
    L"class "+projectName+L" final : public EngineProject\n"
    L"{\n"
    L"public:\n"
    L"   void Init() override {}\n"
    L"   void Update(float deltaTime) override {}\n"
    L"   void Shutdown() override {}\n"
    L"};\n\n"
    L"// This function is for handling project reloading. It should not be removed!\n"
    L"extern \"C\" __declspec(dllexport) inline EngineProject* GenerateProject() { return new "+projectName+L"(); }";
}

std::wstring ProjectHandler::DefaultCMakeFile(const std::wstring& projectName)
{
    std::wstring enginePath = std::filesystem::current_path().wstring();
    std::ranges::replace(enginePath, '\\', '/');

    return L"cmake_minimum_required(VERSION 3.16)\n\n"
    L"cmake_policy(SET CMP0077 NEW)\n\n"
    L"project("+projectName+L")\n\n"
    L"set(CMAKE_CXX_STANDARD 20)\n\n"
    L"set(ENGINE_PATH "+enginePath+L")\n\n"
    L"add_subdirectory(\n\t\t${ENGINE_PATH}\n\t\t${CMAKE_BINARY_DIR}/engine_raytrace_build\n)\n\n"
    L"add_library("+projectName+L" SHARED Source/"+projectName+L".cpp)\n\n"
    L"target_link_libraries("+projectName+L" PRIVATE Engine)\n\n"
    L"target_include_directories("+projectName+L" PRIVATE\n\t\t${ENGINE_PATH}/include\n\t\t${ENGINE_PATH}/src\n)\n\n";
}
}
