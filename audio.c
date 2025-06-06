#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "audio.h"
#include <setjmp.h>
#include "Serial_ISR.h"
#define Pink1 B4
#define Orange2 D5
#define Blue1 A4
#define Green1 F4
#define LGreen2 F5
#define Yellow2 E5
#define Purple2 As5
#define Pink2 B5
#define Blue2 A5
#define Orange1 C5
#define LGreen1 F5
#define Purple1 As5
#define Green2 F5
#define Red1 E5
#define Yellow1 E5
#define White Rest

const notes melody[] = { // golden wind starting melody
  Pink1, Rest, Pink1, Rest, Pink1, Blue1, Rest, Pink1, Rest,
  Orange2, Rest, Pink1, Rest, Green1, Blue1, Rest, Pink1,Rest,
  Pink1, Rest, Pink1, Rest, Pink1, Blue1, Rest, Pink1, Rest,
  Orange2, Rest, Pink1, Rest, Green1, Blue1, Rest, Pink1,Rest
   
};



int len[5] = {duration, duration/2, duration/4, duration/6, duration/8};

int freq[] = {
    0,     // Rest
    // Octave 3
    130, 138, 146, 155, 164, 174, 185, 196, 207, 220, 233, 246,
    // Octave 4
    261, 277, 293, 311, 329, 349, 369, 392, 415, 440, 466, 493,
    // Octave 5
    523, 554, 587, 622, 659, 698, 739, 783, 830, 880, 932, 987,
    // Octave 6
    1046, 1108, 1174, 1244, 1318, 1396, 1480, 1568, 1661, 1760, 1864, 1975,
    // Octave 7
    2093, 2217, 2349, 2489, 2637, 2793, 2960, 3136, 3322, 3520, 3729, 3951
};

jmp_buf jump_target;

// play note but allow interrupts
void play_note_jump(notes note, lengths length)
{
  if (note == Rest) {
    TPM0->MOD = 0;
  } else {
    TPM0->MOD = ClockFreq / freq[note];
  }
  TPM0_C0V = TPM0->MOD * 0.3;
  if (endAudio == 0x01) {
    off_audio();
    endAudio = 0;
    longjmp(jump_target, 1);
  }
  osDelay(42*len[length]);
}
// play mode but dont allow interrupts
void play_note(notes note, lengths length)
{
  if (note == Rest) {
    TPM0->MOD = 0;
  } else {
    TPM0->MOD = ClockFreq / freq[note];
  }
  TPM0_C0V = TPM0->MOD * 0.3;
  osDelay(42*len[length]);
}



void off_audio()
{
  TPM0->MOD = 0;
  TPM0_C0V = 0;
}

void connected_tune(void)
{
  for (int i = 0; i < 2; i++) {
    play_note(D5, quaver);
    play_note(A5, quaver);
  }
  off_audio();
}

void background_tune(void)
{
  //melody loop
  // Might need to loop indefinitely
  while (setjmp(jump_target) == 0) {

for (int i = 0; i < sizeof(melody); i++) { // calls array of golden wind melody
	if(melody[i]== Rest){
		play_note_jump(melody[i],semiquaver);
	}
	else{
  play_note_jump(melody[i],semiquaver);
	}
}

  }
}

void ending_tune(void) // mario ending tune
{
  //bar 1
  play_note(G3, triplet);
  play_note(C4, triplet);
  play_note(E4, triplet);
  
  play_note(G4, triplet);
  play_note(C5, triplet);
  play_note(E5, triplet);
  
  play_note(G5, crotchet);
  
  play_note(E4, crotchet);
  
  //bar 2
  play_note(Ab3, triplet);
  play_note(C4, triplet);
  play_note(Eb4, triplet);
  
  play_note(Ab4, triplet);
  play_note(C5, triplet);
  play_note(Eb5, triplet);
  
  play_note(Ab5, crotchet);
  
  play_note(Eb5, crotchet);
  
  //bar 3
  play_note(Bb3, triplet);
  play_note(D4, triplet);
  play_note(F4, triplet);
  
  play_note(Bb4, triplet);
  play_note(D5, triplet);
  play_note(F5, triplet);
  
  play_note(Bb5, crotchet);
  
  play_note(Bb5, triplet);
  play_note(Bb5, triplet);
  play_note(Bb5, triplet);
  
  //bar 4
  play_note(C6, crotchet);
  off_audio();
}

void initAudio(void)
{
  // Enable clock for PortD
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
  
  // TPM - Timer/PWM Module
  // Clear MUX Mask, then setting it to TPM0_CH0
  // Port D Pin 0
  PORTD->PCR[PTD0_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD0_Pin] |= PORT_PCR_MUX(4);
  
  // Clear MUX Mask, then setting it to TPM0_CH1
  // Port D Pin 1
  PORTD->PCR[PTD1_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[PTD1_Pin] |= PORT_PCR_MUX(4);
  
  // Enable clock for TPM0
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
  
  // Clear TPMSRC, then setting it to MCGFLLCLK or MCGFLLCLK/2
  //Multipurpose Clock Generator Fixed Frequency Loop Clock
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
  
  //48MHz / 128 = 375kHz
  //375kHz / 50Hz = 7500
  TPM0->MOD = 7500;
  
  // 80% Duty Cycle
  // MOD Count * 0.8 = 6000
  TPM0_C0V = 0;
    
  // Clear CMOD and PS, then setting CMOD to increment counting
  // and PS to divide by 128
  TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  
  // Clear Centre Aligned PWM which sets LPTPM to up counting mode
  // LPTPM - Low Power TPM
  TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
  
  // Clearing TPM1_C0SC, then setting MSB:MSA and ELSB:ELSA to 10
  // which is Edge-Aligned PWM with High true pulses (clear output on match, set output on reload)
  TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}
