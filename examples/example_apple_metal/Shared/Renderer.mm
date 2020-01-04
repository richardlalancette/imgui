#import "Renderer.h"
#import <Metal/Metal.h>
#import <fstream>

#include "imgui.h"
#include "imgui_impl_metal.h"
#import "picojson.h"

// Cloud based design
// https://assets.adobe.com/public/d608e206-7e47-43f4-5c1b-dacdcff45581
// https://github.com/nlohmann/json
#import "json.hpp"
#import "IconsFontAwesome5.h"
#include "designsystem.h"

using json = nlohmann::json;

#if TARGET_OS_OSX
#include "imgui_impl_osx.h"
#import "imgui_internal.h"

#endif

@interface Renderer ()
@property(nonatomic, strong) id <MTLDevice> device;
@property(nonatomic, strong) id <MTLCommandQueue> commandQueue;
@end

Youi::DesignSystem designSystem;
ImFont *bodyFont;
ImFont *H1Font;

void StyleColorsYouiLight();
void DetailsColorButton(const char *name, float pDouble[4], ImGuiColorEditFlags flags);
static void FlavorIdentifierDisplay(const std::string &flavorName);

static void FlavorIdentifierDisplay(const std::string &flavorName)
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

static void DetailedColorTooltip(const char *desc, const char *icon = "?", const char *flavorName = "")
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

@implementation Renderer

- (nonnull instancetype)initWithView:(nonnull MTKView *)view;
{
    self = [super init];
    if (self)
    {
        _device = view.device;
        _commandQueue = [_device newCommandQueue];

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
//        ImGui::StyleColorsLight();
        StyleColorsYouiLight();

        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->AddFontDefault();

        ImFontConfig mainFontConfig;
        mainFontConfig.FontDataOwnedByAtlas = true;
        mainFontConfig.MergeMode = false;

        auto currentApplicationFolder = std::string(getcwd(NULL, 0));
        
        std::string bodyFontPath = currentApplicationFolder + "/../../fonts/Roboto_Mono/RobotoMono-Regular.ttf";
        bodyFont = io.Fonts->AddFontFromFileTTF(bodyFontPath.c_str(), 18.0f, &mainFontConfig);

        ImFontConfig icons_config;
        icons_config.PixelSnapH = true;
        icons_config.MergeMode = true;
        icons_config.OversampleH = 3;

        // Merge this font with the default font for now.
        std::string iconsFontPath = currentApplicationFolder + "/../../fonts/FontAwesome/fa-solid-900.ttf";
        io.Fonts->AddFontFromFileTTF(iconsFontPath.c_str(), 16.0f, &icons_config, icons_ranges);

        // Header 1 font
        ImFontConfig h1FontConfig;
        h1FontConfig.FontDataOwnedByAtlas = true;
        h1FontConfig.MergeMode = false;
        h1FontConfig.OversampleH = 3;
        std::string h1FontPath = currentApplicationFolder + "/../../fonts/Raleway/Raleway-BoldItalic.ttf";
        H1Font = io.Fonts->AddFontFromFileTTF(h1FontPath.c_str(), 24.0f, &h1FontConfig);

        ImGui_ImplMetal_Init(_device);

        std::fstream designSystemJsonFile("/Users/richardlalancette/Desktop/DesignSystemV3.json");

        designSystem = nlohmann::json::parse(designSystemJsonFile);
    }

    return self;
}

- (void)drawInMTKView:(MTKView *)view
{
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize.x = view.bounds.size.width;
    io.DisplaySize.y = view.bounds.size.height;

#if TARGET_OS_OSX
    CGFloat framebufferScale = view.window.screen.backingScaleFactor ? : NSScreen.mainScreen.backingScaleFactor;
#else
    CGFloat framebufferScale = view.window.screen.scale ?: UIScreen.mainScreen.scale;
#endif
    io.DisplayFramebufferScale = ImVec2(framebufferScale, framebufferScale);
    io.DeltaTime = 1 / float(view.preferredFramesPerSecond ? : 60);

    id <MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];

    static float clear_color[4] = {0.28f, 0.36f, 0.5f, 1.0f};

    MTLRenderPassDescriptor *renderPassDescriptor = view.currentRenderPassDescriptor;
    if (renderPassDescriptor != nil)
    {
        renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

        // Here, you could do additional rendering work, including other passes as necessary.

        id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
        [renderEncoder pushDebugGroup:@"ImGui demo"];

        // Start the Dear ImGui frame
        ImGui_ImplMetal_NewFrame(renderPassDescriptor);

#if TARGET_OS_OSX
        ImGui_ImplOSX_NewFrame(view);
#endif
        ImGui::NewFrame();
        ImGui::PushFont(bodyFont);

        ImGuiWindowFlags window_flags = 0;
        static bool open = true;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

//        static bool show_app_style_editor = true;
//        ImGui::Begin("Style Editor", &show_app_style_editor);
//        ImGui::ShowStyleEditor();
//        ImGui::End();
//        ImGui::ShowDemoWindow();

        ImGui::Begin("Design System", &open, window_flags);
        {
            ImGui::BeginTabBar("Design System!");
            {
                [self UIDesignSystemInstructions];
                [self UIColorPalette];
                [self UITypography];
                [self UIMotionDesign];
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        ImGui::PopFont();

        // Rendering
        ImGui::Render();
        ImDrawData *drawData = ImGui::GetDrawData();
        ImGui_ImplMetal_RenderDrawData(drawData, commandBuffer, renderEncoder);

        [renderEncoder popDebugGroup];
        [renderEncoder endEncoding];

        [commandBuffer presentDrawable:view.currentDrawable];
    }

    [commandBuffer commit];
}

- (void)UIMotionDesign
{
    if (ImGui::BeginTabItem(ICON_FA_FIGHTER_JET " Motion"))
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

- (void)UITypography
{
    if (ImGui::BeginTabItem(ICON_FA_FONT " Type"))
    {
        ImGui::EndTabItem();
    }
}

- (void)UIColorPalette
{
    if (ImGui::BeginTabItem(ICON_FA_TINT " Colors"))
    {
        auto colors = designSystem.colors;

        if (colors != nullptr)
        {
            auto flavors = colors->flavors;

            ImGuiColorEditFlags colorDisplayFlags = 0;
            colorDisplayFlags |= ImGuiColorEditFlags_AlphaPreviewHalf;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayRGB;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayHex;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayHSV;

            static bool alpha_preview = true;
            static bool alpha_half_preview = false;
            static bool drag_and_drop = true;
            static bool options_menu = true;
            static bool hdr = false;

            for (auto &flavor : *flavors)
            {
                ImGui::PushFont(H1Font);
                ImGui::Text(flavor.name->c_str());
                ImGui::SameLine();

                DetailedColorTooltip("Additional information and \nmetadata can be found here.", ICON_FA_INFO_CIRCLE, flavor.name->c_str());
                ImGui::Dummy(ImGui::GetStyle().ItemSpacing);
                ImGui::PopFont();

                for (auto &colorSwatch : *flavor.flavor_colors)
                {
                    auto colorRGBA = *colorSwatch.color_rgba;
                    auto colorName = colorSwatch.name->c_str();
                    float color[] = {static_cast<float>(*colorRGBA.r), static_cast<float>(*colorRGBA.g), static_cast<float>(*colorRGBA.b), static_cast<float>(*colorRGBA.a)};

                    DetailsColorButton(colorName, color, colorDisplayFlags);
                }
                
                ImGui::Dummy(ImGui::GetStyle().ItemSpacing);
                ImGui::Separator();
                ImGui::Dummy(ImGui::GetStyle().ItemSpacing);
            }
        }
        else
        {
            ImGui::PushFont(H1Font);
            ImGui::TextWrapped("No colors found in the design system.", "");
            ImGui::PopFont();
        }
        ImGui::EndTabItem();
    }
}

- (void)UIDesignSystemInstructions
{
    if (ImGui::BeginTabItem(ICON_FA_PENCIL_RULER " Design"))
    {
        auto ds = designSystem.instructions;
        ImGui::TextWrapped("Format: %s", ds->design_system_format->c_str());
        ImGui::Separator();
        for (const auto &m : *ds->metadata)
        {
            ImGui::TextWrapped("%s", m.c_str());
        }

        ImGui::EndTabItem();
    }
}

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size
{
}

@end

void DetailsColorButton(const char *text, float col[4], ImGuiColorEditFlags flags)
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
            ImGui::Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", cr, cg, cb, cr, cg, cb, col[0], col[1], col[2]);
        else
            ImGui::Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3]);
    }
    else if (flags & ImGuiColorEditFlags_InputHSV)
    {
        if (flags & ImGuiColorEditFlags_NoAlpha)
            ImGui::Text("H: %.3f, S: %.3f, V: %.3f", col[0], col[1], col[2]);
        else
            ImGui::Text("H: %.3f, S: %.3f, V: %.3f, A: %.3f", col[0], col[1], col[2], col[3]);
    }
}

void StyleColorsYouiLight()
{
    ImGuiStyle &style = ImGui::GetStyle();

    style.WindowPadding.x = 5;
    style.WindowPadding.y = 5;
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
