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
#include "MConfirm.h"

// [Cecil] Reset popup box
static void ResetPopup(CConfirmMenu *pgm, FLOAT fHeight) {
  gm_bPopup = TRUE;
  pgm->gm_mgConfirmLabel.mg_boxOnScreen = BoxPopupLabel(fHeight);
};

void CConfirmMenu::Initialize_t(void) {
  const FLOAT fHeight = 0.2f;

  AddChild(&gm_mgConfirmLabel);
  gm_mgConfirmLabel.mg_iCenterI = 0;
  gm_mgConfirmLabel.mg_bfsFontSize = BFS_LARGE;

  AddChild(&gm_mgConfirmYes);
  gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesLarge(fHeight);
  gm_mgConfirmYes.mg_pActivatedFunction = NULL;
  gm_mgConfirmYes.mg_pmgLeft = gm_mgConfirmYes.mg_pmgRight = &gm_mgConfirmNo;
  gm_mgConfirmYes.mg_iCenterI = 1;
  gm_mgConfirmYes.mg_bfsFontSize = BFS_LARGE;

  AddChild(&gm_mgConfirmNo);
  gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoLarge(fHeight);
  gm_mgConfirmNo.mg_pActivatedFunction = NULL;
  gm_mgConfirmNo.mg_pmgLeft = gm_mgConfirmNo.mg_pmgRight = &gm_mgConfirmYes;
  gm_mgConfirmNo.mg_iCenterI = -1;
  gm_mgConfirmNo.mg_bfsFontSize = BFS_LARGE;

  _pConfimedYes = NULL;
  _pConfimedNo = NULL;

  ResetPopup(this, fHeight);
  SetText("");
}

void CConfirmMenu::BeLarge(FLOAT fHeight) {
  ResetPopup(this, fHeight);

  gm_mgConfirmLabel.mg_bfsFontSize = BFS_LARGE;
  gm_mgConfirmYes.mg_bfsFontSize = BFS_LARGE;
  gm_mgConfirmNo.mg_bfsFontSize = BFS_LARGE;

  gm_mgConfirmLabel.mg_iCenterI = 0;
  gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesLarge(fHeight);
  gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoLarge(fHeight);
}

void CConfirmMenu::BeSmall(FLOAT fHeight) {
  ResetPopup(this, fHeight);

  gm_mgConfirmLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgConfirmYes.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgConfirmNo.mg_bfsFontSize = BFS_MEDIUM;

  gm_mgConfirmLabel.mg_iCenterI = -1;
  gm_mgConfirmYes.mg_boxOnScreen = BoxPopupYesSmall(fHeight);
  gm_mgConfirmNo.mg_boxOnScreen = BoxPopupNoSmall(fHeight);
}

// [Cecil] Set label and button text
void CConfirmMenu::SetText(const CTString &strLabel, const CTString &strYes, const CTString &strNo) {
  gm_mgConfirmLabel.SetText(strLabel);
  gm_mgConfirmYes.SetText(strYes == "" ? LOCALIZE("YES") : strYes);
  gm_mgConfirmNo.SetText(strNo == "" ? LOCALIZE("NO") : strNo);
};

// return TRUE if handled
BOOL CConfirmMenu::OnKeyDown(int iVKey) {
  if ((iVKey == VK_ESCAPE || iVKey == VK_RBUTTON) && gm_mgConfirmNo.mg_pActivatedFunction != NULL) {
    gm_mgConfirmNo.OnActivate();
    return TRUE;
  }
  return CGameMenu::OnKeyDown(iVKey);
}