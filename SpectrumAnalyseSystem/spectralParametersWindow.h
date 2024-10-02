#ifndef __SPECTRAL_PARAMETERS_WINDOW__
#define __SPECTRAL_PARAMETERS_WINDOW__

#include <gtk/gtk.h>

#include "utils.h"

// Crate the spectral parameters window
void createSpectralParametersWindow(GtkWidget* window);
// Update the spectral parameters window
void updateSpectralParametersWindow();
// Close the window
void onClose(GtkWidget* widget, gpointer data);
// Apply button event process
void onBtnApply(GtkWidget* widget, gpointer data);
// All Apply button event process
void onBtnAllApply(GtkWidget* widget, gpointer data);
// Default button event process
void onBtnDefault(GtkWidget* widget, gpointer data);

#endif