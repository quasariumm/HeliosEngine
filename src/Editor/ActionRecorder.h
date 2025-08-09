#pragma once
#include <functional>
#include <string>

#include "Core/Common.h"
#include "Debugger/Debugger.h"

namespace Engine {

struct EditorAction
{
    EditorAction(
        const std::wstring& actionName,
        const std::function<bool()>& undoFunction,
        const std::function<bool()>& redoFunction)
    {
        name = actionName;
        undoAction = undoFunction;
        redoAction = redoFunction;
    }

    void AddRedoIndex(const int index)
    {
        for (const int idx : nextActionIdx) if (idx == index) return;
        nextActionIdx.push_back(index);
    }

    std::wstring name;
    std::function<bool()> undoAction;
    std::function<bool()> redoAction;

    int prevActionIdx = -1;
    std::vector<int> nextActionIdx = {};
};

class ActionRecorder {

    public:
    static ENGINE_API ActionRecorder* Instance()
    {
        static ActionRecorder instance;
        return &instance;
    }

    ActionRecorder()
    {
        RegisterAction({L"Load recorder", [] { return false; }, [] { return false; }});
    };

    void RegisterAction(const EditorAction& action);

    void UndoAction();

    void RedoAction(int path = -1);

    std::vector<EditorAction>& GetActionsList() { return m_actions; }

private:
    std::vector<EditorAction> m_actions;
    int m_currentActionIdx = -1;
};

class ActionsTreeViewer final : public EditorInterface
{
public:
    ActionsTreeViewer() : EditorInterface(L"Actions Tree Viewer", EditorInterfaceSource::EDITOR) { active = false; }

    void DrawInterface() override;
};

// Shortcut
static void RegisterAction(const EditorAction& action)
{
    ActionRecorder::Instance()->RegisterAction(action);
}

}
