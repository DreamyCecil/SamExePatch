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

#include "StdH.h"
#include "VarList.h"
#include "MGVarButton.h"

extern PIX _pixCursorPosI;
extern PIX _pixCursorPosJ;

BOOL CMGVarButton::IsSeparator(void) {
  if (mg_pvsVar == NULL) {
    return FALSE;
  }

  // [Cecil] Check the separator type
  return (mg_pvsVar->vs_eType == CVarSetting::E_SEPARATOR);
}

BOOL CMGVarButton::IsEnabled(void) {
  return (_gmRunningGameMode == GM_NONE || mg_pvsVar == NULL || mg_pvsVar->vs_bCanChangeInGame);
}

// return slider position on scren
PIXaabbox2D CMGVarButton::GetSliderBox(CDrawPort *pdp, INDEX iSliderType) {
  // [Cecil] Big fill slider
  if (iSliderType == CVarSetting::SLD_BIGFILL) {
    extern PIXaabbox2D GetHorSliderBox(CDrawPort *pdp, FLOATaabbox2D boxOnScreen, BOOL bHasLabel);
    return GetHorSliderBox(pdp, mg_boxOnScreen, TRUE);
  }

  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIR = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioR;
  PIX pixJ = box.Min()(2);
  PIX pixISize = box.Size()(1) * 0.13f;
  PIX pixJSize = box.Size()(2);

  return PIXaabbox2D(PIX2D(pixIR + 1, pixJ + 1), PIX2D(pixIR + pixISize - 4, pixJ + pixJSize - 6));
}

extern BOOL _bVarChanged;
BOOL CMGVarButton::OnKeyDown(PressedMenuButton pmb)
{
  if (mg_pvsVar == NULL || mg_pvsVar->vs_eType == CVarSetting::E_SEPARATOR || !mg_pvsVar->Validate() || !mg_bEnabled) {
    // [Cecil] CMenuGadget::OnKeyDown() would call CMGEdit::OnActivate(), which shouldn't happen
    return pmb.Apply(TRUE);
  }

  // [Cecil] Editing the textbox
  if (mg_bEditing) {
    return CMGEdit::OnKeyDown(pmb);
  }

  CVarMenu &gmCurrent = _pGUIM->gmVarMenu;
  const BOOL bSlider = (mg_pvsVar->vs_eSlider != CVarSetting::SLD_NOSLIDER);

  // [Cecil] Toggleable setting
  if (mg_pvsVar->vs_eType == CVarSetting::E_TOGGLE)
  {
    // handle slider
    if (bSlider && !mg_pvsVar->vs_bCustom) {
      // ignore RMB
      if (pmb.iKey == VK_RBUTTON) {
        return TRUE;
      }

      // handle LMB
      if (pmb.iKey == VK_LBUTTON) {
        // get position of slider box on screen
        PIXaabbox2D boxSlider = GetSliderBox(_pdpMenu, mg_pvsVar->vs_eSlider);
        // if mouse is within
        if (boxSlider >= PIX2D(_pixCursorPosI, _pixCursorPosJ)) {
          // set new position exactly where mouse pointer is
          mg_pvsVar->vs_iValue = (FLOAT)(_pixCursorPosI - boxSlider.Min()(1)) / boxSlider.Size()(1) * (mg_pvsVar->vs_ctValues);
          _bVarChanged = TRUE;
        }

        // handled
        return TRUE;
      }
    }

  // [Cecil] Button setting
  } else if (mg_pvsVar->vs_eType == CVarSetting::E_BUTTON) {
    // Enter another option config on click
    if (pmb.Apply(TRUE)) {
      // Copy the string from the setting
      const CTString strConfig = mg_pvsVar->vs_strSchedule;

      FlushVarSettings(FALSE);
      gmCurrent.EndMenu();

      gmCurrent.gm_fnmMenuCFG = strConfig;
      gmCurrent.StartMenu();

      extern CSoundData *_psdPress;
      PlayMenuSound(_psdPress);
      return TRUE;
    }
  }

  if (pmb.Apply(FALSE)) {
    // [Cecil] Emulate the action of clicking on "Apply"
    gmCurrent.gm_mgApply.OnActivate();
    return TRUE;
  }

  // [Cecil] Different types
  switch (mg_pvsVar->vs_eType) {
    // Toggle values
    case CVarSetting::E_TOGGLE: {
      // [Cecil] Increase/decrease the value
      INDEX iPower = pmb.ChangeValue();

      // Try previous/next value
      if (iPower == 0) {
        if (pmb.Prev()) iPower = -1;
        else
        if (pmb.Next()) iPower = +1;
      }

      if (iPower != 0) {
        // Change one value at a time for non-sliders
        if (!bSlider) iPower = SgnNZ(iPower);

        INDEX iOldValue = mg_pvsVar->vs_iValue;
        mg_pvsVar->vs_iValue += iPower;

        // Clamp sliders
        if (bSlider) {
          mg_pvsVar->vs_iValue = Clamp(mg_pvsVar->vs_iValue, (INDEX)0, INDEX(mg_pvsVar->vs_ctValues - 1));

        // Wrap to the beginning
        } else if (mg_pvsVar->vs_iValue >= mg_pvsVar->vs_ctValues) {
          mg_pvsVar->vs_iValue = 0;

        // Wrap to the end
        } else if (mg_pvsVar->vs_iValue < 0) {
          mg_pvsVar->vs_iValue = mg_pvsVar->vs_ctValues - 1;
        }

        if (iOldValue != mg_pvsVar->vs_iValue) {
          _bVarChanged = TRUE;
          mg_pvsVar->vs_bCustom = FALSE;
          mg_pvsVar->Validate();
        }

        return TRUE;
      }
    } break;

    // Reset editing value
    case CVarSetting::E_TEXTBOX: {
      OnStringCanceled();
    } break;
  }

  // not handled
  return CMenuGadget::OnKeyDown(pmb);
}

// [Cecil] Adjust the slider by holding a button
BOOL CMGVarButton::OnMouseHeld(PressedMenuButton pmb)
{
  if (pmb.iKey != VK_LBUTTON) return FALSE;
  if (mg_pvsVar == NULL) return FALSE;

  // If it's a toggleable slider without a custom value that was pressed last
  if (_pmgLastGadgetLMB == this && mg_pvsVar->vs_eType == CVarSetting::E_TOGGLE
   && !mg_pvsVar->vs_bCustom && mg_pvsVar->vs_eSlider != CVarSetting::SLD_NOSLIDER)
  {
    // Forward the key
    return OnKeyDown(pmb);
  }

  return FALSE;
};

void CMGVarButton::Render(CDrawPort *pdp) {
  if (mg_pvsVar == NULL) {
    return;
  }

  SetFontMedium(pdp, mg_fTextScale);

  PIXaabbox2D box = FloatBoxToPixBox(pdp, mg_boxOnScreen);
  PIX pixIL = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioL;
  PIX pixIR = box.Min()(1) + box.Size()(1) * _fGadgetSideRatioR;
  PIX pixIC = box.Center()(1);
  PIX pixJ = box.Min()(2);

  // [Cecil] Different types
  switch (mg_pvsVar->vs_eType) {
    // Separator
    case CVarSetting::E_SEPARATOR: {
      mg_bEnabled = FALSE;
      COLOR col = _pGame->LCDGetColor(C_WHITE | 255, "separator");

      CTString strText = mg_pvsVar->vs_strName;
      pdp->PutTextC(strText, pixIC, pixJ, col);
    } break;

    // Toggleable
    case CVarSetting::E_TOGGLE: {
      if (mg_pvsVar->Validate()) {
        // check whether the variable is disabled
        if (mg_pvsVar->vs_strFilter != "") {
          mg_bEnabled = _pShell->GetINDEX(mg_pvsVar->vs_strFilter);
        }

        COLOR col = GetCurrentColor();
        pdp->PutTextR(mg_pvsVar->vs_strName, pixIL, pixJ, col);

        // custom is by default
        CTString strText = LOCALIZE("Custom");
        BOOL bCenteredText = FALSE;

        // Not a custom value
        if (!mg_pvsVar->vs_bCustom) {
          strText = mg_pvsVar->vs_astrTexts[mg_pvsVar->vs_iValue];

          const FLOAT fFactor = FLOAT(mg_pvsVar->vs_iValue + 1) / (FLOAT)mg_pvsVar->vs_ctValues;
          const CVarSetting::ESliderType eSlider = mg_pvsVar->vs_eSlider;

          // [Cecil] Use pre-calculated slider box for rendering
          PIXaabbox2D boxSlider = GetSliderBox(pdp, eSlider);
          const PIX pixSliderX = boxSlider.Min()(1) + 1;
          const PIX pixSliderY = boxSlider.Min()(2) + 1;
          const PIX2D vSliderSize = boxSlider.Size();

          // [Cecil] Unified both fill slider types
          if (eSlider == CVarSetting::SLD_FILL || eSlider == CVarSetting::SLD_BIGFILL) {
            // Draw box around the slider
            _pGame->LCDDrawBox(0, -1, boxSlider, _pGame->LCDGetColor(C_GREEN | 255, "slider box"));

            // Fill slider
            pdp->Fill(pixSliderX, pixSliderY, (vSliderSize(1) - 2) * fFactor, (vSliderSize(2) - 2), col);

            // [Cecil] Move text in the middle of the box
            if (eSlider == CVarSetting::SLD_BIGFILL) {
              pixIR += vSliderSize(1) / 2;
              pixJ += 2;
              bCenteredText = TRUE;

            } else {
              // Move text to the right of the box
              pixIR += vSliderSize(1) * 1.15f;
            }

          // Ratio slider
          } else if (eSlider == CVarSetting::SLD_RATIO) {
            // Draw box around the slider
            _pGame->LCDDrawBox(0, -1, boxSlider, _pGame->LCDGetColor(C_GREEN | 255, "slider box"));

            FLOAT fUnitWidth = FLOAT(vSliderSize(1) - 1) / mg_pvsVar->vs_ctValues;
            pdp->Fill(pixSliderX + (mg_pvsVar->vs_iValue * fUnitWidth), pixSliderY, fUnitWidth, (vSliderSize(2) - 2), col);
            
            // Move text to the right of the box
            pixIR += vSliderSize(1) * 1.15f;
          }
        }

        // Write value text
        if (bCenteredText) {
          pdp->PutTextC(strText, pixIR, pixJ, col);
        } else {
          pdp->PutText(strText, pixIR, pixJ, col);
        }
      }
    } break;

    // Textbox
    case CVarSetting::E_TEXTBOX: {
      CMGEdit::Render(pdp);
    } break;

    // Button
    case CVarSetting::E_BUTTON: {
      pdp->PutTextC(mg_pvsVar->vs_strName, pixIC, pixJ, GetCurrentColor());
    } break;
  }
}

// [Cecil] Change strings
void CMGVarButton::OnStringChanged(void) {
  // No textbox attached
  if (mg_pvsVar == NULL || mg_pvsVar->vs_eType != CVarSetting::E_TEXTBOX) {
    return;
  }

  // If new hash differs from the old one, mark as changed
  ULONG ulOldHash = static_cast<ULONG>(mg_pvsVar->vs_iOrgValue);

  if (mg_pvsVar->vs_strValue.GetHash() != ulOldHash) {
    _bVarChanged = TRUE;
  }
};

void CMGVarButton::OnStringCanceled(void) {
  // Restore string from the setting
  SetText(mg_pvsVar->vs_strValue);
};
