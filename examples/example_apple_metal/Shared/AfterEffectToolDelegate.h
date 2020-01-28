#ifndef _AFTEREFFECTTOOLDELEGATE_H_
#define _AFTEREFFECTTOOLDELEGATE_H_

#include "AuthoringToolInterface.h"

/*
 * This delegate bridges with after effects
 */
class AfterEffectToolDelegate  : public AuthoringToolInterface
{
public:
    void SetupColorPalette(youi::DesignSystem system) override;
    void Render() override;
};

#endif
