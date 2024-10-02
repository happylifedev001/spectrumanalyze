
#include "spectrogramParametersWindow.h"

SSpectrogramParametersWndControls g_spectrogramParametersWndControls;

GtkEntry* g_spectrogramParameterEntry7[7];
GtkComboBox* g_spectrogramParameterCombobox2[1];
GtkRadioButton* g_spectrogramParameterRadioButton4[4];
XSPECTRO g_spectro;

// Crate the spectrogram parameters window
void createSpectrogramParametersWindow(GtkWidget* window) {

    if (window == NULL)
        return;

    int _i, _parameter, _index;
    char _chDatas[100];
    GtkWidget* _window = window;

    gtk_window_set_title(GTK_WINDOW(_window), "Spectrogram Parameters Setting");
    gtk_window_set_default_size(GTK_WINDOW(_window), 400, 600);

    // Create a vertical box
    GtkWidget* _vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(_window), _vbox);

    // ------ 1: Create a horizontal box ----------
    GtkWidget* _hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectrogramParameterEntry7[0] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectrogramParams[0] / 10.0);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectrogramParams[0] / 10.0);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectrogramParameterEntry7[0]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectrogramParameterEntry7[0]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox1), GTK_WIDGET(g_spectrogramParameterEntry7[0]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "window in ms (zero-padded to FFT frame length)");
#else
    sprintf(_chDatas, "window in ms (zero-padded to FFT frame length)");
#endif

    GtkWidget* _label1 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox1), _label1, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox1, TRUE, TRUE, 0);

    // --------- 2: Create a horizontal box -----------
    GtkWidget* _hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create a combobox
    g_spectrogramParameterCombobox2[0] = (GtkComboBox*)gtk_combo_box_text_new();
    _index = 0;
    _parameter = 8;
    for (_i = 0; _i < 6; _i++) {

        _parameter *= 2;
#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%d", _parameter);
#else
        sprintf(_chDatas, "%d", _parameter);
#endif

        if (_parameter == g_initSpectrogramParams[1])
            _index = _i;

        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(g_spectrogramParameterCombobox2[0]), NULL, _chDatas);
    }
    GtkComboBox* _combobox = GTK_COMBO_BOX(g_spectrogramParameterCombobox2[0]);
    gtk_combo_box_set_active(_combobox, _index);
    // g_signal_connect(_combobox5, "changed", G_CALLBACK(onComboboxChanged), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox2), GTK_WIDGET(g_spectrogramParameterCombobox2[0]), FALSE, FALSE, 0);

    // Add label to the horizontal box
    GtkWidget* _label2 = gtk_label_new("FFT frame length (samples)");
    gtk_box_pack_start(GTK_BOX(_hbox2), _label2, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox2, TRUE, TRUE, 0);

    // ------- 3: Create a horizontal box --------
    GtkWidget* _hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectrogramParameterEntry7[1] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectrogramParams[2]);
#else
    sprintf(_chDatas, "%d", g_initSpectrogramParams[2]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectrogramParameterEntry7[1]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectrogramParameterEntry7[1]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox3), GTK_WIDGET(g_spectrogramParameterEntry7[1]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "number of FFTs averaged");
#else
    sprintf(_chDatas, "number of FFTs averaged");
#endif

    GtkWidget* _label3 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox3), _label3, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox3, TRUE, TRUE, 0);

    // ------ 4: Create a horizontal box ------
    GtkWidget* _hbox4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectrogramParameterEntry7[2] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectrogramParams[3]);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectrogramParams[3]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectrogramParameterEntry7[2]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectrogramParameterEntry7[2]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox4), GTK_WIDGET(g_spectrogramParameterEntry7[2]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "ms offset of each FFT from previous");
#else
    sprintf(_chDatas, "ms offset of each FFT from previous");
#endif

    GtkWidget* _label4 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox4), _label4, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox4, TRUE, TRUE, 0);

    // ------ 5: Create a horizontal box ------
    GtkWidget* _hbox5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectrogramParameterEntry7[3] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectrogramParams[4]);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectrogramParams[4]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectrogramParameterEntry7[3]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectrogramParameterEntry7[3]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox5), GTK_WIDGET(g_spectrogramParameterEntry7[3]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "ms length of displayed spectrogram");
#else
    sprintf(_chDatas, "ms length of displayed spectrogram");
#endif

    GtkWidget* _label5 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox5), _label5, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox5, TRUE, TRUE, 0);

    // ------ 6: Create a horizontal box ------
    GtkWidget* _hbox6 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectrogramParameterEntry7[4] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectrogramParams[5]);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectrogramParams[5]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectrogramParameterEntry7[4]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectrogramParameterEntry7[4]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox6), GTK_WIDGET(g_spectrogramParameterEntry7[4]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "magnitudes less than this mapped to white");
#else
    sprintf(_chDatas, "magnitudes less than this mapped to white");
#endif

    GtkWidget* _label6 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox6), _label6, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox6, TRUE, TRUE, 0);

    // ------ 7: Create a horizontal box ------
    GtkWidget* _hbox7 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectrogramParameterEntry7[5] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectrogramParams[6]);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectrogramParams[6]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectrogramParameterEntry7[5]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectrogramParameterEntry7[5]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox7), GTK_WIDGET(g_spectrogramParameterEntry7[5]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "magnitudes greater than this mapped to black");
#else
    sprintf(_chDatas, "magnitudes greater than this mapped to black");
#endif

    GtkWidget* _label7 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox7), _label7, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox7, TRUE, TRUE, 0);

    // -------- 8: Create a horizontal box ---------------
    GtkWidget* _hbox8 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create radio buttons
    g_spectrogramParameterRadioButton4[0] = (GtkRadioButton*)gtk_radio_button_new_with_label(NULL, "x1");
    // g_signal_connect(radio_button1, "toggled", G_CALLBACK(on_radiobutton_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox8), GTK_WIDGET(g_spectrogramParameterRadioButton4[0]), FALSE, FALSE, 0);

    g_spectrogramParameterRadioButton4[1] = (GtkRadioButton*)gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[0]), "x2");
    // g_signal_connect(_radioButton14_2, "toggled", G_CALLBACK(on_radiobutton_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox8), GTK_WIDGET(g_spectrogramParameterRadioButton4[1]), FALSE, FALSE, 0);

    GtkRadioButton* _radioButton1 = GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[1]);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton1), TRUE);

    // Add label to the horizontal box
    GtkWidget* _label8 = gtk_label_new("displayed spectrogram scaling");
    gtk_box_pack_start(GTK_BOX(_hbox8), _label8, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox8, TRUE, TRUE, 0);

    // ------ 9: Create a horizontal box ------
    GtkWidget* _hbox9 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectrogramParameterEntry7[6] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectrogramParams[8]);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectrogramParams[8]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectrogramParameterEntry7[6]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectrogramParameterEntry7[6]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox9), GTK_WIDGET(g_spectrogramParameterEntry7[6]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "0 = no pre-emphasis, 100 = exact first difference");
#else
    sprintf(_chDatas, "0 = no pre-emphasis, 100 = exact first difference");
#endif

    GtkWidget* _label9 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox9), _label9, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox9, TRUE, TRUE, 0);

    // -------- 10: Create a horizontal box ---------------
    GtkWidget* _hbox10 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create radio buttons
    g_spectrogramParameterRadioButton4[2] = (GtkRadioButton*)gtk_radio_button_new_with_label(NULL, "ms");
    // g_signal_connect(radio_button1, "toggled", G_CALLBACK(on_radiobutton_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox10), GTK_WIDGET(g_spectrogramParameterRadioButton4[2]), FALSE, FALSE, 0);

    g_spectrogramParameterRadioButton4[3] = (GtkRadioButton*)gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[2]), "sec");
    // g_signal_connect(_radioButton14_2, "toggled", G_CALLBACK(on_radiobutton_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox10), GTK_WIDGET(g_spectrogramParameterRadioButton4[3]), FALSE, FALSE, 0);

    GtkRadioButton* _radioButton2 = GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[3]);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton2), TRUE);

    // Add label to the horizontal box
    GtkWidget* _label10 = gtk_label_new("X axis units");
    gtk_box_pack_start(GTK_BOX(_hbox10), _label10, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox10, TRUE, TRUE, 0);

    // -------------- 11: Create a horizontal box
    GtkWidget* _hbox11 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create and add a apply button
    GtkWidget* _button1 = gtk_button_new_with_label("Apply");
    g_signal_connect(_button1, "clicked", G_CALLBACK(onSpectrogramParamtersWndBtnApply), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox11), _button1, TRUE, TRUE, 0);

    // Create and add a button
    GtkWidget* _button2 = gtk_button_new_with_label("Defaults");
    g_signal_connect(_button2, "clicked", G_CALLBACK(onSpectrogramParamtersWndBtnDefault), window);
    gtk_box_pack_start(GTK_BOX(_hbox11), _button2, TRUE, TRUE, 0);

    // Create and add a button
    GtkWidget* _button3 = gtk_button_new_with_label("Cancel");
    g_signal_connect(_button3, "clicked", G_CALLBACK(onSpectrogramParamtersWndClose), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox11), _button3, TRUE, TRUE, 0);

    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox11, TRUE, TRUE, 0);

    gtk_widget_show_all(_window);

    gtk_widget_grab_focus(_button1);
}

// Close the window
void onSpectrogramParamtersWndClose(GtkWidget* widget, gpointer data) {

    // Get the parent window
    GtkWidget* _window = gtk_widget_get_toplevel(widget);

    gtk_widget_destroy(_window);
}

// Update the spectrogram parameters window
void updateSpectrogramParametersWindow() {

    int _i, _c2, _index;
    char _chDatas[250];

    // 1
    if (g_spectrogramParameterEntry7[0] != NULL) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%.1f", g_spectro.winms);
#else
        sprintf(_chDatas, "%.1f", g_spectro.winms);
#endif
        gtk_entry_set_text(g_spectrogramParameterEntry7[0], _chDatas);
    }

    // 2
    if (g_spectrogramParameterCombobox2[0] != NULL) {

        _index = 0;
        _c2 = 8;
        for (_i = 0; _i < 6; _i++) {

            _c2 *= 2;
            if (_c2 == g_spectro.slice) {

                _index = _i;
                break;
            }
        }

        gtk_combo_box_set_active(g_spectrogramParameterCombobox2[0], _index);
    }

    // 3
    if (g_spectrogramParameterEntry7[1] != NULL) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%d", g_spectro.numav);
#else
        sprintf(_chDatas, "%d", g_spectro.numav);
#endif
        gtk_entry_set_text(g_spectrogramParameterEntry7[1], _chDatas);
    }

    // 4
    if (g_spectrogramParameterEntry7[2] != NULL) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%.1f", g_spectro.msdelta);
#else
        sprintf(_chDatas, "%.1f", g_spectro.msdelta);
#endif
        gtk_entry_set_text(g_spectrogramParameterEntry7[2], _chDatas);
    }

    // 5
    if (g_spectrogramParameterEntry7[3] != NULL) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%.1f", g_spectro.specms);
#else
        sprintf(_chDatas, "%.1f", g_spectro.specms);
#endif
        gtk_entry_set_text(g_spectrogramParameterEntry7[3], _chDatas);
    }

    // 6
    if (g_spectrogramParameterEntry7[4] != NULL) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%.1f", g_spectro.minmag);
#else
        sprintf(_chDatas, "%.1f", g_spectro.minmag);
#endif
        gtk_entry_set_text(g_spectrogramParameterEntry7[4], _chDatas);
    }

    // 7
    if (g_spectrogramParameterEntry7[5] != NULL) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%.1f", g_spectro.maxmag);
#else
        sprintf(_chDatas, "%.1f", g_spectro.maxmag);
#endif
        gtk_entry_set_text(g_spectrogramParameterEntry7[5], _chDatas);
    }

    // 8
    if (g_spectro.xpix == 1) {

        GtkRadioButton* _radioButton = GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[0]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton), TRUE);
    }
    else {

        GtkRadioButton* _radioButton = GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[1]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton), TRUE);
    }

    // 9
    if (g_spectrogramParameterEntry7[6] != NULL) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%d", g_spectro.fdifcoef);
#else
        sprintf(_chDatas, "%d", g_spectro.fdifcoef);
#endif
        gtk_entry_set_text(g_spectrogramParameterEntry7[6], _chDatas);
    }

    // 10
    if (g_spectro.sec == 0) {

        GtkRadioButton* _radioButton = GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[2]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton), TRUE);
    }
    else {

        GtkRadioButton* _radioButton = GTK_RADIO_BUTTON(g_spectrogramParameterRadioButton4[3]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton), TRUE);
    }
}

// Get the spectrogram parameters from window UI
bool getSpectrogramParametersWindow() {

    int _i, _index, _number;

    g_spectro.winms = (float)atof(gtk_entry_get_text(g_spectrogramParameterEntry7[0]));

    _index = gtk_combo_box_get_active(g_spectrogramParameterCombobox2[0]);
    _number = 16;
    for (_i = 0; _i < _index; _i++) {

        _number *= 2;
    }
    g_spectro.slice = _number;
    g_spectro.numav = atoi(gtk_entry_get_text(g_spectrogramParameterEntry7[1]));
    g_spectro.msdelta = (float)atof(gtk_entry_get_text(g_spectrogramParameterEntry7[2]));
    g_spectro.specms = (float)atof(gtk_entry_get_text(g_spectrogramParameterEntry7[3]));
    g_spectro.minmag = (float)atof(gtk_entry_get_text(g_spectrogramParameterEntry7[4]));
    g_spectro.maxmag = (float)atof(gtk_entry_get_text(g_spectrogramParameterEntry7[5]));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(g_spectrogramParameterRadioButton4[0])))
        g_spectro.xpix = 1;
    else
        g_spectro.xpix = 2;
    g_spectro.fdifcoef = atoi(gtk_entry_get_text(g_spectrogramParameterEntry7[6]));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(g_spectrogramParameterRadioButton4[2])))
        g_spectro.sec = 0;
    else
        g_spectro.sec = 1;

    return true;
}

// Apply button event process
void onSpectrogramParamtersWndBtnApply(GtkWidget* widget, gpointer data) {

    getSpectrogramParametersWindow();

    g_spectrogramParametersWndControls.changed = true;
}

// Default button event process
void onSpectrogramParamtersWndBtnDefault(GtkWidget* widget, gpointer data) {

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

    updateSpectrogramParametersWindow();

    gtk_widget_queue_draw(widget);
}