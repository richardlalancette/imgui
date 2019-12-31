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

json designSystemJsonObject;
Youi::DesignSystem designSystem;
ImFont *bodyFont;
ImFont *H1Font;

void StyleColorsYouiLight();

void DetailsColorButton(const char *name, float pDouble[4], ImGuiColorEditFlags flags);

// Not perfect... some clipping on the right side sometimes.
void BeginGroupPanel(const char *name, const ImVec2 &size = ImVec2(-1.0f, -1.0f))
{
    ImGui::BeginGroup();
    ImGui::TextUnformatted(name);

    auto itemSpacing = ImGui::GetStyle().ItemSpacing;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();
    ImGui::BeginGroup();

    ImVec2 effectiveSize = size;
    if (size.x < 0.0f)
    {
        effectiveSize.x = ImGui::GetContentRegionAvailWidth();
    }
    else
    {
        effectiveSize.x = size.x;
    }
    ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::SameLine(0.0f, 0.0f);
//    ImGui::TextUnformatted(name);
    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
    ImGui::BeginGroup();

    ImGui::PopStyleVar(2);

    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->Size.x -= frameHeight;

    ImGui::PushItemWidth(effectiveSize.x - frameHeight);
}

void EndGroupPanel()
{
    ImGui::PopItemWidth();

    auto itemSpacing = ImGui::GetStyle().ItemSpacing;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

    auto frameHeight = ImGui::GetFrameHeight();

    ImGui::EndGroup();

//    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

    ImGui::EndGroup();

    ImGui::SameLine(0.0f, 0.0f);
    ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
    ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

    ImGui::EndGroup();

    ImVec2 itemMin = ImGui::GetItemRectMin();
    ImVec2 itemMax = ImGui::GetItemRectMax();
//    ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);
    ImVec2 halfFrame = ImVec2(frameHeight * 0.25f * 0.5f, frameHeight * 0.5f);
//    ImGui::GetWindowDrawList()->AddRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f), ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)), halfFrame.x);

    ImGui::PopStyleVar(2);

    ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
    ImGui::GetCurrentWindow()->Size.x += frameHeight;

    ImGui::Dummy(ImVec2(0.0f, 0.0f));

    ImGui::EndGroup();
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
        ImGui::StyleColorsLight();
        StyleColorsYouiLight();

        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->AddFontDefault();

        ImFontConfig mainFontConfig;
        mainFontConfig.FontDataOwnedByAtlas = true;
        mainFontConfig.MergeMode = false;
        mainFontConfig.OversampleH = 3;

        // This one is a new font; Roboto-Black. Do not merge
//        std::string bodyFontPath = std::string(getcwd(NULL, 0)) + "/../../Roboto-Regular.ttf";
        std::string bodyFontPath = std::string(getcwd(NULL, 0)) + "/../../Roboto-Medium.ttf";
//        std::string bodyFontPath = std::string(getcwd(NULL, 0)) + "/../../DroidSans.ttf";
        bodyFont = io.Fonts->AddFontFromFileTTF(bodyFontPath.c_str(), 16.0f, &mainFontConfig);

        ImFontConfig icons_config;
        icons_config.PixelSnapH = true;
        icons_config.MergeMode = true;

        // Merge this font with the default font for now.
        std::string iconsFontPath = std::string(getcwd(NULL, 0)) + "/../../fa-solid-900.ttf";
        io.Fonts->AddFontFromFileTTF(iconsFontPath.c_str(), 16.0f, &icons_config, icons_ranges);

        // Header 1 font
        ImFontConfig h1FontConfig;
        h1FontConfig.FontDataOwnedByAtlas = true;
        h1FontConfig.MergeMode = false;
        std::string h1FontPath = std::string(getcwd(NULL, 0)) + "/../../Roboto-Black.ttf";
        H1Font = io.Fonts->AddFontFromFileTTF(h1FontPath.c_str(), 18.0f, &h1FontConfig);

        ImGui_ImplMetal_Init(_device);

        std::fstream designSystemJsonFile("/Users/richardlalancette/Desktop/DesignSystemV3.json");
//        designSystemJsonFile >> designSystemJsonObject;

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
        auto motionSpec = designSystemJsonObject.find("motiondesign");

        ImGui::BeginTabBar("Design System!");
        {
            if (ImGui::BeginTabItem(" Macro"))
            {
                auto macro = motionSpec->find("macro");
                for (auto &element : *macro)
                {
                    ImGui::Separator();
                    ImGui::BeginGroup();
                    {
                        ImGui::Indent(ImGui::GetStyle().IndentSpacing);
                        ImGui::TextUnformatted(element["name"].get<std::string>().c_str());
                        ImGui::TextUnformatted(element["description"].get<std::string>().c_str());
                        ImGui::TextUnformatted(element["element"].get<std::string>().c_str());
                        ImGui::TextUnformatted(element["transformation"].get<std::string>().c_str());
                        ImGui::TextUnformatted(element["duration"].get<std::string>().c_str());
                        ImGui::TextUnformatted(element["delay"].get<std::string>().c_str());

                        //                            auto curve = macro->find("curve");
                        ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
                    }
                    ImGui::EndGroup();
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem(" Micro"))
            {
                auto micro = motionSpec->find("micro");
                for (auto &element : *micro)
                {
                }
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
        auto typography = designSystemJsonObject.find("typography");

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

/*
     ImGuiColorEditFlags_None            = 0,
    ImGuiColorEditFlags_NoAlpha         = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
    ImGuiColorEditFlags_NoPicker        = 1 << 2,   //              // ColorEdit: disable picker when clicking on colored square.
    ImGuiColorEditFlags_NoOptions       = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
    ImGuiColorEditFlags_NoSmallPreview  = 1 << 4,   //              // ColorEdit, ColorPicker: disable colored square preview next to the inputs. (e.g. to show only the inputs)
    ImGuiColorEditFlags_NoInputs        = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview colored square).
    ImGuiColorEditFlags_NoTooltip       = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
    ImGuiColorEditFlags_NoLabel         = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
    ImGuiColorEditFlags_NoSidePreview   = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small colored square preview instead.
    ImGuiColorEditFlags_NoDragDrop      = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.

    // User Options (right-click on widget to change some of them).
    ImGuiColorEditFlags_AlphaBar        = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
    ImGuiColorEditFlags_AlphaPreview    = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
    ImGuiColorEditFlags_AlphaPreviewHalf= 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
    ImGuiColorEditFlags_HDR             = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use ImGuiColorEditFlags_Float flag as well).
    ImGuiColorEditFlags_DisplayRGB      = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
    ImGuiColorEditFlags_DisplayHSV      = 1 << 21,  // [Display]    // "
    ImGuiColorEditFlags_DisplayHex      = 1 << 22,  // [Display]    // "
    ImGuiColorEditFlags_Uint8           = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
    ImGuiColorEditFlags_Float           = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
    ImGuiColorEditFlags_PickerHueBar    = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
    ImGuiColorEditFlags_PickerHueWheel  = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
    ImGuiColorEditFlags_InputRGB        = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
    ImGuiColorEditFlags_InputHSV        = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.
 */

            ImGuiColorEditFlags colorDisplayFlags = 0;
            colorDisplayFlags |= ImGuiColorEditFlags_AlphaPreviewHalf;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayRGB;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayHex;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayHSV;

            for (auto &flavor : *flavors)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_ChildBg]);
                ImGui::PushFont(H1Font);
                BeginGroupPanel(flavor.name->c_str(), ImVec2(150.0f, -1.0f));
                {
                    ImGui::PopFont();

                    for (auto &colorSwatch : *flavor.flavor_colors)
                    {
                        auto colorRGBA = *colorSwatch.color_rgba;
                        auto colorName = colorSwatch.name->c_str();
//                        json colorRGBA = *flavor.get["colorRGBA"];

//                    ImVec4 *color = &colorSwatch["colorRGBA"];
                        float color[] = {static_cast<float>(*colorRGBA.r), static_cast<float>(*colorRGBA.g), static_cast<float>(*colorRGBA.b), static_cast<float>(*colorRGBA.a)};
//                        ImGui::ColorPicker4(colorName, color, colorDisplayFlags);

                        DetailsColorButton(colorName, color, colorDisplayFlags);
//                        ImGui::SameLine();
//                        ImGui::Text(colorName);
                    }
                }
                EndGroupPanel();
                ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::ColorConvertFloat4ToU32(ImGui::GetStyle().Colors[ImGuiCol_PopupBg]));
                ImGui::PopStyleColor();
                ImGui::PopStyleVar();
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
