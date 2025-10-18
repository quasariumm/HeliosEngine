#include "editor/editor_menus.hpp"

using namespace Engine;
using namespace Editor;
using namespace Log;


void LogViewer::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (!ImGui::Begin("Logs"))
	{
		ImGui::PopStyleVar();
		return;
	}

	constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable
	                                       | ImGuiTableFlags_ScrollY;

	if (!ImGui::BeginTable("LogsTable", 2, tableFlags))
	{
		ImGui::End();
		ImGui::PopStyleVar();
		return;
	}

	ImGui::TableSetupScrollFreeze(0, 1);
	ImGui::TableSetupColumn(ICON_TOOLTIP_QUESTION, ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize,
	                        ImGui::GetFontSize());
	ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_NoHide);
	ImGui::TableHeadersRow();

	ImGuiListClipper             clipper;
	const std::vector<Log::Log>& logs = Systems::GetLogHandler()->GetLogs();
	clipper.Begin(static_cast<int>(Systems::GetLogHandler()->GetLogs().size()));
	while (clipper.Step())
	{
		for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			switch (logs[row].type)
			{
			case LogType::INFO:
				ImGui::TextColored({0, .5f, .8f, 1}, ICON_INFORMATION);
				break;
			case LogType::WARNING:
				ImGui::TextColored({.9f, .8f, .2f, 1}, ICON_ALERT);
				break;
			case LogType::ERROR:
				ImGui::TextColored({.9f, .2f, 0, 1}, ICON_ALERT_OCTAGON);
				break;
			case LogType::DONE:
				ImGui::TextColored({.1f, .9f, 0.1f, 1}, ICON_CHECK_CIRCLE);
				break;
			}

			ImGui::TableNextColumn();
			ImGui::PushFont(EditorInterface::Get()->GetCodeFont());
			ImGui::Text("%s", logs[row].message.c_str());
			ImGui::PopFont();
		}
	}

	ImGui::EndTable();
	ImGui::End();
	ImGui::PopStyleVar();
}
