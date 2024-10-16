//
//  3DRenderer.h
//  WeatherWav
//
//  Created by syro Fullerton on 12/10/2024.
//

#ifndef _DRenderer_h
#define _DRenderer_h

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cmath>
#include <unistd.h>
#include <array>
#include <utility>
#include <vector>

//#include <ncurses.h>
//#include <panel.h>

#include "/Users/syro/Desktop/xcode/3dRendererLib/FINAL/libs/include/rply/my_types.hpp"
#include "/Users/syro/Desktop/xcode/3dRendererLib/FINAL/libs/include/rply/rplycpp.hpp"

#define PlaneX 0.0f
#define PlaneY -1.0f

#define CamX 0.0f
#define CamY -8.0f
#define CamZ 0.0f
#define XY 1
#define YZ 0

const int MAXPOINTS = 4096;
const int MAXOBJ = 10;

//HeliosPoint Frame[1][900];
int upKey = 0;
int numObj = 0;


#define WIDTH 800
#define HEIGHT 600

typedef struct
{
    std::uint16_t x; //12 bit (from 0 to 0xFFF)
    std::uint16_t y; //12 bit (from 0 to 0xFFF)
    std::uint8_t r;    //8 bit    (from 0 to 0xFF)
    std::uint8_t g;    //8 bit (from 0 to 0xFF)
    std::uint8_t b;    //8 bit (from 0 to 0xFF)
    std::uint8_t i;    //8 bit (from 0 to 0xFF)
    
} HeliosPoint;

HeliosPoint Frame[10][1000];

using namespace myproject;

unsigned long sizeOfFrame = ws.ws_col * ws.ws_col;

void initRenderer(){
    sizeOfFrame = ws.ws_col * ws.ws_col;
}

Surface<PointXYZRGB> PLYRead(const char* File){
    Surface<PointXYZRGB> cloud;
   // cloud.points.clear();
    rplycpp::PLYReader reader;
    reader.Open(File);
    
    auto vertices_handler = [&cloud](const std::vector<double> &vertex){
        PointXYZRGB point;
        point.x = vertex[0];
        point.y = vertex[1];
        point.z = vertex[2];
        cloud.points.push_back(point);
    };
    auto faces_handler = [&cloud](const std::vector<double> &vertex_indexes) {
        Face face;
        face.resize(static_cast<size_t>(vertex_indexes[0]));
        face[0] = vertex_indexes[1];
        face[1] = vertex_indexes[2];
        face[2] = vertex_indexes[3];
        cloud.faces.push_back(face);
      };

    std::vector<rplycpp::PLYReadHandler> handlers;
    handlers.push_back(vertices_handler);
    handlers.push_back(faces_handler);
    reader.Read(handlers);
    return cloud;
}

float PLYMaxValue(Cloud<PointXYZRGB> Points){

    float x = 0;
    float y = 0;
    float z = 0;
    float max = 0;
    
    for (int i = 0; i < (Points.points.size()); i++) {
        x = Points.points.at(i).x;
        y = Points.points.at(i).y;
        z = Points.points.at(i).z;
        if((abs(x) > max) || (abs(y) > max) || (abs(z) > max)){
            max = fmax(fmax(abs(x),abs(y)),abs(z));
        }
    }
    return  max;
}

void PLYRotX( double thetaDeg, Cloud<PointXYZRGB> &Points){
    float x,y,z = 0;
    float xbuf,ybuf,zbuf = 0;
    
    double A = cos(thetaDeg);
    double B = sin(thetaDeg);
    
    double Mat[9] =    {1.0, 0.0, 0.0,
                         0.0, A  , -B,
                         0.0, B  , A};

    for (int i = 0; i < (Points.points.size()); i++) {
        xbuf = Points.points.at(i).x;
        ybuf = Points.points.at(i).y;
        zbuf = Points.points.at(i).z;
        
        x = (Mat[0] * xbuf) + (Mat[1] * ybuf) + (Mat[2] * zbuf);
        y = (Mat[3] * xbuf) + (Mat[4] * ybuf) + (Mat[5] * zbuf);
        z = (Mat[6] * xbuf) + (Mat[7] * ybuf) + (Mat[8] * zbuf);
        
        Points.points.at(i).x = x;
        Points.points.at(i).y = y;
        Points.points.at(i).z = z;
    }
}

void PLYRotY( double thetaDeg, Cloud<PointXYZRGB> &Points){
    float x,y,z = 0;
    float xbuf,ybuf,zbuf = 0;
    
    double A = cos(thetaDeg);
    double B = sin(thetaDeg);
    double Mat[9] =    {A,   0.0, B,
                        0.0, 1.0, 0.0,
                        -B,  0.0, B};
    
    for (int i = 0; i < (Points.points.size()); i++) {
        xbuf = Points.points.at(i).x;
        ybuf = Points.points.at(i).y;
        zbuf = Points.points.at(i).z;
        
        x = (Mat[0] * xbuf) + (Mat[1] * ybuf) + (Mat[2] * zbuf);
        y = (Mat[3] * xbuf) + (Mat[4] * ybuf) + (Mat[5] * zbuf);
        z = (Mat[6] * xbuf) + (Mat[7] * ybuf) + (Mat[8] * zbuf);
        
        Points.points.at(i).x = x;
        Points.points.at(i).y = y;
        Points.points.at(i).z = z;
    }
}

void PLYRotZ( double thetaDeg, Cloud<PointXYZRGB> &Points){
    float x,y,z = 0;
    float xbuf,ybuf,zbuf = 0;
    
    double A = cos(thetaDeg);
    double B = sin(thetaDeg);
    double Mat[9] =    {A,    -B, 0.0,
                        B,     A, 0.0,
                        0.0, 0.0, 1.0};
    
    for (int i = 0; i < (Points.points.size()); i++) {
        xbuf = Points.points.at(i).x;
        ybuf = Points.points.at(i).y;
        zbuf = Points.points.at(i).z;
        
        x = (Mat[0] * xbuf) + (Mat[1] * ybuf) + (Mat[2] * zbuf);
        y = (Mat[3] * xbuf) + (Mat[4] * ybuf) + (Mat[5] * zbuf);
        z = (Mat[6] * xbuf) + (Mat[7] * ybuf) + (Mat[8] * zbuf);
        
        Points.points.at(i).x = x;
        Points.points.at(i).y = y;
        Points.points.at(i).z = z;
    }
}
// normValue is -1 to 1


float scale(float normValue, float min, float max){
    normValue = normValue / 2 + 0.5;
    return ((max * normValue ) + (min * -normValue)) + min;
}

void vectorSubtraction(float one[3], float two[3], float result[3]){
    for (int i = 0; i < 3; i++) {
        result[i] = one[i] - two[i];
    }
}
void vectorAddition(float one[3], float two[3], float result[3]){
    for (int i = 0; i < 3; i++) {
        result[i] = one[i] + two[i];
    }
}
void vectorMultiplication(float one[3], float two[3], float result[3]){
    for (int i = 0; i < 3; i++) {
        result[i] = one[i] * two[i];
    }
}
float dotProd(float one[3], float two[3]){
    float result = 0;
    for (int i = 0; i < 3; i++) {
        result += one[i] * two[i];
    }
    return result;
    
}
void crossProd(float one[3], float two[3], float result[3]){
    result[0] = one[1] * two[2] - one[2] * two[1];
    result[1] = one[2] * two[0] - one[0] * two[2];
    result[2] = one[0] * two[1] - one[1] * two[0];
}

void normal(float one[3], float two[3], float three[3], float result[3]){
    float B_A[3];
    float C_A[3];
    vectorSubtraction(two, one, B_A);
    vectorSubtraction(three, one, C_A);
    crossProd(B_A, C_A, result);
}
int insideOutside(float q[3], float normal[3],float one[3],float two[3]){
    float subtract1[3];
    float subtract2[3];
    float cross[3];
    vectorSubtraction(one, two, subtract1);
    vectorSubtraction(q, two, subtract2);
    crossProd(subtract1, subtract2, cross);
    return (dotProd(cross, normal) >= 0.0f);
}


int backFace(Surface<PointXYZRGB> Faces, Cloud<PointXYZRGB> Points, std::array<int, MAXPOINTS> visibleFlag){

    float VOI[3] = {0,0,0};
    float CAM[3] = {CamX,CamY,CamZ};
    float Fvert1[3],Fvert2[3],Fvert3[3] = {0,0,0};
    float faceNormal[3];
    float rayDirection[3];
    float d;
    float t;
    float tArry[3];
    float qBuff[3];
    float q[3];
    int BA;
    int CB;
    int AC;
    int camSide;
    float VOIqDistance[3];
    int blockedAccum = 0;
    
    double x,y,z;
    
    for (int i = 0; i < MAXPOINTS; i++) { // try only run when you have to, mark visible flag as dirty
        visibleFlag[i] = 0;
    }
    
    for (unsigned long i = Points.points.size()-1; i == 0; i--) {
        
        x = Faces.points.at(i).x;
        y = Faces.points.at(i).y;
        z = Faces.points.at(i).z;
        VOI[0] = x;
        VOI[1] = y;
        VOI[2] = z;
        
        blockedAccum = 0;
        //replace faces.points.size with all less than distance
        for(int j = 0; j < Faces.faces.size(); j++){
            
            if(i == Faces.faces.at(j)[0] ||
               i == Faces.faces.at(j)[1] ||
               i == Faces.faces.at(j)[2] )   {continue;}
  
                Fvert1[0] = Points.points.at(Faces.faces.at(j)[0]).x;
                Fvert1[1] = Points.points.at(Faces.faces.at(j)[0]).y;
                Fvert1[2] = Points.points.at(Faces.faces.at(j)[0]).z;
                
                Fvert2[0] = Points.points.at(Faces.faces.at(j)[1]).x;
                Fvert2[1] = Points.points.at(Faces.faces.at(j)[1]).y;
                Fvert2[2] = Points.points.at(Faces.faces.at(j)[1]).z;
                
                Fvert3[0] = Points.points.at(Faces.faces.at(j)[2]).x;
                Fvert3[1] = Points.points.at(Faces.faces.at(j)[2]).y;
                Fvert3[2] = Points.points.at(Faces.faces.at(j)[2]).z;
                
            normal(Fvert1,Fvert2,Fvert3,faceNormal);
            vectorSubtraction(VOI, CAM, rayDirection);
            d = dotProd(faceNormal, Fvert1);
            t = (d - (dotProd(faceNormal, CAM))) / (dotProd(faceNormal, rayDirection));
            for (int i = 0; i < 3; i++) {
                tArry[i] = t;
            }
            vectorMultiplication(tArry, rayDirection, qBuff);
            vectorAddition(CAM, qBuff, q);
            BA = insideOutside(q, faceNormal, Fvert2, Fvert1);
            CB = insideOutside(q, faceNormal, Fvert3, Fvert2);
            AC = insideOutside(q, faceNormal, Fvert1, Fvert3);
            vectorSubtraction(q, VOI, VOIqDistance);
            camSide = ((VOIqDistance[0] + VOIqDistance[1] + VOIqDistance[2])/3.0f) <= 0.0f;
            blockedAccum += (BA && CB && AC && camSide);
            
        }
        visibleFlag[i] = blockedAccum == 0;  //== Faces.faces.size();
        
    }
    return  0;
}
void normalizePoints(Cloud<PointXYZRGB> &Points, float maxValue, double Scale){
    float maxValueScalar = pow(maxValue, -1.0f);
    double min = -1;
    double max = 1;
    
    min /= Scale;
    max /= Scale;
    for (int i = 0; i < Points.points.size(); i++) {
        Points.points.at(i).x = scale(Points.points.at(i).x * maxValueScalar,-max,max);
        Points.points.at(i).y = scale(Points.points.at(i).y * maxValueScalar,-max,max);
        Points.points.at(i).z = scale(Points.points.at(i).z * maxValueScalar,-max,max);
    }
}

void normalizeSurface(Surface<PointXYZRGB> &Surface, float maxValue, double Scale){
    float maxValueScalar = pow(maxValue, -1.0f);
    double max = 1;
    max /= Scale;
    for (int i = 0; i < Surface.points.size(); i++) {
        Surface.points.at(i).x = scale(Surface.points.at(i).x * maxValueScalar,-max,max);
        Surface.points.at(i).y = scale(Surface.points.at(i).y * maxValueScalar,-max,max);
        Surface.points.at(i).z = scale(Surface.points.at(i).z * maxValueScalar,-max,max);
    }
}


void project(int plane, Cloud<PointXYZRGB> Points[], float maxValue[], std::vector<std::array<int, MAXPOINTS>> &vec){
    float x = 0;
    float y = 0;
    float z = 0;
    float Projected = 0;
    float distanceXY = 0;
    float distanceYZ = 0;
    std::array<int, MAXPOINTS> projected;
    
    for (int i = 0; i < (Points[0].points.size()); i++) {
            x = Points[0].points.at(i).x;
            y = Points[0].points.at(i).y;
            z = Points[0].points.at(i).z;
        
        if(plane){
            float angle = atan( (x-CamX) / (y-CamY) );
            distanceXY = sqrt( pow((x-CamX),2.0f) + pow((y-CamY),2.0f));
            Projected = tan(angle) * (PlaneY - CamY);
        }
        if(!plane){
            float angle = atan( (z - CamZ) / (y-CamY) );
            distanceYZ = sqrt( pow((z - CamZ),2.0f) + pow((y-CamY),2.0f));
            Projected = tan(angle) * (PlaneY - CamY);
        }
        projected[i] = (int)scale(Projected,0,0xFFF);
    }
    
    vec.push_back(projected);
}

void pointFace(Cloud<PointXYZRGB> &cloud, Surface<PointXYZRGB> surface){
    for (int i = 0; i < surface.faces.size(); i++) {
        unsigned long vertA = surface.faces.at(i)[0];
        cloud.points.at(vertA).red = i;
    }
}

int scale2(int first, int last, int incrMax, int incr){
    return  ((first - last)/-incrMax) * (incr - incrMax) + last ;
}

float steppedInterp(int iter, int iterMax){
    float val;
    float hold = 10;
    
    val = fmin(fmax((float(iter)-hold)/(float(iterMax)-(hold * 2.f)),0.f),1.f);
    val *= 10;
    val = trunc(val);
    val /= 10;
    
    return val;
}


int indexCarry = 0;
int indexHelios = 0;
int mod = 0;
int jWrap = 0;

void HeliosMakeFrame(std::vector<std::array<int, MAXPOINTS>> projectedXVec,
                     std::vector<std::array<int, MAXPOINTS>> projectedYVec,
                     int FrameTarget, unsigned long numFaces[],
                     std::vector<std::array<int, MAXPOINTS>> visibleFlagVec,
                     Surface<PointXYZRGB> surface[], Cloud<PointXYZRGB> cloud[],
                     std::vector<std::vector<std::array<int, 900>>> &vectorOfFrames){
    int hold = 40;
    hold *= 3;
    int x,y = 0;
    int xi,yi = 0;
    float interp;
    unsigned long faceIndicies[3];
    int faceIncr;
    unsigned long firstVert,secondVert,thirdVert = 0;
    int line = 0;
    int triMod = 0;
    std::vector<std::array<int, 900>> vec (5);
    vectorOfFrames.push_back(vec);
    std::array<int, 900> processedX;
    std::array<int, 900> processedY;
    std::array<int, 900> processedR;
    std::array<int, 900> processedG;
    std::array<int, 900> processedB;
    
    // 6 tri per frame
    for(int i = 0; i < 900; i ++){ // instead of looping, when indexhelios == numfaces switch to second object
        
        mod = (i+indexCarry)%hold;
        indexHelios += (mod == 0);
        
        if (indexHelios == numFaces[0]) {
            indexHelios = 0;
            // finished frame
        }
        firstVert = surface[0].faces.at(indexHelios)[0]; // repeats the same vertex multiple times
        secondVert= surface[0].faces.at(indexHelios)[1];
        thirdVert = surface[0].faces.at(indexHelios)[2];
        
        faceIndicies[0] = firstVert;
        faceIndicies[1] = secondVert;
        faceIndicies[2] = thirdVert;
        
        while ((visibleFlagVec[0][firstVert])+(visibleFlagVec[0][secondVert])+(visibleFlagVec[0][thirdVert]) < 0) {
            firstVert = surface[0].faces.at(indexHelios)[0];
            secondVert= surface[0].faces.at(indexHelios)[1];
            thirdVert = surface[0].faces.at(indexHelios)[2];
            indexHelios ++;
            indexHelios %= numFaces[0];
        }
        
        faceIncr = mod/(hold/3);

        x = projectedXVec[0][faceIndicies[faceIncr]]; // projectedVec is empty
        y = projectedYVec[0][faceIndicies[faceIncr]];
        
        xi = projectedXVec[0][faceIndicies[(faceIncr+2)%3]];
        yi = projectedYVec[0][faceIndicies[(faceIncr+2)%3]];
        
        
        triMod = mod%(hold/3);
        interp = steppedInterp(triMod, hold/3);
        
        line = (triMod > 20);
        
        x = ((float)x * interp) + ((float)xi * (-interp+1.f));
        y = ((float)y * interp) + ((float)yi * (-interp+1.f));
        
        processedX[i] = x;
        processedY[i] = y;
        processedR[i] = (line) * 255;
        processedG[i] = (line) * 255;
        processedB[i] = (line) * 255;
        
        if (i == 899) {
            indexCarry = mod;
        }
    }
    
    vec[0] = processedX;
    vec[1] = processedY;
    vec[2] = processedR;
    vec[3] = processedG;
    vec[4] = processedB;
    
    vectorOfFrames[0][0] = vec[0];
    vectorOfFrames[0][1] = vec[1];
    vectorOfFrames[0][2] = vec[2];
    vectorOfFrames[0][3] = vec[3];
    vectorOfFrames[0][4] = vec[4];
}



int count = 0;
std::string makeFrame(std::vector<std::vector<std::array<int, 900>>> vectorOfFrames){
    
    updateGUI(ws);
    int row = ws.ws_row;
    int col = ws.ws_col;
    
    std::string frameStr;
    
    count++;
    for(int i = row; i > 0; i--){
        for(int j = 0; j < col; j++){
            std::string chr;
            
            if(vectorOfFrames[0][2][i] > 127){// red
                chr = "@";
            }else{
                chr = " ";
            }
            
            frameStr += chr;
        }
        frameStr += "\n";
    }
    
    return frameStr;
}

#define Cube "/Users/syro/WeatherWav/Media/colourTest1.ply"
void mainRenderer() {
    
    bool running = true;

    Cloud<PointXYZRGB> cloudArray[10] = {
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube)};
    
    Surface<PointXYZRGB> surfaceArray[10] = {
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube),
        PLYRead(Cube)}; // make vectors
    
    int currentObj = 0;
    numObj = 10;
    
    unsigned long NumPoints[numObj]; /* int -> array */
    unsigned long numFaces[numObj];
    float maxValue[numObj];
    double scale = 5;
    std::array<int, MAXPOINTS> visibleFlag;
    std::vector<std::array<int, MAXPOINTS>> visibleFlagVec;
    std::vector<std::vector<std::array<int, 900>>> vectorOfFrames;
    
    for (int i = 0; i < numObj; i++) {
        NumPoints[i] = cloudArray[i].points.size();
        numFaces[i]  = surfaceArray[i].faces.size();
        if (NumPoints[i] >= MAXPOINTS) {
            printf("Object has too many points, Index: %i\n",i);
        }
        maxValue[i] = PLYMaxValue(cloudArray[i]);
        normalizePoints(cloudArray[i],maxValue[i],scale);
        normalizeSurface(surfaceArray[i], maxValue[i], scale);
        backFace(surfaceArray[i], cloudArray[i], visibleFlag); // error
        visibleFlagVec.push_back(visibleFlag);
        
    }
    
    std::array<int, MAXPOINTS> projectedX;
    std::array<int, MAXPOINTS> projectedY;
    std::vector<std::array<int, MAXPOINTS>> projectedXVec;
    std::vector<std::array<int, MAXPOINTS>> projectedYVec;


    project(XY, cloudArray , maxValue, projectedXVec); // clamp loops in all functions to that objects numPoints;
    project(YZ, cloudArray , maxValue, projectedYVec);

    
    
    while(running){
        
        backFace(surfaceArray[0], cloudArray[0], visibleFlag);
        
        HeliosMakeFrame(projectedXVec, projectedYVec, 0, numFaces, visibleFlagVec,surfaceArray,cloudArray, vectorOfFrames);
//        makeFrame(vectorOfFrames);
        
        project(XY, cloudArray , maxValue, projectedXVec);
        project(YZ, cloudArray , maxValue, projectedYVec);
        
        
        backFace(surfaceArray[0], cloudArray[0], visibleFlag);
        
        std::cout<<makeFrame(vectorOfFrames);
        struct timespec tim; tim.tv_nsec = 500'000'000; nanosleep(&tim, NULL);
       
    }
    
}
#endif /* _DRenderer_h */
