#include <JuceHeader.h>
#include "Midi.hpp"
#include "GUI.hpp"
#include "utils.h"
#include "API.h"
#include "cbmp/cbmp.h"
#include <chrono>
#include <nlohmann/json.hpp>  
#include <curl/curl.h>
#include <curses.h>

#define METSERVICE 1
#define OPENWEATHER 2

// KR coordinates

 // -41.298665, 174.775490


int main(){
    bool running = true;
    
    std::string home{std::getenv("HOME")};
    std::string path = home;
    path += "/WeatherWav/Media/";
    
    int weatherService = 0;
    weatherService = OPENWEATHER;
    
    //----- GUI ------//
    initGUI();

    imgData logo;
    loadImage("Logo.bmp", &logo); 
    
    std::vector<std::vector<pixelData>> logodata(logo.width,std::vector<pixelData>(logo.height));
    logodata = getImageData(logo.bmp, logo.width, logo.height);
    
    bclose(logo.bmp);
    
    unsigned long frameCounter = 0;
    while (running) {
        updateGUI();
        setTransform(1., 1., frameCounter%logo.width, 0.);
        
        std::string frame;
        
        //width & height are the wrong way around but works
        frame = bitMapView(logo.width, logo.height, logodata, 1.f);
        std::cout << frame;
        frameCounter ++;
        
        struct timespec tim;
        tim.tv_nsec = 50'000'000;
        nanosleep(&tim, NULL);
        running = false;
        
    }
    
    //----- JSON - send ------//
    std::map pos = std::map<std::string,float>{
        {"lon",174.7842} , {"lat",-37.7935}
    };
    int repeats = 24;
    
    json jMS = requestMS(pos,repeats);
    std::string jsonString = jMS.dump();
    
    std::string keyOW = "0f7dae7414bfba2cbd826fc5c6b04dd6";
    std::string openWeather;
    openWeather = "https://api.openweathermap.org/data/2.5/weather?lat=-41.29&lon=174.77&appid=";
    openWeather = "api.openweathermap.org/data/2.5/forecast?lat=-41.29&lon=174.77&appid=";
    openWeather += keyOW;
    
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
    
    if (weatherService == METSERVICE)
        curl_easy_setopt(handle, CURLOPT_URL, "https://forecast-v2.metoceanapi.com/point/time");
    
    if (weatherService == OPENWEATHER)
        curl_easy_setopt(handle, CURLOPT_URL, openWeather.c_str());
  
    
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &CURLWriteFunction);
        
    if (weatherService == METSERVICE) {
        std::string key_header_MS = "x-api-key: " + key;
        headers = curl_slist_append(headers, key_header_MS.c_str());
        
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, jsonString.c_str());
    }

    res = curl_easy_perform(handle);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(handle);
     
    printf("Page data:\n\n%s\n", data);
    
    //----- JSON - Recieve ------//
    
    json jsonResponse;
    json waveHeight;
    std::vector<double> values;
    if (weatherService == METSERVICE) {
        jsonResponse = json::parse(data);

        json variables = jsonResponse["variables"];
        waveHeight = variables["wave.height"]["data"];
        values = normalizeValues(waveHeight);
        // put more variables //
    }
    
    //std::vector<struct openWeather> OWVec;
    struct openWeather OWVec;
    
    if (weatherService == OPENWEATHER) {
        jsonResponse = json::parse(data);
        int count = jsonResponse["cnt"];

        OWVec.cityName = jsonResponse["city"]["name"];
        OWVec.cityCountry = jsonResponse["city"]["country"];
        OWVec.citySunrise = jsonResponse["city"]["sunrise"];
        OWVec.citySunset = jsonResponse["city"]["sunset"];
        OWVec.count = count;
        
        fillStruct(jsonResponse, OWVec, count);
        
        normalizeResponseDouble(OWVec.temp);
        normalizeResponseDouble(OWVec.temp_max);
        normalizeResponseDouble(OWVec.temp_min);
        normalizeResponseDouble(OWVec.feels_like);
        normalizeResponseDouble(OWVec.windSpeed);
        normalizeResponseDouble(OWVec.windGust);
        normalizeResponseDouble(OWVec.pop);
        normalizeResponseDouble(OWVec.rain3h);
        normalizeResponseDouble(OWVec.snow3h);
        
        normalizeResponseInt(OWVec.pressure);
        normalizeResponseInt(OWVec.sea_level);
        normalizeResponseInt(OWVec.grnd_level);
        normalizeResponseInt(OWVec.humidity);
        normalizeResponseInt(OWVec.clouds);
        normalizeResponseInt(OWVec.visibility);
    }
    
    free(data);
    
    //----- MIDI ------//
    std::string midiPath_S = "/Users/syro/WeatherWav/Media/";
    char* midiPath = "midi.mid";
    midiPath_S += midiPath;
    
    std::remove(midiPath_S.data());
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(96);

    juce::MidiMessageSequence funcSeq = writeSequence(OWVec.windSpeed);

    midiFile.addTrack(funcSeq);
    juce::FileOutputStream stream = juce::File(midiPath_S.data());
    midiFile.writeTo(stream);
    
    return 0;
}
