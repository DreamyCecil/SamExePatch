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
#include "MMain.h"

void CMainMenu::Initialize_t(void) {
  // intialize main menu
  /*
  gm_mgTitle.SetText("SERIOUS SAM - BETA");  // nothing to see here, kazuya
  gm_mgTitle.mg_boxOnScreen = BoxTitle();
  AddChild(& gm_mgTitle.mg_lnNode);
  */

  gm_mgVersionLabel.SetText(sam_strVersion);
  gm_mgVersionLabel.mg_boxOnScreen = BoxVersion();
  gm_mgVersionLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgVersionLabel.mg_iCenterI = +1;
  gm_mgVersionLabel.mg_bEnabled = FALSE;
  gm_mgVersionLabel.mg_bLabel = TRUE;
  AddChild(&gm_mgVersionLabel);

  extern CTString sam_strModName;
  gm_mgModLabel.SetText(sam_strModName);
  gm_mgModLabel.mg_boxOnScreen = BoxMediumRow(-2.0f);
  gm_mgModLabel.mg_bfsFontSize = BFS_MEDIUM;
  gm_mgModLabel.mg_iCenterI = 0;
  gm_mgModLabel.mg_bEnabled = FALSE;
  gm_mgModLabel.mg_bLabel = TRUE;
  AddChild(&gm_mgModLabel);

  gm_mgSingle.SetText(TRANS("SINGLE PLAYER"));
  gm_mgSingle.mg_bfsFontSize = BFS_LARGE;
  gm_mgSingle.mg_boxOnScreen = BoxBigRow(0.0f);
  gm_mgSingle.mg_strTip = TRANS("single player game menus");
  AddChild(&gm_mgSingle);
  gm_mgSingle.mg_pmgUp = &gm_mgQuit;
  gm_mgSingle.mg_pmgDown = &gm_mgNetwork;
  gm_mgSingle.mg_pActivatedFunction = NULL;

  gm_mgNetwork.SetText(TRANS("NETWORK"));
  gm_mgNetwork.mg_bfsFontSize = BFS_LARGE;
  gm_mgNetwork.mg_boxOnScreen = BoxBigRow(1.0f);
  gm_mgNetwork.mg_strTip = TRANS("LAN/iNet multiplayer menus");
  AddChild(&gm_mgNetwork);
  gm_mgNetwork.mg_pmgUp = &gm_mgSingle;
  gm_mgNetwork.mg_pmgDown = &gm_mgSplitScreen;
  gm_mgNetwork.mg_pActivatedFunction = NULL;

  gm_mgSplitScreen.SetText(TRANS("SPLIT SCREEN"));
  gm_mgSplitScreen.mg_bfsFontSize = BFS_LARGE;
  gm_mgSplitScreen.mg_boxOnScreen = BoxBigRow(2.0f);
  gm_mgSplitScreen.mg_strTip = TRANS("play with multiple players on one computer");
  AddChild(&gm_mgSplitScreen);
  gm_mgSplitScreen.mg_pmgUp = &gm_mgNetwork;
  gm_mgSplitScreen.mg_pmgDown = &gm_mgDemo;
  gm_mgSplitScreen.mg_pActivatedFunction = NULL;

  gm_mgDemo.SetText(TRANS("DEMO"));
  gm_mgDemo.mg_bfsFontSize = BFS_LARGE;
  gm_mgDemo.mg_boxOnScreen = BoxBigRow(3.0f);
  gm_mgDemo.mg_strTip = TRANS("play a game demo");
  AddChild(&gm_mgDemo);
  gm_mgDemo.mg_pmgUp = &gm_mgSplitScreen;
  gm_mgDemo.mg_pmgDown = &gm_mgMods;
  gm_mgDemo.mg_pActivatedFunction = NULL;

  gm_mgMods.SetText(TRANS("MODS"));
  gm_mgMods.mg_bfsFontSize = BFS_LARGE;
  gm_mgMods.mg_boxOnScreen = BoxBigRow(4.0f);
  gm_mgMods.mg_strTip = TRANS("run one of installed game modifications");
  AddChild(&gm_mgMods);
  gm_mgMods.mg_pmgUp = &gm_mgDemo;
  gm_mgMods.mg_pmgDown = &gm_mgHighScore;
  gm_mgMods.mg_pActivatedFunction = NULL;

  gm_mgHighScore.SetText(TRANS("HIGH SCORES"));
  gm_mgHighScore.mg_bfsFontSize = BFS_LARGE;
  gm_mgHighScore.mg_boxOnScreen = BoxBigRow(5.0f);
  gm_mgHighScore.mg_strTip = TRANS("view list of top ten best scores");
  AddChild(&gm_mgHighScore);
  gm_mgHighScore.mg_pmgUp = &gm_mgMods;
  gm_mgHighScore.mg_pmgDown = &gm_mgOptions;
  gm_mgHighScore.mg_pActivatedFunction = NULL;

  gm_mgOptions.SetText(TRANS("OPTIONS"));
  gm_mgOptions.mg_bfsFontSize = BFS_LARGE;
  gm_mgOptions.mg_boxOnScreen = BoxBigRow(6.0f);
  gm_mgOptions.mg_strTip = TRANS("adjust video, audio and input options");
  AddChild(&gm_mgOptions);
  gm_mgOptions.mg_pmgUp = &gm_mgHighScore;
  gm_mgOptions.mg_pmgDown = &gm_mgQuit;
  gm_mgOptions.mg_pActivatedFunction = NULL;

  gm_mgQuit.SetText(TRANS("QUIT"));
  gm_mgQuit.mg_bfsFontSize = BFS_LARGE;
  gm_mgQuit.mg_boxOnScreen = BoxBigRow(7.0f);
  gm_mgQuit.mg_strTip = TRANS("exit game immediately");
  AddChild(&gm_mgQuit);
  gm_mgQuit.mg_pmgUp = &gm_mgOptions;
  gm_mgQuit.mg_pmgDown = &gm_mgSingle;
  gm_mgQuit.mg_pActivatedFunction = NULL;
}

void CMainMenu::StartMenu(void) {
  gm_mgSingle.mg_bEnabled = IsMenuEnabled("Single Player");
  gm_mgNetwork.mg_bEnabled = IsMenuEnabled("Network");
  gm_mgSplitScreen.mg_bEnabled = IsMenuEnabled("Split Screen");
  gm_mgHighScore.mg_bEnabled = IsMenuEnabled("High Score");
  CGameMenu::StartMenu();
}