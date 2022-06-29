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
#include "MenuPrinting.h"
#include "LevelInfo.h"
#include "MLevels.h"

void CLevelsMenu::Initialize_t(void) {
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  gm_mgTitle.SetName(TRANS("CHOOSE LEVEL"));
  AddChild(&gm_mgTitle);

  for (INDEX iLabel = 0; iLabel < LEVELS_ON_SCREEN; iLabel++) {
    INDEX iPrev = (LEVELS_ON_SCREEN + iLabel - 1) % LEVELS_ON_SCREEN;
    INDEX iNext = (iLabel + 1) % LEVELS_ON_SCREEN;
    // initialize label gadgets
    gm_mgManualLevel[iLabel].mg_pmgUp = &gm_mgManualLevel[iPrev];
    gm_mgManualLevel[iLabel].mg_pmgDown = &gm_mgManualLevel[iNext];
    gm_mgManualLevel[iLabel].mg_boxOnScreen = BoxMediumRow(iLabel - 1.0f);
    gm_mgManualLevel[iLabel].mg_pActivatedFunction = NULL; // never called!
    AddChild(&gm_mgManualLevel[iLabel]);
  }

  AddChild(&gm_mgArrowUp);
  AddChild(&gm_mgArrowDn);
  gm_mgArrowUp.mg_adDirection = AD_UP;
  gm_mgArrowDn.mg_adDirection = AD_DOWN;
  gm_mgArrowUp.mg_boxOnScreen = BoxArrow(AD_UP);
  gm_mgArrowDn.mg_boxOnScreen = BoxArrow(AD_DOWN);
  gm_mgArrowUp.mg_pmgRight = gm_mgArrowUp.mg_pmgDown = &gm_mgManualLevel[0];
  gm_mgArrowDn.mg_pmgRight = gm_mgArrowDn.mg_pmgUp = &gm_mgManualLevel[LEVELS_ON_SCREEN - 1];

  gm_ctListVisible = LEVELS_ON_SCREEN;
  gm_pmgArrowUp = &gm_mgArrowUp;
  gm_pmgArrowDn = &gm_mgArrowDn;
  gm_pmgListTop = &gm_mgManualLevel[0];
  gm_pmgListBottom = &gm_mgManualLevel[LEVELS_ON_SCREEN - 1];
}

void CLevelsMenu::FillListItems(void) {
  // disable all items first
  for (INDEX i = 0; i < LEVELS_ON_SCREEN; i++) {
    gm_mgManualLevel[i].mg_bEnabled = FALSE;
    gm_mgManualLevel[i].SetText(TRANS("<empty>"));
    gm_mgManualLevel[i].mg_iInList = -2;
  }

  BOOL bHasFirst = FALSE;
  BOOL bHasLast = FALSE;
  INDEX ctLabels = _lhFilteredLevels.Count();
  INDEX iLabel = 0;
  FOREACHINLIST(CLevelInfo, li_lnNode, _lhFilteredLevels, itli) {
    CLevelInfo &li = *itli;
    INDEX iInMenu = iLabel - gm_iListOffset;
    if ((iLabel >= gm_iListOffset) && (iLabel < (gm_iListOffset + LEVELS_ON_SCREEN))) {
      bHasFirst |= (iLabel == 0);
      bHasLast |= (iLabel == ctLabels - 1);
      gm_mgManualLevel[iInMenu].SetText(li.li_strName);
      gm_mgManualLevel[iInMenu].mg_fnmLevel = li.li_fnLevel;
      gm_mgManualLevel[iInMenu].mg_bEnabled = TRUE;
      gm_mgManualLevel[iInMenu].mg_iInList = iLabel;

      // [Cecil] Levels from other games cannot be played
      if (li.li_eFormat != CLevelInfo::E_LF_SE100) {
        CTString strFormat = "1.50";

        if (li.li_eFormat == CLevelInfo::E_LF_SSR) {
          strFormat = "SSR";
        }
        
        CTString strTip;
        strTip.PrintF(TRANS("This is a level in the %s format, it cannot be played!"), strFormat);

        gm_mgManualLevel[iInMenu].mg_strTip = strTip;
        gm_mgManualLevel[iInMenu].mg_bEnabled = FALSE;
      }
    }

    iLabel++;
  }

  // enable/disable up/down arrows
  gm_mgArrowUp.mg_bEnabled = !bHasFirst && ctLabels > 0;
  gm_mgArrowDn.mg_bEnabled = !bHasLast && ctLabels > 0;
}

void CLevelsMenu::StartMenu(void) {
  // set default parameters for the list
  gm_iListOffset = 0;
  gm_ctListTotal = _lhFilteredLevels.Count();
  gm_iListWantedItem = 0;
  // for each level
  INDEX i = 0;
  FOREACHINLIST(CLevelInfo, li_lnNode, _lhFilteredLevels, itlid) {
    CLevelInfo &lid = *itlid;
    // if it is the chosen one
    if (lid.li_fnLevel == _pGame->gam_strCustomLevel) {
      // demand focus on it
      gm_iListWantedItem = i;
      break;
    }
    i++;
  }
  CGameMenu::StartMenu();
}