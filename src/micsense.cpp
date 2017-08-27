/*
 * 
 */

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <portaudio.h>

#define SAMPLE_RATE 44100

using namespace std;

int main(void) {
    PaError err = paNoError;
    PaStream *stream;
    PaStreamParameters inputParam, outputParam;
    

    err = Pa_Initialize();
    if(err != paNoError) {
        Pa_Terminate();
        cout << "PortAudio Init failed\n";
        return -1;   
    }

    //err = Pa_OpenStream(&stream, &inputParam, &outputParam, SAMPLE_RATE, FRAMES_PER_CALLBACK, paClipOff, 
                        // TODO: some callback,
                        // TODO: some config);
    err = Pa_StartStream(stream);

    cout << "exiting";
    err = Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;    
}

