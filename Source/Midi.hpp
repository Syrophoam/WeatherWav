#include <sys/ioctl.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <JuceHeader.h>
#include <nlohmann/json.hpp>
  
double lerp(double a, double b, double interp){
    interp = pow(interp, 2);
    return (((b*2*interp)+(a*-2*interp))/2) + a;
}



juce::MidiMessageSequence writeSequence(nlohmann::json values, int numOfValues){
    
    juce::MidiMessage ccMsg;
    juce::MidiMessageSequence seq;
    
    for (int j = 0; j < numOfValues; j++) {
        
        double val1 = values[j];
        double val2 = values[(j+1)%4];
        
        for (int i = 0; i < (96*4); i++) {
            ccMsg = ccMsg.controllerEvent(1, 11, uint8_t(lerp(val1, val2, double(i)/(96.f*4.f) ) * 127) );
            ccMsg.addToTimeStamp(i + (j*96*4));
            seq.addEvent(ccMsg);
        }
    }
    return seq;
}


