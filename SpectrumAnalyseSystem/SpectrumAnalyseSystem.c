#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <glib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <sndfile.h>
//#include <gst/gst.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "global.h"
#include "draw.h"
#include "utils.h"
#include "constants.h"

#include "waveFormWindow.h"
#include "dftWindow.h"
#include "waveFormSubWindow.h"
#include "spectralParametersWindow.h"
#include "spectrogramWindow.h"
#include "reassignSpectrogramWindow.h"
#include "spectrogramParametersWindow.h"

/*
 * see makefbank
 */

float cbskrt[SIZCBSKIRT] = {
1.00f,  .998f,  .995f,  .984f,  .972f,  .952f,  .931f,  .907f,  .883f,  .849f,
.814f,  .775f,  .735f,  .699f,  .662f,  .618f,  .573f,  .529f,  .484f,  .447f,
.410f,  .370f,  .329f,  .292f,  .255f,  .238f,  .221f,  .206f,  .191f,  .179f,
.167f,  .157f,  .146f,  .137f,  .128f,  .120f,  .112f,  .106f,  .099f,  .093f,
.087f,  .082f,  .077f,  .073f,  .069f,  .065f,  .061f,  .058f,  .055f,  .052f,
.049f,  .047f,  .044f,  .042f,  .040f,  .038f,  .036f,  .035f,  .033f,  .032f,
.030f,  .029f,  .027f,  .026f,  .025f,  .024f,  .023f,  .022f,  .021f,  .021f,
.020f,  .019f,  .018f,  .018f,  .017f,  .017f,  .016f,  .016f,  .015f,  .015f,
.014f,  .014f,  .013f,  .013f,  .012f,  .012f,  .0115f, .0113f, .011f,  .0105f,
.010f,  .0098f, .0095f, .0093f, .009f,  .0088f, .0085f, .0083f, .008f,  .0077f,
};

const float g_cWaveDataCoef = 9997.56f / 32767.0f;

GtkWidget* g_mainWindow;

#define SAMPLE_RATE 10000
#define BITS_PER_SAMPLE 16
#define NUM_CHANNELS 1

static gchar* g_waveFileName;
static SNDFILE* g_waveSndFile;

SWaveData g_waveData;
extern XSPECTRO g_spectro;

static GtkWidget* window = NULL;

static bool g_bWaveFormWindowUpdate = false;

static GtkWidget* g_waveFormWindow = NULL;
SMousePos g_waveFormWindowMousePos = { 0, 0 };

static GtkWidget* g_waveFormSubWindow = NULL;
SMousePos g_waveFormSubWindowMousePos = { 0, 0 };

static GtkWidget* g_DFTWindow = NULL;
static bool g_dftWindowWorking = false;
static SMousePos g_dftWindowMousePos = { 100, 100 };

// Spectro aval dialog variables
GtkLabel* g_spectroAvalDlgLabel = NULL;
GtkEntry* g_spectroAvalDlgEntry = NULL;
static int g_spectroAvalDlgCount = 0;

// Spectro avg dialog variables
GtkLabel* g_spectroAvgDlgLabel = NULL;
GtkEntry* g_spectroAvgDlgEntry = NULL;
static int g_spectroAvgDlgCount = 0;

// Spectro specms dialog variable
GtkEntry* g_spectroSpecmsDlgEntry = NULL;

// Spectro avg dialog response identifier
enum {
    RESPONSE_ID_SPECTRO_AVG_APPLY = 1
};

// Spectral Parameters Settings Widget
static GtkWidget* g_spectralParametersWindow = NULL;
extern SSpectralParametersWndControls g_spectralParametersWndControls;

// Spectrogram window variables
static GtkWidget* g_spectrogramWindow = NULL;
SMousePos g_spectrogramWindowMousePos = { 0, 0 };

// Reasign Spectrogram window variables
static GtkWidget* g_reassignSpectrogramWindow = NULL;
SMousePos g_reassignSpectrogramWindowMousePos = { 0, 0 };

// Spectrogram Parameters Settings Widget
static GtkWidget* g_spectrogramParametersWindow = NULL;
extern SSpectrogramParametersWndControls g_spectrogramParametersWndControls;

/**********************************************************************/
/*void	timeFocus (XSPECTRO *, float)                                  */
/**********************************************************************/
void timeFocus(XSPECTRO* spectro, float focusTime);

static GtkWidget* create_menu(gint     depth) {
    GtkWidget* menu;
    GtkWidget* menuitem;
    GSList* group;
    char buf[32];
    int i, j;

    if (depth < 1)
        return NULL;

    menu = gtk_menu_new();
    group = NULL;

    for (i = 0, j = 1; i < 5; i++, j++)
    {

#ifdef SAS_VS2022
        sprintf_s(buf, 32, "item %2d - %d", depth, j);
#else
        sprintf(buf, "item %2d - %d", depth, j);
#endif

        menuitem = gtk_radio_menu_item_new_with_label(group, buf);
        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));

        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
        gtk_widget_show(menuitem);
        if (i == 3)
            gtk_widget_set_sensitive(menuitem, FALSE);

        gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), create_menu(depth - 1));
    }

    return menu;
}

static void change_orientation(GtkWidget* button,
    GtkWidget* menubar)
{
    GtkWidget* parent;
    GtkOrientation orientation;

    parent = gtk_widget_get_parent(menubar);
    orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(parent));
    //gtk_orientable_set_orientation(GTK_ORIENTABLE(parent), 1 - orientation);

    if (orientation == GTK_ORIENTATION_VERTICAL)
        g_object_set(menubar, "pack-direction", GTK_PACK_DIRECTION_TTB, NULL);
    else
        g_object_set(menubar, "pack-direction", GTK_PACK_DIRECTION_LTR, NULL);

}

static void appClose() {

}

struct fch_result {
    gint response;
    // other information to return like filename,...
};

static gboolean fch_dialog(gpointer user_data)
{
    struct fch_result* result = (struct fch_result*)user_data;
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open file...", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    result->response = gtk_dialog_run(GTK_DIALOG(dialog));
    // now add other information to result

    gtk_widget_destroy(dialog);
    gtk_main_quit();  // terminate the gtk_main loop called from caller
    return FALSE;
}

static gboolean onDrawWaveForm(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    if (g_waveFileName != NULL) {

        SMousePos _mousePos = g_waveFormWindowMousePos;
        updateWaveFormWindow(widget, cr, &g_waveData, &g_spectro, &_mousePos);
    }

    return FALSE;
}

// Wave form window mouse move event process
gboolean waveFormWindowMouseMoveEvent(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {

    g_waveFormWindowMousePos.x = (float)event->x;
    g_waveFormWindowMousePos.y = (float)event->y;

    // Update the dft window
    if (g_waveFormWindow != NULL) {

        gtk_widget_queue_draw(g_waveFormWindow);
    }

    return FALSE;
}

// Wave form window button release event process
gboolean waveFormWindowButtonReleaseEvent(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {

    // DFT window data is be moved into focus point
    timeFocus( &g_spectro, g_spectro.focusTime);

    // Save the save time and save index of waveform window for subwindow
    g_spectro.focusTime0 = g_spectro.savetime;
    g_spectro.focusIndex0 = g_spectro.saveindex;

    // Calculate the spectrum
    newSpectrum(&g_spectro);

    // Update the dft window
    if (g_DFTWindow != NULL)
        gtk_widget_queue_draw(g_DFTWindow);

    // Update the waveform subwindow
    if (g_waveFormSubWindow != NULL)
        gtk_widget_queue_draw(g_waveFormSubWindow);

    // Update the spectrogram window
    if (g_spectrogramWindow != NULL)
        gtk_widget_queue_draw(g_spectrogramWindow);

    // Update the reassign spectrogram window
    if (g_reassignSpectrogramWindow != NULL)
        gtk_widget_queue_draw(g_reassignSpectrogramWindow);

    return FALSE;
}

// Crate the wave for window
void createWaveFormWindow() {

    if (g_waveFormWindow != NULL)
        return;

    g_waveFormWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(g_waveFormWindow), "Wave Form Window");
    gtk_window_set_default_size(GTK_WINDOW(g_waveFormWindow), WND_WIDTH, WND_HEIGHT);
    g_signal_connect(g_waveFormWindow, "destroy", G_CALLBACK(gtk_widget_destroyed), &g_waveSndFile);

    GtkWidget* _vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(g_waveFormWindow), _vbox);

    GtkWidget* _drawingArea = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(_drawingArea), "draw", G_CALLBACK(onDrawWaveForm), NULL);
    gtk_box_pack_start(GTK_BOX(_vbox), _drawingArea, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(g_waveFormWindow), "motion-notify-event", G_CALLBACK(waveFormWindowMouseMoveEvent), NULL);
    g_signal_connect(G_OBJECT(g_waveFormWindow), "button_release_event", G_CALLBACK(waveFormWindowButtonReleaseEvent), NULL);
    
    gtk_widget_show_all(g_waveFormWindow);

}

// onDraw for wave form subwindow
static gboolean onDrawWaveFormSubWindow(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    if (g_waveFileName != NULL) {

        SMousePos _mousePos = g_waveFormSubWindowMousePos;
        updateWaveFormSubWindow(widget, cr, &g_spectro, &_mousePos);
    }

    return FALSE;
}

// Wave form subwindow mouse move event process
gboolean waveFormSubWindowMouseMoveEvent(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {

    g_waveFormSubWindowMousePos.x = (float)event->x;
    g_waveFormSubWindowMousePos.y = (float)event->y;

    // Update the dft window
    if (g_waveFormSubWindow != NULL) {

        gtk_widget_queue_draw(g_waveFormSubWindow);
    }

    return FALSE;
}

// Wave form subwindow button release event process
gboolean waveFormSubWindowButtonReleaseEvent(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {

    // DFT window data is be moved into focus point
    timeFocus(&g_spectro, g_spectro.focusTime);

    // Calculate the spectrum
    newSpectrum(&g_spectro);

    // Update the dft window
    if (g_DFTWindow != NULL) {

        gtk_widget_queue_draw(g_DFTWindow);
    }

    // Update the waveform window
    if (g_waveFormWindow != NULL)
        gtk_widget_queue_draw(g_waveFormWindow);

    return FALSE;
}

// Crate the wave for sub window
void createWaveFormSubWindow() {

    if (g_waveFormSubWindow != NULL)
        return;

    g_waveFormSubWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(g_waveFormSubWindow), "Wave Form SubWindow");
    gtk_window_set_default_size(GTK_WINDOW(g_waveFormSubWindow), WND_WIDTH, 400);
    g_signal_connect(g_waveFormSubWindow, "destroy", G_CALLBACK(gtk_widget_destroyed), NULL);

    GtkWidget* _vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(g_waveFormSubWindow), _vbox);

    GtkWidget* _drawingArea = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(_drawingArea), "draw", G_CALLBACK(onDrawWaveFormSubWindow), NULL);
    gtk_box_pack_start(GTK_BOX(_vbox), _drawingArea, TRUE, TRUE, 0);
    g_signal_connect(G_OBJECT(g_waveFormSubWindow), "motion-notify-event", G_CALLBACK(waveFormSubWindowMouseMoveEvent), NULL);
    g_signal_connect(G_OBJECT(g_waveFormSubWindow), "button_release_event", G_CALLBACK(waveFormSubWindowButtonReleaseEvent), NULL);

    gtk_widget_show_all(g_waveFormSubWindow);

}

// Wave form window mouse move event process
gboolean dftWindowMouseMoveEvent(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {

    g_dftWindowMousePos.x = (float)event->x;
    g_dftWindowMousePos.y = (float)event->y;

    // Update the dft window
    if (g_DFTWindow != NULL) {

        gtk_widget_queue_draw(g_DFTWindow);
    }

    return FALSE;
}

static gboolean onDrawDFTWindow(GtkWidget* widget, cairo_t* cr, gpointer data)
{

    if (g_dftWindowWorking)
        return FALSE;

    g_dftWindowWorking = true;

    if (g_waveFileName == NULL) {

        g_dftWindowWorking = false;
        return FALSE;
    }

    updateDFTWindow(widget, cr, &g_spectro, &g_dftWindowMousePos);
    
    g_dftWindowWorking = false;

    return TRUE;
}

// Create the DFT window
void createDFTWindow() {

    if (g_DFTWindow != NULL)
        return;

    g_DFTWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(g_DFTWindow), "DFT Window");
    gtk_window_set_default_size(GTK_WINDOW(g_DFTWindow), 640, 480);
    g_signal_connect(g_DFTWindow, "destroy", G_CALLBACK(gtk_widget_destroyed), NULL);
    g_signal_connect(G_OBJECT(g_DFTWindow), "motion-notify-event", G_CALLBACK(dftWindowMouseMoveEvent), NULL);

    GtkWidget* _vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(g_DFTWindow), _vbox);

    GtkWidget* _drawingArea = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(_vbox), _drawingArea, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(_drawingArea), "draw", G_CALLBACK(onDrawDFTWindow), NULL);

    gtk_widget_show_all(g_DFTWindow);

}

// onDraw for spectrogram window
static gboolean onDrawSpectrogramWindow(GtkWidget* widget, cairo_t* cr, gpointer data)
{
        
    SMousePos _mousePos = g_spectrogramWindowMousePos;
    updateSpectrogramWindow(widget, cr, &g_spectro, &_mousePos);

    return FALSE;
}

// Spectrogram window mouse move event process
gboolean spectrogramWindowMouseMoveEvent(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {

    g_spectrogramWindowMousePos.x = (float)event->x;
    g_spectrogramWindowMousePos.y = (float)event->y;

    // Update the spectrogram window
    if (g_spectrogramWindow != NULL) {

        gtk_widget_queue_draw(g_spectrogramWindow);
    }

    return FALSE;
}

// Crate the spectrogram window
void createSpectrogramWindow() {

    int _wndWidth = 800;
    int _wndHeight = 400;

    g_spectro.wymax = _wndHeight;
    g_spectro.xgram = _wndWidth;

    gtk_window_set_title(GTK_WINDOW(g_spectrogramWindow), "Spectrogram Window");
    gtk_window_set_default_size(GTK_WINDOW(g_spectrogramWindow), _wndWidth, _wndHeight);
    g_signal_connect(g_spectrogramWindow, "destroy", G_CALLBACK(gtk_widget_destroyed), NULL);

    GtkWidget* _vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(g_spectrogramWindow), _vbox);

    GtkWidget* _drawingArea = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(_drawingArea), "draw", G_CALLBACK(onDrawSpectrogramWindow), NULL);
    gtk_box_pack_start(GTK_BOX(_vbox), _drawingArea, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(g_spectrogramWindow), "motion-notify-event", G_CALLBACK(spectrogramWindowMouseMoveEvent), NULL);

    gtk_widget_show_all(g_spectrogramWindow);

}

// onDraw for reassign spectrogram window
static gboolean onDrawReassignSpectrogramWindow(GtkWidget* widget, cairo_t* cr, gpointer data)
{

    SMousePos _mousePos = g_reassignSpectrogramWindowMousePos;
    updateReassignSpectrogramWindow(widget, cr, &g_spectro, &_mousePos);

    return FALSE;
}

// Reassign Spectrogram window mouse move event process
gboolean reassignSpectrogramWindowMouseMoveEvent(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {

    g_reassignSpectrogramWindowMousePos.x = (float)event->x;
    g_reassignSpectrogramWindowMousePos.y = (float)event->y;

    // Update the reassign spectrogram window
    if (g_reassignSpectrogramWindow != NULL) {

        gtk_widget_queue_draw(g_reassignSpectrogramWindow);
    }

    return FALSE;
}

// Crate the reassign spectrogram window
void createReassignSpectrogramWindow() {

    int _wndWidth = 800;
    int _wndHeight = 400;

    g_spectro.wymax = _wndHeight;
    g_spectro.xgram = _wndWidth;

    gtk_window_set_title(GTK_WINDOW(g_reassignSpectrogramWindow), "Reassign Spectrogram Window");
    gtk_window_set_default_size(GTK_WINDOW(g_reassignSpectrogramWindow), _wndWidth, _wndHeight);
    g_signal_connect(g_reassignSpectrogramWindow, "destroy", G_CALLBACK(gtk_widget_destroyed), NULL);

    GtkWidget* _vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(g_reassignSpectrogramWindow), _vbox);

    GtkWidget* _drawingArea = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(_drawingArea), "draw", G_CALLBACK(onDrawReassignSpectrogramWindow), NULL);
    gtk_box_pack_start(GTK_BOX(_vbox), _drawingArea, TRUE, TRUE, 0);

    g_signal_connect(G_OBJECT(g_reassignSpectrogramWindow), "motion-notify-event", G_CALLBACK(reassignSpectrogramWindowMouseMoveEvent), NULL);

    gtk_widget_show_all(g_reassignSpectrogramWindow);

}

// Reassign Spectrogram
static void reassignSpectrogram() {

    if (g_reassignSpectrogramWindow == NULL) {

        g_reassignSpectrogramWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        createReassignSpectrogramWindow(g_reassignSpectrogramWindow);
    }
    else {

        if (!gtk_widget_get_visible(g_reassignSpectrogramWindow)) {

            g_reassignSpectrogramWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            createReassignSpectrogramWindow(g_reassignSpectrogramWindow);
        }
    }

    XSPECTRO* _spectro = &g_spectro;

    if (_spectro->respectrogram == 1) {

        // Update the spectrogram window
        if (g_reassignSpectrogramWindow != NULL)
            gtk_widget_queue_draw(g_reassignSpectrogramWindow);
        return;
    }

    _spectro->respectrogram = 1;

    //spectro->spratio = 1100;
    findsdelta(_spectro);
    findypix(_spectro);
    printf("before re---spectorgram...............\n");
    calculate_spectrogram(_spectro);
    //calculate_spectra(spectro);  

    // Update the reassign spectrogram window
    if (g_reassignSpectrogramWindow != NULL)
        gtk_widget_queue_draw(g_reassignSpectrogramWindow);
}


// Display a wave form
static void displayWaveform(const gchar* title, const gchar* fileName){

    if (g_waveFormWindow == NULL)
        createWaveFormWindow();

    if (g_waveFormSubWindow == NULL)
        createWaveFormSubWindow();

    if (g_waveFileName != NULL)
        g_free(g_waveFileName);
    g_waveFileName = g_strdup(fileName);
}


// Display a DFT
static void displayDFT() {

    if (g_DFTWindow == NULL)
        createDFTWindow();
}

// Set the spectro with a wave data
void setSpectro(XSPECTRO* spectro, SWaveData* waveData) {

    int _frames = waveData->frames;

    spectro->spers = (float)waveData->sampleRate;
    spectro->totsamp = waveData->frames;
    spectro->sizwin = 256;
    spectro->sizfft = 256;

    float _totalTime = (float)spectro->totsamp * 1000 / spectro->spers;

    float _time = _totalTime / 2.0f;
    _time = 2431.1f; // 702.0; // 300.50; // 1682.7;
    spectro->focusTime = _time;
    spectro->useFocusTime = false;
    int _index = (int)(_time * spectro->spers / 1000.0f);

    _index -= spectro->sizfft / 2;
    spectro->saveindex = _index;
    spectro->savetime = _time;

    spectro->oldindex = _index;
    spectro->oldtime = _time;

    // for subwindow
    spectro->focusTime0 = _time;
    spectro->focusIndex0 = _index;

    if (spectro->iwave != NULL) {

        free(spectro->iwave);
    }

    spectro->iwave = (short*)malloc(sizeof(short) * _frames);

    int _i;
    for (_i = 0; _i < _frames; _i++) {

        float _data = waveData->datas[_i];
        _data *= 32767.0f / 9997.56f;
        short _sData = (short)(_data * 10000);
        spectro->iwave[_i] = _sData;
    }
}

// Read the new wav file
static void readNewWaveFile(GtkWidget* widget, gpointer data) {

    printf("read new wave file.\n");

    GtkWidget* _parentWindow = gtk_widget_get_toplevel(widget);
    GtkFileChooserAction _action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint _result;

    GtkWidget* _dlg = gtk_file_chooser_dialog_new("Open File",
        GTK_WINDOW(_parentWindow),
        _action,
        "_Cancel",
        GTK_RESPONSE_CANCEL,
        "_Open",
        GTK_RESPONSE_ACCEPT,
        NULL);
    _result = gtk_dialog_run(GTK_DIALOG(_dlg));

    if (_result == GTK_RESPONSE_ACCEPT)
    {
        char* _fileName = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(_dlg));

        g_print("File selected: %s\n", _fileName);

        if (_fileName){

#ifdef SAS_VS2022
            sprintf_s(g_spectro.wavefile, 200, _fileName);
#else
            sprintf(g_spectro.wavefile, _fileName);
#endif
            // Read a wave data from a file
            readWaveData(_fileName, &g_waveData);
            
            // printf("frames=%d\n", g_waveData.frames);

            // Set the spectro with the wave data
            setSpectro(&g_spectro, &g_waveData);

            // Calculate the spectrum
            newSpectrum(&g_spectro);

            //if (g_waveFormWindow == NULL)
            //    createWaveFormWindow();
            // Display waveform in a new window
            displayWaveform( "Read a new wave file", _fileName);

            // Display DFT
            displayDFT();

            g_free( _fileName);
        }
        else
        {
            g_print("No file selected.\n");
        }
    }

    gtk_widget_destroy(_dlg);
    //struct fch_result data;
    //g_idle_add(fch_dialog, &data);
}


// Replace the current wave file
static void replaceCurrentWaveFile(GtkWidget* widget, gpointer data) {

    printf("replace current wave file.\n");

    GtkWidget* _parentWindow = gtk_widget_get_toplevel(widget);
    GtkFileChooserAction _action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint _result;

    GtkWidget* _dlg = gtk_file_chooser_dialog_new("Open File",
        GTK_WINDOW(_parentWindow),
        _action,
        "_Cancel",
        GTK_RESPONSE_CANCEL,
        "_Open",
        GTK_RESPONSE_ACCEPT,
        NULL);
    _result = gtk_dialog_run(GTK_DIALOG(_dlg));

    if (_result == GTK_RESPONSE_ACCEPT)
    {
        char* _fileName = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(_dlg));

        g_print("File selected: %s\n", _fileName);

        if (_fileName) {

            // Display waveform in a new window
            displayWaveform("Replace current wave file", _fileName);
            g_free(_fileName);
        }
        else
        {
            g_print("No file selected.\n");
        }
    }

    gtk_widget_destroy(_dlg);
}

// Close the current wave file
static void closeWaveFile(GtkWidget* widget, gpointer data) {

    printf("Close current wave file.\n");

    // Display waveform in a new window
    displayWaveform("Replace current wave file", "");
}
 
// --------- TIME MENU

/**********************************************************************/
/*void	timeFocus (XSPECTRO *, float)                                  */
/**********************************************************************/
void timeFocus(XSPECTRO* spectro, float focusTime)
{

    spectro->oldindex = spectro->saveindex;
    spectro->oldtime = spectro->savetime;

    spectro->savetime = focusTime;

    spectro->saveindex = (int)(spectro->savetime * spectro->spers / 1000.0f + .5f);
    spectro->savetime = (float)spectro->saveindex / spectro->spers * 1000.0f;

    validindex(spectro);
}

/**********************************************************************/
/*void	timestep (XSPECTRO *, float)                                  */
/**********************************************************************/
void timeStep(XSPECTRO* spectro, float step)
{

    spectro->oldindex = spectro->saveindex;
    spectro->oldtime = spectro->savetime;

    spectro->savetime = spectro->savetime + step;

    spectro->saveindex = (int)(spectro->savetime * spectro->spers / 1000.0f + .5f);
    spectro->savetime = (float)spectro->saveindex / spectro->spers * 1000.0f;
    
    validindex(spectro);
}

// Move back time step 10ms
static void timeBack10(GtkWidget* widget, gpointer data) {

    timeStep(&g_spectro, -10.0);

    // Calculate the spectrum
    newSpectrum(&g_spectro);

    // Update the wave form window
    if (g_waveFormWindow != NULL)
        gtk_widget_queue_draw(g_waveFormWindow);
    
    // Update the dft window
    if (g_DFTWindow != NULL)
        gtk_widget_queue_draw(g_DFTWindow);
}

// Move forward time step 10ms
static void timeForward10(GtkWidget* widget, gpointer data) {

    timeStep(&g_spectro, 10.0);

    // Calculate the spectrum
    newSpectrum(&g_spectro);

    // Update the wave form window
    if (g_waveFormWindow != NULL)
        gtk_widget_queue_draw(g_waveFormWindow);

    // Update the dft window
    if (g_DFTWindow != NULL)
        gtk_widget_queue_draw(g_DFTWindow);
}

// Move back time step 50ms
static void timeBack50(GtkWidget* widget, gpointer data) {

    timeStep(&g_spectro, -50.0);

    // Calculate the spectrum
    newSpectrum(&g_spectro);

    // Update the wave form window
    if (g_waveFormWindow != NULL)
        gtk_widget_queue_draw(g_waveFormWindow);

    // Update the dft window
    if (g_DFTWindow != NULL)
        gtk_widget_queue_draw(g_DFTWindow);
}

// Move forward time step 50ms
static void timeForward50(GtkWidget* widget, gpointer data) {

    timeStep(&g_spectro, 50.0);

    // Calculate the spectrum
    newSpectrum(&g_spectro);

    // Update the wave form window
    if (g_waveFormWindow != NULL)
        gtk_widget_queue_draw(g_waveFormWindow);

    // Update the dft window
    if (g_DFTWindow != NULL)
        gtk_widget_queue_draw(g_DFTWindow);
}

//------------ Create the File Menu --------------------
static GtkWidget* createFileMenu() {

    GtkWidget* _menu;
    GtkWidget* _menuItem;

    _menu = gtk_menu_new();

    // 1. Create the <Read new .wav file> menu item
    _menuItem = gtk_menu_item_new_with_label("Read new .wav file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(readNewWaveFile), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 2. Create the <Replace current .wav file> menu item
    _menuItem = gtk_menu_item_new_with_label("Replace current .wav file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(replaceCurrentWaveFile), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 3. Create the <Close .wav file> menu item
    _menuItem = gtk_menu_item_new_with_label("Close .wav file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(closeWaveFile), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 4. Create the <Read spectrum file> menu item
    _menuItem = gtk_menu_item_new_with_label("Read spectrum file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 5. Create the <Save spectrum to file> menu item
    _menuItem = gtk_menu_item_new_with_label("Save spectrum to file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 6. Create the <Read parameter file> menu item
    _menuItem = gtk_menu_item_new_with_label("Read parameter file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 7. Create the <Save parameter file> menu item
    _menuItem = gtk_menu_item_new_with_label("Save parameter file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 8. Create the <Save w->e to .wav file> menu item
    _menuItem = gtk_menu_item_new_with_label("Save w->e to .wav file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 9. Create the <Save w->e to ASCII file> menu item
    _menuItem = gtk_menu_item_new_with_label("Save w->e to ASCII file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 10. Create the <Read Label file> menu item
    _menuItem = gtk_menu_item_new_with_label("Read Label file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 11. Create the <Delete all labels> menu item
    _menuItem = gtk_menu_item_new_with_label("Delete all labels");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 12. Create the <Edit labels> menu item
    _menuItem = gtk_menu_item_new_with_label("Edit labels");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 13. Create the <Make label @ cursor> menu item
    _menuItem = gtk_menu_item_new_with_label("Make label @ cursor");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 14. Create the <Save labels> menu item
    _menuItem = gtk_menu_item_new_with_label("Save labels");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 15. Create the <Open/close 1 spectra/pg PS file> menu item
    _menuItem = gtk_menu_item_new_with_label("Open/close 1 spectra/pg PS file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 16. Create the <Save window to PS file (G)> menu item
    _menuItem = gtk_menu_item_new_with_label("Save window to PS file (G)");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 17. Create the <Open/close 4 spectra/pg PS file> menu item
    _menuItem = gtk_menu_item_new_with_label("Open/close 4 spectra/pg PS file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 18. Create the <Save spectrum to 4/pg PS file (g)> menu item
    _menuItem = gtk_menu_item_new_with_label("Save spectrum to 4/pg PS file (g)");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 19. Create the <List freq. & BW in window> menu item
    _menuItem = gtk_menu_item_new_with_label("List freq. & BW in window");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 20. Create the <List spectrum values in window> menu item
    _menuItem = gtk_menu_item_new_with_label("List spectrum values in window");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 21. Create the <Open/close amp. & freq. file> menu item
    _menuItem = gtk_menu_item_new_with_label("Open/close amp. & freq. file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 22. Create the <Save amp. & freq. to file (H)> menu item
    _menuItem = gtk_menu_item_new_with_label("Save amp. & freq. to file (H)");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 23. Create the <Select waveform> menu item
    _menuItem = gtk_menu_item_new_with_label("Select waveform");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 24. Create the <Forward one waveform> menu item
    _menuItem = gtk_menu_item_new_with_label("Forward one waveform");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 25. Create the <Backward one waveform> menu item
    _menuItem = gtk_menu_item_new_with_label("Backward one waveform");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 26. Create the <Quit> menu item
    _menuItem = gtk_menu_item_new_with_label("Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect_swapped(G_OBJECT(_menuItem), "activate", G_CALLBACK(gtk_main_quit), window);

    gtk_widget_show(_menuItem);

    return _menu;
}

//------------ Create the Time Menu --------------------
static GtkWidget* createTimeMenu() {

    GtkWidget* _menu;
    GtkWidget* _menuItem;

    _menu = gtk_menu_new();

    // 1. Create the <Move back 10 ms> menu item
    _menuItem = gtk_menu_item_new_with_label("Move back 10 ms");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(timeBack10), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 2. Create the <Move forward 10 ms> menu item
    _menuItem = gtk_menu_item_new_with_label("Move forward 10 ms");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(timeForward10), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 3. Create the <Move back 50 ms> menu item
    _menuItem = gtk_menu_item_new_with_label("Move back 50 ms");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(timeBack50), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 4. Create the <Move forward 50 ms> menu item
    _menuItem = gtk_menu_item_new_with_label("Move forward 50 ms");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(timeForward50), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 5. Create the <Step left by width of window 1> menu item
    _menuItem = gtk_menu_item_new_with_label("Step left by width of window 1");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 6. Create the <Step right by width of window 1> menu item
    _menuItem = gtk_menu_item_new_with_label("Step right by width of window 1");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 7. Create the <Move maker to time t> menu item
    _menuItem = gtk_menu_item_new_with_label("Move maker to time t");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 8. Create the <Set start of selected region> menu item
    _menuItem = gtk_menu_item_new_with_label("Set start of selected region");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 9. Create the <Set end of selected region> menu item
    _menuItem = gtk_menu_item_new_with_label("Set end of selected region");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 10. Create the <Set w to beginning of file> menu item
    _menuItem = gtk_menu_item_new_with_label("Set w to beginning of file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 11. Create the <Set e to end of file> menu item
    _menuItem = gtk_menu_item_new_with_label("Set e to end of file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 12. Create the <Set both W and E> menu item
    _menuItem = gtk_menu_item_new_with_label("Set both W and E");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 13. Create the <Move marker to w> menu item
    _menuItem = gtk_menu_item_new_with_label("Move marker to w");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 14. Create the <Move marker to e> menu item
    _menuItem = gtk_menu_item_new_with_label("Move marker to e");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 15. Create the <Show w->e in window 1> menu item
    _menuItem = gtk_menu_item_new_with_label("Show w->e in window 1");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 16. Create the <Peak/valley picking on/off> menu item
    _menuItem = gtk_menu_item_new_with_label("Peak/valley picking on/off");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 17. Create the <Zoom in (window 0 & 1 only)> menu item
    _menuItem = gtk_menu_item_new_with_label("Zoom in (window 0 & 1 only)");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 18. Create the <Zoom out (window 0 & 1 only)> menu item
    _menuItem = gtk_menu_item_new_with_label("Zoom out (window 0 & 1 only)");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 19. Create the <Lock cursor arrows> menu item
    _menuItem = gtk_menu_item_new_with_label("Lock cursor arrows");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 20. Create the <Unlock cursor arrows> menu item
    _menuItem = gtk_menu_item_new_with_label("Unlock cursor arrows");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    return _menu;
}

// Spectrum Menu : DFT magnitede
void spectroDFTmagnitude() {

    if (g_spectro.option != 'd') {

        g_spectro.option = 'd';

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectrum Menu : Smoothed spectrum
void spectroSmooth() {

    if (g_spectro.option != 's') {
        
        g_spectro.history = 0;
        g_spectro.option = 's';

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectrum Menu : DFT magnitede and Smoothed spectrum
void spectroDFTmagnitudeSmooth() {

    if (g_spectro.option != 'S') {

        g_spectro.option = 'S';

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectrum Menu : Critical Band
void spectroCriticalBand() {

    if (g_spectro.option != 'c') {

        g_spectro.option = 'c';

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectrum Menu : Critical Band + DFT
void spectroCriticalBandDFT() {

    if (g_spectro.option != 'j') {

        g_spectro.option = 'j';

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectrum Menu : Compute Slope
void spectroComputeSlope() {

    if (g_spectro.option != 'T') {

        g_spectro.option = 'T';

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectrum Menu : Linear Prediction + DFT
void spectroLinearPredictionDFT() {

    if (g_spectro.option != 'l') {

        g_spectro.option = 'l';

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectro Aval Dialog Response
static void onSpectroAvalDlgResponse(GtkDialog* dialog, gint response_id, gpointer user_data) {

    char _chDatas[50];
    bool _isDestroy = false;
    bool _isChanged = false;
    bool _isUpdate = false;
    float _time = 0.0f;

    GtkEntry* _entry = GTK_ENTRY(user_data);

    const gchar* _text = gtk_entry_get_text(_entry);

    if (_text != NULL && strlen(_text) > 0) {

        _time = (float)atof(_text);
        _isChanged = true;
    }

    if (response_id == GTK_RESPONSE_OK) {

        switch (g_spectroAvalDlgCount) {
        case 0:

            g_spectro.avgcount = 1;
            if (_isChanged)
                g_spectro.avgtimes[0] = (int)(_time + 0.5f);
            g_spectroAvalDlgCount++;

#ifdef SAS_VS2022
            sprintf_s(_chDatas, 32, "%d", g_spectro.avgtimes[1]);
#else
            sprintf(_chDatas, "%d", g_spectro.avgtimes[1]);
#endif

            gtk_entry_set_text(g_spectroAvalDlgEntry, _chDatas);

            gtk_label_set_text(g_spectroAvalDlgLabel, "End Time:");
            break;
        default:

            g_spectro.avgcount = 2;
            g_spectro.avgtimes[1] = (int)(_time + 0.5f);
            g_spectroAvalDlgCount = 0;
            _isDestroy = true;
            _isUpdate = true;

            gtk_label_set_text(g_spectroAvalDlgLabel, "Begine Time:");
            break;
        }
    }
    else if (response_id == GTK_RESPONSE_CANCEL) {

        _isDestroy = true;
    }

    if (_isDestroy) {

        gtk_widget_destroy(GTK_WIDGET(dialog));
    }

    if (_isUpdate) {

        getavg(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

// Spectrum Menu : aval
void spectroAval(GtkWidget* widget, gpointer data) {

    /*a avg. DFT mag over interval*/
    /* don't save averaging option */
    if (g_spectro.option != 'a'
        && g_spectro.option != 'A'
        && g_spectro.option != 'k')
        g_spectro.savoption = g_spectro.option;

    g_spectro.option = 'a';
    g_spectro.avgcount = 0;

    g_spectro.avgtimes[0] = (int)(g_spectro.savetime + 0.5f);
    g_spectro.avgtimes[1] = (int)(g_spectro.savetime + 0.5f);

    GtkWidget* _dialog;
    GtkWidget* _contentArea;
    GtkWidget* _box;

    _dialog = gtk_dialog_new_with_buttons("Input Time",

        GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        "OK",
        GTK_RESPONSE_OK,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        NULL);

    _contentArea = gtk_dialog_get_content_area(GTK_DIALOG(_dialog));

    _box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(_contentArea), _box);

    g_spectroAvalDlgLabel = (GtkLabel*)gtk_label_new("begine time:");
    gtk_box_pack_start(GTK_BOX(_box), GTK_WIDGET( g_spectroAvalDlgLabel), TRUE, TRUE, 0);

    g_spectroAvalDlgEntry = (GtkEntry*)gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(_box), GTK_WIDGET(g_spectroAvalDlgEntry), TRUE, TRUE, 0);

    gtk_widget_show_all(_dialog);

    g_spectroAvalDlgCount = 0;
    char _chDatas[50];

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 32, "%d", g_spectro.avgtimes[0]);
#else
    sprintf(_chDatas, "%d", g_spectro.avgtimes[0]);
#endif

    gtk_entry_set_text(g_spectroAvalDlgEntry, _chDatas);

    g_signal_connect(_dialog, "response", G_CALLBACK(onSpectroAvalDlgResponse), g_spectroAvalDlgEntry);
}

// Spectro Avg Dialog Response
static void onSpectroAvgDlgResponse(GtkDialog* dialog, gint response_id, gpointer user_data) {

    char _chDatas[50];
    bool _isDestroy = false;
    bool _isChanged = false;
    bool _isUpdate = false;
    float _time = 0.0f;

    GtkEntry* _entry = GTK_ENTRY(user_data);

    const gchar* _text = gtk_entry_get_text(_entry);

    if (_text != NULL && strlen(_text) > 0) {

        _time = (float)atof(_text);
        _isChanged = true;
    }
    
    if (response_id == GTK_RESPONSE_OK) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 32, "%d", (int)(g_spectro.savetime + 0.5f));
#else
        sprintf(_chDatas, "%d", (int)(g_spectro.savetime + 0.5f));
#endif
        gtk_entry_set_text(g_spectroAvgDlgEntry, _chDatas);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 32, "time %d:", 1);
#else
        sprintf(_chDatas, "time %d:", 1);
#endif
        gtk_label_set_text(g_spectroAvgDlgLabel, _chDatas);

        g_spectroAvgDlgCount = 0;
        _isDestroy = true;
        _isUpdate = true;
    }
    else if (response_id == RESPONSE_ID_SPECTRO_AVG_APPLY) {

        g_spectroAvgDlgCount++;

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 32, "%d", (int)(g_spectro.savetime + 0.5f));
#else
        sprintf(_chDatas, "%d", (int)(g_spectro.savetime + 0.5f));
#endif
        gtk_entry_set_text(g_spectroAvgDlgEntry, _chDatas);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 32, "time %d:", g_spectroAvgDlgCount + 1);
#else
        sprintf(_chDatas, "time %d:", g_spectroAvgDlgCount + 1);
#endif
        gtk_label_set_text(g_spectroAvgDlgLabel, _chDatas);

        g_spectro.avgcount = g_spectroAvgDlgCount;
        if (_isChanged) {

            g_spectro.avgtimes[g_spectroAvgDlgCount - 1] = (int)(_time + 0.5f);
        }
        else {

            g_spectro.avgtimes[g_spectroAvgDlgCount - 1] = (int)(g_spectro.savetime + 0.5f);
        }

        if (g_spectroAvgDlgCount >= AVGLIMIT) {

            _isDestroy = true;
            _isUpdate = true;
        }
    }
    else if (response_id == GTK_RESPONSE_CANCEL) {

        _isDestroy = true;
    }

    if (_isDestroy) {

        gtk_widget_destroy(GTK_WIDGET(dialog));
    }

    if (_isUpdate) {

        getavg(&g_spectro);

        // Update the dft window
        if (g_DFTWindow != NULL)
            gtk_widget_queue_draw(g_DFTWindow);
    }
}

void spectroAvg(GtkWidget *widget, gpointer data) {

    if (g_spectro.option != 'a'
        && g_spectro.option != 'A'
        && g_spectro.option != 'k')
        g_spectro.savoption = g_spectro.option;

    g_spectro.option = 'A';
    g_spectro.avgcount = 0;

    g_spectro.avgtimes[0] = (int)(g_spectro.savetime + 0.5f);
    g_spectro.avgtimes[1] = (int)(g_spectro.savetime + 0.5f);

    GtkWidget* _dialog;
    GtkWidget* _contentArea;
    GtkWidget* _box;

    _dialog = gtk_dialog_new_with_buttons("Input Time",

        GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        "OK",
        RESPONSE_ID_SPECTRO_AVG_APPLY,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        "Done do it",
        GTK_RESPONSE_OK,
        NULL);

    _contentArea = gtk_dialog_get_content_area(GTK_DIALOG(_dialog));

    _box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(_contentArea), _box);

    g_spectroAvgDlgLabel = (GtkLabel*)gtk_label_new("time 1:");
    gtk_box_pack_start(GTK_BOX(_box), GTK_WIDGET(g_spectroAvgDlgLabel), TRUE, TRUE, 0);

    g_spectroAvgDlgEntry = (GtkEntry*)gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(_box), GTK_WIDGET( g_spectroAvgDlgEntry), TRUE, TRUE, 0);

    gtk_widget_show_all(_dialog);

    g_signal_connect(_dialog, "response", G_CALLBACK(onSpectroAvgDlgResponse), g_spectroAvgDlgEntry);

    g_spectroAvgDlgCount = 0;
    char _chDatas[50];

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 32, "%d", g_spectro.avgtimes[0]);
#else
    sprintf(_chDatas, "%d", g_spectro.avgtimes[0]);
#endif

    gtk_entry_set_text(g_spectroAvgDlgEntry, _chDatas);
}

void spectroAvgKn2(GtkWidget* widget, gpointer data) {

    if (g_spectro.option != 'a'
        && g_spectro.option != 'A'
        && g_spectro.option != 'k')
        g_spectro.savoption = g_spectro.option;

    g_spectro.option = 'k';

    getavg(&g_spectro);

    // Update the dft window
    if (g_DFTWindow != NULL)
        gtk_widget_queue_draw(g_DFTWindow);
}

// Spectral Parameters Setting
void spectralParametersSetting() {

    if (g_spectralParametersWindow == NULL) {

        g_spectralParametersWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        createSpectralParametersWindow(g_spectralParametersWindow);
    }
    else {

        if (!gtk_widget_get_visible(g_spectralParametersWindow)) {

            g_spectralParametersWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            createSpectralParametersWindow(g_spectralParametersWindow);
        }
    }

    updateSpectralParametersWindow();
}

// Include Spectrogram
void includeSpectrogram() {

    if (g_spectrogramWindow == NULL) {

        g_spectrogramWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        createSpectrogramWindow(g_spectrogramWindow);
    }
    else {

        if (!gtk_widget_get_visible(g_spectrogramWindow)) {

            g_spectrogramWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            createSpectrogramWindow(g_spectrogramWindow);
        }
    }

    XSPECTRO* _spectro = &g_spectro;

    if (_spectro->spectrogram == 1) {

        // Update the spectrogram window
        if (g_spectrogramWindow != NULL)
            gtk_widget_queue_draw(g_spectrogramWindow);
        return;
    }

    _spectro->spectrogram = 1;

    findsdelta( _spectro);
    findypix( _spectro);
    calculate_spectra(_spectro);

    // Update the spectrogram window
    if (g_spectrogramWindow != NULL)
        gtk_widget_queue_draw(g_spectrogramWindow);

}

// Lighten spectrogram
void lightenSpectrogram() {

    XSPECTRO* _spectro = &g_spectro;

    /* , Lighten spectrogram */
    if (_spectro->spectrogram) {

        _spectro->maxmag += 2;
        _spectro->minmag += 2;
        remapgray( _spectro);
    }

    // Update the spectrogram window
    if (g_spectrogramWindow != NULL)
        gtk_widget_queue_draw(g_spectrogramWindow);
}

// Darker spectrogram
void darkerSpectrogram() {

    XSPECTRO* _spectro = &g_spectro;

    /* , Lighten spectrogram */
    if (_spectro->spectrogram) {

        _spectro->maxmag -= 2;
        _spectro->minmag -= 2;
        remapgray(_spectro);
    }

    // Update the spectrogram window
    if (g_spectrogramWindow != NULL)
        gtk_widget_queue_draw(g_spectrogramWindow);
}

// Spectro specms Dialog Response
static void onSpectroSpecmsDlgResponse(GtkDialog* dialog, gint response_id, gpointer user_data) {

    bool _isDestroy = true;
    bool _isChanged = false;
    bool _isUpdate = false;
    float _specms = 0.0f;

    GtkEntry* _entry = GTK_ENTRY(user_data);

    const gchar* _text = gtk_entry_get_text(_entry);

    if (_text != NULL && strlen(_text) > 0) {

        _specms = (float)atof(_text);
        _isChanged = true;
    }

    if (response_id == GTK_RESPONSE_OK) {

        if (_isChanged) {

            g_spectro.specms = _specms;
            _isUpdate = true;
        }
    }

    if (_isDestroy) {

        gtk_widget_destroy(GTK_WIDGET(dialog));
    }

    if (_isUpdate) {

        // Update the spectrogram window
        if (g_spectrogramWindow != NULL)
            gtk_widget_queue_draw(g_spectrogramWindow);

        // Update the reassign spectrogram window
        if (g_reassignSpectrogramWindow != NULL)
            gtk_widget_queue_draw(g_reassignSpectrogramWindow);
    }
}

// Set the specms
void setSpecms(GtkWidget* widget, gpointer data) {

    GtkWidget* _dialog;
    GtkWidget* _contentArea;
    GtkWidget* _box;

    _dialog = gtk_dialog_new_with_buttons("Set a specms",

        GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        "OK",
        GTK_RESPONSE_OK,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        NULL);

    _contentArea = gtk_dialog_get_content_area(GTK_DIALOG(_dialog));

    _box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(_contentArea), _box);

    GtkLabel * _label = (GtkLabel*)gtk_label_new("specms:");
    gtk_box_pack_start(GTK_BOX(_box), GTK_WIDGET(_label), TRUE, TRUE, 0);

    g_spectroSpecmsDlgEntry = (GtkEntry*)gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(_box), GTK_WIDGET(g_spectroSpecmsDlgEntry), TRUE, TRUE, 0);

    char _chDatas[50];

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 32, "%.0f", g_spectro.specms);
#else
    sprintf(_chDatas, "%.0f", g_spectro.specms);
#endif

    gtk_entry_set_text(g_spectroSpecmsDlgEntry, _chDatas);

    g_signal_connect(_dialog, "response", G_CALLBACK(onSpectroSpecmsDlgResponse), g_spectroSpecmsDlgEntry);

    gtk_widget_show_all(_dialog);
}

// Spectrogram Parameters Setting
void spectrogramParametersSetting() {

    if (g_spectrogramParametersWindow == NULL) {

        g_spectrogramParametersWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        createSpectrogramParametersWindow(g_spectrogramParametersWindow);
    }
    else {

        if (!gtk_widget_get_visible(g_spectrogramParametersWindow)) {

            g_spectrogramParametersWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
            createSpectrogramParametersWindow(g_spectrogramParametersWindow);
        }
    }

    updateSpectrogramParametersWindow();
}

// Recalculate a spectrogram
void recalculateSpectrogram() {

    XSPECTRO* _spectro = &g_spectro;

    _spectro->spectrogram = 1;

    findsdelta(_spectro);
    findypix(_spectro);
    calculate_spectra(_spectro);

    // Update the spectrogram window
    if (g_spectrogramWindow != NULL)
        gtk_widget_queue_draw(g_spectrogramWindow);

    _spectro->respectrogram = 1;

    findsdelta(_spectro);
    findypix(_spectro);
    calculate_spectrogram(_spectro);

    // Update the reassign spectrogram window
    if (g_reassignSpectrogramWindow != NULL)
        gtk_widget_queue_draw(g_reassignSpectrogramWindow);
}

//------------ Create the Spectrum Menu --------------------
static GtkWidget* createSpectrumMenu() {

    GtkWidget* _menu;
    GtkWidget* _menuItem;

    _menu = gtk_menu_new();

    // 1. Create the <DFT magnitude> menu item
    _menuItem = gtk_menu_item_new_with_label("DFT magnitude");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroDFTmagnitude), NULL);
    gtk_widget_show(_menuItem);

    // 2. Create the <Smoothed spectrum> menu item
    _menuItem = gtk_menu_item_new_with_label("Smoothed spectrum");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroSmooth), NULL);
    gtk_widget_show(_menuItem);

    // 3. Create the <Smoothed spectrum + DFT> menu item
    _menuItem = gtk_menu_item_new_with_label("Smoothed spectrum + DFT");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroDFTmagnitudeSmooth), NULL);
    gtk_widget_show(_menuItem);

    // 4. Create the <Critical-band> menu item
    _menuItem = gtk_menu_item_new_with_label("Critical-band");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroCriticalBand), NULL);
    gtk_widget_show(_menuItem);

    // 5. Create the <Critical-band + DFT> menu item
    _menuItem = gtk_menu_item_new_with_label("Critical-band + DFT");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroCriticalBandDFT), NULL);
    gtk_widget_show(_menuItem);

    // 6. Create the <c-b compute slope > 1kHz> menu item
    _menuItem = gtk_menu_item_new_with_label("c-b compute slope > 1kHz");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroComputeSlope), NULL);
    gtk_widget_show(_menuItem);

    // 7. Create the <Linear-prediction + DFT> menu item
    _menuItem = gtk_menu_item_new_with_label("Linear-prediction + DFT");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroLinearPredictionDFT), NULL);
    gtk_widget_show(_menuItem);

    // 8. Create the <Avg. DFT mag over interval> menu item
    _menuItem = gtk_menu_item_new_with_label("Avg. DFT mag over interval");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroAval), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 9. Create the <Avg. DFT mag> menu item
    _menuItem = gtk_menu_item_new_with_label("Avg. DFT mag");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroAvg), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 10. Create the <Spectral avg. -kn/2 to kn/2> menu item
    _menuItem = gtk_menu_item_new_with_label("Spectral avg. -kn/2 to kn/2");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectroAvgKn2), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 11. Create the <Change spectrum parameters> menu item
    _menuItem = gtk_menu_item_new_with_label("Change spectrum parameters");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectralParametersSetting), NULL);
    gtk_widget_show(_menuItem);

    // 12. Create the <Include spectrogram> menu item
    _menuItem = gtk_menu_item_new_with_label("Include spectrogram");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(includeSpectrogram), NULL);
    gtk_widget_show(_menuItem);

    // 13. Create the <Re-assigned spectrogram> menu item
    _menuItem = gtk_menu_item_new_with_label("Re-assigned spectrogram");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(reassignSpectrogram), NULL);
    gtk_widget_show(_menuItem);

    // 14. Create the <Lighten spectrogram> menu item
    _menuItem = gtk_menu_item_new_with_label("Lighten spectrogram");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(lightenSpectrogram), NULL);
    gtk_widget_show(_menuItem);

    // 15. Create the <Darken spectrogram> menu item
    _menuItem = gtk_menu_item_new_with_label("Darken spectrogram");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(darkerSpectrogram), NULL);
    gtk_widget_show(_menuItem);

    // 16. Create the <Set msec in spectrogram> menu item
    _menuItem = gtk_menu_item_new_with_label("Set msec in spectrogram");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(setSpecms), g_mainWindow);
    gtk_widget_show(_menuItem);

    // 17. Create the <Change spectrogram parameters> menu item
    _menuItem = gtk_menu_item_new_with_label("Change spectrogram parameters");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectrogramParametersSetting), NULL);
    gtk_widget_show(_menuItem);

    // 18. Create the <Re-assigned spectrogram parameters> menu item
    _menuItem = gtk_menu_item_new_with_label("Re-assigned spectrogram parameters");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(spectrogramParametersSetting), NULL);
    gtk_widget_show(_menuItem);

    // 19. Create the <Recalculate spectrogram> menu item
    _menuItem = gtk_menu_item_new_with_label("Recalculate spectrogram");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(recalculateSpectrogram), NULL);
    gtk_widget_show(_menuItem);

    return _menu;
}

//static GstElement* pipeline = NULL;

static void playWaveFile(const gchar* filename) {

    //GstStateChangeReturn ret;

    //// Create the pipeline
    //pipeline = gst_pipeline_new("audio-player");

    //// Create the source element (in this case, a file source)
    //GstElement* source = gst_element_factory_make("filesrc", "file-source");
    //g_object_set(source, "location", filename, NULL);

    //// Create the decoder element
    //GstElement* decoder = gst_element_factory_make("wavparse", "wav-parser");

    //// Create the audio sink element
    //GstElement* sink = gst_element_factory_make("autoaudiosink", "audio-sink");

    //// Add the elements to the pipeline and link them
    //gst_bin_add_many(GST_BIN(pipeline), source, decoder, sink, NULL);
    //gst_element_link_many(source, decoder, sink, NULL);

    //// Start playing
    //ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    //if (ret == GST_STATE_CHANGE_FAILURE) {
    //    g_printerr("Unable to set the pipeline to the playing state.\n");
    //    gst_object_unref(pipeline);
    //    return;
    //}
}

// Audio Menu : Play between w->e
void audioPlayWE() {

    playWaveFile(g_spectro.wavefile);
}

//------------ Create the Audio Menu --------------------
static GtkWidget* createAudioMenu() {

    GtkWidget* _menu;
    GtkWidget* _menuItem;

    _menu = gtk_menu_new();

    // 1. Create the <Play between w->e> menu item
    _menuItem = gtk_menu_item_new_with_label("Play between w->e");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    g_signal_connect(G_OBJECT(_menuItem), "activate", G_CALLBACK(audioPlayWE), NULL);
    gtk_widget_show(_menuItem);

    // 2. Create the <Play entire file> menu item
    _menuItem = gtk_menu_item_new_with_label("Play entire file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 3. Create the <Abort audio output> menu item
    _menuItem = gtk_menu_item_new_with_label("Abort audio output");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 4. Create the <Record to .wav file> menu item
    _menuItem = gtk_menu_item_new_with_label("Record to .wav file");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    // 5. Create the <Change record parameters> menu item
    _menuItem = gtk_menu_item_new_with_label("Change record parameters");
    gtk_menu_shell_append(GTK_MENU_SHELL(_menu), _menuItem);
    gtk_widget_show(_menuItem);

    return _menu;
}

GtkWidget* createMenus(GtkWidget* do_widget) {

    GtkWidget* box;
    GtkWidget* box1;

    if (!window)
    {
        GtkWidget* _menubar;
        GtkWidget* _menu;
        GtkWidget* _menuItem;
        GtkAccelGroup* _accelGroup;

        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
        gtk_window_set_default_size(GTK_WINDOW(window), WND_WIDTH, WND_HEIGHT);
        gtk_window_set_title(GTK_WINDOW(window), "Spectrum Analyse System");

        gtk_window_set_screen(GTK_WINDOW(window), gtk_widget_get_screen(do_widget));
        g_signal_connect(window, "destroy",
            G_CALLBACK(gtk_widget_destroyed), &window);

        _accelGroup = gtk_accel_group_new();
        gtk_window_add_accel_group(GTK_WINDOW(window), _accelGroup);

        gtk_container_set_border_width(GTK_CONTAINER(window), 0);

        box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_container_add(GTK_CONTAINER(window), box);
        gtk_widget_show(box);

        box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
        gtk_container_add(GTK_CONTAINER(box), box1);
        gtk_widget_show(box1);

        _menubar = gtk_menu_bar_new();
        gtk_box_pack_start(GTK_BOX(box1), _menubar, FALSE, TRUE, 0);
        gtk_widget_show(_menubar);

        // Create the File Menu
        _menu = createFileMenu();

        _menuItem = gtk_menu_item_new_with_label("File");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(_menuItem), _menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(_menubar), _menuItem);
        gtk_widget_show(_menuItem);

        // Create the Time Menu
        _menu = createTimeMenu();

        _menuItem = gtk_menu_item_new_with_label("Time");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(_menuItem), _menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(_menubar), _menuItem);
        gtk_widget_show(_menuItem);

        // Create the Spectrum Menu
        _menu = createSpectrumMenu();

        _menuItem = gtk_menu_item_new_with_label("Spectrum");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(_menuItem), _menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(_menubar), _menuItem);
        gtk_widget_show(_menuItem);

        // Create the Audio Menu
        _menu = createAudioMenu();

        _menuItem = gtk_menu_item_new_with_label("Audio");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(_menuItem), _menu);
        gtk_menu_shell_append(GTK_MENU_SHELL(_menubar), _menuItem);
        gtk_widget_show(_menuItem);
    }

    if (!gtk_widget_get_visible(window))
    {
        gtk_widget_show(window);
    }
    else
    {
        gtk_widget_destroy(window);
        window = NULL;
    }

    return window;
}


// Initialize the system
void initSystem() {

    int _i;
    /* Initialization to make filter skirts go down further (to 0.00062)*/
    for ( _i = 100; _i < SIZCBSKIRT; _i++)
        cbskrt[ _i] = 0.975f * cbskrt[ _i - 1];
    
    // Initialize the g_spectro member variables
    for (_i = 0; _i < NFMAX; _i++) {

        g_spectro.nstart[_i] = 0;
        g_spectro.ntot[_i] = 0;
    }

    g_spectro.iwave = NULL;
    g_spectro.oldsizwin = 0;
    g_spectro.oldwintype = 0;

    g_spectro.sizwin = 256;
    g_spectro.params[NW] = 1;
    g_spectro.sizfft = 256;
    g_spectro.params[FD] = 0;

    g_spectro.type_spec = DFT_MAG;
    g_spectro.option = 'S';

    g_spectro.params[WC] = g_initSpectralParams[0];
    g_spectro.params[WD] = g_initSpectralParams[1]; // sizwin
    g_spectro.params[WS] = g_initSpectralParams[2];
    g_spectro.params[WL] = g_initSpectralParams[3];
    g_spectro.params[NC] = g_initSpectralParams[4];
    g_spectro.params[FD] = g_initSpectralParams[5];
    g_spectro.params[B0] = g_initSpectralParams[6];
    g_spectro.params[B1] = g_initSpectralParams[7];
    g_spectro.params[BS] = g_initSpectralParams[8];
    g_spectro.params[F1] = g_initSpectralParams[9];
    g_spectro.params[F2] = g_initSpectralParams[10];
    g_spectro.params[FL] = g_initSpectralParams[11];
    g_spectro.params[SG] = g_initSpectralParams[12];
    g_spectro.params[NW] = g_initSpectralParams[13];
    g_spectro.params[SD] = g_initSpectralParams[14]; // g_spectro.sizfft; // 512
    g_spectro.params[KN] = g_initSpectralParams[15];

    g_spectro.allmag = NULL;
    g_spectro.allgvs = NULL;
    g_spectro.posti = NULL;
    g_spectro.pix = NULL;
    g_spectro.iwave = NULL;
    g_spectro.locked = 0;

    /* used to set sizwin */
    g_spectro.hamm_in_ms[0] = 25.7f;/*wc*/
    g_spectro.hamm_in_ms[1] = 29.9f;/*wd*/ /*f0*/
    g_spectro.hamm_in_ms[2] = 25.6f;/*ws*/
    g_spectro.hamm_in_ms[3] = 25.6f;/*wl*/

    /* set these when sampling rate is known */

    g_spectro.localtimemax = 0; /* EC 2/27/97 */
    g_spectro.localfreqmax = 0; /* EC 2/27/97 */

    g_spectro.oldwintype = 0; /* cause window to be calulated first time */
    g_spectro.oldsizwin = 0;

    /*spectro->quadcount = 4; when 0 <= quadcount <=3 put 4 spectra on page*/

    g_spectro.history = 0;
    g_spectro.segmode = 1; /* play w to e*/
    g_spectro.param_active = 0; /* widget for changing params not active*/
    g_spectro.labels = NULL;  // init labels list

    g_spectro.focusTime = 0;
    g_spectro.focusTime0 = 0;
    g_spectro.focusIndex0 = 0;

    g_spectro.winms = (float)g_initSpectrogramParams[0] / 10.0f;
    g_spectro.slice = g_initSpectrogramParams[1];
    g_spectro.numav = g_initSpectrogramParams[2];
    g_spectro.msdelta = (float)g_initSpectrogramParams[3];
    g_spectro.specms = (float)g_initSpectrogramParams[4];
    g_spectro.minmag = (float)g_initSpectrogramParams[5];
    g_spectro.maxmag = (float)g_initSpectrogramParams[6];
    g_spectro.xpix = g_initSpectrogramParams[7];
    g_spectro.fdifcoef = g_initSpectrogramParams[8];
    g_spectro.sec = g_initSpectrogramParams[9];

    g_spectro.savemsdelta = g_spectro.msdelta;
    g_spectro.savewinms = g_spectro.winms;

    /* no cursor position yet */
    g_spectro.saveindex = -1;

    // 000
    g_spectro.devheight = 192;
    g_spectro.slice = 128;
    g_spectro.ypix = 3;

    //as this number increases x get shorter relative to y
    g_spectro.spratio = 650;
    g_spectro.numcol = 9;
    g_spectro.cinc = (g_spectro.maxmag - g_spectro.minmag) / (g_spectro.numcol - 2);
    // assume approx 80 pixels per inch
    g_spectro.tickspace = 2;
    g_spectro.axisdist = 8;
    // time offset in pixels
    g_spectro.t0 = 0;
}

// Destroy the system variables
void destroySystem(){

    if (g_spectro.iwave != NULL)
        free(g_spectro.iwave);
}

// Timer event
gboolean timerProcess(gpointer data) {

    if (g_spectralParametersWindow != NULL
        && g_spectralParametersWndControls.changed) {

        g_spectralParametersWndControls.changed = false;

        // Calculate the spectrum
        newSpectrum(&g_spectro);

        // Update the wave form window
        if (g_waveFormWindow != NULL) {

            gtk_widget_queue_draw(g_waveFormWindow);
        }

        // Update the wave sub form window
        if (g_waveFormSubWindow != NULL) {

            gtk_widget_queue_draw(g_waveFormSubWindow);
        }

        // Update the dft window
        if (g_DFTWindow != NULL) {

            gtk_widget_queue_draw(g_DFTWindow);
        }

        // Update the spectrogram window
        if (g_spectrogramWindow != NULL)
            gtk_widget_queue_draw(g_spectrogramWindow);

        // Update the reassign spectrogram window
        if (g_reassignSpectrogramWindow != NULL)
            gtk_widget_queue_draw(g_reassignSpectrogramWindow);
    }

    if (g_spectrogramParametersWindow != NULL
        && g_spectrogramParametersWndControls.changed) {

        g_spectrogramParametersWndControls.changed = false;

        // Update the spectrogram window
        if (g_spectrogramWindow != NULL)
            gtk_widget_queue_draw(g_spectrogramWindow);

        // Update the reassign spectrogram window
        if (g_reassignSpectrogramWindow != NULL)
            gtk_widget_queue_draw(g_reassignSpectrogramWindow);
    }

    return TRUE;
}

int main(int argc, char* argv[])
{

    // Initialize the system
    initSystem();

    gtk_init(&argc, &argv);

    //Create window
    g_mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_position(GTK_WINDOW(g_mainWindow), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(g_mainWindow), WND_WIDTH, WND_HEIGHT);
    gtk_window_set_title(GTK_WINDOW(g_mainWindow), "Spectrum Analyse System");

    // Create a menus
    createMenus(g_mainWindow);

    // Add a timer event with 1000 milliseconds interval (1 second)
    guint _timerId = g_timeout_add(1000, timerProcess, NULL);

    // Main loop
    gtk_main();

    // Destroy the system
    destroySystem();

    // Remove the timer event when exiting the main loop
    g_source_remove(_timerId);

    return 0;
}
