#define PTD0_Pin 0
#define PTD1_Pin 1
#define ClockFreq 375000
#define SIZE 25
#define duration 24

typedef enum {Rest, 
                                   G3, Ab3,     Bb3,
              C4, D4, Eb4, E4, F4, G4, Ab4,  A4, Bb4, B4, 
              C5, D5, Eb5, E5, F5, G5, Ab5, A5, Bb5, B5, 
              C6} notes;
typedef enum {minim, crotchet, quaver, triplet, semiquaver} lengths;

void play_note(notes note, lengths length);
void connected_tune(void);
void background_tune(void);
void ending_tune(void);
void initAudio(void);
void off_audio(void);
