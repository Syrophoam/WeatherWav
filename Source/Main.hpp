#include <JuceHeader.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include "Midi.hpp"
#include "GUI.hpp"
#include "utils.h"
#include "API.h"
#include "audio.h"
#include "cbmp/cbmp.h"
#include <nlohmann/json.hpp>  
#include <curl/curl.h>

#include <thread>
#include <curses.h>
#include <chrono>
#include <pthread.h>

#include "audio/MainComponent.h"

#define METSERVICE 1
#define OPENWEATHER 2

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

#define CONSOLEWIDTH 160
#define CONSOLEHEIGHT 48

// KR coordinates
 // -41.298665, 174.775490

// terminal dimensions for images 
// 560 × 336

std::string lon = "174.775490";
std::string lat = "-41.298665"; // input

double      longitude, latitude = 0; // input to string

bool runningFunc = true;


void *main_t(void* input){
    
     std::cout<< "amb_mode ? user.pressedY() : user.pressedN()"<<std::endl;
     char usr = NULL;
     std::cin>>usr;
     
     bool ambientMode;
     if((usr == 'Y') || (usr == 'y'))
     {
         ambientMode = true;
     }
     else if((usr == 'N') || (usr == 'N'))
     {
         ambientMode = false;
     }
    
    std::ofstream dayVal;
    dayVal.open("/Users/syro/WeatherWav/Media/dayValData");
    std::string dayValDate;

    const auto now = std::chrono::system_clock::now();
    std::string currentTime;
    currentTime = std::format("{:%FT%H:%M:00Z}", now); // <-behind an hour , daylight savings ?

    //- seperate thread init -//
    mainThread *mainT=(mainThread *)input;
    
    // ----- Audio ----- //
    
    mainT->b = 1;
    
    // ----- Init ----- //
    bool running = true;
    bool useDefault = true;
    
    std::string home{std::getenv("HOME")};
    std::string path = home;
    path += "/WeatherWav/Media/";
    
    int weatherService = 0;
    weatherService = METSERVICE;
    
    //----- GUI ------//
    struct winsize ws;
    initGUI(ws);
    updateGUI(ws);

    
    long currentFrame = -1;
    long frameCounter = 0;
    
    while (running) {

        
        switch (currentFrame) {
                
            case -1:{
                //width & height are the wrong way around but works
                
                
                currentFrame = 0;
                
                std::string settingsInput;
                std::cout << "use default settings? Y / N" << std::endl;
                std::cin >> settingsInput;
                
                if ((settingsInput == "Y") || (settingsInput == "y") ) {
                    running = false;
                    mainT->b = 1;
                }
                
                
                break;
            }
            case 0:{
                
                std::cout<< "use Metservice[1] or openWeather[2]"<< std::endl;
                std::cin >> weatherService;
                currentFrame =1;
                break;
            }
                
            case 1:{
                
                
                
                break;
            }
            case 2:{
                

                
                break;
            }
            case 3:{
                
                
                
                std::cout << "enter coodrinates [1] or locate on map [2]";
                int option;
                std::cin >> option;
                if (option == 1) {
                    currentFrame = 4;
                }else if (option == 2){
                    currentFrame = 5;
                }
                
                break;
            }
                
            case 4:{
                
                std::cout << "Longitude ?";
                std::cin >> lon;
                std::cout << "Lattitude ?";
                std::cin >> lat;
                running = false;
                mainT->b = 1;
                
                std::cout<<lon<<std::endl;
                std::cout<<lat<<std::endl;
                break;
            }
            case 5:{
                
                break;
            }
                
            default:
                break;
        }
        
        struct timespec tim; tim.tv_nsec = 50'000'000; nanosleep(&tim, NULL);
        frameCounter++;
        //running = false;
    }
    
    //----- JSON - send ------//
    double lonDouble = std::stod(lon),  latDouble = std::stod(lat);
    std::map pos = std::map<std::string,float>{
//        {"lon",174.7842} , {"lat",-37.7935} // kr coords ... ive sinced move out try dox me
        {"lon",lonDouble} , {"lat",latDouble}
    };
    
    int repeats = 24;
    
    std::string metServiceRequest = requestMS(pos,repeats,0).dump(); // 0 = 1h, 1 = 1m
    
    std::string keyOW = "0f7dae7414bfba2cbd826fc5c6b04dd6";
    std::string openWeather;
    //openWeather = "api.openweathermap.org/data/2.5/forecast?lat=-41.29&lon=174.77&appid=";
    openWeather = "api.openweathermap.org/data/2.5/forecast?lat=";
    openWeather += lat;
    openWeather += "&lon=";
    openWeather += lon;
    openWeather += "&appid=";
    openWeather += keyOW;
    
    //----- CURL ------//
    json jsonResponse;
    struct response chunk = {.memory = (char *)malloc(0), .size = 0};
    
    std::string key = "S14jxYHPDoXhtPwrvYRm9m";
    CURL *handle = curl_easy_init();
    struct curl_slist *headers = NULL;
    CURLcode res;
    
    if (!handle) {
        printf("curl couldnt init");
        return nullptr;
    }
    
    if (weatherService == METSERVICE)
        curl_easy_setopt(handle, CURLOPT_URL, "https://forecast-v2.metoceanapi.com/point/time");
    
    if (weatherService == OPENWEATHER)
        curl_easy_setopt(handle, CURLOPT_URL, openWeather.data());
    
//    curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &mem_cb);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
    
    if (weatherService == METSERVICE) {
        std::string key_header_MS = "x-api-key: " + key;
        headers = curl_slist_append(headers, key_header_MS.c_str());
        
        headers = curl_slist_append(headers, "accept: application/json");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, metServiceRequest.c_str());
    }
        
    res = curl_easy_perform(handle);
    
    printf("Page data:\n\n%s\n", chunk.memory);
    curl_easy_cleanup(handle);
    curl_slist_free_all(headers);
    jsonResponse = json::parse(chunk.memory);
    
    initNorm(jsonResponse);
    
    //----- ambient version! -//
    CURL *handle2 = curl_easy_init();
    struct curl_slist *headers2 = NULL;
    unsigned int valCounter = 0;
    json ambJson;
    
    while(ambientMode){
        
        struct response ambChunk = {.memory = (char *)malloc(0), .size = 0};
        
        std::cout<<"starting ambient mode"<<'\n';
        json ambJson = requestMS(pos,1,1); // 0 = 1h, 1 = 1m
        std::string jsonString = ambJson.dump();
        
        curl_easy_setopt(handle2, CURLOPT_WRITEFUNCTION, &mem_cb);
        curl_easy_setopt(handle2, CURLOPT_WRITEDATA, (void *)&ambChunk);
        
        std::string key_header_MS = "x-api-key: " + key;
        headers2 = curl_slist_append(headers2, key_header_MS.c_str());
        headers2 = curl_slist_append(headers2, "accept: application/json");
        headers2 = curl_slist_append(headers2, "Content-Type: application/json");
        
        curl_easy_setopt(handle2, CURLOPT_URL, "https://forecast-v2.metoceanapi.com/point/time");
        curl_easy_setopt(handle2, CURLOPT_HTTPHEADER, headers2);
        curl_easy_setopt(handle2, CURLOPT_POSTFIELDS, jsonString.c_str());
        
        res = curl_easy_perform(handle2);
        
        ambJson = json::parse(ambChunk.memory);
        free(ambChunk.memory);
        
        normalizeAmb(ambJson);
        
        std::vector<std::vector<double>> variableGroup[5];
        
        juce::MidiBuffer ambBuff;
        
        for(int i = 0; i < 5; ++i ){
            ambBuff.addEvents(groupWriteSequenceAmb(variableGroup[i], mainT->bufferOffest, i), 0, 44100 * 60, 0);
        }
        
        mainT->midiBuffer.clear();
        mainT->midiBuffer.addEvents(ambBuff, 0, 44100 * 60, 0);
        mainT->sendMsg = true;
        
        mainT->b = 1.f;
        
        std::this_thread::sleep_for(std::chrono::seconds(60)); // use chrono sleep to account for the time curl and midi code takes
    }
    
    curl_easy_cleanup(handle2);
    curl_slist_free_all(headers2);
    
    
    //----- JSON - Recieve ------//
    std::cout<<"parsing Data"<<std::endl;
    
    std::vector<std::vector<u_int8_t>> variableGroup[5];
    
    if(weatherService == METSERVICE)
        formatResponse(variableGroup, jsonResponse["variables"]);
  
    struct openWeather OWVec;
    
    if (weatherService == OPENWEATHER) {

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
        
        std::vector<int> t;
        int t2 = t[1];
        
        normalizeResponseInt(OWVec.pressure);
        normalizeResponseInt(OWVec.sea_level);
        normalizeResponseInt(OWVec.grnd_level);
        normalizeResponseInt(OWVec.humidity);
        normalizeResponseInt(OWVec.clouds);
        normalizeResponseInt(OWVec.visibility);
    }
    
    //----- MIDI ------//
    std::cout<<"Writing midi file..."<< std::endl;
    std::string midiPath_S = "/Users/syro/WeatherWav/Media/";
//    midiPath_S = "/Users/syrofullerton/WeatherWav/Media";
    std::string midiPath = "weatherWavO_";

    std::string::iterator currentTimeIter = currentTime.end();
    currentTime.erase(currentTimeIter - 10, currentTimeIter);
    midiPath += currentTime;
    midiPath += ".mid";
    midiPath_S += midiPath;

//    std::remove(midiPath_S.data());

    juce::MidiFile midiFile;
    
    midiFile.setTicksPerQuarterNote(48 * 10); // multiply to make track shorter
    juce::StringRef name;
    
    if(weatherService == METSERVICE){
        
        juce::MidiMessageSequence air;
        air.addSequence(groupWriteSequence(variableGroup[0]),0);
        air.addEvent(juce::MidiMessage::textMetaEvent(3, "Air"));
        midiFile.addTrack(air);
        
        juce::MidiMessageSequence cloud;
        cloud.addSequence(groupWriteSequence(variableGroup[1]),0);
        cloud.addEvent(juce::MidiMessage::textMetaEvent(3, "Cloud"));
        midiFile.addTrack(cloud);
        
        juce::MidiMessageSequence wind;
        wind.addSequence(groupWriteSequence(variableGroup[2]),0);
        wind.addEvent(juce::MidiMessage::textMetaEvent(3, "Wind"));
        midiFile.addTrack(wind); 
        
        juce::MidiMessageSequence flux;
        flux.addSequence(groupWriteSequence(variableGroup[3]),0); 
        flux.addEvent(juce::MidiMessage::textMetaEvent(3, "Flux"));
        midiFile.addTrack(flux);
        
        juce::MidiMessageSequence wave;
        wave.addSequence(groupWriteSequence(variableGroup[4]),0);
        wave.addEvent(juce::MidiMessage::textMetaEvent(3, "Waves"));
        midiFile.addTrack(wave);
        
    }
    
    if (weatherService == OPENWEATHER) {
        
        juce::MidiMessageSequence wind;
        wind.addSequence(writeSequence(OWVec.windDeg, 1),0);
        wind.addSequence(writeSequence(OWVec.windGust,2),0);
        name = "wind";
        juce::MidiMessage nameEvent = juce::MidiMessage::textMetaEvent(3, name);
        wind.addEvent(nameEvent);
        midiFile.addTrack(wind);
        
        writeSequence(OWVec.visibility,1);
        writeSequence(OWVec.pop, 1);
        writeSequence(OWVec.temp, 1);
        writeSequence(OWVec.temp_min, 1);
        writeSequence(OWVec.temp_max, 1);
        writeSequence(OWVec.feels_like, 1);
        writeSequence(OWVec.pressure, 1);
        writeSequence(OWVec.humidity, 1);
        writeSequence(OWVec.clouds, 1);
        
        writeSequence(OWVec.rain3h, 1);
        writeSequence(OWVec.snow3h, 1);
    }
    
    
    juce::FileOutputStream stream = juce::File(midiPath_S.data());
    
    if(stream.openedOk()){
        
        midiFile.writeTo(stream);
    }else{
        std::cout<<"failed to open stream"<<std::endl;
    }
    std::cout<<"Written midi file"<<std::endl;
    
    mainT->b = 1;
    
    free(chunk.memory);
    
    pthread_exit(NULL);
    return 0;
}
