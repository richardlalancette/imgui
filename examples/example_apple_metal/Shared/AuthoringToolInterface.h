#ifndef _AUTHORINGTOOLINTERFACE_H_
#define _AUTHORINGTOOLINTERFACE_H_

#include "designsystem.h"

class AuthoringToolInterface
{
public:
    virtual void SetupColorPalette(youi::DesignSystem system) = 0;
    virtual void Render() = 0;
};

#endif
