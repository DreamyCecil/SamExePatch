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

// [Cecil] Tabs of options
CStaticStackArray<CVarTab> _aTabs;

CTString _strFile;
INDEX _ctLines;

CTString GetNonEmptyLine_t(CTStream &strm) {
  FOREVER {
    if (strm.AtEOF()) {
      ThrowF_t(TRANS("Unexpected end of file"));
    }

    CTString str;
    _ctLines++;

    strm.GetLine_t(str);
    str.TrimSpacesLeft();

    if (str.RemovePrefix("//")) { // skip comments
      continue;
    }

    if (str != "") {
      str.TrimSpacesRight();
      return str;
    }
  }
}

void TranslateLine(CTString &str) {
  str.TrimSpacesLeft();

  if (str.RemovePrefix("TTRS")) {
    str.TrimSpacesLeft();
    str = TranslateConst(str, 0);
  }

  str.TrimSpacesLeft();
}

void FixupFileName_t(CTString &strFnm) {
  strFnm.TrimSpacesLeft();
  strFnm.TrimSpacesRight();

  if (!strFnm.RemovePrefix(CTString("TF") + "NM ")) { // must not directly have ids in code
    ThrowF_t(TRANS("Expected %s%s before filename"), "TF", "NM");
  }
}

void CheckPVS_t(CVarSetting *pvs) {
  if (pvs == NULL) {
    ThrowF_t("Gadget expected");
  }
}

void ParseCFG_t(CTStream &strm) {
  CVarSetting *pvs = NULL;

  // [Cecil] All options tab
  CVarTab &tabAll = _aTabs.Push();
  tabAll.strName = TRANS("All options");

  CListHead &lhAll = tabAll.lhVars;

  // repeat
  FOREVER {
    // read one line
    CTString strLine = GetNonEmptyLine_t(strm);

    if (strLine.RemovePrefix("MenuEnd")) {
      break;

    } else if (strLine.RemovePrefix("Gadget:")) {
      pvs = new CVarSetting;
      lhAll.AddTail(pvs->vs_lnNode);

      TranslateLine(strLine);
      strLine.TrimSpacesLeft();
      pvs->vs_strName = strLine;

    } else if (strLine.RemovePrefix("Type:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      if (strLine == "Toggle") {
        pvs->vs_bSeparator = FALSE;
      } else if (strLine == "Separator") {
        pvs->vs_bSeparator = TRUE;
      }

    } else if (strLine.RemovePrefix("Schedule:")) {
      CheckPVS_t(pvs);
      FixupFileName_t(strLine);
      pvs->vs_strSchedule = strLine;

    } else if (strLine.RemovePrefix("Tip:")) {
      CheckPVS_t(pvs);
      TranslateLine(strLine);

      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_strTip = strLine;

    } else if (strLine.RemovePrefix("Var:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      // [Cecil] Replace old wide screen command
      if (strLine == "sam_bWideScreen") {
        pvs->vs_strVar = "sam_bAdjustForAspectRatio";
      } else {
        pvs->vs_strVar = strLine;
      }

    } else if (strLine.RemovePrefix("Filter:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_strFilter = strLine;

    } else if (strLine.RemovePrefix("Slider:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      if (strLine == "Fill") {
        pvs->vs_iSlider = 1;
      } else if (strLine == "Ratio") {
        pvs->vs_iSlider = 2;
      } else {
        pvs->vs_iSlider = 0;
      }

    } else if (strLine.RemovePrefix("InGame:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();

      if (strLine == "No") {
        pvs->vs_bCanChangeInGame = FALSE;
      } else {
        ASSERT(strLine == "Yes");
        pvs->vs_bCanChangeInGame = TRUE;
      }

    } else if (strLine.RemovePrefix("String:")) {
      CheckPVS_t(pvs);
      TranslateLine(strLine);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_astrTexts.Push() = strLine;

    } else if (strLine.RemovePrefix("Value:")) {
      CheckPVS_t(pvs);
      strLine.TrimSpacesLeft();
      strLine.TrimSpacesRight();
      pvs->vs_astrValues.Push() = strLine;

    } else {
      ThrowF_t(TRANS("unknown keyword"));
    }
  }

  // [Cecil] Current tab
  CVarTab *pCur = NULL;

  // [Cecil] Go through each setting
  FOREACHINLIST(CVarSetting, vs_lnNode, lhAll, itvs) {
    CVarSetting &vs = *itvs;

    // If it's a separator with a name
    if (vs.vs_bSeparator && vs.vs_strName != "") {
      // Start a new tab
      pCur = &_aTabs.Push();
      pCur->strName = vs.vs_strName;
    }

    // Copy setting into the new tab
    if (pCur != NULL) {
      CVarSetting *pvsCopy = new CVarSetting(vs);
      pCur->lhVars.AddTail(pvsCopy->vs_lnNode);
    }
  }
}

void LoadVarSettings(const CTFileName &fnmCfg) {
  FlushVarSettings(FALSE);

  try {
    CTFileStream strm;
    strm.Open_t(fnmCfg);
    _ctLines = 0;
    _strFile = fnmCfg;
    ParseCFG_t(strm);

  } catch (char *strError) {
    CPrintF("%s (%d) : %s\n", (const char *)_strFile, _ctLines, strError);
  }

  // [Cecil] For each tab
  for (INDEX iTab = 0; iTab < _aTabs.Count(); iTab++)
  {
    FOREACHINLIST(CVarSetting, vs_lnNode, _aTabs[iTab].lhVars, itvs) {
      CVarSetting &vs = *itvs;

      if (!vs.Validate() || vs.vs_bSeparator) {
        continue;
      }

      INDEX ctValues = vs.vs_ctValues;
      CTString strValue = _pShell->GetValue(vs.vs_strVar);

      vs.vs_bCustom = TRUE;
      vs.vs_iOrgValue = vs.vs_iValue = -1;

      for (INDEX iValue = 0; iValue < ctValues; iValue++) {
        if (strValue == vs.vs_astrValues[iValue]) {
          vs.vs_iOrgValue = vs.vs_iValue = iValue;
          vs.vs_bCustom = FALSE;
          break;
        }
      }
    }
  }
}

void FlushVarSettings(BOOL bApply) {
  CStaticStackArray<CTString> astrScheduled;

  // [Cecil] For each tab
  for (INDEX iTab = 0; iTab < _aTabs.Count(); iTab++)
  {
    if (bApply) {
      FOREACHINLIST(CVarSetting, vs_lnNode, _aTabs[iTab].lhVars, itvs) {
        CVarSetting &vs = *itvs;

        if (vs.vs_iValue != vs.vs_iOrgValue) {
          CTString strCmd;
          _pShell->SetValue(vs.vs_strVar, vs.vs_astrValues[vs.vs_iValue]);

          if (vs.vs_strSchedule != "") {
            BOOL bSheduled = FALSE;

            for (INDEX i = 0; i < astrScheduled.Count(); i++) {
              if (astrScheduled[i] == vs.vs_strSchedule) {
                bSheduled = TRUE;
                break;
              }
            }

            if (!bSheduled) {
              astrScheduled.Push() = vs.vs_strSchedule;
            }
          }
        }
      }
    }

    {FORDELETELIST(CVarSetting, vs_lnNode, _aTabs[iTab].lhVars, itvs) {
      delete &*itvs;
    }}
  }

  // [Cecil] Clear tabs
  _aTabs.Clear();

  for (INDEX i = 0; i < astrScheduled.Count(); i++) {
    CTString strCmd;
    strCmd.PrintF("include \"%s\"", astrScheduled[i]);
    _pShell->Execute(strCmd);
  }
}

CVarSetting::CVarSetting() {
  Clear();
}

void CVarSetting::Clear() {
  vs_iOrgValue = 0;
  vs_iValue = 0;
  vs_ctValues = 0;
  vs_bSeparator = FALSE;
  vs_bCanChangeInGame = TRUE;
  vs_iSlider = 0;
  vs_strName.Clear();
  vs_strTip.Clear();
  vs_strVar.Clear();
  vs_strFilter.Clear();
  vs_strSchedule.Clear();
  vs_bCustom = FALSE;
}

BOOL CVarSetting::Validate(void) {
  if (vs_bSeparator) {
    return TRUE;
  }

  vs_ctValues = Min(vs_astrValues.Count(), vs_astrTexts.Count());

  if (vs_ctValues <= 0) {
    ASSERT(FALSE);
    return FALSE;
  }

  if (!vs_bCustom) {
    vs_iValue = Clamp(vs_iValue, 0L, vs_ctValues - 1L);
  }

  return TRUE;
}

// [Cecil] Copy constructor
CVarSetting::CVarSetting(const CVarSetting &vsOther) {
  if (&vsOther == this) return;

  vs_bSeparator       = vsOther.vs_bSeparator;
  vs_bCanChangeInGame = vsOther.vs_bCanChangeInGame;
  vs_iSlider          = vsOther.vs_iSlider;
  vs_strName          = vsOther.vs_strName;
  vs_strTip           = vsOther.vs_strTip;
  vs_strVar           = vsOther.vs_strVar;
  vs_strFilter        = vsOther.vs_strFilter;
  vs_strSchedule      = vsOther.vs_strSchedule;
  vs_iValue           = vsOther.vs_iValue;
  vs_ctValues         = vsOther.vs_ctValues;
  vs_iOrgValue        = vsOther.vs_iOrgValue;
  vs_bCustom          = vsOther.vs_bCustom;

  INDEX i;
  INDEX ct;
  CTString *pstr;

  // Copy texts
  ct = vsOther.vs_astrTexts.Count();
  pstr = vs_astrTexts.Push(ct);

  for (i = 0; i < ct; i++) {
    pstr[i] = vsOther.vs_astrTexts[i];
  }
  
  // Copy values
  ct = vsOther.vs_astrValues.Count();
  pstr = vs_astrValues.Push(ct);
  
  for (i = 0; i < ct; i++) {
    pstr[i] = vsOther.vs_astrValues[i];
  }
};
