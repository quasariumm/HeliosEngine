#include "rendering/renderer.hpp"

#include "rendering/window.hpp"

using namespace Engine;

void Renderer::Initialize()
{
    CreateWin(
        m_window,
        glm::uvec2(1920, 1080),
        "Helios Engine",
        EngineWindowFlags_NoVsync
    );

    m_window->SetMaximized(true);


}

void Renderer::Prepare() const
{
    m_window.get()->BeginFrame();
}

void Renderer::Render() const
{
    m_window.get()->SwapBuffers();
}
