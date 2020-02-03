#include "YouiDesignSystemPanel.h"

#include "json.hpp"
#include "IconsFontAwesome5.h"
#include "YouiGuiDataModel.h"

#include <fstream>
#include <zconf.h>

using json = nlohmann::json;

void YouiGui::Init(std::unique_ptr<AuthoringToolInterface> delegate)
{
    SetAfterEffectDarkTheme();

    static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    static const ImWchar icons_ranges_icomatic[] = {0xe900, 0xe958, 0};
    ImGuiIO &io = ImGui::GetIO();

    ImFontConfig mainFontConfig;
    mainFontConfig.FontDataOwnedByAtlas = true;
    mainFontConfig.MergeMode = false;

    auto currentApplicationFolder = std::string(getcwd(NULL, 0));

//    std::string mainFont = "fonts/imgui/ProggyClean.ttf";
    std::string mainFont = "fonts/Source_Code_Pro/SourceCodePro-SemiBold.ttf";
//    std::string mainFont = "fonts/Source_Code_Pro/SourceCodePro-Regular.ttf";
    std::string bodyFontPath = "/Library/Fonts/" + mainFont;
    bodyFont = io.Fonts->AddFontFromFileTTF(bodyFontPath.c_str(), 16.0f, &mainFontConfig);
//    bodyFont = io.Fonts->AddFontDefault();

    ImFontConfig icons_config;
    icons_config.PixelSnapH = true;
    icons_config.MergeMode = true;
    icons_config.OversampleH = 3;

    // Merge this font with the default font for now.
    std::string iconsFontPath = "/Library/Fonts/fonts/icomoon/icomoon.ttf";
//    std::string iconsFontPath = "/Library/Fonts/fonts/fontawesome/fa-solid-900.ttf";
    io.Fonts->AddFontFromFileTTF(iconsFontPath.c_str(), 18.0f, &icons_config, icons_ranges_icomatic);

    std::string fileBrowserFont = "/Library/Fonts/" + mainFont;
    filebrowserFont = io.Fonts->AddFontFromFileTTF(fileBrowserFont.c_str(), 18.0f, &mainFontConfig);

    // Header 1 font
    ImFontConfig h1FontConfig;
    h1FontConfig.FontDataOwnedByAtlas = true;
    h1FontConfig.MergeMode = false;
    h1FontConfig.OversampleH = 3;
    std::string h1FontPath = "/Library/Fonts/" + mainFont;
    H1Font = io.Fonts->AddFontFromFileTTF(h1FontPath.c_str(), 32.0f, &h1FontConfig);

    // Header 2 font
    ImFontConfig h2FontConfig;
    h2FontConfig.FontDataOwnedByAtlas = true;
    h2FontConfig.MergeMode = false;
    h2FontConfig.OversampleH = 3;
    std::string h2FontPath = "/Library/Fonts/" + mainFont;
    H2Font = io.Fonts->AddFontFromFileTTF(h2FontPath.c_str(), 24.0f, &h1FontConfig);

//    ImFontConfig IconMoonFontConfig;
//    IconMoonFontConfig.FontDataOwnedByAtlas = true;
//    IconMoonFontConfig.MergeMode = false;
//    IconMoonFontConfig.OversampleH = 3;
//    std::string iconMoonPath = "/Library/Fonts/icomoon.ttf";
//    iconMoonFont = io.Fonts->AddFontFromFileTTF(iconMoonPath.c_str(), 28.0f, &IconMoonFontConfig);

    m_toolDelegate = std::move(delegate);
}

void YouiGui::Render()
{
    ImGui::NewFrame();

    float menuBottom = m_toolDelegate->RenderCustomMainMenu(m_youiGuiDataModel);

    if (m_youiGuiDataModel.devMode.active && m_youiGuiDataModel.devMode.demoWindow.visible)
    {
        ImGui::ShowDemoWindow(&m_youiGuiDataModel.devMode.demoWindow.visible);
    }

    if (ShouldRenderMainMenu())
    {
        RenderMainMenu();
    }

    RenderMainAEPanel(&m_youiGuiDataModel.mainAEPanel.visible, menuBottom);

    m_toolDelegate->Render(m_youiGuiDataModel);

    ImGui::Render();
}

bool YouiGui::ShouldRenderMainMenu() const
{
    bool bToolShouldRenderMainMenu = m_toolDelegate->ShouldRenderDefaultMenu();

    return m_youiGuiDataModel.devMode.active || !m_youiGuiDataModel.mainAEPanel.visible || bToolShouldRenderMainMenu;
}

void YouiGui::RenderMainMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Window"))
        {
            ImGui::MenuItem("Main Window", NULL, &m_youiGuiDataModel.mainAEPanel.visible);
            ImGui::MenuItem("Demo Window", NULL, &m_youiGuiDataModel.devMode.demoWindow.visible);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Developer Mode", NULL, &m_youiGuiDataModel.devMode.active);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void YouiGui::RenderMainAEPanel(bool *open, float clientRectY)
{
    if (!*open)
    {
        return;
    }

    ImGui::PushFont(bodyFont);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

    if (!m_youiGuiDataModel.devMode.active)
    {
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGui::SetNextWindowPos(ImVec2(0, clientRectY), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
    }
    else
    {
        ImVec2 &framePadding = ImGui::GetStyle().FramePadding;
        ImGui::SetNextWindowPos(ImVec2(framePadding.x * 5, framePadding.y * 5 + clientRectY), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x/4, ImGui::GetIO().DisplaySize.y * 0.75), ImGuiCond_Always);
        window_flags |= ImGuiWindowFlags_MenuBar;
    }

    ImGui::Begin("Design System", open, window_flags);
    {
        TabBar(bDesignSystemLoaded);

        if (bDesignSystemLoaded)
        {
            ImGui::BeginTabBar("Design System!");
            {
                RenderColorTab(&m_youiGuiDataModel.mainAEPanel.ColorsTabVisible);
                RenderTypographyTab(&m_youiGuiDataModel.mainAEPanel.TypographyTabVisible);
                RenderMotionTab(&m_youiGuiDataModel.mainAEPanel.MotionTabVisible);
                RenderLayoutTab(&m_youiGuiDataModel.mainAEPanel.LayoutTabVisible);
                RenderDesignSystemInstructionsTab(&m_youiGuiDataModel.mainAEPanel.DesignTabVisible);
                RenderSettingsTab(&m_youiGuiDataModel.mainAEPanel.SettingsTabVisible);
            }
            ImGui::EndTabBar();
        }
        else
        {
            RenderEmptyDesignSystemTab();
        }

        m_youiGuiDataModel.RenderAdditionalWindows();
    }

    ImGui::End();

    ImGui::PopFont();
}

void YouiGui::RenderColorTab(bool *open)
{
    if (!*open)
    {
        return;
    }

    if (ImGui::BeginTabItem("Colors"))
    {
        ImGui::BeginChild("child");
        {
            auto colors = gDesignSystem.colors;

            if (colors != nullptr)
            {
                auto flavors = colors->flavors;

                ImGuiColorEditFlags colorDisplayFlags = 0;
                colorDisplayFlags |= ImGuiColorEditFlags_NoPicker;
                colorDisplayFlags |= ImGuiColorEditFlags_AlphaPreviewHalf;
                colorDisplayFlags |= ImGuiColorEditFlags_NoDragDrop;

                // Show only the first flavor
                auto flavor = (*flavors)[0];
                {
                    ImGui::Indent();

                    for (auto &colorSwatch : *flavor.flavor_colors)
                    {
                        auto colorRGBA = *colorSwatch.color_rgba;
                        auto colorName = colorSwatch.name->c_str();

                        float color[] = {static_cast<float>(*colorRGBA.r), static_cast<float>(*colorRGBA.g), static_cast<float>(*colorRGBA.b), static_cast<float>(*colorRGBA.a)};

                        std::string fullTokenName = std::string(flavor.name->c_str()) + colorSwatch.name->c_str();
                        size_t hash1 = std::hash<std::string>{}(fullTokenName);
                        std::string refCount = std::to_string(hash1 % 6);
                        ImGui::Text(refCount.c_str(), "");
                        ImGui::SameLine();

                        if (ImGui::ColorButton(colorName, color, colorDisplayFlags, ImVec2(30.0f, 30.0f)))
                        {
                            CommandAddLinkedSolid(fullTokenName, color);
                        }

                        ImGui::SameLine();
                        ImGui::TextWrapped(colorName);
                    }
                }
            }
            else
            {
                ImGui::PushFont(H2Font);
                ImGui::TextWrapped("No colors found in the design system.", "");
                ImGui::PopFont();
            }
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void YouiGui::RenderTypographyTab(bool *open)
{
    if (!*open)
    {
        return;
    }

    ImGuiColorEditFlags colorDisplayFlags = 0;
    colorDisplayFlags |= ImGuiColorEditFlags_NoPicker;
    colorDisplayFlags |= ImGuiColorEditFlags_AlphaPreviewHalf;
    colorDisplayFlags |= ImGuiColorEditFlags_NoDragDrop;

    if (ImGui::BeginTabItem("Type"))
    {
        auto typography = gDesignSystem.typography;

        if (typography != nullptr)
        {
            auto flavors = typography->flavors;
            auto flavor = (*flavors)[0];

            ImGui::BeginChild("typographyList");
            {
                for (auto &typeStyle : *flavor.typography_styles)
                {
                    auto colorRGBA = *typeStyle.color_rgba;
                    auto styleName = typeStyle.name->c_str();
                    auto fontName = *typeStyle.fontname;
                    int fontSize = *typeStyle.fontsize;
                    std::string path = fontName;
                    std::string sectionName = fontName + std::to_string(fontSize);

                    const std::string &fullFontName = fontName + std::to_string(fontSize);
                    ImFont *font = userFonts[fullFontName];
                    ImGui::PushFont(font);
                    float estimatedTextHeight = std::max(ImGui::CalcTextSize(fontName.c_str()).y, 30.0f);
                    ImGui::PopFont();

                    ImGui::Indent(ImGui::GetStyle().FramePadding.x);
                    ImGui::BeginChildFrame(ImGui::GetID(sectionName.c_str()), ImVec2(ImGui::GetWindowWidth() - ImGui::GetStyle().FramePadding.x * 2, estimatedTextHeight * 1.5), true);
                    {
                        float color[] = {static_cast<float>(*colorRGBA.r), static_cast<float>(*colorRGBA.g), static_cast<float>(*colorRGBA.b), static_cast<float>(*colorRGBA.a)};

                        std::string valueToHash = std::string(flavor.name->c_str()) + typeStyle.name->c_str();
                        size_t hash1 = std::hash<std::string>{}(valueToHash);
                        std::string refCount = std::to_string(hash1 % 6);
                        ImGui::Text(refCount.c_str(), "");
                        ImGui::SameLine();

//                        if (ImGui::ColorButton(styleName, color, colorDisplayFlags, ImVec2(30.0f, 30.0f)))
//                        {
//                            CommandAddLinkedSolid(color);
//                        }
//
                        ImGui::PushFont(font);
                        ImGui::SameLine();
                        ImGui::TextColored(color, fontName.c_str());
                        ImGui::SameLine();
                        ImGui::TextColored(color, std::to_string(fontSize).c_str());
                        ImGui::PopFont();
                    }
                    ImGui::EndChildFrame();
                    ImGui::Unindent(ImGui::GetStyle().FramePadding.x);
                }
            }
            ImGui::EndChild();
        }

        ImGui::EndTabItem();
    }
}

void YouiGui::RenderMotionTab(bool *open)
{
    if (!*open)
    {
        return;
    }

    if (ImGui::BeginTabItem("Motion"))
    {
        ImGui::BeginTabBar("Design System!");
        {
            if (ImGui::BeginTabItem(" Macro"))
            {
//                auto macro = motionSpec->find("macro");
//                for (auto &element : *macro)
//                {
//                    ImGui::Separator();
//                    ImGui::BeginGroup();
//                    {
//                        ImGui::Indent(ImGui::GetStyle().IndentSpacing);
//                        ImGui::TextUnformatted(element["name"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["description"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["element"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["transformation"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["duration"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["delay"].get<std::string>().c_str());
//
//                        //                            auto curve = macro->find("curve");
//                        ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
//                    }
//                    ImGui::EndGroup();
//                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(" Micro"))
            {
//                auto micro = motionSpec->find("micro");
//                for (auto &element : *micro)
//                {
//                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::EndTabItem();
    }
}

void YouiGui::RenderSettingsTab(bool *open)
{
    if (!*open)
    {
        return;
    }

    if (ImGui::BeginTabItem("Settings"))
    {
        static int style_idx = -1;
        if (ImGui::Combo("Colors##Selector", &style_idx, "Classic\0Dark\0Light\0You.i Light\0You.i Dark\0After Effects Dark\0"))
        {
            switch (style_idx)
            {
                case 0:
                    ImGui::StyleColorsClassic();
                    break;
                case 1:
                    ImGui::StyleColorsDark();
                    break;
                case 2:
                    ImGui::StyleColorsLight();
                    break;
                case 3:
                    SetYouiLightTheme();
                    break;
                case 4:
                    SetYouiDarkTheme();
                    break;
                case 5:
                    SetAfterEffectDarkTheme();
                    break;
                default:
                    ImGui::StyleColorsClassic();
                    break;
            }
        }

        ImGuiIO &io = ImGui::GetIO();
        static float window_scale = 1.0f;

        if (ImGui::DragFloat("window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.2f"))
        {   // scale only this window
            ImGui::SetWindowFontScale(window_scale);
        }

        ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.2f");      // scale everything
        ImGui::Checkbox("Developer Mode", &m_youiGuiDataModel.devMode.active);

        ImGui::EndTabItem();
    }
}

void YouiGui::RenderLayoutTab(bool *open)
{
    if (!*open)
    {
        return;
    }

    if (ImGui::BeginTabItem("Layout"))
    {
        ImGui::BeginChild("child");
        {
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void YouiGui::RenderDesignSystemInstructionsTab(bool *open)
{
    if (!*open)
    {
        return;
    }

    if (ImGui::BeginTabItem("Design"))
    {
        ImGui::BeginChild("child");
        {
            auto ds = gDesignSystem.instructions;
            ImGui::TextWrapped("Format: %s", ds->design_system_format->c_str());
            ImGui::Separator();
            for (const auto &m : *ds->metadata)
            {
                ImGui::TextWrapped("%s", m.c_str());
                ImGui::Separator();
            }
        }
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void YouiGui::RenderEmptyDesignSystemTab()
{
    ImGui::BeginChild("UIEmptyDesignSystem");
    ImGui::PushFont(H2Font);
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2.0 - ImGui::GetFontSize());
    std::string text = "No Design System Loaded";
    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textSize.x) / 2.0f);
    ImGui::TextWrapped(text.c_str());
    ImGui::PopFont();
    ImGui::EndChild();
}

void YouiGui::CommandAddLinkedSolid(const std::string &fullTokenName, float *color)
{
    m_youiGuiDataModel.notificationWindows[fullTokenName] = std::make_tuple(fullTokenName, "", "", 2500);

    /*
    function makeColorEntry(name, color)
    {
        var params = new Object;
        params.color = color;
        params.color[0] = parseFloat(params.color[0]) / 255.0;
        params.color[1] = parseFloat(params.color[1]) / 255.0;
        params.color[2] = parseFloat(params.color[2]) / 255.0;
        params.name = name;

        app.beginUndoGroup("create palette entry");

        // find current palette
        var myComp = null;
        for (var i = 1; i <= app.project.numItems; i ++) {
            if ((app.project.item(i) instanceof CompItem) && (app.project.item(i).name === 'Color Palette'))
            {
                myComp = app.project.item(i);
                break;
            }
        }

        // create if not existing
        if (myComp == null)
            return;

        // create
        var effect1 = myComp.layer(1).Effects.addProperty("ADBE Fill")
        effect1("Color").setValue(params.color);
        effect1.name = params.name;

        app.endUndoGroup();

    }
     */

    // 1) make a fill effect on selected layer
    // 2) link color for fill to palette

    /*  function makeLink(key)
        {
            var linkExpression = "palette = comp(\"Color Palette\"); \
                        try{ palette.layer(1).effect(\"" + key + "\").param(\"Color\");\
                        }catch(e){ value; }";

            var myLayers = app.project.activeItem.selectedLayers;
            if (myLayers.length == 0)
                return;

            var layer = myLayers[0];

            if (isSolid(layer))
            {
                // make a fill effect, then link via expressions
                // look for existing fill link
                var effect1 = null;
                for(var i=1;i<=layer.effect.numProperties;i++)
                {
                    var curFx = layer.effect(i);
                    if (curFx.name == "colorlink")
                    {
                        effect1 = curFx;
                        break;
                    }
                }

                if (effect1 == null)
                {
                    effect1 = layer.Effects.addProperty("ADBE Fill")
                    effect1.name = "colorlink";
                }
                effect1("Color").expression = linkExpression;
            }
        }
     */
}

void YouiGui::SetYouiLightTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowPadding.x = 10;
    style.WindowPadding.y = 10;
    style.FramePadding.x = 5;
    style.FramePadding.y = 5;
    style.ItemSpacing.x = 5;
    style.ItemSpacing.y = 5;
    style.ItemInnerSpacing.x = 5;
    style.ItemInnerSpacing.y = 5;
    style.TouchExtraPadding.x = 0;
    style.TouchExtraPadding.y = 0;
    style.IndentSpacing = 20;
    style.ScrollbarSize = 10;
    style.GrabMinSize = 5;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;

    style.WindowRounding = 2;
    style.ChildRounding = 2;
    style.FrameRounding = 2;
    style.PopupRounding = 2;
    style.ScrollbarRounding = 2;
    style.GrabRounding = 2;
    style.TabRounding = 2;

    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.97f, 0.97f, 0.97f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.42f, 0.42f, 0.42f, 0.23f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.02f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.99f, 0.99f, 0.99f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.81f, 0.81f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.92f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.96f, 0.96f, 0.96f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.79f, 0.79f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.08f, 0.27f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.79f, 0.79f, 0.80f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.73f, 0.73f, 0.73f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.54f, 0.54f, 0.54f, 0.40f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.81f, 0.81f, 0.81f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.74f, 0.74f, 0.74f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.65f, 0.08f, 0.27f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.65f, 0.08f, 0.27f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.73f, 0.73f, 0.73f, 0.40f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.54f, 0.54f, 0.54f, 0.40f);
    colors[ImGuiCol_TabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void YouiGui::SetYouiDarkTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowPadding.x = 10;
    style.WindowPadding.y = 10;
    style.FramePadding.x = 5;
    style.FramePadding.y = 5;
    style.ItemSpacing.x = 5;
    style.ItemSpacing.y = 5;
    style.ItemInnerSpacing.x = 5;
    style.ItemInnerSpacing.y = 5;
    style.TouchExtraPadding.x = 0;
    style.TouchExtraPadding.y = 0;
    style.IndentSpacing = 20;
    style.ScrollbarSize = 10;
    style.GrabMinSize = 5;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;

    style.WindowRounding = 2;
    style.ChildRounding = 2;
    style.FrameRounding = 2;
    style.PopupRounding = 2;
    style.ScrollbarRounding = 2;
    style.GrabRounding = 2;
    style.TabRounding = 2;

    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
    colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
    colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
    colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
    colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void YouiGui::SetAfterEffectDarkTheme()
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowPadding.x = 10;
    style.WindowPadding.y = 10;
    style.FramePadding.x = 5;
    style.FramePadding.y = 5;
    style.ItemSpacing.x = 5;
    style.ItemSpacing.y = 5;
    style.ItemInnerSpacing.x = 5;
    style.ItemInnerSpacing.y = 5;
    style.TouchExtraPadding.x = 0;
    style.TouchExtraPadding.y = 0;
    style.IndentSpacing = 20;
    style.ScrollbarSize = 10;
    style.GrabMinSize = 5;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 1;
    style.FrameBorderSize = 1;
    style.TabBorderSize = 1;

    style.WindowRounding = 2;
    style.ChildRounding = 2;
    style.FrameRounding = 2;
    style.PopupRounding = 2;
    style.ScrollbarRounding = 2;
    style.GrabRounding = 2;
    style.TabRounding = 2;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.31f, 0.31f, 0.31f, 0.50f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.23f, 0.24f, 0.23f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.46f, 0.46f, 0.46f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.13f, 0.28f, 0.47f, 0.95f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

ImFont *YouiGui::LoadUserFont(const FontDetailsTupleType &fontDetails, const std::string &filename)
{
    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = true;
    fontConfig.MergeMode = false;
    fontConfig.OversampleH = 3;

    return LoadUserFont(fontDetails, filename, fontConfig);
}

bool YouiGui::CheckFileExist(const std::string &path)
{
    std::ifstream Infield(path);

    return Infield.good();
}

ImFont *YouiGui::LoadUserFont(const FontDetailsTupleType &fontDetails, const std::string &filename, const ImFontConfig &fontConfig)
{
    std::string fontName = std::get<0>(fontDetails);
    int fontSize = std::get<1>(fontDetails);
    std::string key = fontName + std::to_string(fontSize);

    auto fontIterator = userFonts.find(key);

    if (fontIterator == userFonts.end())
    {
        std::string home = getenv("HOME");
        std::string pathToFonts = "/Library/Fonts/";
        std::string pathUserToFonts = home + "/Library/Fonts/";
        std::string pathToTest;
        std::string availablePath;

        pathToTest = pathToFonts + filename + ".ttf";

        if (CheckFileExist(pathToTest))
        {
            availablePath = pathToTest;
        }
        else
        {
            pathToTest = pathToFonts + filename + ".otf";

            if (CheckFileExist(pathToTest))
            {
                availablePath = pathToTest;
            }
            else
            {
                pathToTest = pathUserToFonts + filename + ".ttf";

                if (CheckFileExist(pathToTest))
                {
                    availablePath = pathToTest;
                }
                else
                {
                    pathToTest = pathUserToFonts + filename + ".otf";

                    if (CheckFileExist(pathToTest))
                    {
                        availablePath = pathToTest;
                    }
                    else
                    {
                        availablePath = pathUserToFonts + "Arial-Regular.ttf";
                    }
                }
            }
        }

        if (!availablePath.empty())
        {
            ImFont *font = ImGui::GetIO().Fonts->AddFontFromFileTTF(availablePath.c_str(), fontSize, &fontConfig);

            userFonts[key] = font;

            return font;
        }
        else
        {
            return nullptr;
        }
    }

    return fontIterator->second;
}

void YouiGui::DisplayNotes(std::shared_ptr<std::vector<std::string>> notes)
{
    if (notes != nullptr)
    {
        ImGui::Indent(ImGui::GetStyle().IndentSpacing);
        for (const auto &note : *notes)
        {
            ImGui::TextWrapped("%s", note.c_str());
        }
        ImGui::Unindent();
    }
}

void YouiGui::ProcessDesignSystemFile(const std::string &designSystemFilename)
{
    recently_used_files.push_back(designSystemFilename);

    std::fstream designSystemJsonFile(designSystemFilename);

    if (designSystemJsonFile.is_open())
    {
        try
        {
            bDesignSystemLoaded = true;
            gDesignSystem = nlohmann::json::parse(designSystemJsonFile);
            m_toolDelegate->SetupColorPalette(gDesignSystem);

            auto flavors = gDesignSystem.typography->flavors;

            // TODO process all flavors to find fonts
            // For this PoC we are doing only one.
            auto flavor = (*flavors)[0];

            for (auto &typeStyle : *flavor.typography_styles)
            {
                auto fontName = *typeStyle.fontname;
                int fontSize = *typeStyle.fontsize;
                std::string path = fontName;

                YouiGui::FontDetailsTupleType fontDetails = std::tie(fontName, fontSize);

                gFontToLoadMap[fontDetails] = path;
            }
        }
        catch(json::parse_error)
        {
            bDesignSystemLoaded = false;
            ImGui::OpenPopup("Error Parsing");
        }
    }
}

void YouiGui::DetailsColorButton(const char *text, float col[4], ImGuiColorEditFlags flags)
{
    ImGuiContext &g = *ImGui::GetCurrentContext();

    ImGui::Text(text);

    ImVec2 sz(g.FontSize * 3 + g.Style.FramePadding.y * 2, g.FontSize * 3 + g.Style.FramePadding.y * 2);
    ImVec4 cf(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
    int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & ImGuiColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
    ImGui::ColorButton("##preview", cf, (flags & (ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf)) | ImGuiColorEditFlags_NoTooltip, sz);
    ImGui::SameLine();
    if ((flags & ImGuiColorEditFlags_InputRGB) || !(flags & ImGuiColorEditFlags__InputMask))
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
        {
            ImGui::Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", cr, cg, cb, cr, cg, cb, col[0], col[1], col[2]);
        }
        else
        {
            ImGui::Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3]);
        }
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
        {
            ImGui::Text("H: %.3f, S: %.3f, V: %.3f", col[0], col[1], col[2]);
        }
        else
        {
            ImGui::Text("H: %.3f, S: %.3f, V: %.3f, A: %.3f", col[0], col[1], col[2], col[3]);
        }
    }
}

void YouiGui::YouiSimpleTooltip(const std::string &tooltipText)
{
    ImGuiContext *imGuiContext = ImGui::GetCurrentContext();

    if (ImGui::IsItemHovered() && imGuiContext && imGuiContext->HoveredIdTimer > 1.0)
    {
        ImGui::SetTooltip(tooltipText.c_str());
    }
}

void YouiGui::TabBar(bool bDesignSystemLoaded)
{
    RenderOpenDesignSystemButton();
    RenderReloadDesignSystemButton(bDesignSystemLoaded);

    if (bDesignSystemLoaded)
    {
        ImGui::SameLine();

        std::shared_ptr<youi::Colors> colors = gDesignSystem.colors;

        if (colors != nullptr)
        {
            std::shared_ptr<std::vector<youi::ColorsFlavor>> flavors = colors->flavors;

            if (m_youiGuiDataModel.designSystem.selectedFlavor.empty())
            {
                m_youiGuiDataModel.designSystem.selectedFlavor = *flavors->at(0).name;
            }

            if (ImGui::BeginCombo("", m_youiGuiDataModel.designSystem.selectedFlavor.c_str(), ImGuiComboFlags_None))
            {
                for (int i = 0; i < flavors->size(); i++)
                {
                    bool is_selected = (m_youiGuiDataModel.designSystem.selectedFlavor == *flavors->at(i).name);

                    if (ImGui::Selectable(flavors->at(i).name->c_str(), is_selected))
                    {
                        m_youiGuiDataModel.designSystem.selectedFlavor = *flavors->at(i).name;
                    }

                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }
        }
    }
}

void YouiGui::TabPageHeader(const std::string &title, ImTextureID textureId)
{
    ImVec2 currentPosition = ImGui::GetCursorPos();
    ImGui::Image(textureId, ImVec2(675, 51), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, 0.85f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::SetCursorPos(currentPosition);

    ImGui::PushFont(H1Font);
    const ImVec4 &white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImGui::Indent(ImGui::GetStyle().IndentSpacing);
    ImGui::TextColored(white, title.c_str());
    ImGui::Unindent();
    ImGui::PopFont();
}

void YouiGui::ShowParsingErrorDialog()
{
    if (ImGui::BeginPopupModal("Error Parsing", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Enable to load the design system file. Ensure that it is the appropriate version.\n\n");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void YouiGui::FlavorIdentifierDisplay(const std::string &flavorName)
{
    // Orientation
    if (flavorName.find("landscape") != std::string::npos)
    {
        ImGui::Text(ICON_FA_IMAGE);
        ImGui::SameLine();
        ImGui::TextDisabled(" Landscape");
    }

    if (flavorName.find("portrait") != std::string::npos)
    {
        ImGui::Text(ICON_FA_PORTRAIT);
        ImGui::SameLine();
        ImGui::TextDisabled(" portrait");
    }

    // form factor
    if (flavorName.find("tv") != std::string::npos)
    {
        ImGui::Text(ICON_FA_TV);
        ImGui::SameLine();
        ImGui::TextDisabled(" tv");
    }

    if (flavorName.find("handset") != std::string::npos)
    {
        ImGui::Text(ICON_FA_MOBILE);
        ImGui::SameLine();
        ImGui::TextDisabled(" handset");
    }

    if (flavorName.find("tablet") != std::string::npos)
    {
        ImGui::Text(ICON_FA_TABLET);
        ImGui::SameLine();
        ImGui::TextDisabled(" tablet");
    }

    // Density
    if (flavorName.find("ldpi") != std::string::npos)
    {
        ImGui::Text(ICON_FA_EYE);
        ImGui::SameLine();
        ImGui::TextDisabled(" ldpi (~120dpi) (0.75x)");
    }

    if (flavorName.find("mdpi") != std::string::npos)
    {
        ImGui::Text(ICON_FA_EYE);
        ImGui::SameLine();
        ImGui::TextDisabled(" mdpi (~160dpi) (1.0x baseline)");
    }

    if (flavorName.find("hdpi") != std::string::npos)
    {
        ImGui::Text(ICON_FA_EYE);
        ImGui::SameLine();
        ImGui::TextDisabled(" hdpi (~240dpi) (1.5x)");
    }

    if (flavorName.find("xhdpi") != std::string::npos)
    {
        ImGui::Text(ICON_FA_EYE);
        ImGui::SameLine();
        ImGui::TextDisabled(" xhdpi (~320dpi) (2.0x)");
    }

    if (flavorName.find("xxhdpi") != std::string::npos)
    {
        ImGui::Text(ICON_FA_EYE);
        ImGui::SameLine();
        ImGui::TextDisabled(" xxhdpi (~480dpi) (3.0x)");
    }

    if (flavorName.find("xxxhdpi") != std::string::npos)
    {
        ImGui::Text(ICON_FA_EYE);
        ImGui::SameLine();
        ImGui::TextDisabled(" xxxhdpi (~640dpi) (4.0x)");
    }

    if (flavorName.find("nodpi") != std::string::npos)
    {
        ImGui::Text(ICON_FA_EYE);
        ImGui::SameLine();
        ImGui::TextDisabled(" nodpi (density-independent/no scaling)");
    }

    /*
        ldpi	Resources for low-density (ldpi) screens (~120dpi).
        mdpi	Resources for medium-density (mdpi) screens (~160dpi). (This is the baseline density.)
        hdpi	Resources for high-density (hdpi) screens (~240dpi).
        xhdpi	Resources for extra-high-density (xhdpi) screens (~320dpi).
        xxhdpi	Resources for extra-extra-high-density (xxhdpi) screens (~480dpi).
        xxxhdpi	Resources for extra-extra-extra-high-density (xxxhdpi) uses (~640dpi).
     */
}

void YouiGui::DetailedColorTooltip(const char *desc, const char *icon, const char *flavorName)
{
    ImGui::PushFont(bodyFont);

    if (strlen(icon))
    {
        ImGui::TextDisabled(icon);
    }
    else
    {
        ImGui::TextDisabled(" ? ");
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushFont(bodyFont);
        FlavorIdentifierDisplay(std::string(flavorName));
        ImGui::PopFont();
        ImGui::Separator();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 150.0f);
        ImGui::TextWrapped(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
    ImGui::PopFont();
}

void YouiGui::RenderOpenDesignSystemButton()
{
    if (ImGui::Button("\ue91c"))
    {
        ProcessDesignSystemFile("/Users/richardlalancette/Desktop/DesignSystemV3.json");
    }

    YouiSimpleTooltip("Open a design system file. (.json)");
}

void YouiGui::RenderReloadDesignSystemButton(bool bDesignSystemLoaded)
{
    ImGui::SameLine();

    if (!bDesignSystemLoaded)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    if (ImGui::Button("\ue943"))
    {
        ProcessDesignSystemFile("/Users/richardlalancette/Desktop/DesignSystemV3.json");
    }

    if (!bDesignSystemLoaded)
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    YouiSimpleTooltip("Reload design system file.");
}

