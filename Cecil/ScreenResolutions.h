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

// Aspect ratio resolutions
typedef CStaticStackArray<PIX2D> CAspectRatio;

extern CAspectRatio _ar4_3;   // Square-ish resolutions
extern CAspectRatio _ar16_9;  // Standard widescreen resolutions
extern CAspectRatio _ar16_10; // Extra widescreen resolutions
extern CAspectRatio _ar21_9;  // Very wide resolutions

// Prepare arrays with window resolutions
void PrepareVideoResolutions(void);

// Count all resolutions from all aspect ratio arrays
INDEX CountAllResolutions(void);

// All aspect ratios
#define CT_ASPECTRATIOS 4

// Aspect ratio names
static const CTString _astrAspectRatios[CT_ASPECTRATIOS] = {
  "4:3",
  "16:9",
  "16:10",
  "21:9",
};

// Aspect ratio arrays
static const CAspectRatio *_aAspectRatios[CT_ASPECTRATIOS] = {
  &_ar4_3,
  &_ar16_9,
  &_ar16_10,
  &_ar21_9,
};
