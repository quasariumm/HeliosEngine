#include "VideoRenderer.h"

#include "Editor/EditorSettings.h"
#include "Projects/ProjectHandler.h"
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <tracy/Tracy.hpp>

#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Debugger/Debugger.h"
#include "Viewport/Viewport.h"


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

namespace Engine {
// TODO: Remove before commiting
// #define VR_TEST_VALUES
// #ifdef VR_TEST_VALUES
// uint16_t VideoRenderer::framesToRender = 60;
// uint16_t VideoRenderer::fps = 30;
// uint16_t VideoRenderer::samplesPerFrame = 500;
//
// std::filesystem::path VideoRenderer::m_videoDirectory = L"C:\\Users\\patri\\Videos";
// std::string VideoRenderer::m_videoName = "Test";
// #else
uint16_t VideoRenderer::framesToRender = 0;
uint16_t VideoRenderer::fps = 0;
uint16_t VideoRenderer::samplesPerFrame = 0;

std::filesystem::path VideoRenderer::m_videoDirectory;
std::string VideoRenderer::m_videoName;
// #endif

AVContext VideoRenderer::m_ctx = {};

VideoRenderer::VideoRenderer()
	: EditorInterface(L"Video Renderer", EditorInterfaceSource::EDITOR)
{
	avformat_network_init();

	m_ctx.codec = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (!m_ctx.codec)
	{
		DebugLog(LogSeverity::INFO, L"Could not find H.264 video codec. Falling back to MPEG-4.");
		m_ctx.codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
		if (!m_ctx.codec)
		{
			m_ctx.disabled = true;
			DebugLog(LogSeverity::SEVERE, L"Could not find MPEG-4 codec. Video rendering feature disabled.");
			return;
		}
	}
}


VideoRenderer::~VideoRenderer()
{
	sws_freeContext(m_ctx.swsCtx);

	avformat_network_deinit();
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

	if (gVideoRenderingEnabled)
	{
		ImGui::Text("Render progress:");
		const float progress = std::clamp((float)gVideoFrame / (float)framesToRender, 0.f, 1.f);
		char buffer[32];
		sprintf_s(buffer, "%d/%d (%.1f%%)", gVideoFrame, framesToRender, 100.f * progress);
		ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buffer);
	}

	ImGui::End();
}


// https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/encode_video.c
static void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt,
				   std::ofstream& outfile)
{
	int ret = avcodec_send_frame(enc_ctx, frame);
	if (ret < 0)
		DebugLog(LogSeverity::SEVERE, L"Error sending a frame for encoding");

	while (ret >= 0)
	{
		ret = avcodec_receive_packet(enc_ctx, pkt);

		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return;

		if (ret < 0)
			DebugLog(LogSeverity::SEVERE, L"Error during encoding");

		outfile.write(reinterpret_cast<const char*>(pkt->data), pkt->size);
		av_packet_unref(pkt);
	}
}


void VideoRenderer::SaveFrame()
{
	if (m_ctx.disabled)
		return;
	// Save using FFmpeg
	int ret = av_frame_make_writable(m_ctx.frame);
	if (ret < 0)
		DebugLog(LogSeverity::SEVERE, L"Could not make video frame writable. This should not happen.");

	const GL46_Texture2D* texture = Viewport::m_renderedImage;

	m_ctx.frame->pts = gVideoFrame;

	texture->UpdateData();

	// The texture is HDR RGB32F, so we need to use sws to convert to the YUV420P format
	const float* data = texture->GetDataHDR();
	auto* rgbData = new uint8_t[3 * texture->GetWidth() * texture->GetHeight()];

	// Convert the float [0,> to uint8_t [0,255]
	for (int i = 0; i < texture->GetWidth() * texture->GetHeight(); i++)
		rgbData[i] = (uint8_t)(std::clamp(data[i], 0.f, 1.f) * 0xFF);

	// Prepare the conversion
	const uint8_t* srcSlice[1] = { reinterpret_cast<const uint8_t*>(rgbData) };
	const int lineStride[1] = { 3 * texture->GetWidth() };

	// Covert to YUV420P
	sws_scale(m_ctx.swsCtx, srcSlice, lineStride, 0, texture->GetHeight(), m_ctx.frame->data, m_ctx.frame->linesize);

	// Delete allocated buffer
	delete[] rgbData;

	encode(m_ctx.codecCtx, m_ctx.frame, m_ctx.packet, m_ctx.file);
}


void VideoRenderer::BeginRecording()
{
	if (m_ctx.disabled)
		return;

	m_ctx.frame = av_frame_alloc();
	if (!m_ctx.frame)
	{
		m_ctx.disabled = true;
		DebugLog(LogSeverity::SEVERE, L"Could not allocate video frame. Video rendering feature disabled.");
		return;
	}

	m_ctx.codecCtx = avcodec_alloc_context3(m_ctx.codec);
	if (!m_ctx.codecCtx)
	{
		m_ctx.disabled = true;
		DebugLog(LogSeverity::SEVERE, L"Could not allocate video codec context. Video rendering feature disabled.");
		return;
	}

	gVideoRenderingEnabled = true;
	gVideoFrame = 0;

	const EditorSettingsData& editorSettings = EditorSettings::Get();
	const vec2u viewportSize = editorSettings.m_viewportRenderSize;

	m_ctx.codecCtx->bit_rate = 400000;
	m_ctx.codecCtx->pix_fmt = AV_PIX_FMT_YUV444P;
	// m_ctx.codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	m_ctx.frame->format = m_ctx.codecCtx->pix_fmt;
	m_ctx.frame->width = (int)viewportSize.x;
	m_ctx.frame->height = (int)viewportSize.y;
	m_ctx.codecCtx->width = (int)viewportSize.x;
	m_ctx.codecCtx->height = (int)viewportSize.y;
	m_ctx.codecCtx->time_base = (AVRational){ 1, fps };
	m_ctx.codecCtx->framerate = (AVRational){ fps, 1 };

	int ret = avcodec_open2(m_ctx.codecCtx, m_ctx.codec, NULL);
	if (ret < 0)
	{
		m_ctx.disabled = true;
		DebugLog(LogSeverity::SEVERE, L"Could not open video codec. Video rendering feature disabled.");
		return;
	}

	if (m_ctx.codec->id == AV_CODEC_ID_H264)
		av_opt_set(m_ctx.codecCtx->priv_data, "preset", "slow", 0);

	ret = av_frame_get_buffer(m_ctx.frame, 0);
	if (ret < 0)
	{
		m_ctx.disabled = true;
		DebugLog(LogSeverity::SEVERE, L"Could not allocate video frame data. Video rendering feature disabled.");
		return;
	}

	m_ctx.swsCtx = sws_getCachedContext(
		m_ctx.swsCtx,
		(int)viewportSize.x, (int)viewportSize.y, AV_PIX_FMT_RGB24,
		(int)viewportSize.x, (int)viewportSize.y, m_ctx.codecCtx->pix_fmt,
		0, nullptr, nullptr, nullptr
	);

	m_ctx.packet = av_packet_alloc();
	if (!m_ctx.packet)
	{
		m_ctx.disabled = true;
		DebugLog(LogSeverity::SEVERE, L"Could not allocate video packet. Video rendering feature disabled.");
		return;
	}

	m_ctx.file.open(m_videoDirectory / (m_videoName + ".mp4"));
}


void VideoRenderer::StopRecording()
{
	if (m_ctx.disabled)
		return;
	gVideoRenderingEnabled = false;
	DebugLog(LogSeverity::DONE, L"Video rendering successfully saved to " + m_videoDirectory.wstring() + L"\\" + STR_TO_WSTR(m_videoName) + L".mp4");

	// Flush the encoder
	encode(m_ctx.codecCtx, nullptr, m_ctx.packet, m_ctx.file);

	m_ctx.file.close();

	av_frame_free(&m_ctx.frame);
	av_packet_free(&m_ctx.packet);
	avcodec_free_context(&m_ctx.codecCtx);
}

} // Engine