#pragma once

#include <gtk/gtk.h>

#include "global.h"
#include "utils.h"

// Update the wave form subwindow
void updateWaveFormSubWindow(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the wave form subwindow curve
void drawWaveFormSubWindowCurve(SWndInfo* wndInfo, XSPECTRO* spectro);
// Draw the waveform subwindow cursor info
void drawWaveFormSubWindowCursorInfo(SWndInfo* wndInfo, XSPECTRO* spectro);
// Draw the waveform subwindow focus info
void drawWaveFormSubWindowFocusInfo(SWndInfo* wndInfo, XSPECTRO* spectro);


