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
// TODO: Remove before commitin
// TODO:
// #define VR_TEST_VALUES
#ifdef VR_TEST_VALUES
uint16_t VideoRenderer::framesToRender = 60;
uint16_t VideoRenderer::fps = 30;
uint16_t VideoRenderer::samplesPerFrame = 500;

std::filesystem::path VideoRenderer::m_videoDirectory = L"C:\\Users\\patri\\Videos";
std::string VideoRenderer::m_videoName = "Test";
#else
uint16_t VideoRenderer::framesToRender = 0;
uint16_t VideoRenderer::fps = 0;
uint16_t VideoRenderer::samplesPerFrame = 0;

std::filesystem::path VideoRenderer::m_videoDirectory;
std::string VideoRenderer::m_videoName;
#endif

AVContext VideoRenderer::m_ctx = {};

VideoRenderer::VideoRenderer()
	: EditorInterface(L"Video Renderer", EditorInterfaceSource::EDITOR)
{
}


VideoRenderer::~VideoRenderer()
{
	sws_freeContext(m_ctx.swsCtx);
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


/**
 * @returns Whether the code should return (in the case of an unprocedable error)
 */
static bool LogFFmpegError(const int errCode, const std::wstring& msg, const std::source_location location = std::source_location::current())
{
	if (errCode == 0)
		return false;
	// Get simple error description
	char errbuffer[AV_ERROR_MAX_STRING_SIZE];
	av_strerror(errCode, errbuffer, AV_ERROR_MAX_STRING_SIZE);
	// Flush outputs to see the FFmpeg logs
	std::cerr << std::flush;
	std::cout << std::flush;
	// Log to the engine
	std::wstring message = msg + L" Video rendering initialisation failed. FFmpeg message: " + STR_TO_WSTR(std::string(errbuffer)) + L". See terminal output for full logs";
	DebugLog(LogSeverity::SEVERE, message, 0, location);
	return true;
}


// https://github.com/FFmpeg/FFmpeg/blob/master/doc/examples/encode_video.c
static void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt,
				   AVFormatContext* formatCtx)
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

		av_interleaved_write_frame(formatCtx, pkt);
		av_packet_unref(pkt);
	}
}


void VideoRenderer::SaveFrame()
{
	// Save using FFmpeg
	int ret = av_frame_make_writable(m_ctx.frame);
	if (ret < 0)
		DebugLog(LogSeverity::SEVERE, L"Could not make video frame writable. This should not happen.");

	const GL46_Texture2D* texture = Viewport::m_renderedImage;

	m_ctx.frame->pts = gVideoFrame;

	texture->UpdateData();

	// The texture is HDR RGB32F, so we need to use sws to convert to the YUV444P format
	const float* data = texture->GetDataHDR();
	auto* rgbData = new uint8_t[3 * texture->GetWidth() * texture->GetHeight()];

	// Convert the float [0,> to uint8_t [0,255]
	for (int i = 0; i < texture->GetWidth() * texture->GetHeight(); i++)
		rgbData[i] = (uint8_t)(std::clamp(data[i], 0.f, 1.f) * 0xFF);

	// Prepare the conversion
	const uint8_t* srcSlice[1] = { reinterpret_cast<const uint8_t*>(rgbData) };
	const int lineStride[1] = { 3 * texture->GetWidth() };

	// Covert to YUV444P
	sws_scale(m_ctx.swsCtx, srcSlice, lineStride, 0, texture->GetHeight(), m_ctx.frame->data, m_ctx.frame->linesize);

	// Delete allocated buffer
	delete[] rgbData;

	encode(m_ctx.codecCtx, m_ctx.frame, m_ctx.packet, m_ctx.formatCtx);
}


void VideoRenderer::BeginRecording()
{
	const std::string outPath = (m_videoDirectory / (m_videoName + ".mp4")).string();

	const EditorSettingsData& editorSettings = EditorSettings::Get();
	const vec2u viewportSize = editorSettings.m_viewportRenderSize;

	// Guess the format
	m_ctx.outputFormat = av_guess_format(nullptr, outPath.c_str(), nullptr);
	if (!m_ctx.outputFormat)
	{
		DebugLog(LogSeverity::SEVERE, L"Could not find output format.");
		return;
	}

	m_ctx.codec = avcodec_find_encoder(m_ctx.outputFormat->video_codec);
	if (!m_ctx.codec)
	{
		DebugLog(LogSeverity::INFO, L"Could not find guessed video codec. Falling back to MPEG-4.");
		m_ctx.codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
		if (!m_ctx.codec)
		{
			DebugLog(LogSeverity::SEVERE, L"Could not find MPEG-4 codec.");
			return;
		}
	}

	// Make format context
	int ret = avformat_alloc_output_context2(&m_ctx.formatCtx, m_ctx.outputFormat, nullptr, outPath.c_str());
	if (LogFFmpegError(ret, L"Could not allocate output context."))
		return;

	m_ctx.formatCtx->video_codec = m_ctx.codec;
	m_ctx.formatCtx->video_codec_id = m_ctx.codec->id;

	// Make stream
	m_ctx.stream = avformat_new_stream(m_ctx.formatCtx, m_ctx.codec);
	if (!m_ctx.stream)
	{
		DebugLog(LogSeverity::SEVERE, L"Could not allocate output stream.");
		goto error_free;
	}
	av_opt_set(&m_ctx.stream->metadata, "copyright", "Helios Engine (2025)", 0);

	// Allocate a frame
	m_ctx.frame = av_frame_alloc();
	if (!m_ctx.frame)
	{
		DebugLog(LogSeverity::SEVERE, L"Could not allocate video frame.");
		goto error_free;
	}

	// Allocate the codec context
	m_ctx.codecCtx = avcodec_alloc_context3(m_ctx.codec);
	if (!m_ctx.codecCtx)
	{
		DebugLog(LogSeverity::SEVERE, L"Could not allocate video codec context.");
		goto error_free;
	}

	// Set up stream and codec parameters
	m_ctx.stream->codecpar->color_range = AVCOL_RANGE_JPEG;
	m_ctx.stream->codecpar->bit_rate = 400000;
	m_ctx.stream->codecpar->sample_rate = 44100;
	m_ctx.stream->codecpar->width = (int)viewportSize.x;
	m_ctx.stream->codecpar->height = (int)viewportSize.y;
	m_ctx.stream->codecpar->framerate = (AVRational){ fps, 1 };
	m_ctx.stream->time_base = (AVRational){ 1, fps };

	// m_ctx.codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	m_ctx.codecCtx->pix_fmt = (m_ctx.codec->id == AV_CODEC_ID_H264) ? AV_PIX_FMT_YUV444P : AV_PIX_FMT_YUV420P;
	m_ctx.codecCtx->color_range = AVCOL_RANGE_JPEG;
	m_ctx.codecCtx->gop_size = 12;
	m_ctx.codecCtx->bit_rate = 1000000;
	m_ctx.codecCtx->sample_rate = 44100;
	m_ctx.codecCtx->coded_width = (int)viewportSize.x;
	m_ctx.codecCtx->coded_height = (int)viewportSize.y;
	m_ctx.codecCtx->width = (int)viewportSize.x;
	m_ctx.codecCtx->height = (int)viewportSize.y;
	m_ctx.codecCtx->time_base = (AVRational){ 1, fps };
	m_ctx.codecCtx->framerate = (AVRational){ fps, 1 };
	if (m_ctx.codec->id == AV_CODEC_ID_H264)
		av_opt_set(m_ctx.codecCtx->priv_data, "preset", "slow", 0);
	m_ctx.frame->format = m_ctx.codecCtx->pix_fmt;
	m_ctx.frame->width = (int)viewportSize.x;
	m_ctx.frame->height = (int)viewportSize.y;

	// Get the SWS context (used to convert RGB24 to YUV444P)
	m_ctx.swsCtx = sws_getCachedContext(
		m_ctx.swsCtx,
		(int)viewportSize.x, (int)viewportSize.y, AV_PIX_FMT_RGB24,
		(int)viewportSize.x, (int)viewportSize.y, m_ctx.codecCtx->pix_fmt,
		0, nullptr, nullptr, nullptr
	);

	// Open the codec
	ret = avcodec_open2(m_ctx.codecCtx, m_ctx.codec, NULL);
	if (LogFFmpegError(ret, L"Could not open video codec."))
		goto error_free;

	// Init the frame's buffer
	ret = av_frame_get_buffer(m_ctx.frame, 0);
	if (LogFFmpegError(ret, L"Could not allocate video frame data."))
		goto error_free;

	// Allocate a packet
	m_ctx.packet = av_packet_alloc();
	if (!m_ctx.packet)
	{
		DebugLog(LogSeverity::SEVERE, L"Could not allocate video packet.");
		goto error_free;
	}

	// Write MP4 headers
	ret = avformat_write_header(m_ctx.formatCtx, nullptr);
	if (LogFFmpegError(ret, L"Could not write video header."))
		goto error_free;

	// Open the video file
	ret = avio_open(&m_ctx.formatCtx->pb, outPath.c_str(), AVIO_FLAG_WRITE);
	if (LogFFmpegError(ret, L"Could not open output file."))
		goto error_free;

	gVideoRenderingEnabled = true;
	gVideoFrame = 0;
	return;

error_free:
	avformat_free_context(m_ctx.formatCtx);
	av_frame_free(&m_ctx.frame);
	avcodec_free_context(&m_ctx.codecCtx);
	av_packet_free(&m_ctx.packet);
}


void VideoRenderer::StopRecording()
{
	gVideoRenderingEnabled = false;
	DebugLog(LogSeverity::DONE, L"Video rendering successfully saved to " + m_videoDirectory.wstring() + L"\\" + STR_TO_WSTR(m_videoName) + L".mp4");

	// Flush the encoder
	encode(m_ctx.codecCtx, nullptr, m_ctx.packet, m_ctx.formatCtx);

	// Write trailer (idk what it does. so, magic?)
	av_write_trailer(m_ctx.formatCtx);

	// Close the file
	if (avio_close(m_ctx.formatCtx->pb) < 0)
		DebugLog(LogSeverity::SEVERE, L"Could not close output file.");

	// Free allocated data
	av_frame_free(&m_ctx.frame);
	av_packet_free(&m_ctx.packet);
	avcodec_free_context(&m_ctx.codecCtx);
	avformat_free_context(m_ctx.formatCtx);
}

} // Engine