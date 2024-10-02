#pragma once

#include <gtk/gtk.h>

#include "global.h"
#include "utils.h"

// Update the wave form window
void updateWaveFormWindow(GtkWidget* widget, cairo_t* cr, SWaveData* waveData, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the wave form curve
void drawWaveformCurve(SWndInfo* wndInfo, XSPECTRO* spectro);
// Draw the waveform cursor info
void drawWaveformCursorInfo(SWndInfo* wndInfo, XSPECTRO* spectro);
// Draw the waveform focus info
void drawWaveformFocusInfo(SWndInfo* wndInfo, XSPECTRO* spectro);


