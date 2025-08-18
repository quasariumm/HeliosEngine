#include "VideoRenderer.h"

#include "Editor/EditorSettings.h"
#include "Projects/ProjectHandler.h"
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <tracy/Tracy.hpp>

#include "Debugger/Debugger.h"


namespace Engine {

uint16_t VideoRenderer::framesToRender = 0;
uint16_t VideoRenderer::fps = 0;
uint16_t VideoRenderer::samplesPerFrame = 0;

std::filesystem::path VideoRenderer::m_videoDirectory;
std::string VideoRenderer::m_videoName;

VideoRenderer::VideoRenderer()
	: EditorInterface(L"Video Renderer", EditorInterfaceSource::EDITOR)
{

}


void VideoRenderer::DrawInterface()
{
	ZoneScopedNC("Video Renderer", tracy::Color::MediumPurple);
	ImGui::Begin(ICON_VIDEO_IMAGE" Video Renderer");

	const EditorSettingsData& editorSettings = EditorSettings::Get();

	ImGui::Text("Viewport pixel size (change in settings):");
	const vec2u viewportSize = editorSettings.m_viewportRenderSize;
	ImGui::Text("%u x %u", viewportSize.x, viewportSize.y);

	ImGui::Separator();

	ImGui::Text("Rendering settings:");

	ImGui::DragScalar("Frames", ImGuiDataType_U16, &framesToRender);
	ImGui::DragScalar("Frames per second", ImGuiDataType_U16, &fps);
	ImGui::DragScalar("Samples per frame", ImGuiDataType_U16, &samplesPerFrame);

	ImGui::Separator();

	ImGui::Text("Render location");

	if (ImGui::Button("Browse"))
		ProjectHandler::ShowFileSelect(m_videoDirectory);
	ImGui::SameLine();
	ImGui::Text(m_videoDirectory.string().c_str());

	ImGui::InputText("Video name (without extension)", &m_videoName);

	if (ImGui::Button("Render"))
		BeginRecording();

	ImGui::End();
}


void VideoRenderer::SaveFrame()
{
	const EditorSettingsData& editorSettings = EditorSettings::Get();
	const vec2u viewportSize = editorSettings.m_viewportRenderSize;
	// Save using OpenCV or something
	DebugLog(LogSeverity::INFO, L"SaveFrame() called");
}


void VideoRenderer::StopRecording()
{
	gVideoRenderingEnabled = false;
}


void VideoRenderer::BeginRecording()
{
	gVideoRenderingEnabled = true;
	gVideoFrame = 0;
}

} // Engine