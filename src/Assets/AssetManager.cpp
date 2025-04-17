#include "AssetManager.h"

#include "imgui.h"
#include "Scene/SceneEditor.h"
#include "Scene/SceneStorage.h"

#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

namespace Engine
{
std::filesystem::path AssetManager::m_currentPath = "";
std::filesystem::path AssetManager::m_selectedAsset = "";
std::vector<std::filesystem::path> AssetManager::m_folderList = {};
std::vector<std::filesystem::path> AssetManager::m_assetList = {};

void AssetManager::DrawInterface()
{
    DrawAssetView();
    DrawAssetInfo();
}

void AssetManager::DrawAssetView()
{
    ImGui::Begin(ICON_VIEW_GRID" Assets", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (!ProjectLoaded())
        {
            ImGui::Text("Please load a project");
            ImGui::EndMenuBar();
            ImGui::End();
            return;
        }

        // Show back arrow. Using relative path because there is sometimes still a / on the end
        if (relative(m_currentPath, ProjectFolder()) == ".")
        {
            ImGui::BeginDisabled();
            ImGui::MenuItem(ICON_ARROW_UP_THICK);
            ImGui::EndDisabled();
        }
        else
        {
            if (ImGui::MenuItem(ICON_ARROW_UP_THICK))
                    m_currentPath = m_currentPath.parent_path(), UpdateAssetList();
        }

        ImGui::Separator();

        // Show current folder path
        std::string pathString = std::string(ICON_FOLDER" ") + ProjectName() + "\\";
        pathString.append(relative(m_currentPath, ProjectFolder()).string());
        ImGui::Text(pathString.c_str());

        ImGui::EndMenuBar();
    }


    for (auto& dir : m_folderList)
        if (ImGui::Selectable((std::string(ICON_FOLDER" ") + dir.filename().string()).c_str()))
            m_currentPath = dir, UpdateAssetList();

    ImGui::Separator();

    for (auto& asset : m_assetList)
        if (ImGui::Selectable((AssetIcon(asset) + asset.filename().string()).c_str(), asset == m_selectedAsset))
            m_selectedAsset = asset;

    ImGui::End();
}

void AssetManager::DrawAssetInfo()
{
    ImGui::Begin(ICON_CUBE_SEND" Asset Viewer");

    if (m_selectedAsset == "")
    {
        ImGui::Text("No asset selected");
        ImGui::End();
        return;
    }

    AssetType type = GetAssetTypeByPath(m_selectedAsset);

    switch (type)
    {
    case AssetType::NONE: break;

    case AssetType::OTHER:
        if (ImGui::Button("Open in external program"))
            ShellExecuteW(nullptr,nullptr, m_selectedAsset.c_str(), nullptr, nullptr, SW_SHOW);
        break;

    case AssetType::SCENE:
        if (ImGui::Button("Load Scene"))
            ImGui::OpenPopup("Confirm open scene");
        ScenePopup();
        break;

    case AssetType::PROJECT:
        break;
    }

    ImGui::End();
}

void AssetManager::UpdateAssetList()
{
    m_folderList.clear();
    m_assetList.clear();
    m_selectedAsset = "";
    for (auto& p : std::filesystem::directory_iterator(m_currentPath))
        if (p.is_directory())
            m_folderList.push_back(p.path());
        else
            m_assetList.push_back(p.path());
}

std::string AssetManager::AssetIcon(const std::filesystem::path& asset)
{
    const std::string t = asset.extension().string();
    if (t == ".scn") return ICON_MAP" ";
    if (t == ".gep") return ICON_CONTROLLER" ";
    if (t == ".md") return ICON_MATH_LOG" ";
    return ICON_CUBE" ";
}

void AssetManager::ScenePopup()
{
    if (ImGui::BeginPopupModal("Confirm open scene"))
    {
        ImGui::Text("Are you sure you want to open the following scene file?");
        ImGui::Text(m_selectedAsset.string().c_str());
        ImGui::TextColored({1, 1, 0, 1}, "Your currently opened scene will not be saved!");
        if (ImGui::Button("Confirm"))
        {
            SceneLoader::LoadFromFile(SceneEditor::m_targetScene, m_selectedAsset);
            SceneEditor::m_sceneFile = m_selectedAsset;
            ImGui::CloseCurrentPopup();
            m_selectedAsset = "";
        }
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            m_selectedAsset = "";
        }
        ImGui::EndPopup();
    }
}
}

     //
     // if (ImGui::BeginPopupModal("Create scene"))
     // {
     //     static char name[128] = "";
     //     ImGui::InputText("Name", name, 128);
     //     if (ImGui::Button("Confirm"))
     //     {
     //         std::ofstream file;
     //         file.open(m_currentPath.string() + name + std::string(".scn"));
     //         bool validName = std::filesystem::exists(m_currentPath.string() + name + std::string(".scn"));
     //         if (file.is_open() && validName)
     //         {
     //             SceneLoader::LoadFromFile(SceneEditor::m_targetScene, m_selectedAsset);
     //             SceneEditor::m_sceneFile = m_currentPath.string() + name + std::string(".scn");
     //             ImGui::CloseCurrentPopup();
     //         }
     //         else
     //             DebugLog(LogSeverity::ERROR, "Failed to create new scene");
     //     }
     //     if (ImGui::Button("Cancel"))
     //         ImGui::CloseCurrentPopup();
     //     ImGui::EndPopup();
     // }
