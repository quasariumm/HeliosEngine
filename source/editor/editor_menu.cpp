#include "editor/editor_menus.hpp"

#include "core/engine.hpp"

using namespace Helios;
using namespace Editor;


void EditorMenu::Draw()
{
	if (!ImGui::BeginMainMenuBar())
		return;

	if (ImGui::BeginMenu("File"))
		ImGui::EndMenu();

	static bool showStyleEditor = false;
	static bool showMetricsMenu = false;

	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::BeginMenu("Viewport"))
		{
			//ImGui::MenuItem("Statistics", nullptr, &Engine.viewport().drawStatistics);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Physics"))
		{
			//ImGui::MenuItem("Wireframes", nullptr, &Engine.Physics().drawDebug);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("ImGui"))
		{
			ImGui::MenuItem("Style Editor", nullptr, &showStyleEditor);
			ImGui::MenuItem("Metrics", nullptr, &showMetricsMenu);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Debug output"))
		{
			if (ImGui::Selectable("None", engineHandle.debugView == 0))
				engineHandle.debugView = 0;
			if (ImGui::Selectable("Normals", engineHandle.debugView == 1))
				engineHandle.debugView = 1;
			if (ImGui::Selectable("Tangents", engineHandle.debugView == 2))
				engineHandle.debugView = 2;
			if (ImGui::Selectable("Bitangents", engineHandle.debugView == 3))
				engineHandle.debugView = 3;
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}

	const float btnWidth = ImGui::GetFontSize() * 4;
	const float hO       = btnWidth * 0.5f;
	const float ohO      = btnWidth * 1.5f;

	switch (engineHandle.GetMode())
	{
	case Mode::EDITOR:
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ohO);
		ImGui::BeginDisabled();
		ImGui::Button(ICON_PAUSE, {btnWidth, 0});
		ImGui::EndDisabled();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - hO);
		if (ImGui::Button(ICON_PLAY, {btnWidth, 0}))
			engineHandle.Play();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f + hO);
		ImGui::BeginDisabled();
		ImGui::Button(ICON_EJECT, {btnWidth, 0});
		ImGui::EndDisabled();
		break;
	case Mode::GAME:
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ohO);
		if (ImGui::Button(ICON_PAUSE, {btnWidth, 0}))
			engineHandle.Pause();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - hO);
		if (ImGui::Button(ICON_STOP, {btnWidth, 0}))
			engineHandle.Stop();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f + hO);
		if (engineHandle.GetUsingEditorCam())
		{
			if (ImGui::Button(ICON_CAMERA_LOCK, {btnWidth, 0}))
				engineHandle.LockCamera();
		}
		else if (ImGui::Button(ICON_EJECT, {btnWidth, 0}))
			engineHandle.ReleaseCamera();
		break;
	case Mode::PAUSED:
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ohO);
		if (ImGui::Button(ICON_PLAY, {btnWidth, 0}))
			engineHandle.Play();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - hO);
		if (ImGui::Button(ICON_STOP, {btnWidth, 0}))
			engineHandle.Stop();
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f + hO);
		ImGui::BeginDisabled();
		ImGui::Button(ICON_EJECT, {btnWidth, 0});
		ImGui::EndDisabled();
		break;
	}

	ImGui::EndMainMenuBar();

	if (showStyleEditor)
		ImGui::ShowStyleEditor();
	if (showMetricsMenu)
		ImGui::ShowMetricsWindow();
}
