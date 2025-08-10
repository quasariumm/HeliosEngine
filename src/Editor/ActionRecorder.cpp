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

    if (!ImGui::BeginTable("ActionTreeTable", 1000))
    {
        ImGui::End();
        return;
    }

    std::map<int, int> unexplored = { };
    int next = 0;
    while (true)
    {
        if (next == -1)
        {
            if (unexplored.empty()) break;
            next = unexplored.back();
            unexplored.pop_back();
            ImGui::NextColumn();
        }




        std::vector<EditorAction> childActions = {};

        ImGui::BeginGroup();
        for (int i = 0; i < nextActions.size(); i++)
        {
            if (nextActions.size() > 1)
            {
                // Show path index
                ImGui::Text(std::to_string(i).c_str());
                ImGui::SameLine();
            }
            ImGui::Text(WStringToUTF8(nextActions[i].name).c_str());
            ImGui::SameLine();

            for (const int idx : nextActions[i].nextActionIdx)
                childActions.push_back(actions[idx]);
        }
        ImGui::EndGroup();
        ImGui::Separator();

        if (childActions.empty()) break;

        nextActions.clear();
        nextActions = childActions;
        childActions.clear();
    }

    ImGui::EndTable();
    ImGui::End();
}

REGISTER_EDITOR_INTERFACE(ActionsTreeViewer);

}

