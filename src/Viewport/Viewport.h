#pragma once
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Editor/EditorInterface.h"
#include "Graphics/Camera.h"

namespace Engine
{

class Viewport : public EditorInterface
{
public:
    Viewport() : EditorInterface(L"Viewport") {}

    void DrawInterface() override;

    static void AppendRenderedImage(GL46_Texture2D* texture) { m_renderedImage = texture; }
    static void AppendEditorCamera(Camera* camera) { m_editorCamera = camera; }

	static ImVec2 position;
	static ImVec2 size;

private:

    static GL46_Texture2D* m_renderedImage;
    static Camera* m_editorCamera;
};

REGISTER_EDITOR_INTERFACE(Viewport);

}

