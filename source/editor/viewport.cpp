#include "editor/editor_menus.hpp"

#include "../backends/opengl46_glfw/graphics/gl46_texture_2d.hpp"
#include "core/engine.hpp"

using namespace Engine;
using namespace Editor;

void Viewport::Draw()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    if (ImGui::Begin(ICON_MONITOR_SCREENSHOT" Viewport"))
    {
        if (m_renderedImage != nullptr)
        {
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
        }
        else
        {
            //Log::Error("Something went wrong when trying to show the rendered image");
        }

        DrawOverlay( ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFontSize() * 2.0f), ImGui::GetContentRegionAvail());
        
        ImGui::End();
    }
    ImGui::PopStyleVar(2);
}

void Viewport::DrawOverlay(ImVec2 pos, ImVec2 size)
{
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    if (!ImGui::Begin("ViewportOverlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoInputs |
                 ImGuiWindowFlags_NoBackground))
    {
        return;
    }

    float f_s = ImGui::GetFontSize();
    float w_w = ImGui::GetContentRegionAvail().x;
    //float w_h = ImGui::GetContentRegionAvail().y;

    // std::string cst = std::format(ICON_VIDEO" {:.0f}", EngineHandle.CameraSystem().GetEditorCamSpeed());
    // ImGui::SetCursorPosX(w_w - f_s * 4.0f);
    // ImGui::ProgressBar(0.05f, ImVec2(f_s * 3.0f, f_s * 1.5f), cst.c_str());

    if (drawStatistics)
    {
        static bool fixed_step = false;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, {0, 0, 0, 0.2f});
        ImGui::SetCursorPos({f_s * 1.0f, 0.0f});
        ImGui::PushStyleColor(ImGuiCol_Border, {0, 0, 0, 0.2f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, f_s * 0.5f));

        if (ImGui::BeginChild("Debug", {f_s * 10.0f ,0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border))
        {
            ImGui::SetCursorPos({f_s * 0.5f, f_s * 0.5f});
            ImGui::Text(ICON_SPEEDOMETER" %s", std::format( "{:.1f} FPS", EngineHandle.GetEngineStats().GetFPS()).c_str());
            ImGui::SetCursorPosX(f_s * 0.5f);
            if (fixed_step)
            {
                static float step = 10.0f;
                ImGui::InputFloat("Step", &step, 0.1f, 1.0f );
                EngineHandle.SetFixedTimeStep(step);
            }
            else
            {
                ImGui::Text(ICON_SPEEDOMETER" %s", std::format("{:.3f} MS", EngineHandle.GetEngineStats().GetDeltaTime() * 1000).c_str());
                EngineHandle.SetFixedTimeStep(-1.0f);
            }
            ImGui::SetCursorPosX(f_s * 0.5f);
            ImGui::Text(ICON_COUNTER" %i", EngineHandle.GetEngineStats().GetFrameCount());
            ImGui::SetCursorPosX(f_s * 0.5f);
            ImGui::Checkbox("Fixed Step", &fixed_step);
            ImGui::EndChild();
        }

        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar();

    }

    ImGui::End();
}
