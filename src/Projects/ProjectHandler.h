#pragma once
#include <filesystem>

namespace Engine
{

struct ProjectData
{
    std::filesystem::path projectPath = "";
    std::string projectName;
};

class ProjectHandler
{
    public:

    static void ShowProjectSelector(bool show) { m_selectorOpen = show; };
    static void ShowProjectCreator(bool show) { m_creatorOpen = show; };
    static void ProjectWindows();

    static bool ShowFileSelect(std::filesystem::path& path, bool projectOnly = false);

    static bool CreateProject(std::filesystem::path& projectPath, const ProjectData& data);
    static bool LoadProject(std::filesystem::path& projectPath);

    static bool ValidateProjectFolder(std::filesystem::path& projectPath);
    static bool ValidateProjectData(const ProjectData& data);

    static bool IsToken(const std::string& line, const std::string& token);
    static std::string TokenValue(const std::string& line);

    static bool m_selectorOpen;
    static bool m_creatorOpen;

    static ProjectData m_projectData;
};

static bool ProjectLoaded() { return !ProjectHandler::m_projectData.projectName.empty(); }
static std::filesystem::path ProjectFolder() { return ProjectHandler::m_projectData.projectPath; }
static std::string ProjectName() { return ProjectHandler::m_projectData.projectName; }
static const ProjectData& FullProjectData() { return ProjectHandler::m_projectData; }

}
