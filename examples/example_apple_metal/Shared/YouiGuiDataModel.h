#ifndef _YOUIGUIDATAMODEL_H_
#define _YOUIGUIDATAMODEL_H_

#include "designsystem.h"
#include "imgui.h"
#include "imgui_internal.h"

class YouiGuiDataModel
{
public:
    struct
    {
        bool visible = true;
        bool ColorsTabVisible = true;
        bool TypographyTabVisible = true;
        bool MotionTabVisible = false;
        bool LayoutTabVisible = false;
        bool SettingsTabVisible = true;
        bool DesignTabVisible = true;
    } mainAEPanel;

    struct
    {
        bool active = false;

        struct
        {
            bool visible = false;
        } demoWindow;
    } devMode;

    struct
    {
        std::string selectedFlavor;
    } designSystem;

    typedef std::tuple<std::string, std::string, std::string, int> notificationInfo;

    std::map<std::string, notificationInfo> notificationWindows;

    void RenderAdditionalWindows();
};
#endif
