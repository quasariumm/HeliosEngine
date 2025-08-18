#pragma once
#include "Editor/EditorInterface.h"


namespace Engine {

class VideoRenderer final : public EditorInterface
{

public:

	VideoRenderer();

	void DrawInterface() override;

	static uint16_t framesToRender;
	static uint16_t fps;
	static uint16_t samplesPerFrame;

	static void SaveFrame();

	static void StopRecording();

private:

	static void BeginRecording();

	static std::filesystem::path m_videoDirectory;
	static std::string m_videoName;

};

REGISTER_EDITOR_INTERFACE(VideoRenderer);

} // Engine
