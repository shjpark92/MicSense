/*
 * 2 input and output channels
 * i/o both interleaved
 * 128 frames per callback
 */

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sndfile.h>
#include <portaudio.h>

//#define SAMPLE_RATE 44100
//#define SAMPLE_RATE 33000 
#define SAMPLE_RATE 22000
#define FRAMES_PER_CALLBACK 4096 
#define BUFFER_LEN 1024 

#define INPUT_DEVICE Pa_GetDefaultInputDevice()
#define INPUT_FORMAT paFloat32
typedef float INPUT_SAMPLE;

#define OUTPUT_DEVICE Pa_GetDefaultOutputDevice()
#define OUTPUT_FORMAT paFloat32
typedef float OUTPUT_SAMPLE;

using namespace std;

typedef struct ioData {
    int isInputInterleaved;
    int isOutputInterleaved;
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
    INPUT_SAMPLE *in;
    OUTPUT_SAMPLE *out;
    short inChannel = 0, outChannel = 0;
    short inputDone = 0, outputDone = 0;
    short finished = 0;
    unsigned int sample;
    
    config->numCallbacks += 1;
    cout << timeInfo->currentTime << endl;

    if (inputBuffer == NULL) {
        return finished;
    }

    /*while (!(inputDone && outputDone)) {
        in = ((INPUT_SAMPLE**)inputBuffer)[inChannel];
        out = ((OUTPUT_SAMPLE**)outputBuffer)[outChannel];
        
        for (sample = 0; sample < framesPerBuffer; ++sample) {
            *out = (*in);
            in++;
            out++;
        } 
        if (inChannel< (config->numInputChannels - 1)) {
            inChannel++;
        } else {
             inputDone = 1;
        }

        if (outChannel< (config->numOutputChannels - 1)) {
            outChannel++;
        } else {
             outputDone = 1;
        }

    }

    return finished;
    */
    out = ((OUTPUT_SAMPLE**)outputBuffer)[outChannel];
    
    memset(out, 0x00, framesPerBuffer);
    long readcount = 0;
    readcount = sf_read_float(config->wavFile, out, framesPerBuffer * 2);
    if (readcount <= 0) {
        cout << "File ended" << endl;
        return paComplete;
    } 
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

    cout << "Init" << endl;
    err = Pa_Initialize();
    if(err != paNoError) {
        Pa_Terminate();
        cout << "PortAudio Init failed\nHit any key to quit" << endl;
        getchar();
        return -1;   
    }

    cout << "Input Format = " << INPUT_FORMAT << endl;
    cout << "Input Device = " << INPUT_DEVICE << endl;
    cout << "Output Format = " << OUTPUT_FORMAT << endl;
    cout << "Output Device = " << OUTPUT_DEVICE << endl;

    /* libsndfile setup */
    if(!(config->wavFile = sf_open("../wav/a2002011001-e02-16kHz.wav", SFM_READ, &config->sfinfo))) {
        cout << "Could not open input.wav" << endl;
        sf_perror(NULL);
        return 1;
    }


    /* Define config */
    config->isInputInterleaved = 0;
    config->isOutputInterleaved = 0;
    config->framesPerCallback = FRAMES_PER_CALLBACK;
    config->numInputChannels = 2; //inputParam.channelCount;
    config->numOutputChannels = 2; //outputParam.channelCount;
    config->numCallbacks = 0;

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
    //inputParam.channelCount = inputDeviceInfo->maxInputChannels;
    inputParam.channelCount = config->numInputChannels;
    inputParam.sampleFormat = INPUT_FORMAT | (config->isInputInterleaved ? 0 : paNonInterleaved);
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
    //outputParam.channelCount = outputDeviceInfo->maxOutputChannels;
    outputParam.channelCount = config->numOutputChannels;
    outputParam.sampleFormat = OUTPUT_FORMAT | (config->isOutputInterleaved ? 0 : paNonInterleaved);
    outputParam.suggestedLatency = Pa_GetDeviceInfo(outputParam.device)->defaultLowOutputLatency;
    //outputParam.suggestedLatency = Pa_GetDeviceInfo(outputParam.device)->defaultHighOutputLatency;
    outputParam.hostApiSpecificStreamInfo = NULL;

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

