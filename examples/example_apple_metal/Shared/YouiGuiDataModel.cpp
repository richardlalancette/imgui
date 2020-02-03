#include "YouiGuiDataModel.h"

void YouiGuiDataModel::RenderAdditionalWindows()
{
    float x = 0;
    float y = 0;

    for (auto &notification : notificationWindows)
    {
        std::string name = std::get<0>(notification.second);
        std::string details = std::get<1>(notification.second);
        std::string iconName = std::get<2>(notification.second);
        int durationLeft = std::get<3>(notification.second);
        bool bFading = false;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoScrollbar;

        if (durationLeft > 0)
        {
            std::get<3>(notification.second) -= 10;

            if (durationLeft<100)
            {
                float fade = durationLeft / 100.0f;
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);
                bFading = true;
            }

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 5);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));

            bool open = true;

            ImGui::SetNextWindowSize(ImVec2(200, 100));
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowWidth() - 200 - ImGui::GetStyle().ItemSpacing.x, y + ImGui::GetStyle().ItemSpacing.y));
            ImGui::Begin(name.c_str(), &open, window_flags);
            ImGui::Text(std::to_string(durationLeft).c_str());
            ImGui::Text(iconName.c_str());
            ImGui::TextWrapped(details.c_str());
            ImGui::End();

            y += 100 + ImGui::GetStyle().ItemSpacing.y;

            ImGui::PopStyleColor();

            if (bFading)
            {
                ImGui::PopStyleVar();
            }

            ImGui::PopStyleVar();
            ImGui::PopStyleVar();
        }
    }

    for (auto it = notificationWindows.cbegin(); it != notificationWindows.cend();)
    {
        int durationLeft = std::get<3>(it->second);

        if (durationLeft == 0)
        {
            notificationWindows.erase(it++);    // or "it = m.erase(it)" since C++11
        }
        else
        {
            ++it;
        }
    }
}
