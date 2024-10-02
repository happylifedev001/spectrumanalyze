#ifndef __SPECTROGRAM_PARAMETERS_WINDOW__
#define __SPECTROGRAM_PARAMETERS_WINDOW__

#include <gtk/gtk.h>

#include "utils.h"

// Crate the spectrogram parameters window
void createSpectrogramParametersWindow(GtkWidget* window);
// Update the spectrogram parameters window
void updateSpectrogramParametersWindow();
// Close the window
void onSpectrogramParamtersWndClose(GtkWidget* widget, gpointer data);
// Apply button event process
void onSpectrogramParamtersWndBtnApply(GtkWidget* widget, gpointer data);
// All Apply button event process
void onSpectrogramParamtersWndBtnAllApply(GtkWidget* widget, gpointer data);
// Default button event process
void onSpectrogramParamtersWndBtnDefault(GtkWidget* widget, gpointer data);

#endif