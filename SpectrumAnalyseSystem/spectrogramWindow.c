#include "spectrogramWindow.h"
#include "draw.h"
#include "constants.h"
#include <math.h>

static float g_spectrogramStartMs = -1.0f;
static unsigned char g_spectrogramStartFlag = 0;

// Update the spectrogram window
void updateSpectrogramWindow(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos) {


    drawSpectrogram(widget, cr, spectro, mousePos);
    drawGrid(widget, cr, spectro, mousePos);
    drawMouseInformation( widget, cr, spectro, mousePos);
}

// Draw the spectrogram
void drawSpectrogram(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos) {

    GtkAllocation _allocation;
    gtk_widget_get_allocation(widget, &_allocation);

    int _wndWidth = _allocation.width;
    int _wndHeight = _allocation.height;    

    bool _validY, _validX;
    int _i, _j, _c, _count, _grayColor, _x0, _y0, _width, _height;

    float _specms = spectro->specms;
    float _totms = (float)spectro->totsamp / spectro->spers * 1000.0f;
    _count = (int)(_totms / 100.0f);
    float _totms2 = _totms - _count * 100.0f;
    if (fabs(_totms2 - _totms) < 1E-5)
        _totms2 = _totms;
    else
        _totms2 = (_count + 1) * 100.0f;

    float _saveTime = spectro->savetime;
    float _ms, _ms0;
    _ms0 = 0.0f;
    if (g_spectrogramStartMs < 0.0f) {

        _ms = _saveTime - _specms / 2.0f;
        _count = (int)(_ms / 100.0f);
        _ms0 = _count * 100.0f;
        if (_ms0 < 0.0f)
            _ms0 = 0.0f;
        else if (_ms0 + _specms >= _totms2)
            _ms0 = _totms2 - _specms;

        g_spectrogramStartMs = _ms0;
    }
    else {

        if (_saveTime < g_spectrogramStartMs || _saveTime >= g_spectrogramStartMs + _specms) {

            _ms = _saveTime - _specms / 2.0f;
            _count = (int)(_ms / 100.0f);
            _ms0 = _count * 100.0f;

            if (_ms0 < 0.0f)
                _ms0 = 0.0f;
            else if (_ms0 + _specms >= _totms2)
                _ms0 = _totms2 - _specms;

            g_spectrogramStartMs = _ms0;
        }
        else {

            _ms0 = g_spectrogramStartMs;
        }
    }

    _x0 = 50;
    _y0 = 50;

    _height = _wndHeight - _y0 * 2 - 10;
    g_spectrogramStartFlag = 0;
    if (fabs(_ms0) < 1E-5) {

        _width = _wndWidth - _x0 * 2 - 10;
        _x0 += 10;
    }
    else if (fabs(_ms0 + _specms - _totms2) < 1E-5) {

        _width = _wndWidth - _x0 * 2;
        g_spectrogramStartFlag = 2;
    } else {

        _width = _wndWidth - _x0 * 2;
        g_spectrogramStartFlag = 1;
    }

    int _xgram = spectro->xgram;
    int _wymax = spectro->wymax;

    GdkPixbuf* _pixbuf;
    _pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, _width, _height);
    gdk_pixbuf_fill(_pixbuf, 0xFFFFFFFF);

    int _nChannels = gdk_pixbuf_get_n_channels(_pixbuf);
    guchar* _pPixels = gdk_pixbuf_get_pixels(_pixbuf);
    int _rowstride = gdk_pixbuf_get_rowstride(_pixbuf);

    int _delta = _rowstride - _width * 3;

    float _coefY = (float)_wymax / _height;
    float _coefMs = (float)_specms / _width;
    float _coefX = _xgram / _totms;
    _c = 0;
    for (_i = 0; _i < _height; _i++) {

        float _y = (float)_i * _coefY;

        int _iy = (int)_y;
        
        _validY = true;

        if (_iy < 0)
            _validY = false;
        if (_iy >= _wymax)
            _validY = false;

        for (_j = 0; _j < _width; _j++) {

            _ms = (float)_j * _coefMs + _ms0;
            float _x = _ms * _coefX;

            _validX = true;

            int _ix = (int)_x;
            if (_ix < 0)
                _validX = false;
            if (_ix >= _xgram)
                _validX = false;

            _grayColor = 255;
            if (_validX && _validY) {

                int _pos = (_ix + 1) * _wymax - _iy;
                char _gv = spectro->allgvs[_pos];

                int _igv = _gv;
                _grayColor = ((10 - _igv) * 255 / 10);
                if (_grayColor < 0)
                    _grayColor = 0;
                if (_grayColor > 255)
                    _grayColor = 255;
            }

            _pPixels[_c] = _grayColor;
            _c++;
            _pPixels[_c] = _grayColor;
            _c++;
            _pPixels[_c] = _grayColor;
            _c++;
        } // end for _j

        _c += _delta;
    } // end for _i

    gdk_cairo_set_source_pixbuf(cr, _pixbuf, _x0, _y0);

    cairo_paint(cr);

    g_object_unref(_pixbuf);
}

// Draw the spectrogram
void drawSpectrogram2(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos) {

    int _wndWidth = spectro->xgram;
    int _wndHeight = spectro->wymax;

    GdkPixbuf* _pixbuf;
    _pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, _wndWidth, _wndHeight);
    gdk_pixbuf_fill(_pixbuf, 0xFFFFFFFF);

    guchar* _pPixels;
    g_object_get(G_OBJECT(_pixbuf), "pixels", &_pPixels, NULL);

    int _count = _wndWidth * _wndHeight;

    int _i, _j, _c, _grayColor;

    int _wymax = _wndHeight;
    int _xgram = _wndWidth;
    _c = 0;
    for (_i = 0; _i < _wymax; _i++) {

        for (_j = 0; _j < _xgram; _j++) {

            char _gv = spectro->allgvs[(_j + 1) * _wymax - _i];

            int _igv = _gv;
            _grayColor = ((10 - _igv) * 255 / 10);
            if (_grayColor < 0)
                _grayColor = 0;
            if (_grayColor > 255)
                _grayColor = 255;

            if (_i == 0 && _j == 0)
                continue;

            _pPixels[_c] = _grayColor;
            _c++;
            _pPixels[_c] = _grayColor;
            _c++;
            _pPixels[_c] = _grayColor;
            _c++;
        }
        _c++;
    }

    // Draw the pixbuf onto the cairo surface
    gdk_cairo_set_source_pixbuf(cr, _pixbuf, 50, 50);
    cairo_paint(cr);
}

// Draw the grid
void drawGrid(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos) {

    char _chDatas[255];
    GtkAllocation _allocation;
    gtk_widget_get_allocation(widget, &_allocation);

    int _wndWidth = _allocation.width;
    int _wndHeight = _allocation.height;

    int _i, _count, _x0, _y0, _width, _height;
    float _specms = spectro->specms;
    float _saveTime = spectro->savetime;
    float _totms = (float)spectro->totsamp / spectro->spers * 1000.0f;

    float _x, _y, _x2, _dx, _dy;

    _x0 = 50;
    _y0 = 50;

    _width = _wndWidth - _x0 * 2;
    _height = _wndHeight - _y0 * 2;

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);
    cairo_set_font_size(cr, 12);
    drawRectangle(cr, (float)_x0, (float)_y0, (float)_width, (float)_height);

    float _startLen = 10.0f;
    if (g_spectrogramStartFlag > 0)
        _startLen = 0.0f;

    _dx = (_width - _startLen) / _specms * 100.0f;
    _dy = (_height - 10.0f) / 5.0f;
    _count = (int)(_specms / 100.0f + 0.5f);
    _x = (float)_x0 + _startLen;
    _y = (float)(_y0 + _height);
    cairo_set_line_width(cr, 1);
    for (_i = 0; _i < _count; _i++) {

        drawLine(cr, _x, _y, _x, _y - 10);

        if (_i == 0)
            cairo_move_to(cr, _x - 2, _y + 20);
        else
            cairo_move_to(cr, _x - 10, _y + 20);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%d", (int)(g_spectrogramStartMs + (float)_i * 100.0f + 0.5f));
#else
        sprintf(_chDatas, "%d", _i);
#endif

        cairo_show_text(cr, _chDatas);

        _x += _dx;
    }

    _count = 6;
    _x = (float)_x0;
    _x2 = (float)(_x0 + _width);
    _y = (float)(_y0 + _height) - 10.0f;
    for (_i = 0; _i < _count; _i++) {

        drawLine(cr, _x, _y, _x2, _y);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%d", _i);
#else
        sprintf(_chDatas, "%d", _i);
#endif

        cairo_move_to(cr, _x - 15, _y + 2);
        cairo_show_text(cr, _chDatas);

        _y -= _dy;
    }

    cairo_move_to(cr, _x0 - 40, _y0 + 10);
    cairo_show_text(cr, "akH");

    if (g_spectrogramStartFlag == 0) {

        cairo_move_to(cr, _x0 + 5, _y0 - 10);
        cairo_show_text(cr, "w");

        double _dashes[] = { 10.0, 10.0 };
        int _numDashes = sizeof(_dashes) / sizeof(_dashes[0]);
        cairo_set_dash(cr, _dashes, _numDashes, 0);
        drawLine(cr, (float)_x0 + _startLen, (float)_y0, (float)_x0 + _startLen, (float)(_y0 + _height));
    }

    if (g_spectrogramStartMs + _specms > _totms) {

        _x = (float)_x0 + _startLen + (float)(_width - _startLen) * (_totms - g_spectrogramStartMs) / _specms;

        cairo_move_to(cr, _x, _y0 - 10);
        cairo_show_text(cr, "e");

        double _dashes[] = { 10.0, 10.0 };
        int _numDashes = sizeof(_dashes) / sizeof(_dashes[0]);
        cairo_set_dash(cr, _dashes, _numDashes, 0);
        drawLine(cr, _x, (float)_y0, _x, (float)(_y0 + _height));
    }

    _x = (float)_x0;
    if (g_spectrogramStartFlag == 0)
        _x += 10.0f;
    _x += _width * (_saveTime - g_spectrogramStartMs) / _specms;
    cairo_set_source_rgb(cr, 0, 0, 255);
    cairo_set_dash(cr, NULL, 0, 0);
    drawLine(cr, _x, (float)_y0, _x, (float)(_y0 + _height));

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 16);
    cairo_move_to(cr, (float)_x0 + _width / 2 - 20, (float)(_y0 + _height) + 40);
    cairo_show_text(cr, "TIME(ams)");
}

// Draw the mouse position information
void drawMouseInformation(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos) {

    GtkAllocation _allocation;
    gtk_widget_get_allocation(widget, &_allocation);

    float _cx0 = (float)_allocation.x;
    float _cy0 = (float)_allocation.y;

    cairo_set_source_rgb(cr, 0, 0, 0);  // Set color to black
    cairo_set_line_width(cr, 1);

    float _mouseX = mousePos->x - _cx0;
    float _mouseY = mousePos->y - _cy0;

    float _x = _mouseX;
    float _y = _mouseY;
    drawLine(cr, _x - 5.0f, _y, _x + 5.0f, _y);
    drawLine(cr, _x, _y - 5.0f, _x, _y + 5.0f);
}