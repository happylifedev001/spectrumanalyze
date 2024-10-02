#include "waveFormSubWindow.h"
#include "draw.h"
#include "constants.h"
#include <math.h>

// Update the wave form subwindow
void updateWaveFormSubWindow(GtkWidget* widget, cairo_t* cr, XSPECTRO* spectro, SMousePos* mousePos) {

    int _i;
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
    float _x2[2], _y2[2];

    int _frames = spectro->totsamp;
    short* _pDatas = spectro->iwave;
    float _timeLen = (float)_frames / spectro->spers * 1000; // unit is a ms
    float _msFrame = _timeLen / _frames;
    float _blankX = 50;
    float _blankY = 50;
    float _width = _width0 - _blankX * 2;
    float _width2 = _width / 2;
    float _height = _height0 - _blankY * 2;
    float _height2 = _height / 2;
    float _x0 = _blankX;
    float _y0 = _blankY;

    SWndInfo _wndInfo;

    _wndInfo.x0 = _x0;
    _wndInfo.y0 = _y0;
    _wndInfo.width = _width;
    _wndInfo.height = _height;
    _wndInfo.mouseX = _mouseX;
    _wndInfo.mouseY = _mouseY;
    _wndInfo.cr = cr;

    // Calculate scaling factor for time axis
    float _timeScalingFactor = (float)_width / (float)_timeLen;

    float _coefY = _height2 / 6000.0f;

    // Draw a coordinate
    float _stepTime = 100;
    int _scaleXCount = (int)(_timeLen / _stepTime);
    cairo_set_source_rgb(cr, 0, 0, 0);
    // Draw a region for data
    cairo_set_line_width(cr, 2);

    drawRectangle(cr, _x0, _y0, _width, _height);

    cairo_set_font_size(cr, 14);
    cairo_set_line_width(cr, 2);

    int _winSize = spectro->sizwin;
    float _timeLine = _msFrame * _winSize;
    float _timeLine2 = _timeLine / 2.0f;
    float _time0 = spectro->focusTime0;
    int _index0 = spectro->focusIndex0;
    float _timeS = _time0 - _timeLine2;
    float _timeE = _time0 + _timeLine2;
    int _i_timeS = (int)_timeS + 1;
    int _i_timeE = (int)_timeE;
    float _deltaX = (float)_width / _timeLine;

    for (_i = _i_timeS; _i <= _i_timeE; _i++) {

        _x2[0] = _x0 + ((float)_i - _timeS) * _deltaX;
        _y2[0] = _y0 + _height - 20;

        _x2[1] = _x2[0];
        _y2[1] = _y0 + _height;

        cairo_move_to(cr, _x2[0], _y2[0]);
        cairo_line_to(cr, _x2[1], _y2[1]);
        cairo_stroke(cr);

        _x2[0] -= 15;
        _y2[0] = _y0 + _height + 25;
        cairo_move_to(cr, _x2[0], _y2[0]);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%d", _i);
#else
        sprintf(_chDatas, "%d", _i);
#endif

        cairo_show_text(cr, _chDatas);
    }

    // Draw the Y Coordinate
    short _stepValue = 1000;
    int _scaleYCount = 6000 / _stepValue;
    for (_i = 0; _i < _scaleYCount - 1; _i++) {

        short _value = (_i + 1) * _stepValue;
        _x2[0] = _x0;
        _y2[0] = _y0 + _height2 - (float)_value * _coefY;

        _x2[1] = _x0 + 20;
        _y2[1] = _y2[0];

        drawLine(cr, _x2[0], _y2[0], _x2[1], _y2[1]);

        _x2[0] -= 40;
        _y2[0] += 5;
        cairo_move_to(cr, _x2[0], _y2[0]);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%d", _value);
#else
        sprintf(_chDatas, "%d", _value);
#endif

        cairo_show_text(cr, _chDatas);
    }

    for (_i = 0; _i < _scaleYCount - 1; _i++) {

        short _value = -(_i + 1) * _stepValue;
        _x2[0] = _x0;
        _y2[0] = _y0 + _height2 - (float)_value * _coefY;

        _x2[1] = _x0 + 20;
        _y2[1] = _y2[0];

        drawLine(cr, _x2[0], _y2[0], _x2[1], _y2[1]);

        _x2[0] -= 40;
        _y2[0] += 5;
        cairo_move_to(cr, _x2[0], _y2[0]);

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%d", _value);
#else
        sprintf(_chDatas, "%d", _value);
#endif
        cairo_show_text(cr, _chDatas);
    }

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 2);

    // Draw X-axis
    drawLine(cr, _x0, _y0 + _height2, _x0 + _width, _y0 + _height2);

    // Draw Y-axis
    drawLine(cr, _x0 + _width2, _y0, _x0 + _width2, _y0 + _height);

    if (spectro->iwave == NULL
        || spectro->totsamp < 1)
        return;

    // Draw the wave form curve
    drawWaveFormSubWindowCurve(&_wndInfo, spectro);

    // Draw a focus information
    drawWaveFormSubWindowFocusInfo(&_wndInfo, spectro);

    // Draw the cursor position info
    drawWaveFormSubWindowCursorInfo(&_wndInfo, spectro);
}

// Draw the waveform subwindow focus info
void drawWaveFormSubWindowFocusInfo(SWndInfo* wndInfo, XSPECTRO* spectro) {

    char _chDatas[255];

    float _x0 = wndInfo->x0;
    float _y0 = wndInfo->y0;
    float _width = wndInfo->width;
    float _height = wndInfo->height;
    float _mouseX = wndInfo->mouseX;
    float _mouseY = wndInfo->mouseY;
    float _height2 = _height / 2.0f;

    int _frames = spectro->totsamp;
    cairo_t* _cr = wndInfo->cr;

    float _timeLen = (float)_frames / spectro->spers * 1000; // unit is a ms
    float _msFrame = _timeLen / _frames;

    int _winSize = spectro->sizwin;
    float _timeLine = _msFrame * _winSize;
    float _timeLine2 = _timeLine / 2.0f;
    float _time0 = spectro->focusTime0;
    int _index0 = spectro->focusIndex0;
    float _timeS = _time0 - _timeLine2;
    float _timeE = _time0 + _timeLine2;
    int _i_timeS = (int)_timeS + 1;
    int _i_timeE = (int)_timeE;
    float _deltaX = (float)_width / _timeLine;

    // Calculate scaling factor for time axis
    float _timeScalingFactor = (float)_width / (float)_timeLen;

    float _coefX = _width / _timeLine;
    float _coefY = _height2 / 6000.0f;

    float _time = spectro->savetime;

    int _dataPos = (int)(_time / _msFrame + 0.5f);

    if (_dataPos < 0)
        _dataPos = 0;

    if (_dataPos >= _frames)
        _dataPos = _frames - 1;

    cairo_set_source_rgb(_cr, 0.0, 0, 0);  // Set color to black
    cairo_set_line_width(_cr, 1);

    // Draw the cycle
    float _x = _x0;
    float _angle = 2.0f * (float)M_PI * (_time - _timeS) / _timeLine;
    float _deltaAngle = 2.0f * (float)M_PI / _width;
    float _y = 0.0f;
    int _c = 0;
    //for (int _i = 0; _i < _width; _i++) {

    //    float _preX = _x;
    //    float _preY = _y;
    //    _x += 1.0f;
    //    float _data = (float)_height2 * (float)cos(_angle);

    //    _y = _y0 + _height2 - _data;

    //    if (_c > 0)
    //        drawLine(_cr, _preX, _preY, _x, _y);

    //    _angle += _deltaAngle;
    //    _c++;
    //} // end for _i

    cairo_set_source_rgb(_cr, 1.00, 0, 0);  // Set color to red
    _x = _x0 + (_time - _timeS) * _coefX;
    drawLine(_cr, _x, _y0, _x, _y0 + _height);

    short _sData = spectro->iwave[_dataPos];
    float _data = (float)_sData;
    _data *= g_cWaveDataCoef;

    _y = _y0 + _height2 - _data * _coefY;

#ifdef SAS_VS2022
    sprintf_s(_chDatas, 255, "%.2fms(%.1f)", _time, _data);
#else
    sprintf(_chDatas, "%.2fms(%d.1f", _time, _data);
#endif

    cairo_set_source_rgb(_cr, 1.0, 0, 0);  // Set color to red

    drawRectangle(_cr, _x - 2, _y - 2, 5, 5);

    cairo_set_font_size(_cr, 14);
    cairo_move_to(_cr, _x, _y0 - 20.0f);
    cairo_show_text(_cr, _chDatas);
}

// Draw the waveform subwindow cursor info
void drawWaveFormSubWindowCursorInfo(SWndInfo* wndInfo, XSPECTRO* spectro) {

    char _chDatas[255];

    float _x0 = wndInfo->x0;
    float _y0 = wndInfo->y0;
    float _width = wndInfo->width;
    float _height = wndInfo->height;
    float _mouseX = wndInfo->mouseX;
    float _mouseY = wndInfo->mouseY;
    float _height2 = _height / 2.0f;

    int _frames = spectro->totsamp;
    cairo_t* _cr = wndInfo->cr;

    float _timeLen = (float)_frames / spectro->spers * 1000; // unit is a ms
    float _msFrame = _timeLen / _frames;

    int _winSize = spectro->sizwin;
    float _timeLine = _msFrame * _winSize;
    float _timeLine2 = _timeLine / 2.0f;
    float _time0 = spectro->focusTime0;
    int _index0 = spectro->focusIndex0;
    float _timeS = _time0 - _timeLine2;
    float _timeE = _time0 + _timeLine2;
    int _i_timeS = (int)_timeS + 1;
    int _i_timeE = (int)_timeE;
    float _deltaX = (float)_width / _timeLine;

    float _coefY = _height2 / 6000.0f;

    if (_mouseX > _x0 && _mouseY > _y0
        && _mouseX < _x0 + _width && _mouseY < _y0 + _height) {

        float _time = _timeS + (_mouseX - _x0) / _deltaX;

        // Set the time into spectro focus time
        spectro->focusTime = _time;

        int _dataPos = (int)(_time / _msFrame + 0.5f);

        if (_dataPos < 0)
            _dataPos = 0;

        if (_dataPos >= _frames)
            _dataPos = _frames - 1;

        cairo_set_source_rgb(_cr, 0, 0, 1.0);  // Set color to blue
        cairo_set_line_width(_cr, 1);

        float _x = _mouseX;

        drawLine(_cr, _x, _y0, _x, _y0 + _height);

        short _sData = spectro->iwave[_dataPos];
        float _data = (float)_sData;
        _data *= g_cWaveDataCoef;

        float _y = _y0 + _height2 - _data * _coefY;

#ifdef SAS_VS2022
        sprintf_s(_chDatas, 255, "%.2fms(%.1f)", _time, _data);
#else
        sprintf(_chDatas, "%.2fms(%.1f)", _time, _data);
#endif

        cairo_set_source_rgb(_cr, 0, 0, 1.0);  // Set color to blue

        drawRectangle(_cr, _x - 2, _y - 2, 5, 5);

        cairo_set_font_size(_cr, 14);
        cairo_move_to(_cr, _x, _y0 - 20.0f);
        cairo_show_text(_cr, _chDatas);
    }
}

// Draw the wave form subwindow curve
void drawWaveFormSubWindowCurve(SWndInfo* wndInfo, XSPECTRO* spectro) {

    int _i, _c;

    float _x0 = wndInfo->x0;
    float _y0 = wndInfo->y0;
    float _width = wndInfo->width;
    float _height = wndInfo->height;
    float _height2 = _height / 2.0f;

    float _preX, _preY, _x, _y;

    int _index0 = spectro->focusIndex0;
    int _winSize = spectro->sizwin;
    int _winSize2 = _winSize / 2;

    int _frames = spectro->totsamp;
    cairo_t* _cr = wndInfo->cr;

    float _timeLen = (float)_frames / spectro->spers * 1000; // unit is a ms

    float _deltaX = (float)_width / _winSize;

    float _coefY = _height2 / 6000.0f;

    cairo_set_source_rgb(_cr, 0, 0, 0);  // Set color to black
    cairo_set_line_width(_cr, 1);

    _x = 0.0f;
    _y = 0.0f;
    _c = 0;
    int _pos = _index0 - _winSize2 - 1;
    for (_i = 0; _i < _winSize; _i++) {

        _pos++;

        if (_i < 0) {

            _c = 0;
            continue;
        }

        if (_i >= _frames) {

            _c = 0;
            continue;
        }

        _preX = _x;
        _preY = _y;

        short _sData = spectro->iwave[_pos];
        float _data = (float)_sData;
        _data *= g_cWaveDataCoef;

        _x = _x0 + (float)(_i) * _deltaX;
        _y = _y0 + _height2 - _data * _coefY;

        if (_c > 0)
            drawLine(_cr, _preX, _preY, _x, _y);
        _c++;
    }
}
