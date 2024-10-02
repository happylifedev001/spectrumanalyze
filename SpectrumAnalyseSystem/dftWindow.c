#include "dftWindow.h"
#include "draw.h"

const int g_cWndBlankWidth = 50;
const int g_cStateWndWidth = 150;

// Update the DFT window
void updateDFTWindow(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos) {

    char _option = spectro->option;

    int _i, _count;
    char _chDatas[255];

    GtkAllocation _allocation;
    gtk_widget_get_allocation(widget, &_allocation);

    float _cx0 = (float)_allocation.x;
    float _cy0 = (float)_allocation.y;

    float _mouseX = mousePos->x - _cx0;
    float _mouseY = mousePos->y - _cy0;

    float _width0 = (float)_allocation.width;
    float _height0 = (float)_allocation.height;

    cairo_set_source_rgb(cr, 0, 0, 0);  // Set color to black
    cairo_set_line_width(cr, 1);

    float _blankX = (float)g_cWndBlankWidth;
    float _blankY = 50.0f;
    float _stateWndWidth = (float)g_cStateWndWidth;
    float _width = _width0 - _blankX - _stateWndWidth;
    float _width2 = _width / 2;
    float _height = _height0 - _blankY * 2;
    float _height2 = _height / 2;
    float _x0 = _blankX;
    float _y0 = _blankY;

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);

    // Draw a graph region
    drawRectangle(cr, _x0, _y0, _width, _height);

    SWndInfo _wndInfo;
    _wndInfo.cr = cr;
    _wndInfo.x0 = _x0;
    _wndInfo.y0 = _y0;
    _wndInfo.width = _width;
    _wndInfo.height = _height;
    _wndInfo.mouseX = _mouseX;
    _wndInfo.mouseY = _mouseY;

    // Set the parameter to draw from a spectro
    int _dataLen = spectro->lenfltr;
    int* _pDatas = spectro->savfltr;
    // Check if a spectro data is valid
    if (_dataLen < 1)
        return;

    if (_pDatas == NULL)
        return;
    float _coefY = (float)_height / (float)SPEC_DB;
    float _coefX = (float)_width / 5.0f;

    // Calculate scaling factor for time axis
    float _timeScalingFactor = (float)_width / (float)_dataLen;

    // Draw the grid y lines
    _count = (int)(SPEC_DB + 0.5) / 10;

    float _x, _y, _x2, _y2;

#ifdef SAS_VS2022
    strcpy_s(_chDatas, 10, "dB");
#else
    strcpy(_chDatas, "dB");
#endif

    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, _x0 - 30, _y0 + 5);
    cairo_show_text(cr, _chDatas);

    _x = _x0;
    _y = _y0;
    _x2 = _x0 + _width;
    float _dbValue = 0.0;
    for (_i = 0; _i < _count; _i++) {

        _y = _y0 + _height - (int)((_dbValue + 5) * _coefY);
        drawLine(cr, _x, _y, _x2, _y);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%d", (int)(_dbValue + 0.5));
#else
        sprintf(_chDatas, "%d", (int)(_dbValue + 0.5));
#endif

        cairo_set_font_size(cr, 14);
        cairo_move_to(cr, _x - 20, _y - 5);
        cairo_show_text(cr, _chDatas);

        _dbValue += 10;
    }

    // Draw the grid x lines
    _count = 10;
    _y = _y0;
    _y2 = _y0 + _height;
    float _frequencyValue = 0.5;
    for (_i = 0; _i < _count; _i++) {

        _x = _x0 + (int)(_frequencyValue * _coefX + 0.5);
        drawLine(cr, _x, _y, _x, _y2);
        _frequencyValue += 0.5;

        if (_i > 0 && _i % 2 == 1) {

#ifdef SAS_VS2022
            sprintf_s(_chDatas, 255, "%d", _i / 2 + 1);
#else
            sprintf(_chDatas, "%d", _i / 2 + 1);
#endif

            cairo_set_font_size(cr, 14);
            cairo_move_to(cr, _x - 5, _y2 + 15);
            cairo_show_text(cr, _chDatas);
        }
    }

#ifdef SAS_VS2022
    strcpy_s(_chDatas, 20, "FREQ ( kHZ )");
#else
    strcpy(_chDatas, "FREQ ( kHZ )");
#endif

    cairo_set_font_size(cr, 14);
    cairo_move_to(cr, _x0 + _width2 - 30, _y0 + _height + 30);
    cairo_show_text(cr, _chDatas);

    // Draw the dft data curve
    spectro->savclip = 0;        
    if (_option == 'a' || _option == 'A' || _option == 'k') {

        drawDft(&_wndInfo, spectro, true);
        drawSmoothedDft(&_wndInfo, spectro, false);
    }

  /* find formants */
    if (_option == 's' || _option == 'S' ||
        _option == 'c' || _option == 'j' ||
        _option == 'T' || _option == 'l') {

        if (_option == 'S' || _option == 'j' || _option == 'T' || _option == 'l')
            drawDft(&_wndInfo, spectro, true);

        if (_option == 'c' || _option == 'j')
            drawSmoothedDft(&_wndInfo, spectro, false);
        else
            drawSmoothedDft(&_wndInfo, spectro, false);

        if (_option == 's' || _option == 'S'
            || _option == 'c' || _option == 'j'
            || _option == 'T' || _option == 'l') {

            // Draw the formant
            getform(spectro);

            drawFormant(&_wndInfo, spectro);
        }
    }

    if(_option == 'd' || _option == 'a' ||
        _option == 'A' || _option == 'k') {

        if (_option == 'd') {

            drawDft(&_wndInfo, spectro, false);
        }
    }

    // Draw the cursor position information
    drawCursorInfo(&_wndInfo, spectro);

    if ((_option == 'c' || _option == 's') &&
        !spectro->history) {

        spectro->history = 1;
        spectro->savspectrum = 0;
    }
}

// Draw the dft
void drawDft(SWndInfo* wndInfo, XSPECTRO* spectro, bool drawSavedData) {

    char _option = spectro->option;

    char _chDatas[255];

    float _x0 = wndInfo->x0;
    float _y0 = wndInfo->y0;
    float _width = wndInfo->width;
    float _height = wndInfo->height;

    float _blankX = (float)g_cWndBlankWidth;

    cairo_t* _cr = wndInfo->cr;

    float _coefY = _height / (float)SPEC_DB;

    int _dataLen;

    int* _pDatas;
    
    if (drawSavedData) {

        _pDatas = spectro->savfltr;
        _dataLen = spectro->lensavfltr;
    }
    else {

        _pDatas = spectro->fltr;
        _dataLen = spectro->lenfltr;
    }

    float _preX, _preY, _x, _y;
    int _i, _count;

    float _timeScalingFactor = _width / (float)_dataLen;
    float _coefX = _width / spectro->sizfft;
    cairo_set_source_rgb(_cr, 0, 0, 0);  // Set color to black
    cairo_set_line_width(_cr, 1);

    _x = 0;
    _y = 0;

    for (_i = 0; _i < _dataLen; _i++) {

        float _data = (float)_pDatas[_i];
        _data /= 10.0;

        _preX = _x;
        _preY = _y;

        _x = (float)_i * _timeScalingFactor;

        _x += _x0;
        _y = _y0 + _height - (_data + 5) * _coefY;

        if (_i > 0)
            drawLine(_cr, _preX, _preY, _x, _y);
    }

    // Draw the toplabel
    _x = _width + _blankX + 10;
    _y = _y0 + 10;

    // win size as ms
    float _winms = (float)spectro->sizwin / spectro->spers * 1000.0f;

    if (spectro->option == 'd') {

        // DFT
#ifdef SAS_VS2022
        strcpy_s(_chDatas, 100, "DFT");
#else
        strcpy(_chDatas, "DFT");
#endif
        cairo_set_source_rgb(_cr, 0.0, 0, 1.0);  // Set color to blue
        cairo_set_font_size(_cr, 18);
        cairo_move_to(_cr, _x, _y);
        cairo_show_text(_cr, _chDatas);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "win:%.1fms", _winms);
#else
        sprintf(_chDatas, "win:%.1fms", _winms);
#endif

        _y += 20;
        cairo_set_source_rgb(_cr, 0.0, 0, 0.0);  // Set color to black
        cairo_set_font_size(_cr, 14);
        cairo_move_to(_cr, _x, _y);
        cairo_show_text(_cr, _chDatas);

        // F0 as Hz
#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "F0 = %d Hz", getf0(spectro->fltr, spectro->sizfft, (int)spectro->spers));
#else
        sprintf(_chDatas, "F0 = %d Hz", getf0(spectro->fltr, spectro->sizfft, (int)spectro->spers));
#endif
        _y += 20;

        cairo_set_font_size(_cr, 14);
        cairo_move_to(_cr, _x, _y);
        cairo_show_text(_cr, _chDatas);

        // Rms as dB
#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "Rms = %d dB", spectro->fltr[spectro->lenfltr] / 10);
#else
        sprintf(_chDatas, "Rms = %d dB", spectro->fltr[spectro->lenfltr] / 10);
#endif

        _y += 20;

        cairo_set_font_size(_cr, 14);
        cairo_move_to(_cr, _x, _y);
        cairo_show_text(_cr, _chDatas);
    } else if (spectro->option == 'k' || spectro->option == 'a' ||
        spectro->option == 'A') {

        cairo_set_font_size(_cr, 18);
        if (spectro->option == 'A') {

#ifdef SAS_VS2022
            strcpy_s(_chDatas, 100, "Avg DFT-spect(A)");
#else
            strcpy(_chDatas, "Avg DFT-spect(A)");
#endif
            cairo_set_font_size(_cr, 14);
        }
        else if (spectro->option == 'k') {

#ifdef SAS_VS2022
            strcpy_s(_chDatas, 100, "Avg DFT-spect(kn)");
#else
            strcpy(_chDatas, "Avg DFT-spect(kn)");
#endif
            cairo_set_font_size(_cr, 14);
        }
        else if (spectro->option == 'a') {

#ifdef SAS_VS2022
            strcpy_s(_chDatas, 100, "Avg DFT-spect(a)");
#else
            strcpy(_chDatas, "Avg DFT-spect(a)");
#endif
            cairo_set_font_size(_cr, 14);
        }

        cairo_set_source_rgb(_cr, 0.0, 0, 1.0);  // Set color to blue
        cairo_move_to(_cr, _x, _y);
        cairo_show_text(_cr, _chDatas);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "win:%.1fms", _winms);
#else
        sprintf(_chDatas, "win:%.1fms", _winms);
#endif

        _y += 20;
        cairo_set_source_rgb(_cr, 0.0, 0, 0.0);  // Set color to black
        cairo_set_font_size(_cr, 14);
        cairo_move_to(_cr, _x, _y);
        cairo_show_text(_cr, _chDatas);

        if (spectro->option == 'A') {

            _count = spectro->avgcount;
            for (_i = 0; _i < _count; _i++) {

#ifdef SAS_VS2022
                sprintf_s(_chDatas, 100, "time %d: %d", _i + 1, spectro->avgtimes[_i]);
#else
                sprintf(_chDatas, "time %d: %d", _i + 1, spectro->avgtimes[_i]);
#endif

                _y += 20;
                cairo_set_source_rgb(_cr, 0.0, 0, 0.0);  // Set color to black
                cairo_set_font_size(_cr, 14);
                cairo_move_to(_cr, _x, _y);
                cairo_show_text(_cr, _chDatas);
            } // end for _i
        } else {

#ifdef SAS_VS2022
            sprintf_s(_chDatas, 100, "start %d", spectro->avgtimes[0]);
#else
            sprintf(_chDatas, "start %d", spectro->avgtimes[0]);
#endif

            _y += 20;
            cairo_set_source_rgb(_cr, 0.0, 0, 0.0);  // Set color to black
            cairo_set_font_size(_cr, 14);
            cairo_move_to(_cr, _x, _y);
            cairo_show_text(_cr, _chDatas);

#ifdef SAS_VS2022
            sprintf_s(_chDatas, 100, "end %d", spectro->avgtimes[1]);
#else
            sprintf(_chDatas, "end %d", spectro->avgtimes[1]);
#endif

            _y += 20;
            cairo_set_source_rgb(_cr, 0.0, 0, 0.0);  // Set color to black
            cairo_set_font_size(_cr, 14);
            cairo_move_to(_cr, _x, _y);
            cairo_show_text(_cr, _chDatas);
        } // end if (spectro.option...
    }
}

// Draw the cursor info
void drawCursorInfo(SWndInfo* wndInfo, XSPECTRO* spectro) {

    char _chDatas[255];

    float _x0 = wndInfo->x0;
    float _y0 = wndInfo->y0;
    float _width = wndInfo->width;
    float _height = wndInfo->height;

    float _mouseX = wndInfo->mouseX;
    float _mouseY = wndInfo->mouseY;

    cairo_t* _cr = wndInfo->cr;

    float _coefY = _height / (float)SPEC_DB;

    int _dataLen = spectro->lenfltr;
    int* _pDatas = spectro->savfltr;

    // Draw cursor
    cairo_set_source_rgb(_cr, 0, 0, 0);  // Set color to black
    cairo_set_line_width(_cr, 1);

    if (_mouseX > _x0 && _mouseY > _y0
        && _mouseX < _x0 + _width && _mouseY < _y0 + _height) {

        float _x = _mouseX;
        float _y = _mouseY;

        //printf("draw cursor %.1f, %.1f\n", _x, _y);

        drawLine( _cr, _x - 5.0f, _y, _x + 5.0f, _y);
        drawLine( _cr, _x, _y - 5.0f, _x, _y + 5.0f);

        int _dataPos = (int)((float)(_mouseX - _x0) / _width * _dataLen);
        if (_dataPos < 0)
            _dataPos = 0;
        if (_dataPos >= _dataLen)
            _dataPos = _dataLen - 1;

        float _data = (float)_pDatas[_dataPos];
        _data /= 10.0f;

        cairo_set_source_rgb( _cr, 0.0, 0, 1.0);  // Set color to blue
        drawLine( _cr, _x, _y0, _x, _y0 + _height);

        cairo_set_source_rgb( _cr, 1.0, 0, 0);  // Set color to red
        _y = _y0 + _height - (_data + 5) * _coefY;
        // Draw the mark of a cursor point
        drawRectangle( _cr, _x - 2, _y - 2, 5, 5);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%.2f", _data);
#else
        sprintf(_chDatas, "%.2f", _data);
#endif

        cairo_set_font_size( _cr, 14);
        cairo_move_to( _cr, _x + 5, _y0 + 15);
        cairo_show_text( _cr, _chDatas);
    }
}

// Draw the formant
void drawFormant( SWndInfo* wndInfo, XSPECTRO* spectro) {

    char _chDatas[200];
    char _option = spectro->option;

    if (_option == 's' || _option == 'S') {

        // Smoothed
#ifdef SAS_VS2022
        strcpy_s(_chDatas, 100, "Smoothed");
#else
        strcpy(_chDatas, "Smoothed");
#endif
    }
    else if (_option == 'c' || _option == 'j'
        || _option == 't' || _option == 'T') {

        // CB
#ifdef SAS_VS2022
        strcpy_s(_chDatas, 100, "CB");
#else
        strcpy(_chDatas, "CB");
#endif
    }
    else if (_option == 'l') {

        // LPC
#ifdef SAS_VS2022
        strcpy_s(_chDatas, 100, "LPC");
#else
        strcpy(_chDatas, "LPC");
#endif
    }
    else {

        return;
    }

    int _i, _j;
    float _x0, _y0, _lenX, _lenY, _preX, _preY,
        _freq, _amp, _x, _y, _width, _height;

    float _blankX = (float)g_cWndBlankWidth;

    float _arox2[2], _aroy3[3];
    SPoint _pt8[8];

    cairo_t* _cr = wndInfo->cr;
    
    _x0 = wndInfo->x0;
    _y0 = wndInfo->y0;
    _width = wndInfo->width;
    _height = wndInfo->height;
    
    float _coefX = _width / 5.0f;
    float _coefY = _height / (float)SPEC_DB;

    // Draw the toplabel
    _x = _width + _blankX + 10;
    _y = _y0 + 100;

    cairo_set_source_rgb(_cr, 0.0, 0, 1.0);  // Set color to blue
    cairo_set_font_size(_cr, 18);
    cairo_move_to(_cr, _x, _y);
    cairo_show_text(_cr, _chDatas);

    // win size as ms
    float _winms = (float)spectro->sizwin / spectro->spers * 1000.0f;

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 100, "win:%.1fms", _winms);
#else
    sprintf(_chDatas, "win:%.1fms", _winms);
#endif

    _y += 20;
    cairo_set_source_rgb(_cr, 0.0, 0, 0.0);  // Set color to blak
    cairo_set_font_size(_cr, 14);
    cairo_move_to(_cr, _x, _y);
    cairo_show_text(_cr, _chDatas);

    cairo_set_source_rgb(_cr, 0.0, 0, 0);  // Set color to blak
    cairo_set_line_width(_cr, 2);

    for (_i = 0; _i < spectro->nforfreq; _i++) {

        int _i_freq = spectro->forfreq[_i];
        int _i_amp = spectro->foramp[_i];

        if (_i_freq < 0) {

            _i_freq = -_i_freq;
        }
        // Rms as dB
#ifdef SAS_VS2022
        sprintf_s(_chDatas, 100, "%d %d", _i_freq, _i_amp / 10);
#else
        sprintf(_chDatas, "%d %d", _i_freq, _i_amp / 10);
#endif

        _y += 20;

        cairo_set_font_size(_cr, 14);
        cairo_move_to(_cr, _x, _y);
        cairo_show_text(_cr, _chDatas);
    }

    // formant arrows
    _lenX = 500;
    _lenY = 500;
    _arox2[0] = _lenX * .008f;
    _arox2[1] = _lenX * .004f;
    _aroy3[0] = _lenY * .02f;
    _aroy3[1] = _lenY * .015f;
    _aroy3[2] = _lenY * .045f;

    for (_i = 0; _i < spectro->nforfreq; _i++) {

        _freq = (float)spectro->forfreq[_i];
        _freq /= 1000.0f;

        _amp = (float)spectro->foramp[_i];
        _amp /= 10.0f;

        bool _asterisk = false;

        if (_freq < 0) {

            _asterisk = true;
            _freq = -_freq;
        }

        _x = _x0 + _freq * _coefX;
        _y = _y0 + _height - (_amp + 5) * _coefY - 10;

        _pt8[0].x = _x;
        _pt8[0].y = _y;
        _pt8[1].x = _x - _arox2[0];
        _pt8[1].y = _y - _aroy3[0];
        _pt8[2].x = _x - _arox2[1];
        _pt8[2].y = _y - _aroy3[1];
        _pt8[3].x = _x - _arox2[1];
        _pt8[3].y = _y - _aroy3[2];
        _pt8[4].x = _x + _arox2[1];
        _pt8[4].y = _y - _aroy3[2];
        _pt8[5].x = _x + _arox2[1];
        _pt8[5].y = _y - _aroy3[1];
        _pt8[6].x = _x + _arox2[0];
        _pt8[6].y = _y - _aroy3[0];
        _pt8[7].x = _x;
        _pt8[7].y = _y;

        for (_j = 0; _j < 8; _j++) {

            _preX = _x;
            _preY = _y;

            _x = _pt8[_j].x;
            _y = _pt8[_j].y;

            if (_j > 0)
                drawLine(_cr, _preX, _preY, _x, _y);
        }
    }
}

// Draw the smoothed dft
void drawSmoothedDft(SWndInfo* wndInfo, XSPECTRO* spectro, bool drawSavedData) {

    float _x0 = wndInfo->x0;
    float _y0 = wndInfo->y0;
    float _width = wndInfo->width;
    float _height = wndInfo->height;

    cairo_t* _cr = wndInfo->cr;

    float _coefY = _height / (float)SPEC_DB;

    int _dataLen;
    int* _pDatas;
    if (drawSavedData) {

        _dataLen = spectro->lensavfltr;
        _pDatas = spectro->savfltr;
    }
    else {

        _dataLen = spectro->lenfltr;
        _pDatas = spectro->fltr;
    }

    int _dataLen2 = _dataLen / 2;

    float _preX, _preY, _x, _y;
    int _i;

    // Calculate scaling factor for time axis
    float _timeScalingFactor = _width / (float)_dataLen;
    float _x1 = 0.0f;
    float _coefX1 = _width / spectro->spers * 2.0f;
    _x = 0;
    _y = 0;
    for (_i = 0; _i < _dataLen; _i++) {

        _preX = _x;
        _preY = _y;

        if (spectro->option == 'c' || spectro->option == 'j' ||
            spectro->option == 'T' || spectro->option == 'l')
            _x = _x0 + (float)(spectro->nfr[_i]) * _coefX1;
        else
            _x = _x0 + (float)_i * _timeScalingFactor;

        //spectro->xsav[_i] = _x1 * _coefX1;
        //_x = _x0 + spectro->xsav[_i] + .5f;

        if (_x <= _x0 + _width) {

            //spectro->ysav[_i] = (float)(spectro->savfltr[_i] / 10.0f) * _coefY;

            float _data = (float)_pDatas[_i] / 10.0f;
            _y = _y0 + _height - (_data + 5) * _coefY;

            if (_i > 0)
                drawLine(_cr, _preX, _preY, _x, _y);

            spectro->savclip++;
        }// clip if drawing 8k on 5k
    } // end for _i
}
