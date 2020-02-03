#ifndef _DESKTOP_TOOL_DELEGATE_H_
#define _DESKTOP_TOOL_DELEGATE_H_

#include "AuthoringToolInterface.h"

/*
 * This delegate facilitate developing the panel using a desktop mac application.
 */
class DesktopToolDelegate : public AuthoringToolInterface
{
public:
    void SetupColorPalette(youi::DesignSystem system) override;
    void Render(const YouiGuiDataModel &model) override;
    bool ShouldRenderDefaultMenu() override;
    float RenderCustomMainMenu(YouiGuiDataModel &model) override;
};

#endif
