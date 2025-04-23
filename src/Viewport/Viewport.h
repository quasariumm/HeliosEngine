#pragma once
#include "Backends/OpenGL46_GLFW/Graphics/GL46_Texture2D.h"
#include "Editor/EditorInterface.h"
#include "Graphics/Camera.h"

namespace Engine
{

struct Line3D { vec3 a, b; };
struct Line2D { ImVec2 a, b; };

class Viewport : public EditorInterface
{
public:
    Viewport() : EditorInterface(L"Viewport") {}

    void DrawInterface() override;

    static void AppendRenderedImage(GL46_Texture2D* texture) { m_renderedImage = texture; }
    static void AppendEditorCamera(Camera* camera) { m_editorCamera = camera; }

private:
    static void DrawGizmo(const vec3& pos, float size = 30.0f);

    static Line2D ProjectLine(const Line3D& line);
    static ImVec2 ProjectPoint(const vec3& PW);

    static GL46_Texture2D* m_renderedImage;
    static Camera* m_editorCamera;
};

REGISTER_EDITOR_INTERFACE(Viewport);

}

