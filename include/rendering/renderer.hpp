#pragma once

#include "rendering/window.hpp"

namespace Engine
{

class Renderer
{

public:
    void Initialize();

    void Prepare() const;
    void Clear() const;
    void Render() const;

    // void Shutdown();

    [[nodiscard]] Window* GetWindow() const { return m_window.get(); }

private:
    std::unique_ptr<Window> m_window;

};

}