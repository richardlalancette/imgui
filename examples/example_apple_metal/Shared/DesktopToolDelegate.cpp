#include "DesktopToolDelegate.h"

#include "ImGui.h"
#include "IconsFontAwesome5.h"

void DesktopToolDelegate::SetupColorPalette(youi::DesignSystem system)
{
}

void DesktopToolDelegate::Render(const YouiGuiDataModel &model)
{
}

bool DesktopToolDelegate::ShouldRenderDefaultMenu()
{
    return false;
}

float DesktopToolDelegate::RenderCustomMainMenu(YouiGuiDataModel &model)
{
    return 0.0f;

//    if (ImGui::BeginMainMenuBar())
//    {
//        if (ImGui::Button(ICON_FA_APPLE_ALT))
//
//        if (ImGui::BeginMenu("Tools"))
//        {
//            ImGui::MenuItem("Developer Mode", NULL, &model.devMode.active);
//            ImGui::EndMenu();
//        }
//
//        if (ImGui::BeginMenu("Window"))
//        {
//            ImGui::MenuItem("Main Window", NULL, &model.mainAEPanel.visible);
//            ImGui::MenuItem("Demo Window", NULL, &model.devMode.demoWindow.visible);
//            ImGui::EndMenu();
//        }
//
//        if (ImGui::BeginMenu("Help"))
//        {
//            ImGui::EndMenu();
//        }
//
//        ImGui::EndMainMenuBar();
//    }
//
//    return ImGui::GetCursorPosY();
}
