#include <JuceHeader.h>
#include "Midi.hpp"
#include "GUI.hpp"
#include "utils.h"
#include "API.h"
#include "audio.hpp"
#include "cbmp/cbmp.h"
#include <nlohmann/json.hpp>  
#include <curl/curl.h>
#include <curses.h>
#include <chrono>

#define METSERVICE 1
#define OPENWEATHER 2

#define RED 0
#define GREEN 1
#define BLUE 2
#define ALPHA 3

// KR coordinates

 // -41.298665, 174.775490

// terminal dimensions for images = 560 × 336

std::string lon = "0";
std::string lat = "0"; // input
double      longitude, latitude = 0; // input to string

struct image {
    imgData imgStruct;
    std::vector<std::vector<pixelData>> imgData;
    int width;
    int height;
};

void loadImage(std::string imageFileName, struct image &imgStrct){
    imgData img;
    imageFileName += ".bmp";
    
    loadImage(imageFileName.data(), &img);
    
    std::vector<std::vector<pixelData>> imgDat(img.width,std::vector<pixelData>(img.height));
    imgDat = getImageData(img.bmp, img.width, img.height);
    
    imgStrct.imgStruct = img;
    imgStrct.width = img.width;
    imgStrct.height = img.height;
    imgStrct.imgData = imgDat;

    bclose(img.bmp);
}

int main(){
    bool running = true;
    
    std::string home{std::getenv("HOME")};
    std::string path = home;
    path += "/WeatherWav/Media/";
    
    int weatherService = 0;
    weatherService = OPENWEATHER;
    
    //----- GUI ------//
    initGUI();
    
    image logo;
    loadImage("Logo", logo);
    
    image map;
    loadImage("map", map);
    
    image multi;
    loadImage("multi", multi);
    
    
    unsigned long currentFrame = 0;
    unsigned long frameCounter = 0;
    while (running) {
        updateGUI();
        setTransform(1., 1., frameCounter%logo.width, 0.);
        
        switch (currentFrame) {
            case 0:{
                std::string init;
                //width & height are the wrong way around but works
                init = bitMapView(logo.width, logo.height, logo.imgData, 0,1.f);
                std::cout << init;
                currentFrame = 1;
                break;
            }
            case 1:{
                std::string logoFrame;
                logoFrame = bitMapView(logo.width, logo.height, logo.imgData, 0,1.f);
                std::cout << logoFrame;
                if (frameCounter > 30) {
                    currentFrame = 2;
                }
                break;
            }
                
            case 2:{
                std::string multiFrame;
                multiFrame = bitMapView(multi.width, multi.height, multi.imgData, frameCounter%3,1.f);
                std::cout<< multiFrame;
                if (frameCounter > 60) {
                    currentFrame = 3;
                }
                break;
            }
            case 3:{
                std::string map_s;
                map_s = bitMapView(map.width, map.height, map.imgData, 0,1.f);
                std::cout << map_s;
                std::cout << "Longitude ?";
                std::cin >> lon;
                std::cout << "Lattitude ?";
                std::cin >> lat;
                currentFrame = 4;
                running = false;
                break;
            }
                
            default:
                break;
        }
        
        struct timespec tim; tim.tv_nsec = 50'000'000; nanosleep(&tim, NULL);
        frameCounter++;
        running = false;
    }
    
//    longitude = std::stod(lon);
//    latitude = std::stod(lat);
    
    //----- JSON - send ------//
    std::map pos = std::map<std::string,float>{
        {"lon",174.7842} , {"lat",-37.7935}
    };
    int repeats = 24;
    
    json jMS = requestMS(pos,repeats);
    std::string jsonString = jMS.dump();
    
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
    midiFile.setTicksPerQuarterNote(96*3);
    
    writeSequence(OWVec.visibility, "visibility", midiFile); 
    writeSequence(OWVec.pop, "probability of rain", midiFile);
    writeSequence(OWVec.temp, "temp", midiFile);
    writeSequence(OWVec.temp_min, "temp Min", midiFile);
    writeSequence(OWVec.temp_max, "temp Max", midiFile);
    writeSequence(OWVec.feels_like, "feels like", midiFile);
    writeSequence(OWVec.pressure, "pressure", midiFile);
    writeSequence(OWVec.sea_level, "seaLevel", midiFile);
    writeSequence(OWVec.grnd_level, "ground level", midiFile);
    writeSequence(OWVec.humidity, "humidity", midiFile);
    writeSequence(OWVec.clouds, "clouds", midiFile);
    writeSequence(OWVec.windDeg, "wind direction", midiFile);
    writeSequence(OWVec.windGust, "wind gust", midiFile);
    writeSequence(OWVec.rain3h, "rain", midiFile);
    writeSequence(OWVec.snow3h, "snow", midiFile);

    juce::FileOutputStream stream = juce::File(midiPath_S.data());
    midiFile.writeTo(stream);
    
    return 0;
}
