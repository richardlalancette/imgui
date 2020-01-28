#ifndef _DESKTOPTOOLDELEGATE_H_
#define _DESKTOPTOOLDELEGATE_H_

#include "AuthoringToolInterface.h"

/*
 * This delegate facilitate developing the panel using a desktop mac application.
 */
class DesktopToolDelegate : public AuthoringToolInterface
{
public:
    void SetupColorPalette(youi::DesignSystem system) override;
    void Render() override;
};

#endif
