#pragma once
#include <filesystem>
#include <future>
#include <Windows.h>

namespace Engine
{
/**
 * @brief Some basic info about the currently loaded project. Gets loaded to and from the project file
 */
struct ProjectData
{
    std::wstring projectName;
    std::filesystem::path projectPath = "";
};

static std::wstring GetTmpFile(const std::wstring& fileName) { return std::filesystem::current_path().append(L"launcher\\Tmp").append(fileName); }
static std::wstring GetProjectLib() { return GetTmpFile(L"ProjLib.dll"); }

/**
 * @brief Base class for the user's project. Includes an init and update function that are called when the game is run.
 * @note A basic file is already generated when creating a project. It is possible (but not recommended) to create another class with a different name that inherits this.
 */
class EngineProject
{
public:
    virtual ~EngineProject() = default;

    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() = 0;
};

typedef EngineProject* (*CreateGameProjectFunc)();
typedef void (*DestroyGameProjectFunc)(EngineProject*);

class ProjectHandler
{
public:

    //====================================================
    // Interfaces
    //====================================================

    static void ShowProjectSelector(bool show) { m_selectorOpen = show; };
    static void ShowProjectCreator(bool show) { m_creatorOpen = show; };
    static void ProjectWindows();



    //====================================================
    // File handling
    //====================================================

    /**
     * @brief Open the default file selector window for the operating system
     * TODO: [Rework to allow specifying which file type, then move into helper class]
     * @param path The path that the user has chosen
     * @param projectOnly (OPTIONAL) False: only allow choosing folder, True: only allow choosing .gep files
     * @return True if the user has chosen a file / folder
     */
    static bool ShowFileSelect(std::filesystem::path& path, bool projectOnly = false);



    //====================================================
    // Project creation and loading
    //====================================================

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
    static bool LoadProject(std::filesystem::path projectPath);

    /**
     * @brief Same as ReloadProject() but runs it async so an overlay can be displayed with the status
     */
    static void ReloadProjectAsync();

    /**
     * @brief Recompiles the currently loaded project, copies the created DLL and loads it in.
     * @note If the build fails, the old DLL will stay loaded
     * @return True if reloading succeeded
     */
    static bool ReloadProject();

    /**
     * @brief Recompile the currently loaded project
     * @return True if the build succeeded
     */
    static bool RecompileProject();

    /**
     * @brief Reloads the library file
     * @return True if the reloading succeeded
     */
    static bool ReloadLibrary();



    //====================================================
    // Validators
    //====================================================

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



    //====================================================
    // Recent project functions
    //====================================================

    /**
     * @brief Adds a recent project to the recent projects file so it can be read later
     * @param projectPath Path to the Project.gep file
     * @param name Name of the project
     */
    static void AddRecentProject(const std::filesystem::path& projectPath, const std::wstring& name);

    /**
     * @brief Read the recent project file that is located in the engine folder
     * @return A map of all projects and project paths that are in the recent projects file
     */
    static std::unordered_map<std::wstring, std::filesystem::path> ReadRecentProjects();



    //====================================================
    // Project info getter functions
    //====================================================

    /**
     * @return True if a project is currently loaded
     */
    static bool ProjectLoaded() { return m_project != nullptr; }

    /**
     * @return Constant pointer to the loaded project
     */
    static const EngineProject* GetProject() { return m_project; }

    /**
     * @return The folder path of the currently opened project
     */
    static std::filesystem::path ProjectFolder() { return m_projectData.projectPath; }

    /**
     * @return The name of the currently opened project
     */
    static std::wstring ProjectName() { return m_projectData.projectName; }

    /**
     * @return A reference to the data of the currently opened project
     */
    static ProjectData& FullProjectData() { return m_projectData; }



    //====================================================
    // Data
    //====================================================

    static bool m_selectorOpen;
    static bool m_creatorOpen;
    static bool m_lockOut;

    static HINSTANCE m_projectLibrary;
    static EngineProject* m_project;
    static ProjectData m_projectData;

    static std::future<bool> m_recompileResult;
};

}

