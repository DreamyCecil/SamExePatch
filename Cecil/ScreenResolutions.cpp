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

#include "Cecil/ScreenResolutions.h"
#include "Cecil/WindowModes.h"

// Initialize aspect ratio arrays
CAspectRatio _ar4_3;
CAspectRatio _ar16_9;
CAspectRatio _ar16_10;
CAspectRatio _ar21_9;

// Prepare arrays with window resolutions
void PrepareVideoResolutions(void) {
  static BOOL bPrepared = FALSE;

  // Already prepared
  if (bPrepared) return;

  // Square-ish resolutions
  _ar4_3.Push() = PIX2D(320,   240);
  _ar4_3.Push() = PIX2D(400,   300);
  _ar4_3.Push() = PIX2D(512,   384);
  _ar4_3.Push() = PIX2D(640,   240); // Dual
  _ar4_3.Push() = PIX2D(640,   480);
  _ar4_3.Push() = PIX2D(720,   540);
  _ar4_3.Push() = PIX2D(800,   300); // Dual
  _ar4_3.Push() = PIX2D(800,   600);
  _ar4_3.Push() = PIX2D(960,   720);
  _ar4_3.Push() = PIX2D(1024,  384); // Dual
  _ar4_3.Push() = PIX2D(1024,  768);
  _ar4_3.Push() = PIX2D(1152,  864);
  _ar4_3.Push() = PIX2D(1280,  480); // Dual
  _ar4_3.Push() = PIX2D(1280,  960);
  _ar4_3.Push() = PIX2D(1280, 1024); // 5:4
  _ar4_3.Push() = PIX2D(1440, 1080);
  _ar4_3.Push() = PIX2D(1600,  600); // Dual
  _ar4_3.Push() = PIX2D(1600, 1200);
  _ar4_3.Push() = PIX2D(1920,  720); // Dual
  _ar4_3.Push() = PIX2D(1920, 1440);
  _ar4_3.Push() = PIX2D(1920, 1920); // 1:1
  _ar4_3.Push() = PIX2D(2048,  786);
  _ar4_3.Push() = PIX2D(2048, 1536);
  _ar4_3.Push() = PIX2D(2560, 2048); // 5:4

  // Standard widescreen resolutions
  _ar16_9.Push() = PIX2D(640,   360);
  _ar16_9.Push() = PIX2D(854,   480);
  _ar16_9.Push() = PIX2D(960,   540);
  _ar16_9.Push() = PIX2D(1024,  576);
  _ar16_9.Push() = PIX2D(1280,  720);
  _ar16_9.Push() = PIX2D(1366,  768);
  _ar16_9.Push() = PIX2D(1600,  900);
  _ar16_9.Push() = PIX2D(1920, 1080);
  _ar16_9.Push() = PIX2D(2560, 1440);
  _ar16_9.Push() = PIX2D(3200, 1800);
  _ar16_9.Push() = PIX2D(3840, 2160);
  _ar16_9.Push() = PIX2D(5120, 2880);
  _ar16_9.Push() = PIX2D(7680, 4320);

  // Extra widescreen resolutions
  _ar16_10.Push() = PIX2D(480,   320); // 3:2
  _ar16_10.Push() = PIX2D(1152,  768); // 3:2
  _ar16_10.Push() = PIX2D(1280,  800);
  _ar16_10.Push() = PIX2D(1280,  854); // 3:2
  _ar16_10.Push() = PIX2D(1440,  900);
  _ar16_10.Push() = PIX2D(1440,  960); // 3:2
  _ar16_10.Push() = PIX2D(1600, 1000); // Extra
  _ar16_10.Push() = PIX2D(1680, 1050);
  _ar16_10.Push() = PIX2D(1920, 1200);
  _ar16_10.Push() = PIX2D(2160, 1440); // 3:2
  _ar16_10.Push() = PIX2D(2560, 1600);
  _ar16_10.Push() = PIX2D(2560, 1700); // 3:2
  _ar16_10.Push() = PIX2D(3840, 2400);

  // Very wide resolutions
  _ar21_9.Push() = PIX2D(1400,  600); // Extra
  _ar21_9.Push() = PIX2D(1680,  720); // Extra
  _ar21_9.Push() = PIX2D(2100,  900); // Extra
  _ar21_9.Push() = PIX2D(2560, 1080);
  _ar21_9.Push() = PIX2D(3440, 1440);
  _ar21_9.Push() = PIX2D(5120, 2160);

  // Set translated window mode names
  _astrWindowModes[0] = TRANS("Window");
  _astrWindowModes[1] = TRANS("Borderless");
  _astrWindowModes[2] = TRANS("Fullscreen");

  bPrepared = TRUE;
};

// Count all resolutions from all aspect ratio arrays
INDEX CountAllResolutions(void) {
  INDEX ctRes = 0;

  for (INDEX i = 0; i < CT_ASPECTRATIOS; i++) {
    ctRes += _aAspectRatios[i]->Count();
  }

  return ctRes;
};

// Find aspect ratio for this size
void SizeToAspectRatio(const PIX2D &vpixSize, INDEX &iAspectRatio) {
  for (iAspectRatio = 0; iAspectRatio < CT_ASPECTRATIOS; iAspectRatio++) {
    const INDEX ctRes = _aAspectRatios[iAspectRatio]->Count();

    for (INDEX iRes = 0; iRes < ctRes; iRes++) {
      const PIX2D &vpixRes = (*_aAspectRatios[iAspectRatio])[iRes];

      // Matching resolution
      if (vpixRes == vpixSize) {
        return;
      }
    }
  }

  // Couldn't determine the aspect ratio
  ASSERTALWAYS("Window size does not exist under any aspect ratio!");
  iAspectRatio = 0;
};
