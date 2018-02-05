/*
 * 
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sndfile.h>
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
    SNDFILE *wavFile;
    SF_INFO sfinfo;
} ioData;

static int io_callback( const void *inputBuffer, void *outputBuffer,
			unsigned long framesPerBuffer,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData ) {
    ioData *config = (ioData*)userData;
    float *in  = (float*)inputBuffer;
    float *out = (float*)outputBuffer;
    //sf_count_t readCounter; // sf_count_t can be replaced with an int 

    config->numCallbacks += 1;
    cout << timeInfo->currentTime << endl;

    /*memset(out, 0, sizeof(float) * framesPerBuffer * config->numOutputChannels);
    readCounter = sf_read_float(config->wavFile, out, framesPerBuffer * config->numOutputChannels);

    if (readCounter < framesPerBuffer) {
        return paComplete;
    }
    */
    return paContinue;
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
        cout << "PortAudio Init failed\nHit any key to quit" << endl;
        getchar();
        return -1;   
    }

    /* libsndfile setup */
    if(!(config->wavFile = sf_open("input.wav", SFM_READ, &config->sfinfo))) {
        cout << "Could not open input.wav" << endl;
        sf_perror(NULL);
        return 1;
    }

    /* Configure input param */
    cout << "Init Input..." << endl;
    inputParam.device = INPUT_DEVICE;

    if (inputParam.device == paNoDevice) {
        cout << "ERROR: Input device not found. Exiting.." << endl;
        Pa_Terminate();
    } 

    inputDeviceInfo = Pa_GetDeviceInfo(inputParam.device);
    cout << "Input Sampling Rate: " << inputDeviceInfo->defaultSampleRate << endl;
    fflush(stdout);
    inputParam.channelCount = inputDeviceInfo->maxInputChannels;
    inputParam.sampleFormat = INPUT_FORMAT | paNonInterleaved;
    inputParam.suggestedLatency = Pa_GetDeviceInfo(inputParam.device)->defaultLowInputLatency;
    inputParam.hostApiSpecificStreamInfo = NULL;

    /* Configure output param */
    cout << "Init Output..." << endl;
    outputParam.device = OUTPUT_DEVICE;
    
    if (outputParam.device == paNoDevice) {
        cout << "ERROR: Output device not found. Exiting.." << endl;
        Pa_Terminate();
    }
    
    outputDeviceInfo = Pa_GetDeviceInfo(outputParam.device);
    cout << "Output Sampling Rate: " << outputDeviceInfo->defaultSampleRate << endl;
    fflush(stdout);
    outputParam.channelCount = outputDeviceInfo->maxOutputChannels;
    outputParam.sampleFormat = OUTPUT_FORMAT | paNonInterleaved;
    outputParam.suggestedLatency = Pa_GetDeviceInfo(outputParam.device)->defaultLowOutputLatency;
    outputParam.hostApiSpecificStreamInfo = NULL;

    /* Define config */
    config->framesPerCallback = FRAMES_PER_CALLBACK;
    config->numInputChannels = inputParam.channelCount;
    config->numOutputChannels = outputParam.channelCount;
    config->numCallbacks = 0;

    /* Open stream and start */
    err = Pa_OpenStream(&stream, &inputParam, &outputParam, 
			SAMPLE_RATE, 
			config->framesPerCallback, paClipOff, 
                        io_callback,
                        config);
    err = Pa_StartStream(stream);
    
    /* Start stream failed */
    if (err != paNoError) {
        cout << "ERROR: paNoError at Pa_StartStream()" << endl;
        fflush(stdout);
        if (stream) {
            Pa_AbortStream(stream);
            Pa_CloseStream(stream);
        }
        Pa_Terminate();
        sf_close(config->wavFile);
        fprintf(stderr, "Error number: %d\n", err);
        fprintf(stderr, "Error msg: %s\n", Pa_GetErrorText(err));
        cout << "Hit any key to quit." << endl;
        fflush(stdout);
        getchar();
        return -1;
    }
    
    /* It works */
    c = getchar();
    
    /* User exit */
    cout << "exiting" << endl;
    cout << "number of successful callbacks = " << config->numCallbacks << endl;
    err = Pa_CloseStream(stream);
    Pa_Terminate();
    sf_close(config->wavFile);    

    return 0;    
}

