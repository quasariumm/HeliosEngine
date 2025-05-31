#include "EditorInterface.h"

#include <tracy/Tracy.hpp>

#include "EditorSettings.h"
#include "Projects/ProjectHandler.h"

namespace Engine
{

EditorInterfaceManager& EditorInterfaceManager::Instance()
{
	static EditorInterfaceManager instance;
	return instance;
}

void EditorInterfaceManager::Initialize(Window* window)
{
    Instance().m_window = window;
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_NavEnableKeyboard;

    io.Fonts->AddFontFromFileTTF("extern/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 18.0f;
    config.GlyphOffset = {0, 2.0f};
    static constexpr ImWchar32 icon_ranges[] = {ICON_MIN_MDI, ICON_MAX_MDI, 0};
    io.Fonts->AddFontFromFileTTF("extern/fonts/materialdesignicons-webfont.ttf", 18.0f, &config, icon_ranges);

	io.Fonts->Build();
}

void EditorInterfaceManager::DrawAllInterfaces() const
{
    // Primary editor windows
    ProjectHandler::ProjectWindows();
    EditorSettings::DrawWindow();

    ImGui::DockSpaceOverViewport();

    static bool showImguiDebug = false;

	// Listen for shortcuts
	static ImGuiInputFlags globalShortcutFlags = ImGuiInputFlags_RouteAlways;
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N, globalShortcutFlags))
		ProjectHandler::ShowProjectCreator(true);
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, globalShortcutFlags))
		ProjectHandler::ShowProjectSelector(true);
	if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Comma, globalShortcutFlags))
		EditorSettings::OpenWindow();
	// TODO(Quillan): CTRL+SHIFT+O to open the "Open recent menu"
	{
    	ZoneScopedNC("Menu bar", tracy::Color::MediumOrchid2);
		if (ImGui::BeginMainMenuBar())
		{
			std::wstring projectName;
			if (ProjectHandler::ProjectLoaded())
				projectName = STR_TO_WSTR(ICON_CONTROLLER) + L" " + ProjectHandler::ProjectName();
			else
				projectName = STR_TO_WSTR(ICON_ALERT) + L" No project loaded";

			if (ImGui::BeginMenu(WStringToUTF8(projectName).c_str()))
			{
				if (ImGui::MenuItem(ICON_PLUS_BOX" New project", "CTRL+N"))
					ProjectHandler::ShowProjectCreator(true);
				if (ImGui::MenuItem(ICON_UPLOAD_BOX" Open project", "CTRL+O"))
					ProjectHandler::ShowProjectSelector(true);
				if (ImGui::BeginMenu(ICON_CLOCK" Open recent"))
				{
					static ImGuiInputFlags recentsInputFlags = ImGuiInputFlags_RouteFocused;
					int32_t i = 1;
					for (auto& recent : ProjectHandler::ReadRecentProjects())
					{
						if (i <= 10 && ImGui::Shortcut(ImGuiKey_0 + (i % 10), recentsInputFlags))
							ProjectHandler::LoadProject(recent.second);
						if (ImGui::MenuItem(
							(WStringToUTF8(recent.first) + "##" + std::to_string(i)).c_str(),
							(i <= 10) ? std::to_string(i).c_str() : nullptr
							))
							ProjectHandler::LoadProject(recent.second);
						++i;
					}
					ImGui::EndMenu();
				}
				if (ImGui::MenuItem(ICON_REFRESH" Recompile project"))
					ProjectHandler::ReloadProjectAsync();
				if (ImGui::MenuItem(ICON_EXIT_RUN" Close editor", (EditorSettings::Get().m_closeOnEscape) ? "ESC" : nullptr))
					m_window->SetShouldClose(true);

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu(ICON_TOOLBOX" Editor"))
			{
				if (ImGui::MenuItem(ICON_COG" Settings", "CTRL+,"))
					EditorSettings::OpenWindow();

				if (ImGui::BeginMenu(ICON_TOOLS" Tools"))
				{
					for (const auto& i : m_editorInterfaces)
						ImGui::MenuItem(WStringToUTF8(i.second->name).c_str(), nullptr, &i.second->active);
					ImGui::MenuItem("ImGui Debugger", nullptr, &showImguiDebug);
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

    if (showImguiDebug)
        ImGui::ShowMetricsWindow();

    for (const auto& i : m_editorInterfaces)
        if (i.second->active)
            i.second->DrawInterface();
}

void EditorInterfaceManager::SetMouseEnabled(bool enable)
{
    ImGuiIO& io = ImGui::GetIO();
    if (enable)
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    else
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void EditorInterfaceManager::SetKeyboardEnable(bool enable)
{
    ImGuiIO& io = ImGui::GetIO();
    if (enable)
        io.ConfigFlags &= ~ImGuiConfigFlags_NoKeyboard;
    else
        io.ConfigFlags |= ImGuiConfigFlags_NoKeyboard;
}
}
