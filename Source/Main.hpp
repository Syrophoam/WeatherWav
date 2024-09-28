#include <JuceHeader.h>
#include "Midi.hpp"
#include "GUI.hpp"
#include "utils.h"
#include "API.h"
#include "cbmp/cbmp.h"
#include <nlohmann/json.hpp>  
#include <curl/curl.h>
#include <thread>
#include <curses.h>
#include <chrono>

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

int mainFunc(){
    
    // ----- Audio ----- //

    
    // ----- Init ----- //
    bool running = true;
    bool useDefault = true;
    
    std::string home{std::getenv("HOME")};
    std::string path = home;
    path += "/WeatherWav/Media/";
    
    int weatherService = 0;
    weatherService = OPENWEATHER;
    

    //----- GUI ------//
    struct winsize ws;
    initGUI(ws);
    updateGUI(ws);
    
//    std::vector<std::vector<double>> coord;

//    initCoordLUT(coord, ws);
    
    image logo;
    loadImage("Logo", logo);
    
    image map;
    loadImage("map", map);
    
    image multi;
    loadImage("multi", multi);
    
    image empty;
    loadImage("empty", empty);
    
    long currentFrame = 0;
//    currentFrame = -1;
    long frameCounter = 0;
    std::vector<int> inputPosition = {ws.ws_col/2,ws.ws_row/2};
    
    while (running) {
        std::string init;
        std::string logoFrame;
        std::string multiFrame; 
        
        updateGUI(ws);
        setTransform(1., 1., frameCounter%logo.width, 0.);
        
        switch (currentFrame) {
                
            case -1:{ // testing
                
//                initCoordLUT(coord, ws);
//                std::cout << frameCounter;
                break;
            }
            case 0:{
                //width & height are the wrong way around but works
                init = bitMapView(logo.width, logo.height, logo.imgData, 0,1.f);
                std::cout << init;
                currentFrame = 1;
                
                std::string settingsInput;
                std::cout << "use default settings? Y / N" << std::endl;
                std::cin >> settingsInput;
                
                if ((settingsInput == "Y") || (settingsInput == "y") ) {
                    running = false;
                }
                
                break;
            }
            case 1:{
                logoFrame = bitMapView(logo.width, logo.height, logo.imgData, 0,1.f);
                std::cout << logoFrame;
                if (frameCounter > 30) {
                    currentFrame = 2;
                }
                break;
            }
            case 2:{
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
                std::string map_str;
                map_str = bitMapView(map.width, map.height, map.imgData, 0,1.f);
                std::cout << map_str;
                std::cout << "Longitude ?";
                std::cin >> lon;
                std::cout << "Lattitude ?";
                std::cin >> lat;
                running = false;
                break;
            }
            case 5:{
                updateGUI(ws);
//                glfwFocusWindow(window);
//
//                glfwPollEvents();
//                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
//                    inputPosition[1] += 1;
//                }
//            
//                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
//                    inputPosition[1] -= 1;
//                }
//                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
//                    inputPosition[0] -= 2;
//                }
//                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
//                    inputPosition[0] += 2;
//                }
                //reimplement with juce keyboard input
                
                std::string map_s;
                map_s = bitMapView(map.width, map.height, map.imgData, 0,1.f);

                int positionIndex =
                (inputPosition[0]%ws.ws_col) + ((inputPosition[1]%ws.ws_row) * (ws.ws_col+1));
                
                std::string::iterator mapIt = map_s.begin();
                mapIt += positionIndex;
                
                map_s.replace(mapIt, mapIt+1, "\033[31m@\033[0m");
                
                std::vector<std::vector<double>> coord(2);
                coord[0].resize(160);
                coord[1].resize(48);
                
                initCoordLUT(coord, ws);
                
//                if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
//                    running = false;
//                    
//                    return 0;
//                }
                
                map_s.erase(map_s.end()-((ws.ws_col+1)*2), map_s.end());
                std::cout << map_s;
                
                std::cout << coord[0][inputPosition[0]] << std::endl;
                std::cout << coord[1][inputPosition[1]] << std::endl;
                //running = false;

                break;
            }
                
            default:
                break;
        }
        
        struct timespec tim; tim.tv_nsec = 50'000'000; nanosleep(&tim, NULL);
        frameCounter++;
        //running = false;
    }
    
//    longitude = std::stod(lon);
//    latitude = std::stod(lat);
    
    //----- JSON - send ------//
    std::map pos = std::map<std::string,float>{
        {"lon",174.7842} , {"lat",-37.7935}
    };
    int repeats = 12;
    
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
    char *data = 0;
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
        curl_easy_setopt(handle, CURLOPT_URL, openWeather.data());
    
    curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
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
      
    long *expectedSizeOfResponse;
    if (!res) {
        curl_off_t *content_length = nullptr;
        curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T,&content_length);
        expectedSizeOfResponse = content_length;
    }
    long xpctsize = long(expectedSizeOfResponse);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(handle); 
    
//    char* dat = data;
//    printf("Page data:\n\n%s\n", data);
    if (std::string(data).length() < xpctsize) {
        return 9;  
    }
    std::string testdat = std::string(data);
    //----- JSON - Recieve ------//
    
    
    json waveHeight;
    std::vector<double> values;
    if (weatherService == METSERVICE) {
        json jsonResponse;
        jsonResponse = json::parse(data);
        
        std::ifstream jsonFile("/Users/syro/WeatherWav/Media/jsonformatter.json");
//        jsonResponse = json::parse(jsonFile); // expected output, curl doesnt give the same string ??????????????..... it works now
  
        json variables = jsonResponse["variables"];
        waveHeight = variables["wave.height"]["data"];
        values = normalizeValues(waveHeight);
        // put more variables //
    }
  
    struct openWeather OWVec;
    
    if (weatherService == OPENWEATHER) {
        json jsonResponse;
        jsonResponse = json::parse(data);
//        jsonResponse = json::f
  
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
    
    free(data);// idk why this was here, mabye important? 
    
    //----- MIDI ------//
    std::string midiPath_S = "/Users/syro/WeatherWav/Media/";
    char* midiPath = "midi.mid";
    midiPath_S += midiPath;
    
    std::remove(midiPath_S.data());
    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(96); // multiply to make track shorter
    
    writeSequence(values, "wind?", midiFile);
    writeSequence(values, "wind2?", midiFile);
    
    if (weatherService == OPENWEATHER) {
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
    }

    juce::FileOutputStream stream = juce::File(midiPath_S.data());
    midiFile.writeTo(stream);
    
    return 0;
}
