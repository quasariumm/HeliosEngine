#pragma once
#include <vector>

#include "Editor/EditorInterface.h"

namespace Engine
{

enum class AssetType
{
    NONE,
    OTHER,
    SCENE
};

class AssetManager : EditorInterface
{
public:
    AssetManager() : EditorInterface("Asset Manager") {}

    void DrawInterface() override;

    static void DrawFolderView();
    static void DrawAssetView();
    static void DrawAssetInfo();

    static std::filesystem::path m_currentPath;
    static std::filesystem::path m_selectedAsset;

    static std::vector<std::filesystem::path> m_assetList;
    static std::vector<std::filesystem::path> m_folderList;

    static void UpdateAssetList();
    static std::string AssetIcon(const std::filesystem::path& asset);

};

}
