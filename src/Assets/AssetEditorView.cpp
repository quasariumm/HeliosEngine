#include "AssetEditorView.h"

#include "Scene/SceneEditor.h"
#include "Scene/SceneStorage.h"

#include <shlobj.h>
#include <tracy/Tracy.hpp>


namespace Engine
{
std::filesystem::path AssetEditorView::m_currentPath = "";
std::filesystem::path AssetEditorView::m_selectedAsset = "";
std::vector<std::filesystem::path> AssetEditorView::m_folderList = {};
std::vector<std::filesystem::path> AssetEditorView::m_assetList = {};

void AssetEditorView::DrawInterface()
{
    DrawAssetView();
    DrawAssetInfo();
}

void AssetEditorView::DrawAssetView()
{
	ZoneScopedNC("Assets", tracy::Color::MediumOrchid);
    ImGui::Begin(ICON_VIEW_GRID" Assets", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (!ProjectHandler::ProjectLoaded())
        {
            ImGui::Text(ICON_ALERT" Please load a project");
            ImGui::EndMenuBar();
            ImGui::End();
            return;
        }

        // Show back arrow. Using relative path because there is sometimes still a / on the end
        if (relative(m_currentPath, ProjectHandler::ProjectFolder()) == ".")
        {
            ImGui::BeginDisabled();
            ImGui::MenuItem(ICON_ARROW_UP_THICK);
            ImGui::SetItemTooltip("Already in project folder");
            ImGui::EndDisabled();
        }
        else
        {
            if (ImGui::MenuItem(ICON_ARROW_UP_THICK))
                m_currentPath = m_currentPath.parent_path(), UpdateAssetList();
            ImGui::SetItemTooltip("Go up one folder");
        }

        ImGui::Separator();

        static bool newItem = false;
        if (ImGui::MenuItem(ICON_PLUS_THICK))
            newItem = true;
        ImGui::SetItemTooltip("Add a new file or folder");

        NewFilePopup(newItem);

        ImGui::Separator();

        if (ImGui::MenuItem(ICON_SYNC))
            UpdateAssetList();
        ImGui::SetItemTooltip("Refresh the view");

        ImGui::Separator();

        // Show current folder path
        std::wstring pathString = STR_TO_WSTR(ICON_FOLDER) + L" " + ProjectHandler::ProjectName() + L"\\";
        pathString.append(relative(m_currentPath, ProjectHandler::ProjectFolder()).wstring());
        ImGui::Text(WStringToUTF8(pathString).c_str());

        ImGui::EndMenuBar();
    }

    static std::filesystem::path deleteFile = "";
    static std::filesystem::path renameFile = "";

    // Show all folders
    std::filesystem::path oldPath = m_currentPath;
    for (auto& dir : m_folderList)
    {
        if (ImGui::Selectable(WStringToUTF8(STR_TO_WSTR(ICON_FOLDER) + L" " + dir.filename().wstring()).c_str()))
            m_currentPath = dir;

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::Selectable(ICON_DELETE" Delete"))
                deleteFile = dir;
            if (ImGui::Selectable(ICON_RENAME" Rename"))
                renameFile = dir;

            // TODO: Make implementation for Linux and Mac as well
            if (ImGui::Selectable(ICON_FOLDER_SEARCH" Open in explorer"))
                ShellExecuteA(nullptr, "open", dir.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
            ImGui::EndPopup();
        }
    }
    // If folder has changed, update the view
    if (m_currentPath != oldPath)
        UpdateAssetList();

    ImGui::Separator();

    for (auto& asset : m_assetList)
    {
        if (ImGui::Selectable((AssetIcon(asset) + asset.filename().string()).c_str(), asset == m_selectedAsset))
            m_selectedAsset = asset;

        if (ImGui::BeginPopupContextItem())
        {
            m_selectedAsset = asset;
            if (ImGui::Selectable(ICON_DELETE" Delete"))
                deleteFile = m_selectedAsset;
            if (ImGui::Selectable(ICON_RENAME" Rename"))
                renameFile = m_selectedAsset;

            // TODO: Make implementation for Linux and Mac as well
            if (ImGui::Selectable(ICON_OPEN_IN_APP" Open in default app"))
                ShellExecuteA(nullptr, "open", asset.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
            if (ImGui::Selectable(ICON_FOLDER_SEARCH" Open in explorer"))
            {
                std::filesystem::path folder = asset;
                ShellExecuteA(nullptr, "open", folder.remove_filename().string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
            }

            ImGui::EndPopup();
        }
    }

    DeletePopup(deleteFile);
    RenamePopup(renameFile);

    ImGui::End();
}

void AssetEditorView::DrawAssetInfo()
{
	ZoneScopedNC("Asset viewer", tracy::Color::MediumOrchid1);
    ImGui::Begin(ICON_CUBE_SEND" Asset Viewer", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (m_selectedAsset == "")
            ImGui::Text(ICON_FILE" No asset selected");
        else
            ImGui::Text((AssetIcon(m_selectedAsset) + m_selectedAsset.filename().string()).c_str());

        ImGui::EndMenuBar();
    }

    switch (GetAssetTypeByPath(m_selectedAsset))
    {
    case AssetType::NONE:
    case AssetType::OTHER: break;

    case AssetType::SCENE:
        if (ImGui::Button(ICON_MAP_PLUS" Load Scene"))
            ImGui::OpenPopup("Confirm open scene");
        ScenePopup();
        break;

    case AssetType::PROJECT:
        break;
    }

    ImGui::End();
}

void AssetEditorView::UpdateAssetList()
{
    if (m_currentPath == L"")
        m_currentPath = ProjectHandler::ProjectFolder();

    m_folderList.clear();
    m_assetList.clear();
    m_selectedAsset = "";
    for (auto& p : std::filesystem::directory_iterator(m_currentPath))
        if (p.is_directory())
            m_folderList.push_back(p.path());
        else
            m_assetList.push_back(p.path());
}

std::string AssetEditorView::AssetIcon(const std::filesystem::path& asset)
{
    const std::string t = asset.extension().string();
    if (t == ".scn") return ICON_MAP" ";
    if (t == ".gep") return ICON_CONTROLLER" ";
    if (t == ".md") return ICON_MATH_LOG" ";
    return ICON_FILE" ";
}

void AssetEditorView::ScenePopup()
{
    if (ImGui::BeginPopupModal("Confirm open scene"))
    {
        ImGui::Text("Are you sure you want to open the following scene file?");
        ImGui::Text(m_selectedAsset.string().c_str());
        ImGui::TextColored({1, 1, 0, 1}, "Your currently opened scene will not be saved!");
        if (ImGui::Button(ICON_CHECK_BOLD" Confirm"))
        {
            SceneLoader::LoadFromFile(SceneEditor::m_targetScene, m_selectedAsset);
            SceneEditor::m_sceneFile = m_selectedAsset;
            ImGui::CloseCurrentPopup();
            m_selectedAsset = "";
        }
        if (ImGui::Button(ICON_CLOSE_THICK" Cancel"))
        {
            ImGui::CloseCurrentPopup();
            m_selectedAsset = "";
        }
        ImGui::EndPopup();
    }
}

void AssetEditorView::DeletePopup(std::filesystem::path& deleteFile)
{
    if (deleteFile != "")
    {
        if (!ImGui::BeginPopupModal("Delete"))
        {
            ImGui::OpenPopup("Delete");
            return;
        }

        ImGui::Text("Are you sure you want to delete this file?\n%s", deleteFile.string().c_str());

        if (ImGui::Button(ICON_CHECK_BOLD" Confirm"))
        {
            std::filesystem::remove(deleteFile);
            UpdateAssetList();
            deleteFile = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_CLOSE_THICK" Cancel"))
        {
            deleteFile = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void AssetEditorView::RenamePopup(std::filesystem::path& renameFile)
{
    if (renameFile != "")
    {
        static char* newName;
        if (!ImGui::BeginPopupModal("Rename"))
        {
            ImGui::OpenPopup("Rename");
            return;
        }

        ImGui::Text("What do you want to name this file?\n%s", renameFile.string().c_str());
        ImGui::InputText("New Name", newName, 64);

        if (ImGui::Button(ICON_CHECK_BOLD" Confirm"))
        {
            std::filesystem::path newPath = renameFile;
            std::string extension = newPath.extension().string();
            newPath = newPath.remove_filename().append(newName + extension);

            std::filesystem::rename(renameFile, newPath);
            UpdateAssetList();
            renameFile = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_CLOSE_THICK" Cancel"))
        {
            renameFile = "";
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}


void AssetEditorView::NewFilePopup(bool& show)
{
    if (!show) return;
    if (!ImGui::BeginPopupModal("Create New File"))
    {
        ImGui::OpenPopup("Create New File");
        return;
    }

    static char name[64];
    static std::string fileType;
    static char otherFileType[16] = "";
    ImGui::Text("What do you want to name this file?");
    ImGui::InputText("Name", name, 64);
    ImGui::Text("What type of file do you want to create?");
    ImGui::Separator();
    if (ImGui::Selectable(ICON_FOLDER" Folder", fileType == "Folder")) fileType = "Folder";
    if (ImGui::Selectable(ICON_MAP" Scene", fileType == ".scn")) fileType = ".scn";
    if (ImGui::Selectable(ICON_FILE" Other", fileType == "Other")) fileType = "Other";
    if (fileType == "Other") ImGui::InputText("Filetype", otherFileType, 16);
    ImGui::Separator();

    if (ImGui::Button(ICON_CHECK_BOLD" Create"))
    {
        if (fileType == "Other") fileType = otherFileType;
        if (fileType == "Folder")
        {
            std::filesystem::path newPath = m_currentPath;
            newPath.append(name);
            create_directories(newPath);
            UpdateAssetList();
            DebugLog((LogSeverity)3, L"New folder created");
        }
        else
        {
            std::filesystem::path newPath = m_currentPath;
            newPath.append(name + fileType);
            std::ofstream file;
            bool alreadyExists = exists(newPath);
            file.open(newPath);
            if (file.is_open() && !alreadyExists)
            {
                UpdateAssetList();
                DebugLog((LogSeverity)3, L"New file created");
            }
            else
                DebugLog((LogSeverity)2, L"Failed to create new file");
        }

        show = false;
        ImGui::CloseCurrentPopup();
    }
    ImGui::SameLine();
    if (ImGui::Button(ICON_CLOSE_THICK" Cancel"))
    {
        show = false;
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

REGISTER_EDITOR_INTERFACE(AssetEditorView);

}
