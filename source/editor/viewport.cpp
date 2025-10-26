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
							)
					);
		}
		else
		{
			//Log::Error("Something went wrong when trying to show the rendered image");
		}

		if (size.x > 0.f && size.y > 0.f)
			m_viewportSize = {size.x, size.y};

		DrawOverlay(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFontSize() * 2.0f), size);

		ImGui::End();
	}
	ImGui::PopStyleVar(2);
}


void Viewport::DrawOverlay( const ImVec2 pos, const ImVec2 size ) const
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

	const float fS = ImGui::GetFontSize();
	//float wW = ImGui::GetContentRegionAvail().x;
	//float wH = ImGui::GetContentRegionAvail().y;

	// std::string cst = std::format(ICON_VIDEO" {:.0f}", EngineHandle.CameraSystem().GetEditorCamSpeed());
	// ImGui::SetCursorPosX(wW - fS * 4.0f);
	// ImGui::ProgressBar(0.05f, ImVec2(fS * 3.0f, fS * 1.5f), cst.c_str());

	if (drawStatistics)
	{
		static bool fixedStep = false;

		ImGui::PushStyleColor(ImGuiCol_ChildBg, {0, 0, 0, 0.2f});
		ImGui::SetCursorPos({fS * 1.0f, 0.0f});
		ImGui::PushStyleColor(ImGuiCol_Border, {0, 0, 0, 0.2f});
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, fS * 0.5f));

		if (ImGui::BeginChild("Debug", {fS * 10.0f, 0}, ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Border))
		{
			ImGui::SetCursorPos({fS * 0.5f, fS * 0.5f});
			ImGui::Text(
					ICON_SPEEDOMETER" %s", std::format("{:.1f} FPS", engineHandle.GetEngineStats().GetFPS()).c_str());
			ImGui::SetCursorPosX(fS * 0.5f);
			if (fixedStep)
			{
				static float step = 10.0f;
				ImGui::InputFloat("Step", &step, 0.1f, 1.0f);
				engineHandle.SetFixedTimeStep(step);
			}
			else
			{
				ImGui::Text(
						ICON_SPEEDOMETER" %s",
						std::format("{:.3f} MS", engineHandle.GetEngineStats().GetDeltaTime() * 1000).c_str());
				engineHandle.SetFixedTimeStep(-1.0f);
			}
			ImGui::SetCursorPosX(fS * 0.5f);
			ImGui::Text(ICON_COUNTER" %i", engineHandle.GetEngineStats().GetFrameCount());
			ImGui::SetCursorPosX(fS * 0.5f);
			ImGui::Checkbox("Fixed Step", &fixedStep);
			ImGui::EndChild();
		}

		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();
	}

	ImGui::End();
}
