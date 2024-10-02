
#include "spectralParametersWindow.h"

SSpectralParametersWndControls g_spectralParametersWndControls;

GtkEntry* g_spectralParameterEntry16[16];
GtkComboBox* g_spectralParameterCombobox2[2];
GtkRadioButton* g_spectralParameterRadioButton2[2];
XSPECTRO g_spectro;

// Crate the spectro parameters window
void createSpectralParametersWindow( GtkWidget * window) {

    if (window == NULL)
        return;

    int _i, _c, _index;
    char _chDatas[100];
    GtkWidget* _window = window;

    gtk_window_set_title(GTK_WINDOW(_window), "Spectrum Parameters Setting");
    gtk_window_set_default_size(GTK_WINDOW(_window), 400, 600);

    // Create a vertical box
    GtkWidget* _vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add(GTK_CONTAINER(_window), _vbox);

    // ------ 1: Create a horizontal box ----------
    GtkWidget* _hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[0] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectralParams[0] / 10.0);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectralParams[0] / 10.0);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[0]), _chDatas);
    gtk_widget_set_size_request( GTK_WIDGET( g_spectralParameterEntry16[0]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox1), GTK_WIDGET(g_spectralParameterEntry16[0]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%.1f : %.1f)", "# ms in Hamming window for critical band spectrum", (float)g_mnSpectralParams[0] / 10.0,
                        (float)g_mxSpectralParams[0] / 10.0f);
#else
    sprintf(_chDatas, "%s (%.1f : %.1f)", "# ms in Hamming window for critical band spectrum", (float)g_mnSpectralParams[0] / 10.0,
                        (float)g_mxSpectralParams[0] / 10.0f);
#endif

    GtkWidget* _label1 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox1), _label1, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox1, TRUE, TRUE, 0);

    // ------- 2: Create a horizontal box --------
    GtkWidget* _hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[1] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectralParams[1] / 10.0);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectralParams[1] / 10.0);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[1]), _chDatas);
    gtk_widget_set_size_request( GTK_WIDGET( g_spectralParameterEntry16[1]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox2), GTK_WIDGET( g_spectralParameterEntry16[1]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%.1f : %.1f)", "# ms in Hamming window for DFT spectrum", (float)g_mnSpectralParams[1] / 10.0,
        (float)g_mxSpectralParams[1] / 10.0f);
#else
    sprintf(_chDatas, "%s (%.1f : %.1f)", "# ms in Hamming window for DFT spectrum", (float)g_mnSpectralParams[1] / 10.0,
        (float)g_mxSpectralParams[1] / 10.0f);
#endif

    GtkWidget* _label2 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox2), _label2, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox2, TRUE, TRUE, 0);

    // ------ 3: Create a horizontal box ------
    GtkWidget* _hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[2] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectralParams[2] / 10.0);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectralParams[2] / 10.0);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[2]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[2]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox3), GTK_WIDGET( g_spectralParameterEntry16[2]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%.1f : %.1f)", "# ms in Hamming window for smoothed spectrum", (float)g_mnSpectralParams[2] / 10.0,
        (float)g_mxSpectralParams[2] / 10.0f);
#else
    sprintf(_chDatas, "%s (%.1f : %.1f)", "# ms in Hamming window for smoothed spectrum", (float)g_mnSpectralParams[2] / 10.0,
        (float)g_mxSpectralParams[2] / 10.0f);
#endif

    GtkWidget* _label3 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox3), _label3, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox3, TRUE, TRUE, 0);

    // -------- 4: Create a horizontal box ---------
    GtkWidget* _hbox4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[3] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%.1f", (float)g_initSpectralParams[3] / 10.0);
#else
    sprintf(_chDatas, "%.1f", (float)g_initSpectralParams[3] / 10.0);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[3]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[3]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox4), GTK_WIDGET( g_spectralParameterEntry16[3]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%.1f : %.1f)", "# ms in Hamming window for LPC spectru", (float)g_mnSpectralParams[3] / 10.0,
        (float)g_mxSpectralParams[3] / 10.0f);
#else
    sprintf(_chDatas, "%s (%.1f : %.1f)", "# ms in Hamming window for LPC spectru", (float)g_mnSpectralParams[3] / 10.0,
        (float)g_mxSpectralParams[3] / 10.0f);
#endif

    GtkWidget* _label4 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox4), _label4, TRUE, TRUE, 0);

    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox4, TRUE, TRUE, 0);

    // --------- 5: Create a horizontal box -----------
    GtkWidget* _hbox5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create a combobox
    g_spectralParameterCombobox2[0] = (GtkComboBox*)gtk_combo_box_text_new();
    _index = 0;
    _c = 0;
    for (_i = g_mnSpectralParams[4]; _i <= g_mxSpectralParams[4]; _i++) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%d", _i);
#else
        sprintf(_chDatas, "%d", _i);
#endif

        if (_i == g_initSpectralParams[4])
            _index = _c;

        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(g_spectralParameterCombobox2[0]), NULL, _chDatas);

        _c++;
    }
    GtkComboBox* _combobox = GTK_COMBO_BOX(g_spectralParameterCombobox2[0]);
    gtk_combo_box_set_active( _combobox, _index);
    // g_signal_connect(_combobox5, "changed", G_CALLBACK(onComboboxChanged), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox5), GTK_WIDGET( g_spectralParameterCombobox2[0]), FALSE, FALSE, 0);

    // Add label to the horizontal box
    GtkWidget* _label5 = gtk_label_new("# of coefficients in analysis for LPC");
    gtk_box_pack_start(GTK_BOX(_hbox5), _label5, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox5, TRUE, TRUE, 0);

    // --------- 6: Create a horizontal box ----------
    GtkWidget* _hbox6 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[5] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[ 5]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[5]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[5]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[5]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox6), GTK_WIDGET( g_spectralParameterEntry16[5]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "First difference. Pre-emphasis if = 100", g_mnSpectralParams[5], g_mxSpectralParams[5]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "First difference. Pre-emphasis if = 100", g_mnSpectralParams[5], g_mxSpectralParams[5]);
#endif

    GtkWidget* _label6 = gtk_label_new(_chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox6), _label6, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox6, TRUE, TRUE, 0);

    // --------- 7: Create a horizontal box ---------
    GtkWidget* _hbox7 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[6] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[6]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[6]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[6]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[6]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox7), GTK_WIDGET( g_spectralParameterEntry16[6]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "Bandwidth of CB filter at lowest frequency", g_mnSpectralParams[6], g_mxSpectralParams[6]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "Bandwidth of CB filter at lowest frequency", g_mnSpectralParams[6], g_mxSpectralParams[6]);
#endif

    GtkWidget* _label7 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox7), _label7, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox7, TRUE, TRUE, 0);

    // ------ 8: Create a horizontal box -----------
    GtkWidget* _hbox8 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[7] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[7]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[7]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[7]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[7]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox8), GTK_WIDGET( g_spectralParameterEntry16[7]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "Bandwidth of CB filter at 1000 Hz", g_mnSpectralParams[7], g_mxSpectralParams[7]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "Bandwidth of CB filter at 1000 Hz", g_mnSpectralParams[7], g_mxSpectralParams[7]);
#endif

    GtkWidget* _label8 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox8), _label8, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox8, TRUE, TRUE, 0);

    // ------- 9: Create a horizontal box --------
    GtkWidget* _hbox9 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[8] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[8]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[8]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[8]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[8]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox9), GTK_WIDGET( g_spectralParameterEntry16[8]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "Bandwidth of smoothed spectrum filter", g_mnSpectralParams[8], g_mxSpectralParams[8]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "Bandwidth of smoothed spectrum filter", g_mnSpectralParams[8], g_mxSpectralParams[8]);
#endif

    GtkWidget* _label9 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox9), _label9, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox9, TRUE, TRUE, 0);

    // ----- 10: Create a horizontal box -------
    GtkWidget* _hbox10 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[9] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[9]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[9]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[9]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[9]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox10), GTK_WIDGET( g_spectralParameterEntry16[9]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "Center frequency of first CB filter", g_mnSpectralParams[9], g_mxSpectralParams[9]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "Center frequency of first CB filter", g_mnSpectralParams[9], g_mxSpectralParams[9]);
#endif

    GtkWidget* _label10 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox10), _label10, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox10, TRUE, TRUE, 0);

    // ----- 11: Create a horizontal box --------
    GtkWidget* _hbox11 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[10] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[10]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[10]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[10]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[10]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox11), GTK_WIDGET( g_spectralParameterEntry16[10]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "Center frequency of second CB filter", g_mnSpectralParams[10], g_mxSpectralParams[10]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "Center frequency of second CB filter", g_mnSpectralParams[10], g_mxSpectralParams[10]);
#endif

    GtkWidget* _label11 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox11), _label11, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox11, TRUE, TRUE, 0);

    // 12: Create a horizontal box
    GtkWidget* _hbox12 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[11] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[11]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[11]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[11]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[11]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox12), GTK_WIDGET( g_spectralParameterEntry16[11]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "Freq at which CB goes from linear to log", g_mnSpectralParams[11], g_mxSpectralParams[11]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "Freq at which CB goes from linear to log", g_mnSpectralParams[11], g_mxSpectralParams[11]);
#endif

    GtkWidget* _label12 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox12), _label12, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox12, TRUE, TRUE, 0);

    // ------ 13: Create a horizontal box ------------
    GtkWidget* _hbox13 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[12] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[12]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[12]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[12]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET( g_spectralParameterEntry16[12]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox13), GTK_WIDGET( g_spectralParameterEntry16[12]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "Spectrum gain in dB times 10", g_mnSpectralParams[12], g_mxSpectralParams[12]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "Spectrum gain in dB times 10", g_mnSpectralParams[12], g_mxSpectralParams[12]);
#endif

    GtkWidget* _label13 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox13), _label13, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox13, TRUE, TRUE, 0);

    // -------- 14: Create a horizontal box ---------------
    GtkWidget* _hbox14 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create radio buttons
    g_spectralParameterRadioButton2[0] = (GtkRadioButton*)gtk_radio_button_new_with_label(NULL, "Yes");
    // g_signal_connect(radio_button1, "toggled", G_CALLBACK(on_radiobutton_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox14), GTK_WIDGET( g_spectralParameterRadioButton2[0]), FALSE, FALSE, 0);

    g_spectralParameterRadioButton2[1] = (GtkRadioButton*)gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(g_spectralParameterRadioButton2[0]), "No");
    // g_signal_connect(_radioButton14_2, "toggled", G_CALLBACK(on_radiobutton_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox14), GTK_WIDGET( g_spectralParameterRadioButton2[1]), FALSE, FALSE, 0);

    GtkRadioButton* _radioButton = GTK_RADIO_BUTTON(g_spectralParameterRadioButton2[1]);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton), TRUE);

    // Add label to the horizontal box
    GtkWidget* _label14 = gtk_label_new("Use window if =1, no windowing if =0");
    gtk_box_pack_start(GTK_BOX(_hbox14), _label14, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox14, TRUE, TRUE, 0);

    // ---------- 15: Create a horizontal box ---------- 
    GtkWidget* _hbox15 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create a combobox
    g_spectralParameterCombobox2[1] = (GtkComboBox*)gtk_combo_box_text_new();

    _index = 0;
    _c = 0;
    _i = g_mnSpectralParams[14];
    while( _i <= g_mxSpectralParams[14]) {

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%d", _i);
#else
        sprintf(_chDatas, "%d", _i);
#endif

        if (_i == g_initSpectralParams[14])
            _index = _c;

        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(g_spectralParameterCombobox2[1]), NULL, _chDatas);

        _i *= 2;
        _c++;
    }
    
    _combobox = GTK_COMBO_BOX(g_spectralParameterCombobox2[1]);
    gtk_combo_box_set_active(_combobox, _index);

    // g_signal_connect(_combobox5, "changed", G_CALLBACK(onComboboxChanged), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox15), GTK_WIDGET( g_spectralParameterCombobox2[1]), FALSE, FALSE, 0);

    // Add label to the horizontal box
    GtkWidget* _label15 = gtk_label_new("Size of DFT (256/512 for others)");
    gtk_box_pack_start(GTK_BOX(_hbox15), _label15, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox15, TRUE, TRUE, 0);

    // -------- 16: Create a horizontal box --------------
    GtkWidget* _hbox16 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    // Create the entry
    g_spectralParameterEntry16[15] = (GtkEntry*)gtk_entry_new();

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%d", g_initSpectralParams[15]);
#else
    sprintf(_chDatas, "%d", g_initSpectralParams[15]);
#endif

    gtk_entry_set_text(GTK_ENTRY(g_spectralParameterEntry16[15]), _chDatas);
    gtk_widget_set_size_request(GTK_WIDGET(g_spectralParameterEntry16[15]), 100, 30);
    gtk_box_pack_start(GTK_BOX(_hbox16), GTK_WIDGET( g_spectralParameterEntry16[15]), FALSE, FALSE, 0);

    // Add label to the horizontal box

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "%s (%d : %d)", "ms on either side of cursor to be averaged", g_mnSpectralParams[15], g_mxSpectralParams[15]);
#else
    sprintf(_chDatas, "%s (%d : %d)", "ms on either side of cursor to be averaged", g_mnSpectralParams[15], g_mxSpectralParams[15]);
#endif

    GtkWidget* _label16 = gtk_label_new( _chDatas);
    gtk_box_pack_start(GTK_BOX(_hbox16), _label16, TRUE, TRUE, 0);
    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox16, TRUE, TRUE, 0);

    // 17: Create a horizontal box
    GtkWidget* _hbox17 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    // Create and add a button
    GtkWidget*  _button1 = gtk_button_new_with_label("Apply to Current Waveform");
    g_signal_connect(_button1, "clicked", G_CALLBACK(onBtnApply), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox17), _button1, TRUE, TRUE, 0);

    // Create and add a button
    GtkWidget* _button2 = gtk_button_new_with_label("Apply to All Waveforms");
    g_signal_connect(_button2, "clicked", G_CALLBACK(onBtnAllApply), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox17), _button2, TRUE, TRUE, 0);

    // Create and add a button
    GtkWidget* _button3 = gtk_button_new_with_label("Defaults");
    g_signal_connect(_button3, "clicked", G_CALLBACK(onBtnDefault), window);
    gtk_box_pack_start(GTK_BOX(_hbox17), _button3, TRUE, TRUE, 0);

    // Create and add a button
    GtkWidget* _button4 = gtk_button_new_with_label("Cancel");
    g_signal_connect(_button4, "clicked", G_CALLBACK(onClose), NULL);
    gtk_box_pack_start(GTK_BOX(_hbox17), _button4, TRUE, TRUE, 0);

    // Add the horizontal box to the vertical box
    gtk_box_pack_start(GTK_BOX(_vbox), _hbox17, TRUE, TRUE, 0);

    gtk_widget_show_all(_window);

    gtk_widget_grab_focus(_button1);
}

// Close the window
void onClose(GtkWidget* widget, gpointer data) {

    // Get the parent window
    GtkWidget* _window = gtk_widget_get_toplevel(widget);

    gtk_widget_destroy(_window);
}

// Update the spectral parameters window
void updateSpectralParametersWindow() {

    int _i, _c2, _index;
    char _chDatas[255];

    for (_i = 0; _i < 16; _i++) {

        GtkEntry* _entry = g_spectralParameterEntry16[_i];
        int _number = g_spectro.params[_i];

        _index = 0;

        switch (_i) {

        case NC:

            if (_number >= g_mnSpectralParams[NC] && _number <= g_mxSpectralParams[NC]) {

                _index = _number - g_mnSpectralParams[NC];
            
                GtkComboBox* _combobox = GTK_COMBO_BOX(g_spectralParameterCombobox2[0]);
                gtk_combo_box_set_active(_combobox, _index);
            }
            break;
        case NW:

            _index = 0;
            _c2 = g_mnSpectralParams[NW];
            while (_c2 <= g_mxSpectralParams[NW]) {

                if (_c2 == _number) {

                    GtkComboBox* _combobox = GTK_COMBO_BOX(g_spectralParameterCombobox2[1]);
                    gtk_combo_box_set_active(_combobox, _index);
                    break;
                }

                _c2 *= 2;
                _index++;
            }
            break;
        case SD:

            if (_number == 1) {

                GtkRadioButton* _radioButton = GTK_RADIO_BUTTON(g_spectralParameterRadioButton2[0]);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton), TRUE);
            }
            else {

                GtkRadioButton* _radioButton = GTK_RADIO_BUTTON(g_spectralParameterRadioButton2[1]);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(_radioButton), TRUE);
            }
            break;
        default:

            if (_entry == NULL)
                continue;

            if (_i < 5) {

#ifdef SAS_VS2022
                sprintf_s(_chDatas, 100, "%.1f", (float)_number / 10.0f);
#else
                sprintf(_chDatas, "%.1f", (float)_number / 10.0f);
#endif
            }
            else {
#ifdef SAS_VS2022
                sprintf_s(_chDatas, 100, "%d", _number);
#else
                sprintf(_chDatas, "%d", _number);
#endif
            }

            gtk_entry_set_text(_entry, _chDatas);
            break;
        }
    }
}

// Get the spectro parameters from window UI
bool getSpectralParametersWindow(){

    int _i, _count, _c, _c2, _index, _number;
    _count = 16;

    for (_i = 0; _i < _count; _i++) {

        GtkEntry* _entry = g_spectralParameterEntry16[_i];
        const char* _szValue = gtk_entry_get_text(_entry);

        switch (_i) {

        case NC:

            _index = gtk_combo_box_get_active( g_spectralParameterCombobox2[0]);
            _number = g_mnSpectralParams[NC] + _index;
            if (_number >= g_mnSpectralParams[NC] && _number <= g_mxSpectralParams[NC]) {

                g_spectro.params[NC] = _number;
            }
            break;
        case NW:

            _index = gtk_combo_box_get_active(g_spectralParameterCombobox2[1]);
            _c2 = g_mnSpectralParams[NW];
            _c = 0;
            while (_c2 <= g_mxSpectralParams[NW]) {

                if (_c == _index) {

                    g_spectro.params[NW] = _c2;
                    break;
                }

                _c2 *= 2;
                _c++;
            }
            break;
        case SD:

            if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(g_spectralParameterRadioButton2[0]))) {
                
                g_spectro.params[SD] = 1;
            }
            else {

                g_spectro.params[SD] = 0;
            }
             
            break;
        default:

            if (_entry == NULL || _szValue == NULL || strlen(_szValue) == 0)
                continue;

            if (_i < 5) {

                g_spectro.params[_i] = (int)(atof( _szValue) * 10.0f + 0.5f);
            }
            else {

                g_spectro.params[_i] = atoi(_szValue);
            }
            break;
        }
    }

    return true;
}

// Apply button event process
void onBtnApply(GtkWidget* widget, gpointer data) {

    getSpectralParametersWindow();

    g_spectralParametersWndControls.changed = true;
}

// All Apply button event process
void onBtnAllApply(GtkWidget* widget, gpointer data) {

    getSpectralParametersWindow();

    g_spectralParametersWndControls.changed = true;
}

// Default button event process
void onBtnDefault(GtkWidget* widget, gpointer data) {

    int _i, _count;

    _count = 16;
    for (_i = 0; _i < 16; _i++) {

        g_spectro.params[_i] = g_initSpectralParams[_i];
    }

    updateSpectralParametersWindow();

    gtk_widget_queue_draw(widget);
}