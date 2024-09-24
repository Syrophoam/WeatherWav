#include "Midi.hpp"
#include "GUI.hpp"
#include "utils.h"
#include "API.h"
#include <JuceHeader.h>
#include "cbmp/cbmp.h"
#include <chrono>
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <curses.h>

int main(){

    bool running = true;
    
    //----- GUI ------//
    initGUI();
    
    BMP *bmpLogo = bopen("/Users/syrofullerton/JUCE/Syro/WeatherWav/Media/Logo.bmp");
    if (!bmpLogo) {
        return -1;
    }
    
    std::vector<int> logoSize = getImageDimensions(bmpLogo);
    int widthLogo = logoSize[0];
    int heightLogo = logoSize[1];
    
    std::vector<std::vector<pixelData>> logodata(widthLogo,std::vector<pixelData>(heightLogo));
    logodata = getImageData(bmpLogo, widthLogo, heightLogo);
    
    
    bclose(bmpLogo);
    unsigned long frameCounter = 0;
    while (running) {
        updateGUI();
        setTransform(1., 1., frameCounter%widthLogo, 0.);
        
        std::string frame;
        frame = bitMapView(widthLogo, heightLogo, logodata, .8f);
        
        std::cout << frame;
        int brk;
        //std::cin >> brk;
        
        struct timespec tim;
        tim.tv_nsec = 50'000'000;
        nanosleep(&tim, NULL);
        //running = false;
        frameCounter ++;
    }
    
    //----- JSON - send ------//
    std::map pos = std::map<std::string,float>{
        {"lon",174.7842} , {"lat",-37.7935}
    };
    int repeats = 24;
    
    json j = request(pos,repeats);
    std::string jsonString = j.dump();
    
    //----- CURL ------//
    char* data = 0;
    std::string key = "S14jxYHPDoXhtPwrvYRm9m";
    CURL *handle = curl_easy_init();
    struct curl_slist *headers = NULL;
    
    if (!handle) {
        printf("curl couldnt init");
        return 1;
    }
        CURLcode res;
        
        curl_easy_setopt(handle, CURLOPT_URL, "https://forecast-v2.metoceanapi.com/point/time");
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &CURLWriteFunction);
        
        std::string key_header = "x-api-key: " + key;
        headers = curl_slist_append(headers, key_header.c_str());
    
    headers = curl_slist_append(headers, "accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, jsonString.c_str());
    
    res = curl_easy_perform(handle);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(handle);
    
    //printf("Page data:\n\n%s\n", data);
    
    //----- JSON - Recieve ------//
    json jsonResponse = json::parse(data);

    json variables = jsonResponse["variables"];
    json waveHeight = variables["wave.height"]["data"];
    
    
    free(data);
    
    //----- MIDI ------//
    std::remove("/Users/syrofullerton/JUCE/Syro/WeatherWav/Midi.mid");
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(96);

    std::vector<double> values;

    values = normalizeValues(waveHeight);
    juce::MidiMessageSequence funcSeq = writeSequence(values, repeats);

    midiFile.addTrack(funcSeq);
    juce::FileOutputStream stream = juce::File("/Users/syrofullerton/JUCE/Syro/WeatherWav/Midi.mid");
    midiFile.writeTo(stream);
    
    return 0;
}
