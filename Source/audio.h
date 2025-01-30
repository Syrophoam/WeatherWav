//
//  audio.h
//  WeatherWav
//
//  Created by syro Fullerton on 29/09/2024.
//

#ifndef audio_h
#define audio_h
//#include <juce_Header.h>
//#include "Main.hpp"
//#include "/Users/syro/WeatherWav/Source/audio/MainComponent.h"

typedef struct mainThread {
    double a;
    int b;
    int keyVal;
    juce::MidiMessage msg;
    bool sendMsg;
    juce::MidiBuffer midiBuffer;
    juce::MidiMessageSequence midiSeq;
    int bufferOffest;
    
} /* test */ ;


void *mainThreadFunc(void* input){

//        std::this_thread::sleep_for(std::chrono::seconds(1));

    
}


#endif /* audio_h */
