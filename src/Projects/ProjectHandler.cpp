#include "ProjectHandler.h"
#include "imgui.h"
#include "Debugger/Debugger.h"

#include <nfd.hpp>

#include "Assets/AssetManager.h"

namespace Engine
{

ProjectData ProjectHandler::m_projectData = ProjectData();
bool ProjectHandler::m_selectorOpen = false;
bool ProjectHandler::m_creatorOpen = false;

void ProjectHandler::ProjectWindows()
{
    static std::filesystem::path path;

    if (m_creatorOpen)
    {
        ImGui::Begin("Project Creator", nullptr, ImGuiWindowFlags_NoDocking);

        if (ImGui::Button("Select folder"))
            ShowFileSelect(path);

        ProjectData newProjectData;

        ImGui::Text(ICON_FOLDER);
        ImGui::SameLine(30);
        ImGui::Text( path.string().c_str());

        static char projectName[128];
        ImGui::Text(ICON_RENAME);
        ImGui::SameLine(30);
        ImGui::InputText("Project Name", projectName, 128);
        newProjectData.projectName = projectName;

        if (ImGui::Button("Create"))
        {
            if (ValidateProjectFolder(path))
                DebugLog(LogSeverity::ERROR, "There is already a project in this folder");
            else if (!ValidateProjectData(newProjectData))
                DebugLog(LogSeverity::ERROR, "Incorrect project settings");
            else
            {
                if (CreateProject(path, newProjectData))
                    if (LoadProject(path))
                        ShowProjectCreator(false);
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
                    DebugLog(LogSeverity::ERROR, "Project doesn't exist");
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
            std::unordered_map<std::string, std::filesystem::path> recentProjects = ReadRecentProjects();
            for (std::pair<std::string, std::filesystem::path> recent : recentProjects)
            {
                if (ImGui::Selectable(recent.first.c_str()))
                {
                    if (!ValidateProjectFolder(recent.second))
                        DebugLog(LogSeverity::ERROR, "Project doesn't exist");
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

    std::filesystem::path projectFilePath = projectPath;
    projectFilePath.append("Project.gep");

    create_directories(projectPath);

    std::ofstream file(projectFilePath);
    if (!file.is_open())
    {
        DebugLog(LogSeverity::ERROR, "Something went wrong creating project: " + projectFilePath.string());
        return false;
    }

    file << "[Project]\n";
    file << "Name = " << data.projectName << "\n";

    file.close();

    AddRecentProject(projectFilePath, data.projectName);

    DebugLog(LogSeverity::DONE, "Project created successfully");
    return true;
}

bool ProjectHandler::LoadProject(std::filesystem::path& projectPath)
{
    m_projectData = ProjectData();

    std::ifstream file(projectPath.append("Project.gep"));
    if (!file.is_open())
    {
        DebugLog(LogSeverity::ERROR, "Could not open project file");
        return false;
    }

    m_projectData.projectPath = projectPath.remove_filename();

    std::string line;

    while (std::getline(file, line))
    {
        if (IsToken(line, "Name"))
            m_projectData.projectName = TokenValue(line);
    }

    file.close();

    AssetManager::m_currentPath = m_projectData.projectPath;
    AssetManager::UpdateAssetList();

    DebugLog(LogSeverity::DONE, "Project loaded successfully");
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

bool ProjectHandler::IsToken(const std::string& line, const std::string& token)
{
    return line.substr(0, line.find(" = ")) == token;
}

std::string ProjectHandler::TokenValue(const std::string& line)
{
    std::string token = line;
    return token.erase(0, line.find(" = ") + 3);
}

void ProjectHandler::AddRecentProject(const std::filesystem::path& projectPath, const std::string& name)
{
    std::ofstream file("RecentProjects.txt", std::ios_base::app);
    if (!file.is_open())
        return;
    file << name << " = " << projectPath.generic_string() << std::endl;
    file.close();
}

std::unordered_map<std::string, std::filesystem::path> ProjectHandler::ReadRecentProjects()
{
    std::ifstream file("RecentProjects.txt");

    std::string line;

    std::unordered_map<std::string, std::filesystem::path> recentProjects;

    while (std::getline(file, line))
    {
        std::string name = line.substr(0, line.find(" = "));
        std::string path = line;
        path = path.erase(0, path.find(" = ") + 3);
        recentProjects[name] = path;
    }

    return recentProjects;
}

}
