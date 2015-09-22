//sound engine header
#ifndef _AUDIO_H                // Prevent multiple definitions if this 
#define _AUDIO_H                // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

#include <xaudio2.h>
//#include <xact3.h>
#include "constants.h"

class Sound
{
    // properties
  private:
    //IXACT3Engine* xactEngine;   //pointer to XACT sound engine
    //IXACT3WaveBank* waveBank;   //pointer to XACT wave bank
    //IXACT3SoundBank* soundBank; //pointer to XACT sound bank
    //XACTINDEX cueI;             //XACT sound index
    void* mapWaveBank;          //handle for memory mapped sound file. Call UnmapViewOfFile() to release file
    void* soundBankData;
    bool coInitialized;         //set true if coInitialize is successful

  public:
    //constructor
    Sound();

    //destructor
    virtual ~Sound();

    //initialize sound engine
    HRESULT initialize();

    //perform periodic sound engine tasks.
    void run();

    //play sound specified by cue from sound bank.
    //if cue does not exist no error occurs, no sound will be played.
    void playCue(const char cue[]);

    //stop a playing sound specified by cue from sound bank.
    //if cue does not exist no error occurs.
    void stopCue(const char cue[]);
};

#endif