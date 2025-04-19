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

   /**
    * @brief Open the default file selector window for the operating system
    * TODO: [Rework to allow specifying which file type, then move into helper class]
    * @param path The path that the user has chosen
    * @param projectOnly (OPTIONAL) False: only allow choosing folder, True: only allow choosing .gep files
    * @return True if the user has chosen a file / folder
    */
   static bool ShowFileSelect(std::filesystem::path& path, bool projectOnly = false);

   /**
    * @brief Create a new project in the given folder, with the given data
    * @param projectPath The folder in which the project will be saved
    * @param data The data for the project which will be written to Project.gep
    * @return
    */
   static bool CreateProject(std::filesystem::path& projectPath, const ProjectData& data);

   /**
    * @brief Load a project, unloads the currently open project
   * @param projectPath The folder path of the project
    * @return True if the project loaded successfully
    */
   static bool LoadProject(std::filesystem::path& projectPath);


   /**
    * @brief Check if the given folder path contains a valid project file
    * @param projectPath The folder path that needs to be validated
    * @return True if the project file exists in the folder path
    */
   static bool ValidateProjectFolder(std::filesystem::path& projectPath);

   /**
    * @brief Check if the given data is safe to work on for the engine
    * @param data The project data that needs to be validated
    * @return True if all values are within acceptable range
    */
   static bool ValidateProjectData(const ProjectData& data);

   /**
    * @brief Check if the current line of a file has a given token
    * TODO: Move into helper class
    * @param line The current line
    * @param token The token you want to check
    * @return True if the token in the line is equal to the token that is given
    */
   static bool IsToken(const std::string& line, const std::string& token);

   /**
    * @brief Get the value in a specific line
    * TODO: Move into helper class
    * @param line The current line
    * @return The value that is set on this line without the token
    */
   static std::string TokenValue(const std::string& line);

   /**
    * @brief Adds a recent project to the recent projects file so it can be read later
    * @param projectPath Path to the Project.gep file
    * @param name Name of the project
    */
   static void AddRecentProject(const std::filesystem::path& projectPath, const std::string& name);

   /**
    * @brief Read the recent project file that is located in the engine folder
    * @return A map of all projects and project paths that are in the recent projects file
    */
   static std::unordered_map<std::string, std::filesystem::path> ReadRecentProjects();

   static bool m_selectorOpen;
   static bool m_creatorOpen;

   static ProjectData m_projectData;
};

/**
 * @return True if a project is currently loaded
 */
static bool ProjectLoaded() { return !ProjectHandler::m_projectData.projectName.empty(); }
/**
 * @return The folder path of the currently opened project
 */
static std::filesystem::path ProjectFolder() { return ProjectHandler::m_projectData.projectPath; }
/**
 * @return The name of the currently opened project
 */
static std::string ProjectName() { return ProjectHandler::m_projectData.projectName; }
/**
 * @return A reference to the data of the currently opened project
 */
static ProjectData& FullProjectData() { return ProjectHandler::m_projectData; }
}
