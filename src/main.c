#include "stm32f0xx.h"
#include <stdlib.h>
#include <math.h>   // for MA_PI
#include "midi.h"
#include "init.h"
#include "step-array.h"

#define N 1000
#define RATE 20000

void TIM2_IRQHandler(void);
void note_on(int time, int chan, int key, int velo);
void note_off(int time, int chan, int key, int velo);


struct {
    uint8_t note;
    uint8_t chan;
    uint8_t volume;
    int     step;
    int     offset;
} voice[15];

short int wavetable[N];
extern uint8_t midifile[];


int main(void)
{
    init_DAC();
    init_TIM6();
    initWave();
    MIDI_Player *mp = midi_init(midifile);
    // The default rate for a MIDI file is 2 beats per second
    // with 48 ticks per beat.  That's 500000/48 microseconds.
    init_TIM2(10417);
    // Nothing else to do at this point.
    for(;;)
        asm("wfi");
}


void TIM2_IRQHandler(void)
{
    // TODO: Remember to acknowledge the interrupt right here!
    TIM2->SR &= ~TIM_SR_UIF;
    midi_play();
}


void TIM6_DAC_IRQHandler(void){
    TIM6->SR &= ~TIM_SR_UIF;
    int sample = 0;
    for(int i=0; i < sizeof voice / sizeof voice[0]; i++) {
        sample += (wavetable[voice[i].offset>>16] * voice[i].volume) /*<< 4*/;
        voice[i].offset += voice[i].step;
        if ((voice[i].offset >> 16) >= sizeof wavetable / sizeof wavetable[0])
            voice[i].offset -= (sizeof wavetable / sizeof wavetable[0]) << 16;
    }
    sample = (sample >> 16) + 2048;
    DAC->DHR12R1 = sample;
}


void note_on(int time, int chan, int key, int velo)
{
  for(int i=0; i < sizeof voice / sizeof voice[0]; i++)
    if (voice[i].step == 0) {
      // configure this voice to have the right step and volume
        if(velo == 0){
            note_off(time, chan, key, velo);
        }
        else{
            voice[i].step = step[key];
            voice[i].note = key;
            voice[i].volume = velo;
        }
      break;
    }
}


void note_off(int time, int chan, int key, int velo)
{
  for(int i=0; i < sizeof voice / sizeof voice[0]; i++)
    if (voice[i].step != 0 && voice[i].note == key) {
      // turn off this voice
      voice[i].step = 0;
      voice[i].note = 0;
      voice[i].volume = 0;
      break;
    }
}

void initWave(void){
    for(int i=0; i < N; i++)
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
    return;
}

