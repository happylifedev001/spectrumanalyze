#include <gtk/gtk.h>

#include "global.h"
#include "utils.h"

// Update the reassign spectrogram window
void updateReassignSpectrogramWindow(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the reassign spectrogram
void drawReassignSpectrogram(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the reassign spectrogram window grid
void reassignSpectrogramDrawGrid(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the mouse position information for reasign spectrogram window
void reassignSpectrogramDrawMouseInformation(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);