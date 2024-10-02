#pragma once

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "tmwtypes.h"
#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>

#define SAS_VS2022

#define		WND_WIDTH	1400
#define		WND_HEIGHT	200 // 1080

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// Complex struct
typedef struct {

	float real;
	float imag;
} COMPLEX;

// Mouse position struct
typedef struct __SMousePos{

	float x;
	float y;
} SMousePos, * LPSMousePos;

// Window infor
typedef struct __SWndInfo {

	cairo_t* cr;

	float x0;
	float y0;

	float width;
	float height;

	float mouseX;
	float mouseY;
} SWndInfo, * LPSWndInfo;

typedef struct __SPoint {

	float x;
	float y;
} SPoint, * LPSPoint;

// Wave data struct
typedef struct __SWaveData {

	bool isData;

	int channels;
	int frames;
	int sampleRate;

	float* datas;
} SWaveData, * LPSWaveData;

// Spectral parameters window controls
typedef struct __SSpectralParametersWndControls {

	bool changed;

} SSpectralParametersWndControls, *LPSSpectralParametersWndControls;

// Spectrogram parameters window controls
typedef struct __SSpectrogramParametersWndControls {

	bool changed;

} SSpectrogramParametersWndControls, * LPSSpectrogramParametersWndControls;

// Input time window controls
typedef struct __SInputTimeWndControls {

	bool changed;

	unsigned char state;

	bool begineTimeChanged;
	float begineTime;
	bool endTimeChanged;
	float endTime;

} SInputTimeWndControls;

#endif