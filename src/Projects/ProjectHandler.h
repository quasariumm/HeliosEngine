#pragma once
#include <filesystem>

namespace Engine
{

static std::filesystem::path g_projectPath;

class ProjectHandler
{
    public:

    static void ShowProjectSelector(bool show) { m_selectorOpen = show; };
    static void ProjectSelectorWindow();

    static bool ShowFileSelect(std::filesystem::path& path, bool projectOnly = false);

    static void CreateProject(std::filesystem::path& projectPath);
    static void LoadProject(const std::filesystem::path& projectPath);
    static bool ValidateProject(std::filesystem::path& projectPath);

    static bool m_selectorOpen;
};

}
