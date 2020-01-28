#ifndef _YOUIDESIGNSYSTEMPANEL_H_
#define _YOUIDESIGNSYSTEMPANEL_H_

#include "designsystem.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "AuthoringToolInterface.h"

class YouiGui
{
public:
    typedef std::tuple<std::string, int> FontDetailsTupleType;

    youi::DesignSystem gDesignSystem;
    ImFont *bodyFont = nullptr;
    ImFont *filebrowserFont = nullptr;
    ImFont *H1Font = nullptr;
    ImFont *H2Font = nullptr;

    bool bWindowBrowseFiles = false;
    bool bDesignSystemLoaded = false;
    std::vector<std::string> recently_used_files;
    std::map<FontDetailsTupleType, std::string> gFontToLoadMap;
    std::unique_ptr<AuthoringToolInterface> m_toolDelegate;

public:
    std::map<std::string, ImFont *> userFonts;

    struct YouiGuiDataModel
    {
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
    } m_youiGuiDataModel;

public:
    void Init(std::unique_ptr<AuthoringToolInterface> delegate);
    void Render();
    void RenderMainAEPanel(bool *open);
    void RenderColorTab(bool *open);
    void RenderTypographyTab(bool *open);
    void RenderMotionTab(bool *open);
    void RenderLayoutTab(bool *open);
    void RenderSettingsTab(bool *open);
    void RenderDesignSystemInstructionsTab(bool *open);
    void RenderEmptyDesignSystemTab();

    void CommandAddLinkedSolid(float *color);

    void SetYouiLightTheme();
    void SetYouiDarkTheme();
    bool CheckFileExist(const std::string &path);

    /*
     * All font added this way is owned by the atlas.
     */
    ImFont *LoadUserFont(const FontDetailsTupleType &fontDetails, const std::string &filename);

    /*
     * All font added this way is owned by the atlas.
     */
    ImFont *LoadUserFont(const FontDetailsTupleType &fontDetails, const std::string &filename, const ImFontConfig &fontConfig);

    void DetailsColorButton(const char *text, float col[4], ImGuiColorEditFlags flags);
    void DisplayNotes(std::shared_ptr<std::vector<std::string>> notes);
    void ProcessDesignSystemFile(const std::string &designSystemFilename);
    void TabPageHeader(const std::string &string, ImTextureID textureId);
    void DetailedColorTooltip(const char *desc, const char *icon = "?", const char *flavorName = "");
    void TabBar(bool b);
    void YouiSimpleTooltip(const std::string &tooltipText);
    void ShowParsingErrorDialog();
    void FlavorIdentifierDisplay(const std::string &flavorName);
    void RenderMainMenu();
    void RenderReloadDesignSystemButton(bool b);
};
#endif
