#include "editor/engine_interface.hpp"

#include "tools/icon_defines.hpp"

constexpr float font_size = 16.0f;

using namespace Engine;
using namespace Editor;

EditorInterface::EditorInterface()
{
    // ImGuiIO& io = ImGui::GetIO(); (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //
    // // Add fonts
    // io.Fonts->AddFontFromFileTTF("extern/imgui/misc/fonts/Roboto-Medium.ttf", font_size);
    //
    // ImFontConfig config;
    // config.MergeMode = true;
    // config.GlyphMinAdvanceX = font_size * 1.1f;
    // config.GlyphOffset = {0, font_size * 0.1f};
    // static constexpr ImWchar32 icon_ranges[] = {ICON_MIN_MDI, ICON_MAX_MDI, 0};
    // io.Fonts->AddFontFromFileTTF("assets/fonts/materialdesignicons-webfont.ttf", font_size * 1.1f, &config, icon_ranges);
    //
    // m_codeFont = io.Fonts->AddFontFromFileTTF("assets/fonts/CascadiaCode.ttf", font_size);
    //
    // io.Fonts->Build();
    //
    // ImGui::StyleColorsDark();
}

EditorInterface::~EditorInterface()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorInterface::StartFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport();
}

void EditorInterface::DrawInterfaces()
{
    for (const auto& draw : interfaces)
        draw();
}

void EditorInterface::EndFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
