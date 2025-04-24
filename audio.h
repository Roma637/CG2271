#define PTD0_Pin 0
#define PTD1_Pin 1
#define ClockFreq 375000
#define SIZE 59
#define duration 24

//note bank
typedef enum {
    Rest,  // Silence
    // Octave 3
    C3, Cs3, D3, Ds3, E3, F3, Fs3, G3, Gs3, Ab3, A3, Bb3, B3,
    // Octave 4
    C4, Cs4, D4, Ds4, Eb4, E4, F4, Fs4, G4, Gs4, Ab4, A4, Bb4, B4,
    // Octave 5
    C5, Cs5, D5, Ds5, Eb5, E5, F5, Fs5, G5, Gs5, Ab5, A5,As5, Bb5, B5,
    // Octave 6
    C6, Cs6, D6, Ds6, E6, F6, Fs6, G6, Gs6, A6, Bb6, B6,
    // Octave 7
    C7, Cs7, D7, Ds7, E7, F7, Fs7, G7, Gs7, A7, Bb7, B7
} notes;

typedef enum {minim, crotchet, quaver, triplet, semiquaver} lengths;

void play_note(notes note, lengths length);
void connected_tune(void);
void background_tune(void);
void ending_tune(void);
void initAudio(void);
void off_audio(void);
