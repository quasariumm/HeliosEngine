#include "Viewport.h"

#include <tracy/Tracy.hpp>

#include "Debugger/Debugger.h"

namespace Engine
{
GL46_Texture2D* Viewport::m_renderedImage = nullptr;
Camera* Viewport::m_editorCamera = nullptr;

void Viewport::DrawInterface()
{
	ZoneScopedNC("Viewport", tracy::Color::MediumPurple);
    ImGui::Begin(ICON_MONITOR_SCREENSHOT" Viewport");

    if (m_renderedImage == nullptr || m_editorCamera == nullptr)
    {
        DebugLog(LogSeverity::ERROR, L"Something went wrong when trying to show the rendered image");
        ImGui::End();
        return;
    }

    m_editorCamera->SetAllowUseControls(ImGui::IsWindowHovered());

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

    const ImVec2 windowOrigin = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();

    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    // ImVec2 test = ProjectPoint({0,0,0});
    // drawList->AddCircleFilled(test, 50.0f, ImColor(255,255,255,255));

    //Line2D line = ProjectLine(Line3D{{0, 0, 0}, {0, 100, 0}});
    //drawList->AddLine(line.a, line.b, ImColor(0, 255, 0, 255), 3);

    //DrawGizmo(m_editorCamera->GetCameraPosition() + m_editorCamera->GetCameraFrontVec() * 10.0f);

    ImGui::End();
}

void Viewport::DrawGizmo(const vec3& pos, float size)
{
    if (m_editorCamera == nullptr) return; // Just doing this to suppress the warnings... They're annoying

    Line2D front = ProjectLine(Line3D(pos, pos + vec3{0, 0, 1}));
    Line2D right = ProjectLine(Line3D(pos, pos + vec3{1, 0, 0}));
    Line2D up = ProjectLine(Line3D(pos, vec3{0, 1, 0}));

    ImDrawList* drawList = ImGui::GetForegroundDrawList();

    drawList->AddLine(front.a, front.b, ImColor(0, 255, 0), 3);
    drawList->AddLine(right.a, right.b, ImColor(255, 0, 0), 3);
    drawList->AddLine(up.a, up.b, ImColor(0, 0, 255), 3);
}

Line2D Viewport::ProjectLine(const Line3D& line)
{
    return {ProjectPoint(line.a), ProjectPoint(line.b)};
}

ImVec2 Viewport::ProjectPoint(const vec3& PW)
{
    if (m_editorCamera == nullptr) return {};
    return {};

    // const ImVec2 viewSize = ImGui::GetWindowSize();
    // const mat4 P = Transpose(m_editorCamera->GetProjectionMatrix({(unsigned)viewSize.x, (unsigned)viewSize.y}));
    // const mat4 V = Transpose(m_editorCamera->GetViewMatrix());
    //
    // const vec4 PC = V * P * vec4(PW.x, PW.y, PW.z, 1.0f);
    // if (PC.w <= 0.0f) return {};
    // const vec3 PNC = vec3(PC) / PC.w;
    //
    // ImVec2 SP = {};
    // SP.x = (PNC.x + 1.0f) * 0.5f * viewSize.x;
    // SP.y = (1.0f - PNC.y) * 0.5f * viewSize.y;
    // return SP + ImGui::GetWindowPos();

    // const vec3 F = m_editorCamera->GetCameraFrontVec();
    // const vec3 R = m_editorCamera->GetCameraRightVec();
    // const vec3 U = m_editorCamera->GetCameraUpVec();
    // const vec3 P = TransformPoint(P, Invert(m_editorCamera->GetViewMatrix()));
    //
    // const float Fd = Dot(F, P);
    // const float Rd = Dot(R, P);
    // const float Ud = -Dot(U, P);
    //
    // const ImVec2 Wp = ImVec2{Rd / Fd, Ud/ Fd};
    //
    // const ImVec2 Vs = ImGui::GetWindowSize();
    // ImVec2 Wpp = {};
    // Wpp.x = (Wp.x + 1.0f) * 0.5f * Vs.x;
    // Wpp.y = (Wp.y + 1.0f) * 0.5f * Vs.y;
    // return Wpp + ImGui::GetWindowPos();
}
}
