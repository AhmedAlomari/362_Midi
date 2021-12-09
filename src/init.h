#ifndef _INIT_
#define _INIT_
#include "stm32f0xx.h"
#include "midi.h"
#include <math.h>   // for MA_PI

void init_DAC(void);
void init_TIM6(void);
void init_TIM2(int val);
void set_tempo(int time, int value, const MIDI_Header *hdr);
void initWave(void);

#endif
