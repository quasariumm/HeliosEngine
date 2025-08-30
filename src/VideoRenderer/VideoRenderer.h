#pragma once

#include "Editor/EditorInterface.h"

struct AVFrame;
struct AVPacket;
struct AVCodec;
struct AVCodecContext;
struct SwsContext;
struct AVFormatContext;
struct AVOutputFormat;
struct AVStream;

namespace Engine {

struct AVContext
{
	AVFrame* frame = nullptr;
	AVPacket* packet = nullptr;

	const AVCodec* codec = nullptr;
	AVCodecContext* codecCtx = nullptr;

	SwsContext* swsCtx = nullptr;

	AVFormatContext* formatCtx = nullptr;
	const AVOutputFormat* outputFormat = nullptr;
	AVStream* stream = nullptr;
};

class VideoRenderer final : public EditorInterface
{

public:

	VideoRenderer();

	~VideoRenderer() override;

	void DrawInterface() override;

	static uint16_t framesToRender;
	static uint16_t fps;
	static uint16_t samplesPerFrame;

	static void SaveFrame();

	static void StopRecording();

	static void BeginRecording();

private:

	static std::filesystem::path m_videoDirectory;
	static std::string m_videoName;

	static AVContext m_ctx;

};

REGISTER_EDITOR_INTERFACE(VideoRenderer);

} // Engine
