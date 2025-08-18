#include "ActionRecorder.h"

namespace Engine
{

void ActionRecorder::RegisterAction(const EditorAction& action)
{
    if (m_isCallingAction)
        return;
    DebugLog(LogSeverity::INFO, L"Registering action: " + action.name);
    m_actions.push_back(action);
    const int m_totalActions = static_cast<int>(m_actions.size());
    m_actions[m_totalActions - 1].prevActionIdx = m_currentActionIdx;
    if (m_currentActionIdx != -1) m_actions[m_currentActionIdx].AddRedoIndex(m_totalActions - 1);
    m_currentActionIdx = m_totalActions - 1;
}

void ActionRecorder::UndoAction()
{
    // Check if there are any changes to undo
    if (m_currentActionIdx == 0)
    {
        DebugLog(LogSeverity::INFO, L"No actions to undo");
        return;
    }

    m_isCallingAction = true;
    if (!m_actions[m_currentActionIdx].undoAction())
    {
        DebugLog(LogSeverity::SEVERE, L"Something went wrong undoing the last action");
        m_isCallingAction = false;
        return;
    }
    m_isCallingAction = false;
    m_currentActionIdx = m_actions[m_currentActionIdx].prevActionIdx;
}

void ActionRecorder::RedoAction(int path)
{
    // Check if there are any changes to redo
    if (m_actions[m_currentActionIdx].nextActionIdx.empty())
    {
        DebugLog(LogSeverity::INFO, L"No actions to redo");
        return;
    }

    // If path is not set or out of bounds, set to latest change
    if (path < 0 || path >= m_actions[m_currentActionIdx].nextActionIdx.size())
        path = static_cast<int>(m_actions[m_currentActionIdx].nextActionIdx.size()) - 1;

    m_currentActionIdx = m_actions[m_currentActionIdx].nextActionIdx[path];
    m_isCallingAction = true;
    if (!m_actions[m_currentActionIdx].redoAction())
        DebugLog(LogSeverity::SEVERE, L"Something went wrong redoing the next action");
    m_isCallingAction = false;
}

void ActionsTreeViewer::DrawInterface()
{
    ImGui::Begin(ICON_FILE_TREE" Action Tree");
    const std::vector<EditorAction> actions = ActionRecorder::Instance()->GetActionsList();

    if (!ImGui::BeginTable("ActionTreeTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable))
    {
        ImGui::End();
        return;
    }

    ImGui::TableSetupColumn("ID");
    ImGui::TableSetupColumn("Action");
    ImGui::TableSetupColumn("Branches");

    int next = 0;

    while (true)
    {
        ImGui::Text("%d", next);
        ImGui::NextColumn();
        ImGui::Text(WStringToUTF8(actions[next].name).c_str());

        const int branches = static_cast<int>(actions[next].nextActionIdx.size()) - 1;
        if (branches > 0)
        {
            ImGui::NextColumn();
            ImGui::Text("%d", branches);
        }

        if (branches == -1) break;
        next = actions[next].nextActionIdx.back();
        ImGui::TableNextRow();
    }

    ImGui::EndTable();
    ImGui::End();
}

REGISTER_EDITOR_INTERFACE(ActionsTreeViewer);

}

