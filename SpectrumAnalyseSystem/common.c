#include <stdio.h>
#include <stdlib.h>

#include "common.h"

// Allocate a wave data buffer with a channels and frame count
bool allocWaveData(SWaveData* waveData) {

	if (waveData == NULL)
		return false;

	int _count = waveData->channels * waveData->frames;
	if (_count < 1)
		return false;

	waveData->datas = (float*)malloc(_count * sizeof(float));
	waveData->isData = false;

	return true;
}

// Free a wave data buffer
bool freeWaveData(SWaveData* waveData) {

	if (waveData == NULL)
		return false;

	if (waveData->datas != NULL)
		free(waveData->datas);

	waveData->isData = false;
	return true;
}
