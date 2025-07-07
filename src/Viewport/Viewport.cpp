#include "Viewport.h"

#include <tracy/Tracy.hpp>

#include "Debugger/Debugger.h"
#include "Graphics/Gizmos.h"


namespace Engine
{
GL46_Texture2D* Viewport::m_renderedImage = nullptr;
Camera* Viewport::m_editorCamera = nullptr;

ImVec2 Viewport::position = ImVec2(0, 0);
ImVec2 Viewport::size = ImVec2(0, 0);

void Viewport::DrawInterface()
{
	ZoneScopedNC("Viewport", tracy::Color::MediumPurple);
    ImGui::Begin(ICON_MONITOR_SCREENSHOT" Viewport");

    if (m_renderedImage == nullptr || m_editorCamera == nullptr)
    {
        DebugLog(LogSeverity::SEVERE, L"Something went wrong when trying to show the rendered image");
        ImGui::End();
        return;
    }

    m_editorCamera->SetAllowUseControls(ImGui::IsWindowHovered());

    // Thanks envoyious! https://github.com/ocornut/imgui/issues/5118
    const ImVec2 screenSize = ImGui::GetContentRegionAvail();
    const float scale = std::min(screenSize.x / static_cast<float>(m_renderedImage->GetWidth()),
                           screenSize.y / static_cast<float>(m_renderedImage->GetHeight()));
    const ImVec2 textureSize = ImVec2(static_cast<float>(m_renderedImage->GetWidth()) * scale,
                                static_cast<float>(m_renderedImage->GetHeight()) * scale);

    const ImVec2 offset = (screenSize - textureSize) * 0.5f;
	const ImVec2 windowPos = ImGui::GetCursorScreenPos();
	const ImVec2 texturePosition = ImGui::GetCursorStartPos() + offset;
    ImGui::SetCursorPos(texturePosition);
    // Draw the final rendered image
    ImGui::Image(
        (ImTextureID)(intptr_t)m_renderedImage->GetID(),
        textureSize
    );

	position = windowPos + texturePosition;
	size = textureSize;

	const Line3D lineTest{{0, 0, 0}, {0, 100, 0}};
	Gizmos::DrawLine(lineTest, vec4f(0.f, 1.f, 0.f, 1.f), 3);

    ImGui::End();
}
}
