#include "editor/editor_menus.hpp"

#include "../backends/opengl46_glfw/graphics/gl46_texture_2d.hpp"

using namespace Engine;
using namespace Editor;

void Viewport::Draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    if (ImGui::Begin(ICON_MONITOR_SCREENSHOT" Viewport"))
    {
        if (m_renderedImage == nullptr)
        {
            Log::Error("Something went wrong when trying to show the rendered image");
            ImGui::End();
            return;
        }

        // Thanks envoyious! https://github.com/ocornut/imgui/issues/5118
        ImVec2 screenSize = ImGui::GetContentRegionAvail();
        float scale = std::min(screenSize.x / static_cast<float>(m_renderedImage->GetWidth()),
                               screenSize.y / static_cast<float>(m_renderedImage->GetHeight()));
        ImVec2 textureSize = ImVec2(static_cast<float>(m_renderedImage->GetWidth()) * scale,
                                    static_cast<float>(m_renderedImage->GetHeight()) * scale);

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
    ImGui::PopStyleVar(2);
}