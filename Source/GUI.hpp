//
//  main.cpp 
//  ncurse
//
//  Created by syro Fullerton on 06/11/2024.
//
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <algorithm>
#include <string>
//#include <pthread/pthread.h>
#include <thread>

#include <ncurses.h>

#include <wchar.h>
//#include <chrono>
#include <time.h>

#include "cbmp/cbmp.h"

#define RED   1
#define GREEN 2
#define BLUE  3
#define WHITE 4

const char* grayScale[] = {"$","@","B","%","8","&","W","M","#","*","o","a","h","k","b","d","p","q","w","m","Z","O","0","Q","L","C","J","U","Y","X","z","c","v","u","n","x","r","j","f","t","/","|","(",")","1","{","}","[","]","?","-","_","+","~","<",">","i","!","l","I",";",":",",","^","`","."," "};
WINDOW* wind;

struct imgData{
    unsigned int width, height;
    BMP *bmp;
};

struct gui {
    unsigned int xChar, yChar;
    unsigned short xPix, yPix;
    winsize w;
    double aspectRatio;
};

struct pixelData{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct vec2{
    int x,y;
};

long getColour(pixelData pixDat){
    if((pixDat.r+pixDat.g+pixDat.b) < 5){
        return 4;
    }
    
    std::vector<int> pixVals = {pixDat.r,pixDat.g,pixDat.b};
    auto maxP = std::max_element(pixVals.begin(), pixVals.end());
    int max = *maxP;
    long maxPos = std::find(pixVals.begin(), pixVals.end(), max) - pixVals.begin();

    return maxPos + 1;
}

void initGui(gui &guiInfo){
    initscr();
//    start_color();
//     
//    initColours();
////    
//    init_pair(RED, COLOR_RED, COLOR_BLACK);
//    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
//    init_pair(BLUE, COLOR_BLUE, COLOR_BLACK);
//    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK);
//
    std::cout << "\e[8;40;120t";
    
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    guiInfo.w = w;
    
    wind = stdscr;
    guiInfo.xChar = getmaxx(wind);
    guiInfo.yChar = getmaxy(wind);
    
    guiInfo.xPix = w.ws_xpixel;
    guiInfo.yPix = w.ws_ypixel;
    guiInfo.aspectRatio = double(w.ws_xpixel) / double(w.ws_ypixel);
    
}

void loadImage(std::string fileName, imgData &imgStruct){
    std::string home{std::getenv("HOME")};
    
    std::string path = home;
    path += "/Desktop/xcode/ncurse/ncurse/Media/";
    path += fileName;
    
    BMP *bmpData;
    bmpData = bopen(path.data());
    if (!bmpData) {
        return;
    }
    
    imgStruct.bmp = bmpData;
    imgStruct.width = get_width(bmpData);
    imgStruct.height = get_height(bmpData);
}

std::vector<std::vector<pixelData>> getImageData(BMP *bmpImage, int width, int height){
    std::vector<std::vector<pixelData>> pixDatVec(width,std::vector<pixelData>(height));
    
    int index = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            index++;
            int x = i;
            int y = j;
            
            int index = y * bmpImage->width + x;
            pixDatVec[i][j].r = bmpImage->pixels[index].red;
            pixDatVec[i][j].g = bmpImage->pixels[index].green;
            pixDatVec[i][j].b = bmpImage->pixels[index].blue;
        }
    }
    return pixDatVec;
}

void printImage(std::vector<std::vector<pixelData>> imageData, gui guiInfo, imgData imageInfo){
    
    for (int j = (guiInfo.yChar - 1); j >= 0; j--) {
        for (int i = 0; i < guiInfo.xChar; i++) {
            unsigned int uvPixelY = double(j)/double(guiInfo.yChar) * double(imageInfo.height);
            unsigned int uvPixelX = double(i)/double(guiInfo.xChar) * double(imageInfo.width);
            
            double pixVal = (imageData[uvPixelX][uvPixelY].r +
                             imageData[uvPixelX][uvPixelY].g +
                             imageData[uvPixelX][uvPixelY].b );
            
            pixVal /= 85.3333333333; /*?*/
            
            int colIndex = getColour(imageData[uvPixelX][uvPixelY]);
            attron(COLOR_PAIR(colIndex));
    
            addch( *(grayScale[int(pixVal)]) );
            attroff(COLOR_PAIR(colIndex));
            
        }
    }
}

void drawbox(int y, int x, int width, std::string text){
    
    move(y,x);
    addch(ACS_ULCORNER);
    
    for(int i = 0; i < width; i++){
        addch(ACS_HLINE);
    }
    addch(ACS_URCORNER);
    move(y+1,x);
    addch(ACS_VLINE);
    
    printw("%s",text.c_str());
    
    move(y+1, width+x+1);

    addch(ACS_VLINE);
    wprintw(stdscr, "┣┨");
    
    move(y+2,x);
    addch(ACS_LLCORNER);
     
    for(int i = 0; i < width; i++){
        addch(ACS_HLINE);
    }
    addch(ACS_LRCORNER);
}

void drawCC(int y, int x, int width, uint8_t val){
    
    double valD = double(val);
    double widthD = double(width);
    
    double per = val/127.f;
    
    int cha = double(width) * per;
    
    mvhline(y, x, 0, cha);
//    mvaddch(y,x,'0');
    
}
bool run = true;
void* quitCheck(void* i){
    while(run){
        
        char inp = getch();
        if (inp == 'q') {
            run = false;
        }
    }
    return nullptr;
}

std::vector<uint8_t> formatValues(std::vector<std::vector<double>> vals){
    
    std::vector<uint8_t> formVals;
    for(int i = 0; i < vals[0].size(); i++){
        formVals.push_back(uint8_t(vals[0][i]));
    }
    return formVals;
}

struct ambientMidiValues2{
    std::vector<std::vector<double>> values;
    std::vector<std::string> variNames;
    bool rstNcurse;
};


void* mainGUI(void* midiValPtr) {
    
    setlocale(LC_ALL, "");
    gui guiInfo;
    initGui(guiInfo);
    
    std::vector<std::string> variNames;
    std::vector<uint8_t> midiValues;
    
    ambientMidiValues2* midiValStruct = (ambientMidiValues2*)midiValPtr;
//    midiValStruct->variNames.reserve(17);
    std::vector<std::vector<double>> midiVal = midiValStruct->values;
    
    midiValues = formatValues(midiVal);
     
    for (int i  = 0; i < 17; i++) {
//        variNames.push_back("<Variable Name>");
        std::string variNameStr = midiValStruct->variNames[i];
        variNameStr.erase(variNameStr.begin()+15, variNameStr.end());
        variNames.push_back(variNameStr);
    }
    
    vec2 boxPos; boxPos.y = 2; boxPos.x = 3;
    
    pthread_t t;
    bool running = true;
    pthread_create(&t, NULL, quitCheck, NULL);
    
    while(run){
        
//        if (midiValStruct->rstNcurse){
//            endwin();
////            pthread_join(t, NULL);
//            pthread_exit(NULL);
//        }
        
        clear();
        setlocale(LC_ALL, "");
        initGui(guiInfo);
        
        box(stdscr, 0, 0);
        
        for (int i = 0; i < variNames.size(); i++) {
            int variNameLen = variNames[i].length();
             
            if((i*4) < (getmaxy(wind) - 4)) {
                drawbox(boxPos.y + (i*4), boxPos.x, variNameLen,variNames[i]);
                
                drawbox(boxPos.y + (i*4), boxPos.x + variNameLen + 4, (getmaxx(wind)/2)-6-(variNameLen*2), "CC val");
                drawbox(boxPos.y + (i*4), boxPos.x + variNameLen + 8 + (getmaxx(wind)/2)-6-(variNameLen*2), 5, "CC " + std::to_string(i));
                drawCC(boxPos.y + (i*4)+1, boxPos.x+variNameLen+11, (getmaxx(wind)/2)-6-(variNameLen*2)-6, midiValues[i]);
            }else{
                drawbox(boxPos.y + (i*4) - (getmaxy(wind)-4), boxPos.x + (getmaxx(wind)/2), variNameLen,variNames[i]);
                
                drawbox(boxPos.y + (i*4) - (getmaxy(wind)-4), boxPos.x + (getmaxx(wind)/2) + variNameLen + 4 , (getmaxx(wind)/2)-6-(variNameLen*2), "CC val");
                drawbox(boxPos.y + (i*4) - (getmaxy(wind)-4), boxPos.x + (getmaxx(wind)/2) + variNameLen + 8 + (getmaxx(wind)/2)-6-(variNameLen*2), 5, "CC " + std::to_string(i));
                drawCC(boxPos.y + (i*4)+1 - (getmaxy(wind)-4), boxPos.x+(getmaxx(wind)/2)+variNameLen+11, (getmaxx(wind)/2)-6-(variNameLen*2)-6, midiValues[i]);
            }
        } 
        
        move(0, 0); 
        struct timespec tim; tim.tv_nsec = 50'000'000; nanosleep(&tim, NULL);
        refresh();
    }
    pthread_join(t, NULL);
     
    endwin(); // Deinitialize the ncurses library
    
    return nullptr;
}
