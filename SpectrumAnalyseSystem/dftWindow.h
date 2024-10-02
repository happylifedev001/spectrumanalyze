#pragma once

#include <gtk/gtk.h>

#include "global.h"
#include "utils.h"

// Update the DFT window
void updateDFTWindow(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos * mousePos);
// Draw the dft
void drawDft(SWndInfo* wndInfo, XSPECTRO* spectro, bool drawSavedData);
// Draw the smoothed dft
void drawSmoothedDft(SWndInfo* wndInfo, XSPECTRO* spectro, bool drawSavedData);
// Draw the cursor info
void drawCursorInfo(SWndInfo* wndInfo, XSPECTRO* spectro);
// Draw the formant
void drawFormant(SWndInfo * wndInfo, XSPECTRO* spectro);
