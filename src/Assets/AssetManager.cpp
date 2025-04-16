#include "AssetManager.h"

#include "imgui.h"
#include "Scene/SceneEditor.h"
#include "Scene/SceneStorage.h"

namespace Engine
{
std::filesystem::path AssetManager::m_currentPath = "";
std::filesystem::path AssetManager::m_selectedAsset = "";
std::vector<std::filesystem::path> AssetManager::m_folderList = {};
std::vector<std::filesystem::path> AssetManager::m_assetList = {};

void AssetManager::DrawInterface()
{
    DrawFolderView();
    DrawAssetView();
    DrawAssetInfo();
}

void AssetManager::DrawFolderView()
{
    ImGui::Begin(ICON_FA_FOLDER" Folders", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        std::string pathString = std::string(ICON_FA_FOLDER" ") + ProjectName() + "\\";
        pathString.append(std::filesystem::relative(m_currentPath, ProjectFolder()).string());
        ImGui::Text(pathString.c_str());
        ImGui::EndMenuBar();
    }

    if (!ProjectLoaded())
    {
        ImGui::Text("Please load a project");
        ImGui::End();
        return;
    }

    // Using relative path because there is sometimes still a / on the end
    if (std::filesystem::relative(m_currentPath, ProjectFolder()) != ".")
        if (ImGui::Selectable(ICON_FA_FOLDER " .."))
        {
            m_currentPath = m_currentPath.parent_path();
            UpdateAssetList();
        }

    for (auto& dir : m_folderList)
        if (ImGui::Selectable((std::string(ICON_FA_FOLDER" ") + dir.filename().string()).c_str()))
        {
            m_currentPath = dir;
            UpdateAssetList();
        }

    ImGui::End();
}

void AssetManager::DrawAssetView()
{
    ImGui::Begin(ICON_FA_TABLE_LIST" Assets", nullptr, ImGuiWindowFlags_MenuBar);

    if (!ProjectLoaded())
    {
        ImGui::Text("Please load a project");
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("New asset"))
        {
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (!ProjectLoaded())
    {
        ImGui::Text("Please load a project");
        ImGui::End();
        return;
    }

    for (auto& asset : m_assetList)
        if (ImGui::Selectable((AssetIcon(asset) + asset.filename().string()).c_str(), asset == m_selectedAsset))
            m_selectedAsset = asset;

    ImGui::End();
}

void AssetManager::DrawAssetInfo()
{
    ImGui::Begin(ICON_FA_TABLE_LIST" Asset Viewer");

    if (m_selectedAsset == "")
    {
        ImGui::Text("No asset selected");
        ImGui::End();
        return;
    }

    // Scene object
    if (m_selectedAsset.extension() == ".scn")
    {
        if (ImGui::Button("Load Scene"))
            ImGui::OpenPopup("Confirm open scene");

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
    if (t == ".scn") return ICON_FA_MAP" ";
    if (t == ".gep") return ICON_FA_CUBES" ";
    if (t == ".md") return ICON_FA_FILE" ";
    return ICON_FA_CUBE" ";
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
