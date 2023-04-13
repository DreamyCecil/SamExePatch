/* Copyright (c) 2022-2023 Dreamy Cecil
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

#include "LevelInfo.h"

// General
INDEX sam_bPatchVersionLabel = TRUE;
INDEX sam_bBackgroundGameRender = TRUE;
INDEX sam_bOptionTabs = TRUE;
INDEX sam_bDecoratedServerNames = TRUE;

INDEX sam_bNotifyAboutUpdates = TRUE;
INDEX sam_iUpdateReminder = 0;

// Custom initialization
void CECIL_Init(void) {
  // Initialize the core
  CECIL_InitCore();

#if CLASSICSPATCH_ENGINEPATCHES

  // Function patches
  CPutString("--- Sam: Intercepting Engine functions ---\n");
  _EnginePatches.CorePatches();
  CPutString("--- Done! ---\n");

#endif // CLASSICSPATCH_ENGINEPATCHES

  // Custom symbols
  _pShell->DeclareSymbol("persistent user INDEX sam_bPatchVersionLabel;",    &sam_bPatchVersionLabel);
  _pShell->DeclareSymbol("persistent user INDEX sam_bBackgroundGameRender;", &sam_bBackgroundGameRender);
  _pShell->DeclareSymbol("persistent user INDEX sam_bOptionTabs;",           &sam_bOptionTabs);
  _pShell->DeclareSymbol("persistent user INDEX sam_bDecoratedServerNames;", &sam_bDecoratedServerNames);

  _pShell->DeclareSymbol("persistent user INDEX sam_bNotifyAboutUpdates;", &sam_bNotifyAboutUpdates);
  _pShell->DeclareSymbol("persistent      INDEX sam_iUpdateReminder;",     &sam_iUpdateReminder);

  _pShell->DeclareSymbol("user void ListLevels(CTString);", &ListLevels);
};
