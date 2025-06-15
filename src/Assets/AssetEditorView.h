#pragma once
#include <vector>

#include "Editor/EditorInterface.h"

namespace Engine
{

enum class AssetType
{
    NONE,
    OTHER,
    SCENE,
    PROJECT
};

class AssetEditorView final : public EditorInterface
{
public:
    AssetEditorView() : EditorInterface(L"Asset Manager") {}

    void DrawInterface() override;

    static void DrawAssetView();
    static void DrawAssetInfo();

    static std::filesystem::path m_currentPath;
    static std::filesystem::path m_selectedAsset;

    static std::vector<std::filesystem::path> m_assetList;
    static std::vector<std::filesystem::path> m_folderList;

    static void UpdateAssetList();
    static std::string AssetIcon(const std::filesystem::path& asset);

    static AssetType GetAssetType(const std::string& extension)
    {
        if (extension.empty()) return AssetType::NONE;
        if (extension == ".scn") return AssetType::SCENE;
        if (extension == ".gep") return AssetType::PROJECT;
        return AssetType::OTHER;
    }
    static AssetType GetAssetTypeByPath(const std::filesystem::path& path) { return GetAssetType(path.extension().string()); }

    static void ScenePopup();
    static void DeletePopup(std::filesystem::path& deleteFile);
    static void RenamePopup(std::filesystem::path& renameFile);
    static void NewFilePopup(bool& show);
};

}
