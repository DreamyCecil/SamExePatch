/* Copyright (c) 2002-2012 Croteam Ltd.
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#ifndef SE_INCL_MENU_GADGET_VARBUTTON_H
#define SE_INCL_MENU_GADGET_VARBUTTON_H
#ifdef PRAGMA_ONCE
#pragma once
#endif

#include "MGButton.h"

// [Cecil] Derive from CMGEdit, which derives from CMGButton
class CMGVarButton : public CMGEdit {
  public:
    class CVarSetting *mg_pvsVar;
    PIXaabbox2D GetSliderBox(INDEX iSliderType);
    BOOL OnKeyDown(PressedMenuButton pmb);
    BOOL OnMouseHeld(PressedMenuButton pmb); // [Cecil]
    void Render(CDrawPort *pdp);
    BOOL IsSeparator(void);
    BOOL IsEnabled(void);

    // [Cecil] Change strings
    void OnStringChanged(void);
    void OnStringCanceled(void);
};

#endif /* include-once check. */