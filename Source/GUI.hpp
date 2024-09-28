#include "cbmp/cbmp.h"
#include <string>

const char* grayScale[] = {"$","@","B","%","8","&","W","M","#","*","o","a","h","k","b","d","p","q","w","m","Z","O","0","Q","L","C","J","U","Y","X","z","c","v","u","n","x","r","j","f","t","/","|","(",")","1","{","}","[","]","?","-","_","+","~","<",">","i","!","l","I",";",":",",","^","`","."," "};

const char* blockScale[] = {"—"," "," "," "," "}; //{"█","▓","▒","░"," "}; —

struct pixelData{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct vec2 {
    double x;
    double y;
};

struct transform{
    double scaleX;
    double scaleY;
    
    int transX;
    int transY;
};

struct imgData{
    int width, height;
    BMP *bmp;
};

struct winsize ws;
void initGUI(struct winsize &wsI){
    std::cout << "\e[8;48;160t";
    
    if( ioctl( 0, TIOCGWINSZ, &ws ) != 0 ){
        fprintf(stderr, "TIOCGWINSZ:%s\n", strerror(errno));
        exit(1);
    }
    wsI = ws;
    
}
void updateGUI(struct winsize &WS){
    ioctl( 0, TIOCGWINSZ, &ws );
    WS = ws;
}

void loadImage(char* fileName, imgData *imgStruct){
    
    std::string home{std::getenv("HOME")};
    
    std::string path = home;
    path += "/WeatherWav/Media/";
    
    std::string logoPath_S = path;
    logoPath_S += fileName;
    
    BMP *bmpData = bopen(logoPath_S.data());
    if (!bmpData) {
        return;
    }
    
    imgStruct->bmp = bmpData;
    imgStruct->width = get_width(bmpData);
    imgStruct->height = get_height(bmpData);
    
}

std::vector<std::vector<pixelData>> getImageData(BMP *bmpImage, int width, int height){
    
    std::vector<std::vector<pixelData>> pixDatVec(width,std::vector<pixelData>(height));
    
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    int index = 0;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            index++;
            int x = i;
            int y = j;
            int index = y * bmpImage->width + x;
            r = bmpImage->pixels[index].red;
            g = bmpImage->pixels[index].green;
            b = bmpImage->pixels[index].blue;
            a = bmpImage->pixels[index].alpha;
            pixDatVec[i][j].r = r;
            pixDatVec[i][j].g = g;
            pixDatVec[i][j].b = b;
            pixDatVec[i][j].a = a;
        }
    }
    return pixDatVec;
}



transform trans;

void setTransform(double scaleX, double scaleY, int transX, int transY){
    trans.scaleX = scaleX;
    trans.scaleY = scaleY;
    
    trans.transX = transX;
    trans.transY = transY;
}

std::string bitMapView(
                 int height, int width, std::vector<std::vector<pixelData>> pixDat,int colour ,double scale){
    std::string frame;
    int row = ws.ws_row;
    int col = ws.ws_col;
    double aspectRatio = double(ws.ws_xpixel)/double(ws.ws_ypixel);
    
    for (int i = row; i > 0; i--) { // height
        double uvY = double(i) / double(row);
        uvY /= aspectRatio;
        uvY *= height;
        
        for (int j = 0; j < col; j++) { // width
            double uvX = double(j) / double(col);
            uvX *= aspectRatio;
            uvX *= width;
            
            double pixVal;
            
            switch (colour) {
                case 0:{
                    pixVal = pixDat[uvX][uvY].r;
                    break;
                }
                case 1:{
                    pixVal = pixDat[uvX][uvY].g;
                    break;
                }
                case 2:{
                    pixVal = pixDat[uvX][uvY].b;
                    break;
                }
                case 3:{
                    pixVal = pixDat[uvX][uvY].a;
                    break;
                }
                    
                default:
                    break;
            }
            
            pixVal = double(pixVal)/(256.f);
            
            int random = rand()%100;

//            if(random > 20){
//                frame += blockScale[int(pixVal*5)];
//            }else{
//                frame += grayScale[int(pixVal*66)];
//            }
            frame += grayScale[int(pixVal*66)];
            
    
        }
        frame += '\n';
    }
    return frame;
}
