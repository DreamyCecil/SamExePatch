/* Copyright (c) 2022 Dreamy Cecil
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

// General
INDEX sam_bBackgroundGameRender = TRUE;
INDEX sam_bAdjustForAspectRatio = TRUE;
INDEX sam_bOptionTabs = TRUE;

// FOV patch
INDEX sam_bUseVerticalFOV = TRUE;
FLOAT sam_fCustomFOV = -1.0f;
FLOAT sam_fThirdPersonFOV = -1.0f;
INDEX sam_bFixMipDistance = TRUE;
INDEX sam_bFixViewmodelFOV = TRUE;
INDEX sam_bCheckFOV = FALSE;

// Red screen on damage
INDEX sam_bRedScreenOnDamage = TRUE;

// Calculate horizontal FOV according to the aspect ratio
void AdjustHFOV(const FLOAT2D &vScreen, FLOAT &fHFOV) {
  // Get aspect ratio of the current resolution
  FLOAT fAspectRatio = vScreen(1) / vScreen(2);

  // 4:3 resolution = 1.0 ratio; 16:9 = 1.333 etc.
  FLOAT fSquareRatio = fAspectRatio * (3.0f / 4.0f);

  // Take current FOV angle and apply square ratio to it
  FLOAT fVerticalAngle = Tan(fHFOV * 0.5f) * fSquareRatio;

  // 90 FOV on 16:9 resolution will become 106.26...
  fHFOV = 2.0f * ATan(fVerticalAngle);
};

// Calculate vertical FOV from horizontal FOV according to the aspect ratio
void AdjustVFOV(const FLOAT2D &vScreen, FLOAT &fHFOV) {
  // Get inverted aspect ratio of the current resolution
  FLOAT fInverseAspectRatio = vScreen(2) / vScreen(1);

  // Take current FOV angle and apply aspect ratio to it
  FLOAT fVerticalAngle = Tan(fHFOV * 0.5f) * fInverseAspectRatio;

  // 90 FOV on 4:3 or 106.26 FOV on 16:9 will become 73.74...
  fHFOV = 2.0f * ATan(fVerticalAngle);
};

// Custom command registy
static CDynamicContainer<CShellSymbol> _cCustomSymbols;
static BOOL _bRegisterCommands = FALSE; // Don't register in the beginning

#define CUSTOM_SYMBOLS_CONFIG "Scripts\\CustomSymbols.ini"

// Register a certain command after its change
static void CECIL_RegisterCommand(void *pCommand) {
  CShellSymbol *pss = NULL;

  for (INDEX i = 0; i < _pShell->sh_assSymbols.Count(); i++) {
    CShellSymbol *pssCheck = _pShell->sh_assSymbols.Pointer(i);

    // Matching command
    if (pssCheck->ss_pvValue == pCommand) {
      pss = pssCheck;
      break;
    }
  }

  if (pss == NULL) {
    CPrintF("Could not find corresponding command in the shell for registering!\n");
    return;
  }

  // Add symbols to the list
  if (!_cCustomSymbols.IsMember(pss)) {
    _cCustomSymbols.Add(pss);
  }

  // Don't resave anything yet
  if (!_bRegisterCommands) return;

  // Special actions for commands
  if (pss->ss_pvValue == &sam_bAdjustForAspectRatio) {
    extern void ApplyVideoMode(void);
    ApplyVideoMode();
  }

  // Save symbol values
  try {
    const CTFileName fnSymbols = _fnmApplicationPath + CTFILENAME(CUSTOM_SYMBOLS_CONFIG);

    // Open file for writing
    FILE *file = fopen(fnSymbols.str_String, "wb+");

    // Couldn't create the file
    if (file == NULL) {
      throw strerror(errno);
    }

    FOREACHINDYNAMICCONTAINER(_cCustomSymbols, CShellSymbol, itss) {
      CShellSymbol *pssWrite = itss;

      // Symbol name and value
      CTString str;
      str.PrintF("%s = %s;\r\n", pssWrite->ss_strName, _pShell->GetValue(pssWrite->ss_strName));

      fwrite(str, sizeof(char), str.Length(), file);
    }

    fclose(file);

  } catch (char *strError) {
    CPrintF("Could not save custom symbols: %s\n", strError);
  }
};

// Display information about the patched executable
static void PatchInfo(void) {
  static CTString strInfo =
    "\n --- Custom Serious Sam Patch ---"
    "\ngithub.com/DreamyCecil/SamExePatch"
    "\n"
    "\n- Engine version: " _SE_VER_STRING
    "\n- EXE patch version: "
    + _pPatchAPI->strVersion
    + "\n\n(c) Dreamy Cecil, 2022\n";

  CPutString(strInfo);
};

// Custom initialization
void CECIL_Init(void) {
  // Initialize executable patch API
  _pPatchAPI = new CPatchAPI();

  {
    CPrintF("Intercepting Engine functions:\n");

    extern void CECIL_ApplyFOVPatch(void);
    extern void CECIL_ApplyScreenBlendPatch(void);
    extern void CECIL_ApplyUndecoratedPatch(void);
    extern void CECIL_ApplySoundListenPatch(void);
    extern void CECIL_ApplyMasterServerPatch(void);
    CECIL_ApplyFOVPatch();
    CECIL_ApplyScreenBlendPatch();
    CECIL_ApplyUndecoratedPatch();
    CECIL_ApplySoundListenPatch();
    CECIL_ApplyMasterServerPatch();

    CPrintF("  done!\n");
  }

  // Command registry
  _pShell->DeclareSymbol("void CECIL_RegisterCommand(INDEX);", &CECIL_RegisterCommand);

  // Information about the patched executable
  _pShell->DeclareSymbol("user void PatchInfo(void);", &PatchInfo);

  // Custom symbols
  {
    // General
    _pShell->DeclareSymbol("user INDEX sam_bBackgroundGameRender post:CECIL_RegisterCommand;", &sam_bBackgroundGameRender);
    _pShell->DeclareSymbol("user INDEX sam_bAdjustForAspectRatio post:CECIL_RegisterCommand;", &sam_bAdjustForAspectRatio);
    _pShell->DeclareSymbol("user INDEX sam_bOptionTabs           post:CECIL_RegisterCommand;", &sam_bOptionTabs);

    // FOV patch
    _pShell->DeclareSymbol("user INDEX sam_bUseVerticalFOV  post:CECIL_RegisterCommand;", &sam_bUseVerticalFOV);
    _pShell->DeclareSymbol("user FLOAT sam_fCustomFOV       post:CECIL_RegisterCommand;", &sam_fCustomFOV);
    _pShell->DeclareSymbol("user FLOAT sam_fThirdPersonFOV  post:CECIL_RegisterCommand;", &sam_fThirdPersonFOV);
    _pShell->DeclareSymbol("user INDEX sam_bFixMipDistance  post:CECIL_RegisterCommand;", &sam_bFixMipDistance);
    _pShell->DeclareSymbol("user INDEX sam_bFixViewmodelFOV post:CECIL_RegisterCommand;", &sam_bFixViewmodelFOV);
    _pShell->DeclareSymbol("user INDEX sam_bCheckFOV;", &sam_bCheckFOV);

    // Red screen on damage
    _pShell->DeclareSymbol("user INDEX sam_bRedScreenOnDamage post:CECIL_RegisterCommand;", &sam_bRedScreenOnDamage);

    // Query manager
    _pShell->DeclareSymbol("user CTString ms_strGameAgentMS  post:CECIL_RegisterCommand;", &ms_strGameAgentMS);
    _pShell->DeclareSymbol("user CTString ms_strMSLegacy     post:CECIL_RegisterCommand;", &ms_strMSLegacy);
    _pShell->DeclareSymbol("user CTString ms_strDarkPlacesMS post:CECIL_RegisterCommand;", &ms_strDarkPlacesMS);
    _pShell->DeclareSymbol("user INDEX ms_iProtocol          post:CECIL_RegisterCommand;", &ms_iProtocol);
    _pShell->DeclareSymbol("user INDEX ms_bDebugOutput       post:CECIL_RegisterCommand;", &ms_bDebugOutput);

    // Master server protocol types
    static const INDEX iMSLegacy   = E_MS_LEGACY;
    static const INDEX iDarkPlaces = E_MS_DARKPLACES;
    static const INDEX iGameAgent  = E_MS_GAMEAGENT;
    _pShell->DeclareSymbol("const INDEX MS_LEGACY;",     (void *)&iMSLegacy);
    _pShell->DeclareSymbol("const INDEX MS_DARKPLACES;", (void *)&iDarkPlaces);
    _pShell->DeclareSymbol("const INDEX MS_GAMEAGENT;",  (void *)&iGameAgent);
  }

  // Restore custom symbol values
  _pShell->Execute("include \"" CUSTOM_SYMBOLS_CONFIG "\";");

  // Ready for registering
  _bRegisterCommands = TRUE;
};
