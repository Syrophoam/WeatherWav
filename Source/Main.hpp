#include <JuceHeader.h>
#include "Midi.hpp"
#include "GUI.hpp"
#include "utils.h"
#include "API.h"
#include "audio.h"
//#include "3DRenderer.h"
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

bool runningFunc = true;
// ^[[Aup ^[[Bdown ^[[Dleft ^[[Cright
void *keyListenrFunction(void *input){
    mainThread *mainT = (mainThread*)input;
//    mainT->keyVal = 0;
    int keyVal = 0;
    
    while(runningFunc){
        std::string keyIn = "";
        std::cin>>keyIn;
        
        if(keyIn.length() > 3){
            runningFunc = false;
            mainT->keyVal = 1;
        }
    }
    // if you can automatiocaly return cin this would work
}




void *mainFunc(void* input){
    bool ambientMode = true;
    
    //- seperate thread init -//
    mainThread *mainT=(mainThread *)input;
    mainT->keyVal = 0;
    
    // ----- Audio ----- //
    
    mainT->b = 1;
    
    //-----midi out?-------//

//    while(true){
//        int randVal = rand()%127;
//        mainT->sendMsg = true;
//        std::this_thread::sleep_for(std::chrono::seconds(3));
//    }
    
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
    
//    initRenderer();
//    mainRenderer(); <- gonna be so hard :(
    
    image logo;
    loadImage("Logo", logo);
    
    image map;
    loadImage("map", map);
    
    image multi;
    loadImage("multi", multi);
    
    image empty;
    loadImage("empty", empty);
    
    long currentFrame = -1;
    long frameCounter = 0;
    std::vector<int> inputPosition = {ws.ws_col/2,ws.ws_row/2};
    
    while (running) {
        std::string init;
        std::string logoFrame;
        std::string multiFrame; 
        
        updateGUI(ws);
        setTransform(1., 1., frameCounter%logo.width, 0.);
        
        switch (currentFrame) {
                

            case -1:{
                //width & height are the wrong way around but works
                init = bitMapView(logo.width, logo.height, logo.imgData, 0,1.f);
                std::cout << init;
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
                
                logoFrame = bitMapView(logo.width, logo.height, logo.imgData, 0,1.f);
                std::cout << logoFrame;
                if (frameCounter > 30) {
                    mainT->b = 1;
                    currentFrame = 2;
                }
                
                break;
            }
            case 2:{
                
                multiFrame = bitMapView(multi.width, multi.height, multi.imgData, frameCounter%3,1.f);
                std::cout<< multiFrame;
                if (frameCounter > 60) {
                    mainT->b = 1;
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
                mainT->b = 1;
                
                std::cout<<lon<<std::endl;
                std::cout<<lat<<std::endl;
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
                //reimple ment with juce keyboard input
                //USE ANOTHER THREAD TO LOOK FOR CIN
                
                
                pthread_t keyListener;
                pthread_create(&keyListener, NULL, keyListenrFunction, (void *)&mainT);
                std::cout<<std::endl<<mainT->keyVal<<std::endl;
                
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
    
    //----- JSON - send ------//
    double lonDouble = std::stod(lon);
    double latDouble = std::stod(lat);
    std::map pos = std::map<std::string,float>{
//        {"lon",174.7842} , {"lat",-37.7935} // kr coords
        {"lon",lonDouble} , {"lat",latDouble}
    };
    int repeats = 24;
    
    json jMS = requestMS(pos,repeats,0); // 0 = 1h, 1 = 1m
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
    json jsonResponse;
    struct response chunk = {.memory = (char *)malloc(0),
                             .size = 0};
    
    std::string key = "S14jxYHPDoXhtPwrvYRm9m";
    CURL *handle = curl_easy_init();
    struct curl_slist *headers = NULL;
    CURLcode res;
    
    if(!ambientMode){
        
        char error[CURL_ERROR_SIZE]; /* needs to be at least this big */
        
        if (!handle) {
            printf("curl couldnt init");
            return nullptr;
        }
        
        if (weatherService == METSERVICE)
            curl_easy_setopt(handle, CURLOPT_URL, "https://forecast-v2.metoceanapi.com/point/time");
        
        if (weatherService == OPENWEATHER)
            curl_easy_setopt(handle, CURLOPT_URL, openWeather.data());
        
        curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
        
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &mem_cb);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, error);
        error[0] = 0; 
        
        if (weatherService == METSERVICE) {
            std::string key_header_MS = "x-api-key: " + key;
            headers = curl_slist_append(headers, key_header_MS.c_str());
            
            headers = curl_slist_append(headers, "accept: application/json");
            headers = curl_slist_append(headers, "Content-Type: application/json");
            
            //        curl_easy_setopt(handle, CURLOPT_BUFFERSIZE, 120000L);
            curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers); 
            curl_easy_setopt(handle, CURLOPT_POSTFIELDS, jsonString.c_str());
        }
        
        
        
        res = curl_easy_perform(handle);
        //        if(res != CURLE_OK){
        const char *strErr = curl_easy_strerror( res );
        printf("libcurl said %s\n", strErr);
        
        size_t len = strlen(error);
        fprintf(stderr, "\nlibcurl: (%d) ", res);
        
        if(len)
            fprintf(stderr, "%s%s", error,
                    ((error[len - 1] != '\n') ? "\n" : ""));
        else
            fprintf(stderr, "%s\n", curl_easy_strerror(res));
        
        printf("Page data:\n\n%s\n", chunk.memory);
        curl_easy_cleanup(handle);
        curl_slist_free_all(headers);
        jsonResponse = json::parse(chunk.memory);
        
    }
    

    //----- ambient version? -//
    
    CURL *handle2 = curl_easy_init();
    struct curl_slist *headers2 = NULL;
    unsigned int valCounter = 0;
    json ambJson;
//    struct response ambChunk = {.memory = (char *)malloc(0),
//                             .size = 0};
    
    
    while(ambientMode){
        
        struct response ambChunk = {.memory = (char *)malloc(0),
                                 .size = 0};

        printf("starting ambient mode");
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
//        printf("Page data:\n\n%s\n", ambChunk.memory);
        
        free(ambChunk.memory);
        
        
        std::vector<std::vector<double>> variableGroup[5];
        
        formatResponse(variableGroup, ambJson["variables"], ambientMode);
        
        
        
        juce::MidiBuffer ambBuff;
        //allocate space later ^
        
//        ambBuff.clear();
        
        ambBuff.addEvents(groupWriteSequenceAmb(variableGroup[0], mainT->bufferOffest, 1), 0, 44100 * 60, 0);
        ambBuff.addEvents(groupWriteSequenceAmb(variableGroup[1], mainT->bufferOffest, 2), 0, 44100 * 60, 0);
        ambBuff.addEvents(groupWriteSequenceAmb(variableGroup[2], mainT->bufferOffest, 3), 0, 44100 * 60, 0);
        ambBuff.addEvents(groupWriteSequenceAmb(variableGroup[3], mainT->bufferOffest, 4), 0, 44100 * 60, 0);
        ambBuff.addEvents(groupWriteSequenceAmb(variableGroup[4], mainT->bufferOffest, 5), 0, 44100 * 60, 0);
        
        mainT->midiBuffer.clear();
        mainT->midiBuffer.addEvents(ambBuff, 0, 44100 * 60, 0);
        mainT->sendMsg = true;
        
        mainT->b = 1.f;

        std::this_thread::sleep_for(std::chrono::seconds(60)); // use chrono sleep to account for the time curl and midi code takes
    }

    if(ambientMode){
        curl_easy_cleanup(handle2);
        curl_slist_free_all(headers2);
    }
    
    //----- JSON - Recieve ------//
    std::cout<<"parsing Data"<<std::endl;
    
    std::vector<std::vector<double>> variableGroup[5];
    
    if(weatherService == METSERVICE)
        formatResponse(variableGroup, jsonResponse["variables"], ambientMode);
  
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
    midiPath_S = "/Users/syrofullerton/WeatherWav/Media";
    std::string midiPath = "midi.mid";
    midiPath_S += midiPath;
    
    std::remove(midiPath_S.data());

    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(48); // multiply to make track shorter
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
