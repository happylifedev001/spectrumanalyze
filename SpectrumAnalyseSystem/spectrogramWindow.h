#include <gtk/gtk.h>

#include "global.h"
#include "utils.h"

// Update the spectrogram window
void updateSpectrogramWindow(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the spectrogram
void drawSpectrogram(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the grid
void drawGrid(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);
// Draw the mouse position information
void drawMouseInformation(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos);