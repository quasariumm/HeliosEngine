#include "Viewport.h"

#include "Debugger/Debugger.h"

namespace Engine
{

GL46_Texture2D* Viewport::m_renderedImage = nullptr;
Camera* Viewport::m_editorCamera = nullptr;

void Viewport::DrawInterface()
{
    ImGui::Begin(ICON_MONITOR_SCREENSHOT" Viewport");

    if (m_renderedImage == nullptr || m_editorCamera == nullptr)
    {
        DebugLog(LogSeverity::ERROR, "Something went wrong when trying to show the rendered image");
        ImGui::End();
        return;
    }

    m_editorCamera->SetAllowUseControls(ImGui::IsWindowHovered());

    // Thanks envoyious! https://github.com/ocornut/imgui/issues/5118
    ImVec2 screenSize = ImGui::GetContentRegionAvail();
    float scale = std::min( screenSize.x / static_cast<float>(m_renderedImage->GetWidth()), screenSize.y / static_cast<float>(m_renderedImage->GetHeight()));
    ImVec2 textureSize = ImVec2( static_cast<float>(m_renderedImage->GetWidth()) * scale, static_cast<float>(m_renderedImage->GetHeight()) * scale);

    ImVec2 offset = (screenSize - textureSize) * 0.5f;
    ImGui::SetCursorPos(ImGui::GetCursorStartPos() + offset);
    // Draw the final rendered image
    ImGui::Image(
        (ImTextureID)(intptr_t)m_renderedImage->GetID(),
        ImVec2(
            static_cast<float>(m_renderedImage->GetWidth()) * scale,
            static_cast<float>(m_renderedImage->GetHeight()) * scale
        )
    );

    ImGui::End();
}

void Viewport::DrawGizmo()
{
    ImDrawList* drawList = ImGui::GetForegroundDrawList();

}
}
