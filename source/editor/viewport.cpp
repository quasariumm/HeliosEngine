// #include "ImGuizmo.h"
#include "editor/editor_menus.hpp"
#include "components/basic_components.hpp"

#include "../backends/opengl46_glfw/graphics/gl46_texture_2d.hpp"
#include "core/engine.hpp"

using namespace Helios;
using namespace Editor;


void Viewport::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
	if (ImGui::Begin(ICON_MONITOR_SCREENSHOT" Viewport"))
	{
		const auto pos = ImGui::GetWindowPos();
		m_viewportPosition = {pos.x, pos.y};
		const auto size = ImGui::GetContentRegionAvail();
		if (m_renderedImage != nullptr)
		{
			// Thanks envoyious! https://github.com/ocornut/imgui/issues/5118
			const float  scale      = std::min(size.x / static_cast<float>(m_renderedImage->GetWidth()),
			                             size.y / static_cast<float>(m_renderedImage->GetHeight()));
			const auto textureSize = ImVec2(static_cast<float>(m_renderedImage->GetWidth()) * scale,
			                                static_cast<float>(m_renderedImage->GetHeight()) * scale);

			const ImVec2 offset = (size - textureSize) * 0.5f;
			ImGui::SetCursorPos(ImGui::GetCursorStartPos() + offset);
			// Draw the final rendered image
			ImGui::Image(
					static_cast<ImTextureID>(static_cast<intptr_t>(m_renderedImage->GetID())),
					ImVec2(
							static_cast<float>(m_renderedImage->GetWidth()) * scale,
							static_cast<float>(m_renderedImage->GetHeight()) * scale
							),
							ImVec2(0, 0),
							ImVec2(1, 1),
							ImVec4(1, 1, 1, 1),
							ImVec4(1, 1, 1, 0)
					);
		}
		else
		{
			//Log::Error("Something went wrong when trying to show the rendered image");
		}

		if (size.x > 0.f && size.y > 0.f)
			m_viewportSize = {size.x, size.y};

		DrawOverlay();

	}
	ImGui::End();
	ImGui::PopStyleVar(2);
}

void Viewport::DrawGizmos() const
{
	// TODO(Quillan): Implement ImGuizmo
    // Camera cam = ECS::Registry()->get<Camera>(systemsHandler.CameraSystem().GetEditorCamera());
    // glm::mat4 cam_matrix = ECS::Registry()->get<Components::Transform>(Engine.CameraSystem().GetEditorCamera()).GetMatrix();
    //
    // glm::mat4 projection = CameraSystem::CalculateProjectionMatrix(cam, GetAspect());
    // glm::mat4 view = CameraSystem::CalculateViewMatrix(cam_matrix);
    //
    // if (SceneGraph::selected == entt::null) return;
    //
    // Components::Transform& transform = ECS::Registry()->get<Components::Transform>(SceneGraph::selected);
    //
    // ImGuizmo::SetDrawlist();
    // ImGuizmo::SetRect(posX, posY, static_cast<float>(width), static_cast<float>(height));
    //
    // glm::mat4 mat = GetGlobalTransform(SceneGraph::selected);
    //
    // const float* actualSnap = snapping ? &snapGrid : nullptr;
    //
    // ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection), operation, ImGuizmo::MODE::WORLD, glm::value_ptr(mat), nullptr, actualSnap);
    //
    // if (ImGuizmo::IsUsing())
    // {
    //     glm::mat4 parentInverse = glm::inverse(GetParentGlobalTransform(SceneGraph::selected));
    //     glm::mat4 newLocalMatrix = parentInverse * mat;
    //
    //     transform.SetMatrix(newLocalMatrix);
    //     transform.MarkModified();
    // }
}


std::optional<glm::uvec2> Viewport::GetMousePositionInViewport() const
{
	const auto mousePos = ImGui::GetMousePos();
	if (!CHECK_AABB2D(mousePos, m_viewportPosition, m_viewportPosition + m_viewportSize))
		return {};

	const auto diff = glm::uvec2{mousePos.x, mousePos.y} - m_viewportPosition;
	return diff;
}


void Viewport::DrawOverlay()
{
    ImGui::SetNextWindowSize(ImGui::GetWindowSize());
    ImGui::SetNextWindowPos({ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFontSize() * 2.5f});
    if (ImGui::Begin("ViewportOverlay", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
		// TODO(Quillan): Implement ImGuizmo
        // glm::mat4 cam_matrix = ECS::Registry()->get<Components::Transform>(Engine.CameraSystem().GetEditorCamera()).GetMatrix();
        // glm::mat4 view = CameraSystem::CalculateViewMatrix(cam_matrix);
        //
        // ImGuizmo::ViewManipulate(glm::value_ptr(view), 100.0f,
        //     {ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - ImGui::GetFontSize() * 5.5f,
        //         ImGui::GetWindowPos().y + ImGui::GetWindowHeight() - ImGui::GetFontSize() * 7.5f},
        //     {ImGui::GetFontSize() * 5.0f, ImGui::GetFontSize() * 5.0f},
        //     ImColor(0.1f,0.1f,0.1f, 0.8f));

        float f_s = ImGui::GetFontSize();
        float w_w = ImGui::GetContentRegionAvail().x;
        //float w_h = ImGui::GetContentRegionAvail().y;

    	// TODO(Quillan): Figure out editor camera
        // Camera speed
        // std::string cst = fmt::format(ICON_VIDEO" {:.0f}", Engine.CameraSystem().GetEditorCamSpeed());
        // ImGui::SetCursorPosX(w_w - f_s * 4.0f);
        // ImGui::ProgressBar(0.05f, ImVec2(f_s * 3.0f, f_s * 1.5f), cst.c_str());

        // Snapping
        ImGui::SameLine();
        ImGui::SetCursorPosX(w_w - f_s * 19.5f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, {0.5f, 0.5f});
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2( 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0));
        if (ImGui::BeginChild("Snap grid", {f_s * 4.5f, f_s * 1.5f}))
        {
            if (ImGui::Selectable(ICON_MAGNET"", snapping, ImGuiSelectableFlags_None, {f_s * 2.0f, f_s * 1.5f}))
                snapping = !snapping;
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_FrameBg, {0,0,0,0});
            const char* fmt = snapGrid >= 10.0f ? "%.0f" : "%.1f";
            float offset = snapGrid >= 100.0f ? 2.5f : 2.75f;
            ImGui::SetCursorPos({f_s * offset, f_s * 0.25f});
            ImGui::InputFloat("##Snap Input", &snapGrid, 0.0f, 0.0f, fmt);
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor();

        // Imguizmo operation
        ImGui::SameLine();
        ImGui::SetCursorPosX(w_w - f_s * 14.0f);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
        ImVec2 area = { f_s * 9.0f, f_s * 1.5f };
        if (ImGui::BeginChild("Movement selector", area))
        {
            // ImVec2 btnSize     = { area.x / 3.0f, area.y };
            // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2( 0, 0));
            // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0));
            // ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, {0.5f, 0.5f});
            // if (ImGui::Selectable(ICON_AXIS_ARROW"", operation == ImGuizmo::TRANSLATE, ImGuiSelectableFlags_None, btnSize))
            //     operation = ImGuizmo::TRANSLATE;
            // ImGui::SameLine();
            // if (ImGui::Selectable(ICON_ROTATE_ORBIT"", operation == ImGuizmo::ROTATE, ImGuiSelectableFlags_None, btnSize))
            //     operation = ImGuizmo::ROTATE;
            // ImGui::SameLine();
            // if (ImGui::Selectable(ICON_RESIZE"", operation == ImGuizmo::SCALE, ImGuiSelectableFlags_None, btnSize))
            //     operation = ImGuizmo::SCALE;
            // ImGui::PopStyleVar(3);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();

        // Statistics
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
                ImGui::Text(ICON_SPEEDOMETER" %s", std::format( "{:.1f} FPS", engineHandle.GetEngineStats().GetFPS()).c_str());
                ImGui::SetCursorPosX(f_s * 0.5f);
                if (fixed_step)
                {
                    static float step = 10.0f;
                    ImGui::InputFloat("Step", &step, 0.1f, 1.0f );
                    engineHandle.SetFixedTimeStep(step);
                }
                else
                {
                    ImGui::Text(ICON_SPEEDOMETER" %s", std::format("{:.3f} MS", engineHandle.GetEngineStats().GetDeltaTime() * 1000).c_str());
                    engineHandle.SetFixedTimeStep(-1.0f);
                }
                ImGui::SetCursorPosX(f_s * 0.5f);
                ImGui::Text(ICON_COUNTER" %i", engineHandle.GetEngineStats().GetFrameCount());
                ImGui::SetCursorPosX(f_s * 0.5f);
                ImGui::Checkbox("Fixed Step", &fixed_step);
            }

            ImGui::EndChild();
            ImGui::PopStyleColor(2);
            ImGui::PopStyleVar();
        } // Statistics

    }
    ImGui::End();
}
