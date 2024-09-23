//
//  Main.cpp
//  WeatherWav - App
//
//  Created by Syro Fullerton on 23/09/2024.
//

#include "Midi.hpp"
#include "GUI.hpp"
#include "utils.h"
#include <JuceHeader.h>
#include "cbmp/cbmp.h"
#include <chrono>

int main(){
    bool running = true;
    
    //----- GUI ------//
    initGUI();

    
    double aspectRatio = double(ws.ws_xpixel)/double(ws.ws_ypixel);
    
    BMP *bmpLogo = bopen("/Users/syrofullerton/JUCE/Syro/WeatherWav/Media/Doom_logo.bmp");
    if (!bmpLogo) {
        return -1;
    }
    std::vector<int> logoSize = getImageDimensions(bmpLogo);
    int widthLogo = logoSize[0];
    int heightLogo = logoSize[1];
    
    std::vector<std::vector<pixelData>> logodata(widthLogo,std::vector<pixelData>(heightLogo));
    logodata = getImageData(bmpLogo, widthLogo, heightLogo);
    setTransform(1., 1., 0., 0.);
    
    
    while (running) {
        updateGUI();
        
        aspectRatio = double(ws.ws_xpixel)/double(ws.ws_ypixel);
        std::string frame;
        frame = bitMapView(widthLogo, heightLogo, logodata, aspectRatio, .8f);
        
        std::cout << frame;
        
        struct timespec tim;
        tim.tv_nsec = 50'000'000;
        nanosleep(&tim, NULL);
    }
    
    //----- MIDI ------//
    std::remove("/Users/syrofullerton/JUCE/Syro/WeatherWav/Midi.mid");
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(96);

    std::vector<double> values;
    values = {1,0.4,0.3,0.2};
    juce::MidiMessageSequence funcSeq = writeSequence(values);

    midiFile.addTrack(funcSeq);
    juce::FileOutputStream stream = juce::File("/Users/syrofullerton/JUCE/Syro/WeatherWav/Midi.mid");
    midiFile.writeTo(stream);
    
    return 0;
}
