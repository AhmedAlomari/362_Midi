#include "stm32f0xx.h"
#include "midi.h"
#include "init.h"
#define N 1000
#define RATE 20000

#ifndef FACTOR_H
#define FACTOR_H

void init_DAC(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 3<<(2*4); //PA4 output

    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR &= ~DAC_CR_EN1;
    DAC->CR &= ~DAC_CR_BOFF1;
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= (7<<3);
    DAC->CR |= DAC_CR_EN1;
}

void init_TIM6(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 0;//48 - 1;
    TIM6->ARR = (48000000/RATE) - 1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    //TIM6->CR1 &= ~(TIM_CR1_CEN);
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;
}

void init_TIM2(int val){
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 48 - 1;
    TIM2->ARR = val - 1;
    TIM2->CR1 &= ~(TIM_CR1_ARPE);
    TIM2->DIER |= 1;
    TIM2->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM2_IRQn;
}

void set_tempo(int time, int value, const MIDI_Header *hdr)
{
    // This assumes that the TIM2 prescaler divides by 48.
    // It sets the timer to produce an interrupt every N
    // microseconds, where N is the new tempo (value) divided by
    // the number of divisions per beat specified in the MIDI header.
    TIM2->ARR = value/hdr->divisions - 1;
}

#endif
