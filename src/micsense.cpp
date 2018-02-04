/*
 * 
 */

#include <iostream>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include <portaudio.h>

#define SAMPLE_RATE 44100
#define FRAMES_PER_CALLBACK 2048

#define INPUT_DEVICE Pa_GetDefaultInputDevice()
#define INPUT_FORMAT paFloat32
typedef float INPUT_SAMPLE;

#define OUTPUT_DEVICE Pa_GetDefaultOutputDevice()
#define OUTPUT_FORMAT paFloat32
typedef float OUTPUT_SAMPLE;

using namespace std;

typedef struct ioData {
    int numInputChannels;
    int numOutputChannels;
    int framesPerCallback;
    int numCallbacks;
} ioData;

static int io_callback( const void *inputBuffer, void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData ) {
    ioData *config = (ioData*)userData;
    float *in  = (float*)inputBuffer;
    float *out = (float*)outputBuffer;
    int framesToCalc = framesPerBuffer;
    unsigned long i = 0;
    short finished = 0;

    float inputFFTBuffer[framesPerBuffer - 1][2];
    float *p_FFTBuffer = inputFFTBuffer[0]; 
    
    config->numCallbacks += 1;

    cout << ("%.2f\n", timeInfo->currentTime);
    /*
    if (data->sampsToGo < framesPerBuffer) {
        framesToCalc = data->sampsToGo;
        finished = paComplete;
    }
    else {
        finished = paContinue;
    }


    while (i < framesToCalc) {
        *out++ = *in++ * data->
    }*/

    return finished;
}

int main(void) {
    PaError err = paNoError;
    PaStream *stream;
    PaStreamParameters inputParam, outputParam;
    const PaDeviceInfo *inputDeviceInfo, *outputDeviceInfo;
    ioData CONFIG;
    ioData *config = &CONFIG;
    short c;

    cout << "Init\n";
    err = Pa_Initialize();
    if(err != paNoError) {
        Pa_Terminate();
        cout << "PortAudio Init failed\nHit any key to quit\n";
        getchar();
        return -1;   
    }

    /* Configure input param */
    cout << "Init Input...\n";
    inputParam.device = INPUT_DEVICE;
    if (inputParam.device == paNoDevice) {
        cout << "ERROR: Input device not found. Exiting..\n";
        Pa_Terminate();
    } 
    inputDeviceInfo = Pa_GetDeviceInfo(inputParam.device);
    cout << ("Input Sampling Rate: %.2f\n", inputDeviceInfo->defaultSampleRate);
    fflush(stdout);
    inputParam.channelCount = inputDeviceInfo->maxInputChannels;
    inputParam.sampleFormat = INPUT_FORMAT | paNonInterleaved;
    inputParam.suggestedLatency = Pa_GetDeviceInfo(inputParam.device)->defaultLowInputLatency;
    inputParam.hostApiSpecificStreamInfo = NULL;


    /* Define config */
    config->framesPerCallback = FRAMES_PER_CALLBACK;
    config->numInputChannels = inputParam.channelCount;
    // numOutputChannel here later
    config->numCallbacks = 0;

    /* Open stream and start */
    err = Pa_OpenStream(&stream, &inputParam, NULL, 
			SAMPLE_RATE, 
			config->framesPerCallback, paClipOff, 
                        io_callback,
                        config);
    err = Pa_StartStream(stream);
    
    /* Start stream failed */
    if (err != paNoError) {
        cout << "ERROR: paNoError at Pa_StartStream()\n";
        fflush(stdout);
        if (stream) {
            Pa_AbortStream(stream);
            Pa_CloseStream(stream);
        }
        Pa_Terminate();
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error msg: %s\n", Pa_GetErrorText(err));
        cout << "Hit any key to quit.\n";
        fflush(stdout);
        getchar();
        return -1;
    }
    
    /* It works */
    c = getchar();
    
    /* User exit */
    cout << "exiting";
    cout << ("number of successful callbacks = %d\n", config->numCallbacks);
    err = Pa_CloseStream(stream);
    Pa_Terminate();
    
    return 0;    
}

