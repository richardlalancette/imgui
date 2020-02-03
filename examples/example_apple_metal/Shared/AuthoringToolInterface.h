#ifndef _AUTHORING_TOOL_INTERFACE_H_
#define _AUTHORING_TOOL_INTERFACE_H_

#include "designsystem.h"

#include "YouiGuiDataModel.h"

class AuthoringToolInterface
{
public:
    virtual void SetupColorPalette(youi::DesignSystem system) = 0;
    virtual void Render(const YouiGuiDataModel &model) = 0;
    virtual bool ShouldRenderDefaultMenu() = 0;
    virtual float RenderCustomMainMenu(YouiGuiDataModel &model) = 0;
};

#endif
