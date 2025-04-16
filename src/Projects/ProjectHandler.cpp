#include "ProjectHandler.h"
#include "imgui.h"
#include "Debugger/Debugger.h"

#include <nfd.hpp>

namespace Engine
{

bool ProjectHandler::m_selectorOpen = false;

void ProjectHandler::ProjectSelectorWindow()
{
    if (!m_selectorOpen)
        return;

    ImGui::Begin("Project Selector", nullptr, ImGuiWindowFlags_NoDocking);

    std::filesystem::path path;

    if (ImGui::Button("Create Project"))
    {
        if (ShowFileSelect(path))
        {
            if (ValidateProject(path))
                DebugLog(LogSeverity::ERROR, "Project already exists");
            else
            {
                ShowProjectSelector(false);
                CreateProject(path);
                LoadProject(path);
            }
        }
    }

    if (ImGui::Button("Open Project"))
    {
        if (ShowFileSelect(path, true))
        {
            if (ValidateProject(path))
            {
                ShowProjectSelector(false);
                LoadProject(path);
            }
            else
                DebugLog(LogSeverity::ERROR, "Project doesn't exist");
        }
    }
    ImGui::End();
}

bool ProjectHandler::ShowFileSelect(std::filesystem::path& path, bool projectOnly)
{
    NFD_Init();

    nfdu8filteritem_t filters[1];
    int filterCount = 0;

    nfdu8char_t *outPath;
    nfdresult_t result;

    if (projectOnly)
    {
        filters[0] = {"Projects", "gep" };
        filterCount++;
        result = NFD_OpenDialog(&outPath, filters, filterCount, nullptr);
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

void ProjectHandler::CreateProject(std::filesystem::path& projectPath)
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
        return;
    }

    file.close();

    g_projectPath = projectPath;

    DebugLog(LogSeverity::DONE, "Project created successfully");
}

void ProjectHandler::LoadProject(const std::filesystem::path& projectPath)
{
    g_projectPath = projectPath;
    DebugLog(LogSeverity::DONE, "Project loaded successfully");
}


bool ProjectHandler::ValidateProject(std::filesystem::path& projectPath)
{
    if (projectPath.has_extension())
        projectPath.remove_filename();

    return std::filesystem::exists(projectPath.string() + "/Project.gep");
}
}
