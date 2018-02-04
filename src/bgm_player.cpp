#include <iostream>
#include "bgm_player.h"
#include "sndfile.h"

#define WAV_RESOURCE_PATH "../wav/a2002011001-e02-16kHz.wav"

SNDFILE *wavFile;
SF_INFO sfinfo;

int init_sndfile(void) {
    if (!(wavFile = sf_open(WAV_RESOURCE_PATH, SFM_READ, &sfinfo))) {
        /* Unable to open */
        std::cout << "Could not open .wav file\n";
        sf_perror(NULL);
        return -1;
    }    
    
    /* Read wav file here */
    
    
    /* Success */
    return 0;
}
