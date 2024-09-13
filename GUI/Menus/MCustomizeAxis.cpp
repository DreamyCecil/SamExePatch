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
#include "MenuStuff.h"
#include "MCustomizeAxis.h"

void PreChangeAxis(INDEX iDummy) {
  _pGUIM->gmCustomizeAxisMenu.ApplyActionSettings();
};

void PostChangeAxis(INDEX iDummy) {
  _pGUIM->gmCustomizeAxisMenu.ObtainActionSettings();
};

void CCustomizeAxisMenu::Initialize_t(void) {
  gm_strName = "CustomizeAxis";
  gm_pmgSelectedByDefault = &gm_mgActionTrigger;

  // intialize axis menu
  gm_mgTitle.SetName(LOCALIZE("CUSTOMIZE AXIS"));
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(&gm_mgTitle);

  TRIGGER_MG(gm_mgActionTrigger, 0, gm_mgSmoothTrigger, gm_mgMountedTrigger, LOCALIZE("ACTION"), astrNoYes);
  gm_mgActionTrigger.mg_strTip = LOCALIZE("choose action to customize");

  TRIGGER_MG(gm_mgMountedTrigger, 2, gm_mgActionTrigger, gm_mgSensitivity, LOCALIZE("MOUNTED TO"), astrNoYes);
  gm_mgMountedTrigger.mg_strTip = LOCALIZE("choose controller axis that will perform the action");

  gm_mgActionTrigger.mg_astrTexts = new CTString[AXIS_ACTIONS_CT];
  gm_mgActionTrigger.mg_ctTexts = AXIS_ACTIONS_CT;

  gm_mgActionTrigger.mg_pPreTriggerChange = &PreChangeAxis;
  gm_mgActionTrigger.mg_pOnTriggerChange = &PostChangeAxis;

  // for all available axis type controlers
  for (INDEX iControler = 0; iControler < AXIS_ACTIONS_CT; iControler++) {
    gm_mgActionTrigger.mg_astrTexts[iControler] = TRANSV(GetGameAPI()->GetAxisName(iControler));
  }
  gm_mgActionTrigger.mg_iSelected = 3;

  INDEX ctAxis = _pInput->GetAvailableAxisCount();
  gm_mgMountedTrigger.mg_astrTexts = new CTString[ctAxis];
  gm_mgMountedTrigger.mg_ctTexts = ctAxis;
  // for all axis actions that can be mounted
  for (INDEX iAxis = 0; iAxis < ctAxis; iAxis++) {
    gm_mgMountedTrigger.mg_astrTexts[iAxis] = _pInput->GetAxisTransName(iAxis);
  }

  gm_mgSensitivity.mg_boxOnScreen = BoxMediumRow(3);
  gm_mgSensitivity.SetText(LOCALIZE("SENSITIVITY"));
  gm_mgSensitivity.mg_pmgUp = &gm_mgMountedTrigger;
  gm_mgSensitivity.mg_pmgDown = &gm_mgDeadzone;
  AddChild(&gm_mgSensitivity);
  gm_mgSensitivity.mg_strTip = LOCALIZE("set sensitivity for this axis");

  gm_mgDeadzone.mg_boxOnScreen = BoxMediumRow(4);
  gm_mgDeadzone.SetText(LOCALIZE("DEAD ZONE"));
  gm_mgDeadzone.mg_pmgUp = &gm_mgSensitivity;
  gm_mgDeadzone.mg_pmgDown = &gm_mgInvertTrigger;
  AddChild(&gm_mgDeadzone);
  gm_mgDeadzone.mg_strTip = LOCALIZE("set dead zone for this axis");

  TRIGGER_MG(gm_mgInvertTrigger, 5, gm_mgDeadzone, gm_mgRelativeTrigger, LOCALIZE("INVERTED"), astrNoYes);
  gm_mgInvertTrigger.mg_strTip = LOCALIZE("choose whether to invert this axis or not");
  TRIGGER_MG(gm_mgRelativeTrigger, 6, gm_mgInvertTrigger, gm_mgSmoothTrigger, LOCALIZE("RELATIVE"), astrNoYes);
  gm_mgRelativeTrigger.mg_strTip = LOCALIZE("select relative or absolute axis reading");
  TRIGGER_MG(gm_mgSmoothTrigger, 7, gm_mgRelativeTrigger, gm_mgActionTrigger, LOCALIZE("SMOOTH"), astrNoYes);
  gm_mgSmoothTrigger.mg_strTip = LOCALIZE("turn this on to filter readings on this axis");
}

CCustomizeAxisMenu::~CCustomizeAxisMenu(void) {
  delete[] gm_mgActionTrigger.mg_astrTexts;
  delete[] gm_mgMountedTrigger.mg_astrTexts;
}

void CCustomizeAxisMenu::ObtainActionSettings(void) {
  ControlsMenuOn();
  CControls &ctrls = *GetGameAPI()->GetControls();
  INDEX iSelectedAction = gm_mgActionTrigger.mg_iSelected;
  INDEX iMountedAxis = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_iAxisAction;

  gm_mgMountedTrigger.mg_iSelected = iMountedAxis;

  gm_mgSensitivity.mg_iMinPos = 0;
  gm_mgSensitivity.mg_iMaxPos = 50;
  gm_mgSensitivity.mg_iCurPos = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity / 2;
  gm_mgSensitivity.ApplyCurrentPosition();

  gm_mgDeadzone.mg_iMinPos = 0;
  gm_mgDeadzone.mg_iMaxPos = 50;
  gm_mgDeadzone.mg_iCurPos = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone / 2;
  gm_mgDeadzone.ApplyCurrentPosition();

  gm_mgInvertTrigger.mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bInvert ? 1 : 0;
  gm_mgRelativeTrigger.mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bRelativeControler ? 1 : 0;
  gm_mgSmoothTrigger.mg_iSelected = ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bSmooth ? 1 : 0;

  gm_mgActionTrigger.ApplyCurrentSelection();
  gm_mgMountedTrigger.ApplyCurrentSelection();
  gm_mgInvertTrigger.ApplyCurrentSelection();
  gm_mgRelativeTrigger.ApplyCurrentSelection();
  gm_mgSmoothTrigger.ApplyCurrentSelection();
}

void CCustomizeAxisMenu::ApplyActionSettings(void) {
  CControls &ctrls = *GetGameAPI()->GetControls();
  INDEX iSelectedAction = gm_mgActionTrigger.mg_iSelected;
  INDEX iMountedAxis = gm_mgMountedTrigger.mg_iSelected;
  FLOAT fSensitivity =
    FLOAT(gm_mgSensitivity.mg_iCurPos - gm_mgSensitivity.mg_iMinPos) /
    FLOAT(gm_mgSensitivity.mg_iMaxPos - gm_mgSensitivity.mg_iMinPos) * 100.0f;
  FLOAT fDeadZone =
    FLOAT(gm_mgDeadzone.mg_iCurPos - gm_mgDeadzone.mg_iMinPos) /
    FLOAT(gm_mgDeadzone.mg_iMaxPos - gm_mgDeadzone.mg_iMinPos) * 100.0f;

  BOOL bInvert = gm_mgInvertTrigger.mg_iSelected != 0;
  BOOL bRelative = gm_mgRelativeTrigger.mg_iSelected != 0;
  BOOL bSmooth = gm_mgSmoothTrigger.mg_iSelected != 0;

  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_iAxisAction = iMountedAxis;
  if (INDEX(ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity) != INDEX(fSensitivity)) {
    ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fSensitivity = fSensitivity;
  }
  if (INDEX(ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone) != INDEX(fDeadZone)) {
    ctrls.ctrl_aaAxisActions[iSelectedAction].aa_fDeadZone = fDeadZone;
  }
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bInvert = bInvert;
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bRelativeControler = bRelative;
  ctrls.ctrl_aaAxisActions[iSelectedAction].aa_bSmooth = bSmooth;
  ctrls.CalculateInfluencesForAllAxis();

  ControlsMenuOff();
}

void CCustomizeAxisMenu::StartMenu(void) {
  ObtainActionSettings();

  CGameMenu::StartMenu();
}

void CCustomizeAxisMenu::EndMenu(void) {
  ApplyActionSettings();
  CGameMenu::EndMenu();
}

// [Cecil] Change to the menu
void CCustomizeAxisMenu::ChangeTo(void) {
  ChangeToMenu(&_pGUIM->gmCustomizeAxisMenu);
};
