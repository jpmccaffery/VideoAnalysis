#include "VideoData.h"
#include <stdlib.h>
#include <vector>
#include <iomanip>
#include "QDebug"
#include "primaryfeature.h"
#include "TopicModel.h"
#include "fstream"
#include "QDir"
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

// Now we can get an array of feature counts for a bin of frames as well as the number
// of possible features as well as the key values for each feature. We'll also need to merge
// feature arrays and remove zero occurence features

using namespace cv;

CvVideoWriter *globalWriter;


//*********************************************************************
// Constructors
//*********************************************************************
VideoData::VideoData(ProjectWidget *p, MainWindow *q) :
    ProjectWidget(p, q)
{

    frameCounts.resize(0);

    M = 0;
    T = 0;
    totalFrames = 0;

    binSize = 0;
    blockWidth = 0;
    blockHeight = 0;

    FEATURE1 = NONE;
    FEATURE2 = NONE;
    FEATURE3 = NONE;
    FEATURE4 = NONE;

    width = 0;
    height = 0;

    features.resize(0);

    fname = "";
    ftype = "";

    qDebug() << "Constructor : " << this;

    dataName = "";


    COLORS.resize(9);
    COLORS[0] = cvScalar(255,0,0,1);
    COLORS[1] = cvScalar(0,0,255,1);
    COLORS[2] = cvScalar(0,255,0,1);

    COLORS[3] = cvScalar(0,255,255,1);
    COLORS[4] = cvScalar(159,121,238,1);
    COLORS[5] = cvScalar(178,58,238,1);

    COLORS[6] = cvScalar(113,198,113,1);
    COLORS[7] = cvScalar(0,69,255,1);
    COLORS[8] = cvScalar(0,128,128,1);


}


VideoData::~VideoData(){

    frameCounts.resize(0);

    M = 0;
    T = 0;
    totalFrames = 0;

    binSize = 0;
    blockWidth = 0;
    blockHeight = 0;

    FEATURE1 = NONE;
    FEATURE2 = NONE;
    FEATURE3 = NONE;
    FEATURE4 = NONE;

    width = 0;
    height = 0;

    fname = "";
    ftype = "";

    features.resize(0);
    COLORS.resize(0);
}

void VideoData::reset(){

    emit analysisComplete(0);

    X.resize(0);
    N.resize(0);

    FEATURE1 = NONE;
    FEATURE2 = NONE;
    FEATURE3 = NONE;
    FEATURE4 = NONE;
    fname = "";
    ftype = "";
    dataName = "";

    frameCounts.resize(0);
    features.resize(0);
}

//*********************************************************************
// Getters
//*********************************************************************

QString VideoData::windowTitle(){

    return wParent->windowTitle();
}

int VideoData::getGenerateTotal(){

    int totalFrames;
    CvCapture *movie;

    QString s;
    s = getProjectName() + "/" + fname + "." + ftype;

    cvSetErrMode(0);
    movie = cvCaptureFromFile(s.toStdString().c_str());
    cvQueryFrame(movie);
    totalFrames = getTotalFrameCount(movie);

    cvReleaseCapture(&movie);

    return totalFrames;
}

QString VideoData::getFeatureName(int featureType){

    QString s = "";


    switch(featureType){

    case NONE:
        s = "None";
        break;
    case EDGE_COUNTS:
        s = "Edge Strength";
        break;
    case EDGE_PRESENCE:
        s = "Edges";
        break;
    case COUNTS_HSV10:
        s = "HSV10 Strength";
        break;
    case DOMINANT_HSV10:
        s = "HSV10 Dominant";
        break;
    case AVERAGE_HSV10:
        s = "HSV10 Average";
        break;
    case COUNTS_HSV20:
        s = "HSV20 Strength";
        break;
    case DOMINANT_HSV20:
        s = "HSV20 Dominant";
        break;
    case AVERAGE_HSV20:
        s = "HSV20 Average";
        break;
    }

    return s;
}

QString VideoData::getFeatureFileName(int featureType){

    QString s = "";


    switch(featureType){

    case NONE:
        s = "None";
        break;
    case EDGE_COUNTS:
        s = "Edge_Str";
        break;
    case EDGE_PRESENCE:
        s = "Edges";
        break;
    case COUNTS_HSV10:
        s = "HSV10_Str";
        break;
    case DOMINANT_HSV10:
        s = "HSV10_Dom";
        break;
    case AVERAGE_HSV10:
        s = "HSV10_Avg";
        break;
    case COUNTS_HSV20:
        s = "HSV20_Str";
        break;
    case DOMINANT_HSV20:
        s = "HSV20_Dom";
        break;
    case AVERAGE_HSV20:
        s = "HSV20_Avg";
        break;
    }

    return s;
}

QString VideoData::getFType(){

    return ftype;
}

QString VideoData::getFName(){

    return fname;
}

QString VideoData::getDataName(){

    return dataName;
}


//*********************************************************************
// Setters
//*********************************************************************


void VideoData::setBinSize(int s){

    binSize = s;
}


void VideoData::setFeature1(int k){

    FEATURE1 = k;
}
void VideoData::setFeature2(int k){

    FEATURE2 = k;
}
void VideoData::setFeature3(int k){

    FEATURE3 = k;
}
void VideoData::setFeature4(int k){

    FEATURE4 = k;
}


void VideoData::setFeature1(QString s){

    int k;

    if(s == "HSV10 Dom "){
        k = DOMINANT_HSV10;
    }else if(s == "Edges Bin "){
        k = EDGE_PRESENCE;
    }else{
        k = NONE;
    }

    FEATURE1 = k;

}
void VideoData::setFeature2(QString s){

    int k;


    if(s == "HSV10 Dom "){
        k = DOMINANT_HSV10;
    }else if(s == "Edges Bin "){
        k = EDGE_PRESENCE;
    }else{
        k = NONE;
    }

    FEATURE2 = k;
}
void VideoData::setFeature3(QString s){

    int k;


    if(s == "HSV10 Dom "){
        k = DOMINANT_HSV10;
    }else if(s == "Edges Bin "){
        k = EDGE_PRESENCE;
    }else{
        k = NONE;
    }

    FEATURE3 = k;
}
void VideoData::setFeature4(QString s){

    int k;

    if(s == "HSV10 Dom "){
        k = DOMINANT_HSV10;
    }else if(s == "Edges Bin "){
        k = EDGE_PRESENCE;
    }else{
        k = NONE;
    }

    FEATURE4 = k;
}

void VideoData::setBlockHeight(int x){

    blockHeight = x;
}

void VideoData::setBlockWidth(int x){

    blockWidth = x;
}

void VideoData::setFName(QString name){

    fname = name;
}

void VideoData::setFType(QString name){

    ftype = name;
}

void VideoData::setDataName(QString name){

    dataName = name;
}


//*********************************************************************
// Utility Functions
//*********************************************************************

// Sometimes the metadata doesn't work so we need to count the number of frames
// manually
int VideoData::getTotalFrameCount(CvCapture *movie){

    int i;
    int totalFrames;

    totalFrames = cvGetCaptureProperty(movie, CV_CAP_PROP_FRAME_COUNT);

    if(totalFrames == 0){

        i= 0;
        while(cvGrabFrame(movie)){
            i++;
        }
        cvSetCaptureProperty(movie, CV_CAP_PROP_POS_AVI_RATIO, 0.0);
        cvSetCaptureProperty(movie, CV_CAP_PROP_POS_FRAMES, 0);
        cvSetCaptureProperty(movie, CV_CAP_PROP_POS_MSEC, 0);
        totalFrames = i-1;
    }

    return totalFrames;
}

// Sometimes the video metadata fails to provide a frame width and height properly
void VideoData::setFrameSize(CvCapture *movie, CvSize &size){

    int ht, wt;
    IplImage *frame;

    ht = cvGetCaptureProperty(movie, CV_CAP_PROP_FRAME_HEIGHT);
    wt = cvGetCaptureProperty(movie, CV_CAP_PROP_FRAME_WIDTH);

    if(ht == 0 || wt == 0){

        frame = cvQueryFrame(movie);
        ht = cvGetSize(frame).height;
        wt = cvGetSize(frame).width;

        cvSetCaptureProperty(movie, CV_CAP_PROP_POS_AVI_RATIO, 0.0);
        cvSetCaptureProperty(movie, CV_CAP_PROP_POS_FRAMES, 0);
        cvSetCaptureProperty(movie, CV_CAP_PROP_POS_MSEC, 0);

    }

    size.height = ht;
    size.width = wt;

    width = wt;
    height = ht;
}

// For now, lets just let alpha be an 8 bit uchar and we'll convert
// Also, we'll just draw onto dest so that we can do this in chunks.
void VideoData::blendAlpha(vector < IplImage* > &frames, IplImage *dest){

    int height, width;
    int i, j, k;
    unsigned char **rowptr;
    unsigned char *destptr;
    float b, g, r;
    float alpha;

    b = 0;
    g = 0;
    r = 0;

    rowptr = new unsigned char* [frames.size()];

    width = dest->width;
    height = dest->height;

    for(i=0; i < height; i++){

        for(k=0; k < frames.size(); k++){
            rowptr[k] = ((unsigned char*) frames[k]->imageData) + i * frames[k]->widthStep;
        }
        destptr = ((unsigned char*) dest->imageData) + i * dest->widthStep;

        for(j=0; j < width; j++){

            b = 0;
            g = 0;
            r = 0;
            for(k=0; k < frames.size(); k++){
                alpha = ((float) rowptr[k][j * 4 + 3]) / 255.0;
                b = b + ((float) rowptr[k][j * 4 + 0]) * alpha;
                g = g + ((float) rowptr[k][j * 4 + 1]) * alpha;
                r = r + ((float) rowptr[k][j * 4 + 2]) * alpha;
            }
            destptr[j*3 + 0] = b;
            destptr[j*3 + 1] = g;
            destptr[j*3 + 2] = r;
        }
    }
}


// For now, lets just let alpha be an 8 bit uchar and we'll convert
// Also, we'll just draw onto dest so that we can do this in chunks.
void VideoData::blend64(vector < IplImage* > frames){

    int height, width;
    int i, j, k;
    float *rowptr;

    width = frames[0]->width;
    height = frames[0]->height;

    for(k=1; k < frames.size(); k++){

        cvAdd(frames[0], frames[k], frames[0]);
    }


    for(i=0; i < height; i++){

        rowptr = ((float*) frames[0]->imageData) + i * 4 * width;//frames[0]->widthStep;

        for(j=0; j < width; j++){

            if(rowptr[j * 4 + 3] > 0){

//                qDebug() << rowptr[j*4 + 3];

                rowptr[j * 4 + 0] = rowptr[j * 4 + 0] / rowptr[j * 4 + 3];
                rowptr[j * 4 + 1] = rowptr[j * 4 + 1] / rowptr[j * 4 + 3];
                rowptr[j * 4 + 2] = rowptr[j * 4 + 2] / rowptr[j * 4 + 3];

            }
        }
    }
}

// This is a short term fix and is JUST FOR 32F type data.
void VideoData::cvMulS(IplImage* src, CvScalar scale, IplImage* dest){

    int height, width;
    int i, j, k;
    int depth;
    int nChannels;

    width = src->width;
    height = src->height;

    depth = src->depth;
    nChannels = src->nChannels;


    switch(depth){

    case IPL_DEPTH_8U:

        unsigned char *urowptr;
        unsigned char *udestptr;

        for(i=0; i < height; i++){

            urowptr = ((unsigned char*) src->imageData) + i * (src->widthStep);
            udestptr = ((unsigned char*) dest->imageData) + i * (src->widthStep);

            for(j=0; j < width; j++){

                for(k=0; k < nChannels; k++){
                    udestptr[j * nChannels + k] = (unsigned char) (((float) urowptr[j * nChannels + k]) * scale.val[k]);
                }
            }
        }
        break;


    case IPL_DEPTH_32F:

        float *frowptr;
        float *fdestptr;

        for(i=0; i < height; i++){

            frowptr = ((float*) src->imageData) + (i * src->widthStep) / 4;
            fdestptr = ((float*) dest->imageData) + (i * src->widthStep) / 4;

            for(j=0; j < width; j++){

                for(k=0; k < nChannels; k++){
                    fdestptr[j * nChannels + k] = frowptr[j * nChannels + k] * scale.val[k];
                }
            }
        }
        break;
    }


}


//**********************************************************************
// Data Generators
//**********************************************************************

// This method will populate X, N, blue, green, red, KEYS
// It will set M, T, width, height
void VideoData::generateCounts(){

    int i, j;
    int t;
    int fourcc, fps;

    int total = 0;

    IplImage *frame;
    CvCapture *movie;

    vector < IplImage* > movieFrames;
    CvSize size;


    // This goes [featuretype][timestep][index]
    vector < vector < vector < int > > > tempX;
    // This goes [featuretype][index]
    vector < vector < PrimaryFeature* > > tempFeatures;

    QString s;
    s = getProjectName() + "/" + fname + "." + ftype;

    cvSetErrMode(0);
    movie = cvCaptureFromFile(s.toStdString().c_str());
    cvQueryFrame(movie);

    // Set video statistics
    fps = cvGetCaptureProperty(movie, CV_CAP_PROP_FPS);
    fourcc = cvGetCaptureProperty(movie, CV_CAP_PROP_FOURCC);
    setFrameSize(movie, size);
    totalFrames = getTotalFrameCount(movie);

    T = (totalFrames / binSize) - 1;

    // Check that there are some frames
    if(T < 1){
        qDebug() << "Error, no frames read";
        return;
    }
    movieFrames.resize(binSize);

    // Initialize all the feature counts
    tempFeatures.resize(4);
    tempX.resize(4);

    tempFeatures[0].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE1));
    tempX[0].resize(totalFrames);
    for(i=0; i < totalFrames; i++){
        tempX[0][i].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE1));
        for(j=0; j < tempX[0][i].size(); j++){
            tempX[0][i][j] = 0;
        }
    }

    tempFeatures[1].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE2));
    tempX[1].resize(totalFrames);
    for(i=0; i < totalFrames; i++){
        tempX[1][i].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE2));
        for(j=0; j < tempX[1][i].size(); j++){
            tempX[1][i][j] = 0;
        }
    }

    tempFeatures[2].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE3));
    tempX[2].resize(totalFrames);
    for(i=0; i < totalFrames; i++){
        tempX[2][i].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE3));
        for(j=0; j < tempX[2][i].size(); j++){
            tempX[2][i][j] = 0;
        }
    }

    tempFeatures[3].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE4));
    tempX[3].resize(totalFrames);
    for(i=0; i < totalFrames; i++){
        tempX[3][i].resize(getNumberFeatures(blockWidth, blockHeight, FEATURE4));
        for(j=0; j < tempX[3][i].size(); j++){
            tempX[3][i][j] = 0;
        }
    }

    frameCounts.resize(totalFrames);

    getFeatureKeys(blockWidth, blockHeight, FEATURE1, tempFeatures[0]);
    getFeatureKeys(blockWidth, blockHeight, FEATURE2, tempFeatures[1]);
    getFeatureKeys(blockWidth, blockHeight, FEATURE3, tempFeatures[2]);
    getFeatureKeys(blockWidth, blockHeight, FEATURE4, tempFeatures[3]);


    // Set up the vectors of feature keys and counts
    // If necessary these will be contracted over zero count features

    // Prep the feature counts one bin at a time.
    t = 0;
    total = T;
    M = 0;
    if(FEATURE1 != NONE)
        M = M + tempX[0][0].size();
    if(FEATURE2 != NONE)
        M = M + tempX[1][0].size();
    if(FEATURE3 != NONE)
        M = M + tempX[2][0].size();
    if(FEATURE4 != NONE)
        M = M + tempX[3][0].size();

    emit workTickMax(totalFrames);
    emit workTick(0);

    while(true){

        // Grab a bin full of frames to get counts
        frame = cvQueryFrame(movie);

        cvSaveImage("move.png", frame);

        if(FEATURE1 != NONE && frame){

            getFeatures(frame, blockWidth, blockHeight, FEATURE1, tempX[0][t]);
        }
        if(FEATURE2 != NONE && frame){

            getFeatures(frame, blockWidth, blockHeight, FEATURE2, tempX[1][t]);
        }
        if(FEATURE3 != NONE && frame){

            getFeatures(frame, blockWidth, blockHeight, FEATURE3, tempX[2][t]);
        }
        if(FEATURE4 != NONE && frame){

            getFeatures(frame, blockWidth, blockHeight, FEATURE4, tempX[3][t]);
        }

        t++;
        if(t == totalFrames){
            break;
        }
        emit workTick(t);
    }

    // Strip out the features that don't occur in the video
    T++;
    M = stripZeroFeatures(tempX, tempFeatures);

    // Lets just call T the number of timesteps in the model including time = 0
    emit MSet(M);
    emit TSet(T);

    N.resize(T);
    N[0] = -1;

    // Initialize the N values
    setN();

    for(i=0; i < M; i++){

        features[i]->setBGR();
        features[i]->setRect();
    }

    cvReleaseCapture(&movie);

    emit workTick(-1);
    emit analysisComplete(this);
}

// This method strips out the the unused features and sets up the X values
int VideoData::stripZeroFeatures(vector < vector < vector < int > > > &tempX, vector < vector < PrimaryFeature * > > &tempFeatures){

    int i, j, k;
    int t;
    int numberFeatures = 0;

    vector < vector < bool > > isUsed;

    isUsed.resize(4);
    for(i=0; i < 4; i++){

        isUsed[i].resize(tempX[i][0].size());

        for(j=0; j < isUsed[i].size(); j++){

            isUsed[i][j] = false;
        }
    }


    for(i=0; i < 4; i++){


        for(t=0; t < tempX[i].size(); t++){

            for(j=0; j < tempX[i][t].size(); j++){


                if(tempX[i][t][j] > 0){

                    isUsed[i][j] = true;
                }
                /******************************************************************/
                // READ THIS
                //Comment this line out when you want to use variable feature numbers
                /******************************************************************/
                isUsed[i][j] = true;
            }
        }
    }

    for(i=0; i < 4; i++){


        for(j=0; j < isUsed[i].size(); j++){

            if(isUsed[i][j]){

                numberFeatures++;
            }
        }
    }

    M = numberFeatures;
    X.resize(T);
    for(int i=0; i < T; i++){
        X[i].resize(M);
    }
    features.resize(M);

    for(i=0; i < frameCounts.size(); i++){
        frameCounts[i].resize(M);
    }

    k = 0;
    for(i=0; i < 4; i++){

        for(j=0; j < isUsed[i].size(); j++){

            if(isUsed[i][j]){


                for(t=0; t < totalFrames; t++){
                    frameCounts[t][k] = tempX[i][t][j];
                }
                features[k] = tempFeatures[i][j];

                k++;
            }
            else{

                delete tempFeatures[i][j];
            }
        }
    }
    setX();

    return numberFeatures;
}

// Populates the X array
void VideoData::setX(){

    int i, j, k;
    int t;

    for(j=0; j < M; j++){

        X[0][j] = -1;
    }

    for(t=1; t < T; t++){

        for(j=0; j < M; j++){

            X[t][j] = 0;
        }
    }

    k = 0;
    for(t=1; t < T; t++){

        for(i=0; i < binSize; i++){

            for(j=0; j < M; j++){

                if(k < totalFrames){

                    X[t][j] = X[t][j] + frameCounts[k][j];
                }
            }
            k++;
        }
    }
}

// Populates the N matrix
void VideoData::setN(){

    int t, j, c;

    for(t=1; t < T; t++){
        c = 0;
        for(j=0; j < M; j++){

            c = c + X[t][j];
        }
        N[t] = c;
    }
}


//**********************************************************************
// Video Generators
//**********************************************************************

// These methods create videos of each feature
// Probably only supports edges for now
void VideoData::generateFeatureVideo1(){

    generateFeatureVideo(FEATURE1);
}
void VideoData::generateFeatureVideo2(){

    generateFeatureVideo(FEATURE2);
}
void VideoData::generateFeatureVideo3(){

    generateFeatureVideo(FEATURE3);
}
void VideoData::generateFeatureVideo4(){

    generateFeatureVideo(FEATURE4);
}

// This generates and saves a video of the features in the video
void VideoData::generateFeatureVideo(int featureType){

    int i, j;
    int maxframes;
    int t;
    int fourcc, fps;

    IplImage *frame;
    CvCapture *movie;
    CvVideoWriter *videoWriter;

    double count = 0;
    int total = 0;

    vector < IplImage* > movieFrames;
    vector < IplImage* > saveFrames;
    CvSize size;

    QString s;
    s = fname + "." + ftype;

    cvSetErrMode(0);
    movie = cvCaptureFromFile(s.toStdString().c_str());
    cvQueryFrame(movie);

    fps = cvGetCaptureProperty(movie, CV_CAP_PROP_FPS);
    fourcc = cvGetCaptureProperty(movie, CV_CAP_PROP_FOURCC);
    maxframes = getTotalFrameCount(movie);
    setFrameSize(movie, size);

    s = fname + "_" + getFeatureFileName(featureType);
    videoWriter = cvCreateVideoWriter(s.toStdString().c_str(), CV_FOURCC('X','V','I','D'), 25, size);


    T = (maxframes / binSize) - 1;

    movieFrames.resize(binSize);
    saveFrames.resize(binSize);

    // Prep the feature counts one bin at a time.
    t = 0;
    while(true){

        i = 0;
        while(frame = cvQueryFrame(movie)){

            movieFrames[i] = cvCreateImage(cvGetSize(frame), frame->depth, frame->nChannels);
            cvCopy(frame, movieFrames[i]);
            saveFrames[i] = cvCreateImage(cvGetSize(movieFrames[i]), movieFrames[i]->depth, movieFrames[i]->nChannels);

            i++;
            if(i == binSize){
                break;
            }
        }

        if(i > 0){

            //******************************************************
            // READ THIS
            // This line needs to be fixed before videos of
            // features can be generated again
            //******************************************************
//            getFeatureFrames(movieFrames, saveFrames, i, blockWidth, blockHeight, featureType);

            for(j=0; j < i; j++){

                if(!cvWriteFrame(videoWriter, saveFrames[j])){
                    cout << "Failed to write frame\n";
                }
            }

            for(j=0; j < i; j++){
                cvReleaseImage(&movieFrames[j]);
                cvReleaseImage(&saveFrames[j]);
            }
        }

        if(i < binSize){
            T = t;
            break;
        }


        count = count + 1;
        total = (count / T) * 100.0;
        emit featureVideoTick(total);

        t++;
        if(t == T){
            break;
        }
    }

    cvReleaseCapture(&movie);
    cvReleaseVideoWriter(&videoWriter);

    generateThumbnail(getProjectName() + "/videos/" + s + ".avi", getProjectName() + "/thumbs/" + s + "_thumbnail.png");
    emit featureVideoDone(s);
}


// This method generates a video with features drawn on based only
// on the feature counts. The original video is not directly used

// This method can optionally take a ZChain and draw on which topics are present or absent
void VideoData::generateCountVideo(QString saveName, vector < vector < bool > > chain){

    int i, j;
    int t;
    double average = 0;
    double max = 0.0;

    CvPoint pt1;
    CvPoint pt2;
    CvVideoWriter *videoWriter;

    IplImage *frame, *color;
    CvSize size;

    char *text;

    std::ostringstream sstream;

    text = new char[100];
    CvFont *font;
    font = new CvFont();
    cvInitFont(font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);

    size.width = width;
    size.height = height;

    frame = cvCreateImage(size, IPL_DEPTH_64F, 3);
    color = cvCreateImage(size, IPL_DEPTH_8U, 3);

    pt1.x = 0;
    pt1.y = 0;
    pt2.x = size.width;
    pt2.y = size.height;
    cvRectangle(frame, pt1, pt2, CV_RGB(0,0,0), CV_FILLED);
    cvRectangle(color, pt1, pt2, CV_RGB(0,0,0), CV_FILLED);

    for(t=1; t < (T+1); t++){
        for(j=0; j < M; j++){

            average = average + X[t][j];
            if(X[t][j] > max){
                max = X[t][j];
            }
        }
    }

    average = average / ((double) (T*M));

    videoWriter = cvCreateVideoWriter(saveName.toStdString().c_str(), CV_FOURCC('X','V','I','D'), 25, size);

    for(t=1; t < (T+1); t++){

        for(j=0; j < M; j++){

            drawKeyWeighted(j, frame, blockWidth, blockHeight, ((double) X[t][j]) / max);
        }
        cvConvertScale(frame, color, 1.0, 0.0);

        // Add the text for each topic
        if(t < T){
            for(int k= 0; k < chain[t].size(); k++){
                sstream.str("");
                strcpy(text, "");
                sstream << "cat " << (k+1) << ". " << chain[t][k];
                strcat(text, sstream.str().c_str());
                pt1.x = 0;
                pt1.y = 20 + 25 *k;
                cvPutText(color, text, pt1, font, CV_RGB(255,0,0));

                if(k > 10){
                    break;
                }
            }
        }

        pt1.x = 0;
        pt1.y = 0;

        for(i=0; i < binSize; i++){

            if(!cvWriteFrame(videoWriter, color)){
                cout << "Failed to write frame\n";
            }
        }
        cvRectangle(frame, pt1, pt2, CV_RGB(0,0,0), CV_FILLED);
    }

    cvReleaseVideoWriter(&videoWriter);
    cvReleaseImage(&frame);
    cvReleaseImage(&color);

    delete [] text;
    delete font;
}



void VideoData::generateCoarseHighlights(TopicModel *model, int slowDown, vector < vector < bool > > &chain){

    double width =0;
    double height = 0;
    double newWidth, newHeight;
    double topicWidth = 0;
    double topicHeight = 0;
    double newTopicWidth, newTopicHeight;
    double aspectRatio;
    double maxProb, maxProb2;
    vector < int > topicTags;
    vector < double > probs;

    vector < int > topicMap;
    topicMap.resize(5);
    topicMap[0] = 0;
    topicMap[1] = 1;
    topicMap[2] = 2;
    topicMap[3] = 3;
    topicMap[4] = 4;

    double scaling;

    int t = binSize;

    int K = model->K;
    int i, j, k;

    CvSize size = cvSize(1000, 600);
    CvRect rect = cvRect(0, 400, 200, 200);

    IplImage *baseImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    IplImage *frame, *scaledFrame, *scaledFinal, *scaledFinal64;
    IplImage *scaledFrame1, *scaledFrame2, *scaledFrame3, *scaledFrame4;
    vector < IplImage* > highlights;
    highlights.resize(M+1);

    IplImage *topic, *scaledTopic;

    CvPoint corner1 = cvPoint(0,0);
    CvPoint corner2 = cvPoint(999,599);

    CvPoint topicPt = cvPoint(400,0);
    CvPoint topicCorner = cvPoint(0,0);
    CvPoint videoPt = cvPoint(0,0);


    QString name = "./" + getProjectName() + "/" + fname + "." + ftype;
    CvCapture *movie = cvCaptureFromFile("/home/jack/waving.avi");//name.toStdString().c_str());
    name = "./" + getProjectName() + "/videos/" + fname + "_highlighted." + ftype;
    CvVideoWriter *videoWriter = cvCreateVideoWriter(name.toStdString().c_str(), CV_FOURCC('X','V','I','D'), 25, size);


    // Prepare the background of the frame
    cvRectangle(baseImage, corner1, corner2, cvScalar(255,255,255), CV_FILLED);
    corner1.x = 3;
    corner1.y = 3;
    corner2.x = 996;
    corner2.y = 596;
    cvRectangle(baseImage, corner1, corner2, cvScalar(0,0,0), CV_FILLED);

    cvSaveImage("a.png", baseImage);

    // Prepare the dimensions of the topics and the video
    frame = cvQueryFrame(movie);

    width = frame->width;
    height = frame->height;

    aspectRatio = ((double) width / (double) height);

    if(400 / aspectRatio < 600){

        newWidth = 390;
        newHeight = (390 / width) * height;

        scaling = 390 / width;

        videoPt.x = 5;
        videoPt.y = 5 + (590 - newHeight) / 2;

    }else{

        newHeight = 590;
        newWidth = (590 / height) * width;

        scaling = 590 / width;

        videoPt.x = 5 + (390 - newWidth) / 2;
        videoPt.y = 5;
    }

    scaledFrame = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 3);
    scaledFrame1 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
    scaledFrame2 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
    scaledFrame3 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
    scaledFrame4 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);

    scaledFinal = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 4);
    scaledFinal64 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_32F, 4);
    for(j=1; j < M+1; j++){

        highlights[j] = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_32F, 4);
    }

    // Then get the topic dimensions ready

    topic = drawTopic(model->Theta0, "", true);

    topicWidth = topic->width;
    topicHeight = topic->height;

    aspectRatio = ((double) topicWidth / (double) topicHeight);

    if(200 / aspectRatio <= 200){

        newTopicWidth = 180;
        newTopicHeight = (180 / topicWidth) * topicHeight;

        topicPt.x = 10;
        topicPt.y = 10 + (180 - newTopicHeight) / 2;

    }else{

        newTopicHeight = 180;
        newTopicWidth = (180 / topicHeight) * topicWidth;

        topicPt.x = 10 + (180 - newTopicWidth) / 2;
        topicPt.y = 10;
    }

    scaledTopic = cvCreateImage(cvSize(newTopicWidth, newTopicHeight), IPL_DEPTH_8U, 3);
    // reset the video to the beginning

    cvSetCaptureProperty(movie, CV_CAP_PROP_POS_AVI_RATIO, 0.0);
    cvSetCaptureProperty(movie, CV_CAP_PROP_POS_FRAMES, 0);
    cvSetCaptureProperty(movie, CV_CAP_PROP_POS_MSEC, 0);

    topicTags.resize(M);
    probs.resize(K+1);

    t = 1;
    i = 0;

    while(frame = cvQueryFrame(movie)){


        cvResize(frame, scaledFrame);

        rect = cvRect(videoPt.x, videoPt.y, newWidth, newHeight);
        cvSetImageROI(baseImage, rect);

        cvSplit(scaledFrame, scaledFrame1, scaledFrame2, scaledFrame3, NULL);
        cvRectangle(scaledFrame4, cvPoint(0,0), cvPoint(newWidth, newHeight), cvScalar(1), CV_FILLED);

        cvMerge(scaledFrame1, scaledFrame2, scaledFrame3, scaledFrame4, scaledFinal);
        cvConvertScale(scaledFinal, scaledFinal64);

        highlights[0] = scaledFinal64;
        cvMulS(highlights[0], cvScalar(1,1,1,0), highlights[0]);
        cvAddS(highlights[0], cvScalar(0,0,0,1), highlights[0]);
        for(j=0; j < M; j++){


            cvRectangle(highlights[j+1], cvPoint(0,0), cvPoint(newWidth, newHeight), cvScalar(0,0,0,0), CV_FILLED);

            if(X[t][j] > 0){

                probs[0] = model->Theta0[j] * model->Sigma0;

                for(k=0; k < K; k++){

                    if(chain[t][k]){
                        probs[k+1] = model->Theta[k][j] * model->Sigma[k];
                    }else{
                        probs[k+1] = 0;
                    }
                }

                maxProb = probs[0];
                maxProb2 = -1.0;
                topicTags[j] = 0;

                for(k=0; k < K; k++){

                    if(probs[k+1] > maxProb){
                        maxProb2 = maxProb;
                        maxProb = probs[k+1];
                        topicTags[j] = k+1;
                    }else if(probs[k+1] > maxProb2){

                        maxProb2 = probs[k+1];
                    }
                }

                if(maxProb2 == 0 || (maxProb / maxProb2 > 1.0)){

                    if(topicTags[j] > 0){
                        cvRectangle(highlights[j+1], cvPoint(scaling * features[j]->x, scaling * features[j]->y), cvPoint(scaling * (features[j]->x + features[j]->width), scaling * (features[j]->y + features[j]->height)), COLORS[topicMap[topicTags[j]-1]], CV_FILLED);
                    }else{
                        cvRectangle(highlights[j+1], cvPoint(scaling * features[j]->x, scaling * features[j]->y), cvPoint(scaling * (features[j]->x + features[j]->width), scaling * (features[j]->y + features[j]->height)), cvScalar(122,122,122,0.5), CV_FILLED);
                    }
                }
            }
        }

        blend64(highlights);

        cvConvertScale(scaledFinal64, scaledFinal);
        cvSplit(scaledFinal, scaledFrame1, scaledFrame2, scaledFrame3, scaledFrame4);
        cvMerge(scaledFrame1, scaledFrame2, scaledFrame3, NULL, scaledFrame);

        cvSaveImage("a.png", scaledFrame);

        cvCopy(scaledFrame, baseImage);

        for(k=0; k < K; k++){

            int m = topicMap[k];

            topicCorner.x = 400 + 200 * (m/3);
            topicCorner.y = 0 + 200 * (m%3);

            topic = drawTopic(model->Theta[k], "", true);
            cvResize(topic, scaledTopic);

            rect = cvRect(topicCorner.x + topicPt.x, topicCorner.y + topicPt.y, newTopicWidth, newTopicHeight);
            cvSetImageROI(baseImage, cvRect(0,0,1000,600));

            if(chain[t][k]){
                cvRectangle(baseImage, topicCorner, cvPoint(topicCorner.x + 200, topicCorner.y + 200), COLORS[m], CV_FILLED);
            }else{
                cvRectangle(baseImage, topicCorner, cvPoint(topicCorner.x + 200, topicCorner.y + 200), cvScalar(0,0,0), CV_FILLED);
                cvMulS(scaledTopic, cvScalar(0.5, 0.5, 0.5, 1), scaledTopic);
            }

            cvSetImageROI(baseImage, rect);
            cvCopy(scaledTopic, baseImage);

            cvReleaseImage(&topic);
        }

        cvSetImageROI(baseImage, cvRect(0,0,1000,600));
        for(j=0; j < slowDown; j++){
            cvWriteFrame(videoWriter, baseImage);
        }

        i++;
        if(i == binSize){
            i=0;
            t++;
            emit generateCountsTick();
        }
        qDebug() << "t : " << t << "\n";
    }
    topicTags.resize(0);

    cvReleaseImage(&baseImage);
    cvReleaseImage(&scaledFrame);
    cvReleaseImage(&scaledTopic);

    cvReleaseVideoWriter(&videoWriter);
    cvReleaseCapture(&movie);
}


void VideoData::generateFineHighlights(TopicModel *model, int slowDown, vector < vector < bool > > &chain){

    double width =0;
    double height = 0;
    double newWidth, newHeight;
    double topicWidth = 0;
    double topicHeight = 0;
    double newTopicWidth, newTopicHeight;
    double aspectRatio;
    double maxProb, maxProb2;
    vector < int > topicTags;
    vector < double > probs;

    vector < int > topicMap;
    topicMap.resize(5);
    topicMap[0] = 0;
    topicMap[1] = 1;
    topicMap[2] = 2;
    topicMap[3] = 3;
    topicMap[4] = 4;

    double scaling;

    int t = binSize;

    int K = model->K;
    int i, j, k;

    CvSize size = cvSize(1000, 600);
    CvRect rect = cvRect(0, 400, 200, 200);

    IplImage *baseImage = cvCreateImage(size, IPL_DEPTH_8U, 3);
    IplImage *frame, *scaledFrame, *scaledFinal, *scaledFinal64;
    IplImage *scaledFrame1, *scaledFrame2, *scaledFrame3, *scaledFrame4;
    vector < IplImage* > highlights;
    highlights.resize(M+1);

    IplImage *topic, *scaledTopic;

    CvPoint corner1 = cvPoint(0,0);
    CvPoint corner2 = cvPoint(999,599);

    CvPoint topicPt = cvPoint(400,0);
    CvPoint topicCorner = cvPoint(0,0);
    CvPoint videoPt = cvPoint(0,0);


    QString name = "./" + getProjectName() + "/" + fname + "." + ftype;
    CvCapture *movie = cvCaptureFromFile("/home/jack/waving.avi");//name.toStdString().c_str());
    name = "./" + getProjectName() + "/videos/" + fname + "_highlighted." + ftype;
    CvVideoWriter *videoWriter = cvCreateVideoWriter(name.toStdString().c_str(), CV_FOURCC('X','V','I','D'), 25, size);


    // Prepare the background of the frame
    cvRectangle(baseImage, corner1, corner2, cvScalar(255,255,255), CV_FILLED);
    corner1.x = 3;
    corner1.y = 3;
    corner2.x = 996;
    corner2.y = 596;
    cvRectangle(baseImage, corner1, corner2, cvScalar(0,0,0), CV_FILLED);

    cvSaveImage("a.png", baseImage);

    // Prepare the dimensions of the topics and the video
    frame = cvQueryFrame(movie);

    width = frame->width;
    height = frame->height;

    aspectRatio = ((double) width / (double) height);

    if(400 / aspectRatio < 600){

        newWidth = 390;
        newHeight = (390 / width) * height;

        scaling = 390 / width;

        videoPt.x = 5;
        videoPt.y = 5 + (590 - newHeight) / 2;

    }else{

        newHeight = 590;
        newWidth = (590 / height) * width;

        scaling = 590 / width;

        videoPt.x = 5 + (390 - newWidth) / 2;
        videoPt.y = 5;
    }

    scaledFrame = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 3);
    scaledFrame1 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
    scaledFrame2 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
    scaledFrame3 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
    scaledFrame4 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);

    scaledFinal = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 4);
    scaledFinal64 = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_32F, 4);
    for(j=1; j < M+1; j++){
        highlights[j] = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_32F, 4);
    }

    // Then get the topic dimensions ready

    topic = drawTopic(model->Theta0, "", true);

    topicWidth = topic->width;
    topicHeight = topic->height;

    aspectRatio = ((double) topicWidth / (double) topicHeight);

    if(200 / aspectRatio <= 200){

        newTopicWidth = 180;
        newTopicHeight = (180 / topicWidth) * topicHeight;

        topicPt.x = 10;
        topicPt.y = 10 + (180 - newTopicHeight) / 2;

    }else{

        newTopicHeight = 180;
        newTopicWidth = (180 / topicHeight) * topicWidth;

        topicPt.x = 10 + (180 - newTopicWidth) / 2;
        topicPt.y = 10;
    }

    scaledTopic = cvCreateImage(cvSize(newTopicWidth, newTopicHeight), IPL_DEPTH_8U, 3);
    // reset the video to the beginning

    cvSetCaptureProperty(movie, CV_CAP_PROP_POS_AVI_RATIO, 0.0);
    cvSetCaptureProperty(movie, CV_CAP_PROP_POS_FRAMES, 0);
    cvSetCaptureProperty(movie, CV_CAP_PROP_POS_MSEC, 0);

    topicTags.resize(M);
    probs.resize(K+1);

    t = 1;
    i = 0;

    while(frame = cvQueryFrame(movie)){


        cvResize(frame, scaledFrame);

        rect = cvRect(videoPt.x, videoPt.y, newWidth, newHeight);
        cvSetImageROI(baseImage, rect);

        cvSplit(scaledFrame, scaledFrame1, scaledFrame2, scaledFrame3, NULL);
        cvRectangle(scaledFrame4, cvPoint(0,0), cvPoint(newWidth, newHeight), cvScalar(1), CV_FILLED);

        cvMerge(scaledFrame1, scaledFrame2, scaledFrame3, scaledFrame4, scaledFinal);
        cvConvertScale(scaledFinal, scaledFinal64);

        highlights[0] = scaledFinal64;
        cvMulS(highlights[0], cvScalar(1,1,1,0), highlights[0]);
        cvAddS(highlights[0], cvScalar(0,0,0,1), highlights[0]);
        for(j=0; j < M; j++){


            cvRectangle(highlights[j+1], cvPoint(0,0), cvPoint(newWidth, newHeight), cvScalar(0,0,0,0), CV_FILLED);

            if(frameCounts[i][j] > 0){

                probs[0] = model->Theta0[j] * model->Sigma0;

                for(k=0; k < K; k++){

                    if(chain[t][k]){
                        probs[k+1] = model->Theta[k][j] * model->Sigma[k];
                    }else{
                        probs[k+1] = 0;
                    }
                }

                maxProb = probs[0];
                maxProb2 = -1.0;
                topicTags[j] = 0;

                for(k=0; k < K; k++){

                    if(probs[k+1] > maxProb){
                        maxProb2 = maxProb;
                        maxProb = probs[k+1];
                        topicTags[j] = k+1;
                    }else if(probs[k+1] > maxProb2){

                        maxProb2 = probs[k+1];
                    }
                }

                if(maxProb2 == 0 || (maxProb / maxProb2 > 1.0)){

                    if(topicTags[j] > 0){
                        cvRectangle(highlights[j+1], cvPoint(scaling * features[j]->x, scaling * features[j]->y), cvPoint(scaling * (features[j]->x + features[j]->width), scaling * (features[j]->y + features[j]->height)), COLORS[topicMap[topicTags[j]-1]], CV_FILLED);
                    }else{
                        cvRectangle(highlights[j+1], cvPoint(scaling * features[j]->x, scaling * features[j]->y), cvPoint(scaling * (features[j]->x + features[j]->width), scaling * (features[j]->y + features[j]->height)), cvScalar(122,122,122,0.5), CV_FILLED);
                    }
                }
            }
        }

        blend64(highlights);


        cvConvertScale(scaledFinal64, scaledFinal);
        cvSplit(scaledFinal, scaledFrame1, scaledFrame2, scaledFrame3, scaledFrame4);
        cvMerge(scaledFrame1, scaledFrame2, scaledFrame3, NULL, scaledFrame);

        cvSaveImage("a.png", scaledFrame);

        cvCopy(scaledFrame, baseImage);

        for(k=0; k < K; k++){

            int m = topicMap[k];

            topicCorner.x = 400 + 200 * (m/3);
            topicCorner.y = 0 + 200 * (m%3);

            topic = drawTopic(model->Theta[k], "", true);
            cvResize(topic, scaledTopic);

            rect = cvRect(topicCorner.x + topicPt.x, topicCorner.y + topicPt.y, newTopicWidth, newTopicHeight);
            cvSetImageROI(baseImage, cvRect(0,0,1000,600));

            if(chain[t][k]){
                cvRectangle(baseImage, topicCorner, cvPoint(topicCorner.x + 200, topicCorner.y + 200), COLORS[m], CV_FILLED);
            }else{
                cvRectangle(baseImage, topicCorner, cvPoint(topicCorner.x + 200, topicCorner.y + 200), cvScalar(0,0,0), CV_FILLED);
                cvMulS(scaledTopic, cvScalar(0.5, 0.5, 0.5, 1), scaledTopic);
            }

            cvSetImageROI(baseImage, rect);
            cvCopy(scaledTopic, baseImage);

            cvReleaseImage(&topic);
        }

        cvSetImageROI(baseImage, cvRect(0,0,1000,600));
        for(j=0; j < slowDown; j++){
            cvWriteFrame(videoWriter, baseImage);
        }

        i++;

        if(i%binSize == 0){

            t++;
        }

        if(i == totalFrames){
            break;
        }
        emit generateCountsTick();
    }
    topicTags.resize(0);

    cvReleaseImage(&baseImage);
    cvReleaseImage(&scaledFrame);
    cvReleaseImage(&scaledTopic);

    cvReleaseVideoWriter(&videoWriter);
    cvReleaseCapture(&movie);
}

//**********************************************************************
// Image Generators
//**********************************************************************

// Creates a visualization of the topic specified by weights
IplImage* VideoData::drawTopic(vector < double > &weights, QString s, bool wantReturnImage){

    double max;
    int i;
    int count;

    max = 0;
    for(i=0; i < M; i++){

        if(weights[i] > max){
            max = weights[i];
        }
    }
    cout << "Max : " << max << "\n";

    CvSize size;
    CvPoint pt1, pt2;
    size.width = width;
    size.height = height;

    IplImage *blank;
    IplImage *final;

    blank = cvCreateImage(size, IPL_DEPTH_64F, 3);
    final = cvCreateImage(size, IPL_DEPTH_8U, 3);

    pt1.x = 0;
    pt1.y = 0;
    pt2.x = size.width;
    pt2.y = size.height;
    cvRectangle(blank, pt1, pt2, cvScalar(0,0,0), CV_FILLED);
    cvRectangle(final, pt1, pt2, cvScalar(0,0,0), CV_FILLED);

    count = 0;
    for(i=0; i < M; i++){
        count++;
        drawKeyWeighted(i, blank, blockWidth, blockHeight, weights[i] / max);
    }

    if(!wantReturnImage){
        cvSaveImage(s.toStdString().c_str(), blank);
    }

    cvReleaseImage(&final);
    if(!wantReturnImage){
        cvReleaseImage(&blank);
        blank = 0;
    }
    return blank;
}

// Just be sure to do everything in HSV and then convert when it's time to save
// This requires an alpha channel weight between 0 and 1.
// Lets switch to BGR in this method
// Also, we're just going to take an IPL_DEPTH_64F image so that we can keep a running total in one image
// We were having memory problems and filling a lot of time.
void VideoData::drawKeyWeighted(int feature, IplImage *image, int bwidth, int bheight, double weight){

    int x, y;
    int i, j;
    double *rowptr;

    x = features[feature]->x;
    y = features[feature]->y;



    for(i=0; i < bheight; i++){
        if((y + i) >= height){break;}
        rowptr = (((double*) image->imageData) + (y + i) * (image->widthStep) / 8) + x * 3;
        for(j=0; j < bwidth; j++){
            if((x + j) >= width){break;}
            rowptr[j * 3 + 0] = rowptr[j * 3 + 0] + weight * features[feature]->blue;
            rowptr[j * 3 + 1] = rowptr[j * 3 + 1] + weight * features[feature]->green;
            rowptr[j * 3 + 2] = rowptr[j * 3 + 2] + weight * features[feature]->red;
        }
    }
}

void VideoData::generateThumbnail(QString sourceVideo, QString thumbOut){

    int a;
    string test;

    Mat frame;

    test = sourceVideo.toStdString();
    VideoCapture cap(sourceVideo.toStdString());

    if(!cap.isOpened()){

        qDebug() << "Failure to initialize camera";
    }
    for(a=0; a < 5; a++){

        cap >> frame;
    }
    imwrite(thumbOut.toStdString(), frame);

    cap.release();

}

//**********************************************************
// Save / Load functions
//**********************************************************

int VideoData::getSaveTotal(){

    int total = 0;

    total = total + 14;
    total = total + 5 * M;
    total = total + M * (T+1);
    total = total + totalFrames * M;

    return 2 * total;
}


int VideoData::getFastSaveTotal(){

    int total = 0;

    total = total + 14;
    total = total + 5 * M;
    total = total + M * (T+1);
    total = total + totalFrames * M;

    return total;
}

int VideoData::getLoadTotal(){

    ifstream file;

    file.open((getProjectName() + "/data/" + dataName + "_data.data").toStdString().c_str());

    file >> T;
    file >> M;
    file >> totalFrames;

    int total = 0;

    total = total + 14;
    total = total + 5 * M;
    total = total + M * (T+1);
    total = total + totalFrames * M;

    file.close();

    return total;
}


void VideoData::save(){

    save(getProjectName() + "/data/" + dataName + "_data.db");
    fastSave(getProjectName() + "/data/" + dataName + "_data.data");
}


void VideoData::fastSave(){

    fastSave(getProjectName() + "/data/" + dataName + "_data.data");
}


void VideoData::slowLoad(){

//    slowLoad(getProjectName() + "/data/" + dataName + "_data.db");
    load(getProjectName() + "/data/" + dataName + "_data.db");
}


void VideoData::load(){

    load(getProjectName() + "/data/" + dataName + "_data.data");
}

void VideoData::save(QString qname){

    DataManager *datamanager = new DataManager(this, 0);

    connect(datamanager, SIGNAL(saveTick(int)), this, SIGNAL(loadSaveTick(int)));

    cout << "Saving dataset...\n";

    /*************************************/
    // Save parameters
    /*************************************/

    datamanager->saveInt(qname, "totalFrames", "totalFrames", totalFrames);
    datamanager->saveInt(qname, "T", "T", T);
    datamanager->saveInt(qname, "M", "M", M);
    datamanager->saveInt(qname, "Width", "W", width);
    datamanager->saveInt(qname, "Height", "H", height);
    datamanager->saveInt(qname, "binSize", "binSize", binSize);
    datamanager->saveInt(qname, "blockW", "blockW", blockWidth);
    datamanager->saveInt(qname, "blockH", "blockH", blockHeight);

//    datamanager->saveString(qname, "filename", "filename", fname);
//    datamanager->saveString(qname, "filetype", "filetype", ftype);

    datamanager->saveInt(qname, "feature1", "feature1", FEATURE1);
    datamanager->saveInt(qname, "feature2", "feature2", FEATURE2);
    datamanager->saveInt(qname, "feature3", "feature3", FEATURE3);
    datamanager->saveInt(qname, "feature4", "feature4", FEATURE4);


    /*************************************/
    // Save bgr
    /*************************************/

    vector < vector < double > > storage;
    storage.resize(M);
    for(int i=0; i < M; i++){
        storage[i].resize(5);

        storage[i][0] = features[i]->blue;
        storage[i][1] = features[i]->green;
        storage[i][2] = features[i]->red;
        storage[i][3] = features[i]->x;
        storage[i][4] = features[i]->y;
    }

    vector < QString > sqlKeys;
    sqlKeys.resize(5);
    sqlKeys[0] = "blue";
    sqlKeys[1] = "green";
    sqlKeys[2] = "red";
    sqlKeys[3] = "x";
    sqlKeys[4] = "y";

    vector < QString > types;
    types.resize(5);
    types[0] = "INT";
    types[1] = "INT";
    types[2] = "INT";
    types[3] = "INT";
    types[4] = "INT";

    datamanager->saveDump(qname, "bgr", sqlKeys, types, storage);

    cout << "Saving counts...\n";
    /*************************************/
    // Save N counts
    /*************************************/

    datamanager->saveArray(qname, "N", "timestep", "count", N);

    /*************************************/
    // Save X counts
    /*************************************/

    sqlKeys.resize(3);
    sqlKeys[0] = "time";
    sqlKeys[1] = "feature";
    sqlKeys[2] = "count";

    datamanager->saveArray(qname, "X", sqlKeys, X);

    /*************************************/
    // Save Frame counts
    /*************************************/

    vector < vector < double > > frameSave;
    frameSave.resize(totalFrames);
    for(int i=0; i < totalFrames; i++){
        frameSave.resize(M);
        for(int j=0; j < M; j++){

            frameSave[i][j] = (double) frameCounts[i][j];
        }
    }

    sqlKeys.resize(3);
    sqlKeys[0] = "time";
    sqlKeys[1] = "feature";
    sqlKeys[2] = "count";

    datamanager->saveArray(qname, "FrameX", sqlKeys, frameSave);

    cout << "Finished saving dataset.\n";

    disconnect(datamanager, SIGNAL(saveTick(int)), this, SIGNAL(loadSaveTick(int)));

    delete datamanager;

}


void VideoData::fastSave(QString qname){

    int remainder;

    ofstream file;

    file.open(qname.toStdString().c_str());

    cout << "Saving dataset...\n";


    /*************************************/
    // Save parameters
    /*************************************/

    file << T << endl;
    file << M << endl;
    file << totalFrames << endl;

    file << fname.toStdString() << endl;
    file << ftype.toStdString() << endl;

    file << FEATURE1 << endl;
    file << FEATURE2 << endl;
    file << FEATURE3 << endl;
    file << FEATURE4 << endl;

    file << width << endl;
    file << height << endl;
    file << binSize << endl;
    file << blockWidth << endl;
    file << blockHeight << endl;

    emit loadSaveTick(14);

    /*************************************/
    // Save bgr
    /*************************************/

    remainder = M * 5;

    for(int i=0; i < M; i++){

        file << features[i]->blue << endl;
        file << features[i]->green << endl;
        file << features[i]->red << endl;
        file << features[i]->x << endl;
        file << features[i]->y << endl;

        if((i+1)%200 == 0){

            emit loadSaveTick(1000);
            remainder = remainder - 1000;
        }
    }
    emit loadSaveTick(remainder);

    cout << "Saving counts...\n";
    /*************************************/
    // Save N counts
    /*************************************/

    remainder = T;
    for(int i=0; i < T; i++){

        file << N[i] << endl;
        if((i+1)%500 == 0){

            emit loadSaveTick(500);
            remainder = remainder - 500;
        }
    }
    emit loadSaveTick(remainder);
    /*************************************/
    // Save X counts
    /*************************************/

    remainder = T * M;
    for(int t=0; t < T; t++){
        for(int j=0; j < M; j++){

            file << X[t][j] << endl;
        }
        if((t+1)%20 == 0){

            emit loadSaveTick(20 * M);
            remainder = remainder - 20 * M;
        }
    }
    emit loadSaveTick(remainder);

    /*************************************/
    // Save Frame counts
    /*************************************/

    remainder = totalFrames * M;
    for(int i=0; i < totalFrames; i++){
        for(int j=0; j < M; j++){

            file << frameCounts[i][j] << endl;
        }
        if((i+1)%20 == 0){

            emit loadSaveTick(20 * M);
            remainder = remainder - 20 * M;
        }
    }
    emit loadSaveTick(remainder);

    cout << "Finished saving dataset.\n";

    file.close();
}


void VideoData::load(QString qname){

    string nm, tp;

    ifstream file;

    int remainder;

    file.open(qname.toStdString().c_str());

    cout << "Saving dataset...\n";


    /*************************************/
    // Save parameters
    /*************************************/

    file >> T;
    file >> M;
    file >> totalFrames;

    file >> nm;
    file >> tp;

    fname = QString::fromStdString(nm);
    ftype = QString::fromStdString(tp);


    file >> FEATURE1;
    file >> FEATURE2;
    file >> FEATURE3;
    file >> FEATURE4;

    file >> width;
    file >> height;
    file >> binSize;
    file >> blockWidth;
    file >> blockHeight;

    emit loadSaveTick(14);

    /*************************************/
    // Save bgr
    /*************************************/

    remainder = 5 * M;
    features.resize(M);
    for(int i=0; i < M; i++){

        features[i] = new PrimaryFeature();

        file >> features[i]->blue;
        file >> features[i]->green;
        file >> features[i]->red;
        file >> features[i]->x;
        file >> features[i]->y;

        if((i+1)%200 == 0){

            emit loadSaveTick(1000);
            remainder = remainder - 1000;
        }
    }
    emit loadSaveTick(remainder);

    cout << "Saving counts...\n";
    /*************************************/
    // Save N counts
    /*************************************/

    N.resize(T);
    remainder = T;
    for(int i=0; i < T; i++){

        file >> N[i];
        if((i+1)%500 == 0){

            emit loadSaveTick(500);
            remainder = remainder - 500;
        }
    }
    emit loadSaveTick(remainder);

    /*************************************/
    // Save X counts
    /*************************************/

    X.resize(T);
    remainder = T * M;
    for(int t=0; t < T; t++){
        X[t].resize(M);
        for(int j=0; j < M; j++){

            file >> X[t][j];
        }
        if((t+1)%20 == 0){

            emit loadSaveTick(20 * M);
            remainder = remainder - 20 * M;
        }
    }
    emit loadSaveTick(remainder);

    /*************************************/
    // Save Frame counts
    /*************************************/

    remainder = totalFrames * M;
    frameCounts.resize(totalFrames);
    for(int i=0; i < totalFrames; i++){

        frameCounts[i].resize(M);
        for(int j=0; j < M; j++){

            file >> frameCounts[i][j];
        }
        if((i+1)%20 == 0){

            emit loadSaveTick(20 * M);
            remainder = remainder - 20 * M;
        }
    }
    emit loadSaveTick(remainder);

    cout << "Finished saving dataset.\n";

    file.close();
}

//**************************************************************
// Processing Functions
//**************************************************************

void VideoData::subtractBackground(QString subtractName){

    VideoCapture movie("./jogging.avi");
    VideoWriter writer;
    Mat frame;
    Mat means;
    float *rowptr;
    unsigned char *movieptr;
    int numframes = 1;


    // Collect the averages
    if(movie.read(frame)){

        means = Mat::zeros(frame.rows, frame.cols, CV_32FC3);

        qDebug() << frame.channels();


        for(int i=0; i < means.rows; i++){

            rowptr = (float*) means.ptr(i);
            movieptr = (unsigned char*) frame.ptr(i);
            for(int j=0; j < means.cols; j++){

                rowptr[j*3] = (float) movieptr[j*3];
                rowptr[j*3+1] = (float) movieptr[j*3+1];
                rowptr[j*3+2] = (float) movieptr[j*3+2];
            }
        }


        while(movie.read(frame)){

            for(int i=0; i < means.rows; i++){

                rowptr = (float*) means.ptr(i);
                movieptr = (unsigned char*) frame.ptr(i);
                for(int j=0; j < means.cols; j++){

                    rowptr[j*3] = rowptr[j*3] + (float) movieptr[j*3];
                    rowptr[j*3+1] = rowptr[j*3+1] + (float) movieptr[j*3+1];
                    rowptr[j*3+2] = rowptr[j*3+2] + (float) movieptr[j*3+2];
                }
            }

            numframes++;
        }


        for(int i=0; i < means.rows; i++){

            rowptr = (float*) means.ptr(i);
            for(int j=0; j < means.cols; j++){

                rowptr[j*3] = rowptr[j*3] / (float) numframes;
                rowptr[j*3+1] = rowptr[j*3+1] / (float) numframes;
                rowptr[j*3+2] = rowptr[j*3+2] / (float) numframes;
            }
        }

        imwrite("test.png", means);
    }

    movie.release();

    // Then subtract them off and write the output

    movie.open("jogging.avi");

    if(movie.read(frame)){

        writer.open("joggingOut.avi", CV_FOURCC('X', 'V', 'I', 'D'), 30, frame.size());

        for(int i=0; i < means.rows; i++){

            rowptr = (float*) means.ptr(i);
            movieptr = (unsigned char*) frame.ptr(i);
            for(int j=0; j < means.cols; j++){


                if(movieptr[j*3] > (unsigned char) rowptr[j*3]){
                    movieptr[j*3] = movieptr[j*3] - (unsigned char) rowptr[j*3];
                }else{
                    movieptr[j*3] = 0;
                }
                if(movieptr[j*3+1] > (unsigned char) rowptr[j*3+1]){
                    movieptr[j*3+1] = movieptr[j*3+1] - (unsigned char) rowptr[j*3+1];
                }else{
                    movieptr[j*3+1] = 0;
                }
                if(movieptr[j*3+2] > (unsigned char) rowptr[j*3+2]){
                    movieptr[j*3+2] = movieptr[j*3+2] - (unsigned char) rowptr[j*3+2];
                }else{
                    movieptr[j*3+2] = 0;
                }
            }
        }
        writer << frame;

        while(movie.read(frame)){

            for(int i=0; i < means.rows; i++){

                rowptr = (float*) means.ptr(i);
                movieptr = (unsigned char*) frame.ptr(i);
                for(int j=0; j < means.cols; j++){


                    if(movieptr[j*3] > (unsigned char) rowptr[j*3]){
                        movieptr[j*3] = movieptr[j*3] - (unsigned char) rowptr[j*3];
                    }else{
                        movieptr[j*3] = 0;
                    }
                    if(movieptr[j*3+1] > (unsigned char) rowptr[j*3+1]){
                        movieptr[j*3+1] = movieptr[j*3+1] - (unsigned char) rowptr[j*3+1];
                    }else{
                        movieptr[j*3+1] = 0;
                    }
                    if(movieptr[j*3+2] > (unsigned char) rowptr[j*3+2]){
                        movieptr[j*3+2] = movieptr[j*3+2] - (unsigned char) rowptr[j*3+2];
                    }else{
                        movieptr[j*3+2] = 0;
                    }
                }
            }
            writer << frame;
        }

        movie.release();
    }
}



void VideoData::centerOnMass(QString fname, QString outname){

    VideoCapture movie(fname.toStdString().c_str());
    VideoWriter writer;
    Mat frame;
    Mat means;

    VideoWriter writer2;

    vector < int > xmeans;
    vector < int > ymeans;

    unsigned char *rowptr;
    unsigned char *movieptr;
    int numframes = 1;

    double xtotal = 0;
    double ytotal = 0;

    double xvalue = 0;
    double yvalue = 0;

    int xmin = 10000;
    int ymin = 10000;
    int xmax = 0;
    int ymax = 0;

    int k;

    int croppedWidth;
    int croppedHeight;

    int width, height;
    vector < Mat > canny;

    xmeans.resize(0);
    ymeans.resize(0);

    // Collect the averages
    if(movie.read(frame)){

        split(frame, canny);
        cv::Canny(canny[0], canny[1],100,300);
        canny[1].copyTo(canny[0]);
        canny[1].copyTo(canny[2]);

        width = frame.cols;
        height = frame.rows;

        means = Mat(height, width, CV_8UC1);
        split(frame,canny);

        for(int i=0; i < frame.rows; i++){

            movieptr = (unsigned char*) canny[0].ptr(i);
            for(int j=0; j < frame.cols; j++){

                xvalue = xvalue + j * (double) movieptr[j];
                xtotal = xtotal + (double) movieptr[j];

                yvalue = yvalue + i * (double) movieptr[j];
                ytotal = ytotal + (double) movieptr[j];
            }
        }

        if(xtotal > 0){
            xvalue = xvalue / xtotal;
        }else{
            xvalue = width / 2;
        }
        if(ytotal > 0){
            yvalue = yvalue / ytotal;
        }else{
            yvalue = height / 2;
        }

        if(xvalue < 20){
            xvalue = 20;
        }if(yvalue < 60){
            yvalue = 60;
        }
        if((width - xvalue) < 20){
            xvalue = width - 20;
        }if((height - yvalue) < 60){
            yvalue = height - 60;
        }

        if(xvalue > xmax){
            xmax = xvalue;
        }if(xvalue < xmin){
            xmin = xvalue;
        }if(yvalue > ymax){
            ymax = yvalue;
        }if(yvalue < ymin){
            ymin = yvalue;
        }

        xmeans.push_back(xvalue);
        ymeans.push_back(yvalue);


        writer2.open("canny2.avi", CV_FOURCC('X', 'V', 'I', 'D'), 30, cv::Size(width, height));


        while(movie.read(frame)){

            xvalue = 0;
            yvalue = 0;
            xtotal = 0;
            ytotal = 0;

            split(frame, canny);
            cv::Canny(canny[0], canny[1],100,300);
            canny[1].copyTo(canny[0]);
            canny[1].copyTo(canny[2]);

            for(int i=0; i < frame.rows; i++){

                movieptr = (unsigned char*) canny[0].ptr(i);
                for(int j=0; j < frame.cols; j++){

                    xvalue = xvalue + j * (double) movieptr[j];
                    xtotal = xtotal + (double) movieptr[j];

                    yvalue = yvalue + i * (double) movieptr[j];
                    ytotal = ytotal + (double) movieptr[j];
                }
            }

            cv::merge(canny, frame);

            writer2 << frame;

            if(xtotal > 0){
                xvalue = xvalue / xtotal;
            }else{
                xvalue = width / 2;
            }
            if(ytotal > 0){
                yvalue = yvalue / ytotal;
            }else{
                yvalue = height / 2;
            }


            if(xvalue < 20){
                xvalue = 20;
            }if(yvalue < 60){
                yvalue = 60;
            }
            if((width - xvalue) < 20){
                xvalue = width - 20;
            }if((height - yvalue) < 60){
                yvalue = height - 60;
            }


            if(xvalue > xmax){
                xmax = xvalue;
            }if(xvalue < xmin){
                xmin = xvalue;
            }if(yvalue > ymax){
                ymax = yvalue;
            }if(yvalue < ymin){
                ymin = yvalue;
            }

            xmeans.push_back(xvalue);
            ymeans.push_back(yvalue);

            numframes++;
        }

    }

    if(ymin < (height - ymax)){
        croppedHeight = 2 * ymin;
    }else{
        croppedHeight = 2 * (height - ymax);
    }

    if(xmin < (width - xmax)){
        croppedWidth = 2 * xmin;
    }else{
        croppedWidth = 2 * (width - xmax);
    }

//    croppedWidth = 40;
//    croppedHeight = 100;


    movie.release();

    // Then subtract them off and write the output

    movie.open(fname.toStdString().c_str());

    k = 0;

    if(movie.read(frame)){

        Mat outFrame(croppedHeight, croppedWidth, CV_8UC3);
        Mat mask(height, width, CV_8UC3);

        writer.open(outname.toStdString().c_str(), CV_FOURCC('X', 'V', 'I', 'D'), 30, cv::Size(width, height));

        for(int i=0; i < mask.rows; i++){

            rowptr = (unsigned char*) mask.ptr(i);
            for(int j=0; j < mask.cols; j++){

                if(i > (ymeans[k] - (croppedHeight/2)) && i < (ymeans[k] + (croppedHeight/2)) && j > (xmeans[k] - (croppedWidth/2)) && j < (xmeans[k] + (croppedWidth/2))){
                    rowptr[j*3] = 255;
                    rowptr[j*3+1] = 255;
                    rowptr[j*3+2] = 255;
                }else{
                    rowptr[j*3] = 0;
                    rowptr[j*3+1] = 0;
                    rowptr[j*3+2] = 0;
                }
            }
        }

//        outFrame = frame(Rect(xmeans[k] - (croppedWidth / 2), ymeans[k] - (croppedHeight / 2), croppedWidth,croppedHeight));

//        frame.copyTo(outFrame, mask);
//        writer << outFrame;
        k++;

        while(movie.read(frame)){

            Mat subRegionSrc, subRegionDest;
            outFrame = Mat::zeros(frame.rows, frame.cols, CV_8UC1);

            split(frame, canny);

            subRegionSrc = canny[0](Rect(xmeans[k] - (croppedWidth / 2), ymeans[k] - (croppedHeight / 2), croppedWidth,croppedHeight));
//            outFrame = canny[0](Rect(0,0,width/2,height/2));
            subRegionDest = outFrame(Rect(width/2 - croppedWidth/2, height/2 - croppedHeight/2, croppedWidth, croppedHeight));
            subRegionSrc.copyTo(subRegionDest);


            for(int u=0; u < 20; u++){
                blur(outFrame, frame, Size(5,5));
                blur(frame, outFrame, Size(5,5));
                subRegionSrc.copyTo(subRegionDest);
            }

            canny[0] = outFrame;
            canny[1] = outFrame;
            canny[2] = outFrame;

            cv::merge(canny, outFrame);

            imwrite("test.png", outFrame);
  //          frame.copyTo(outFrame, mask);
            writer << outFrame;


            k++;
        }

        movie.release();
    }
}




//**************************************************************************
// Entering the big section that
// Counts features in video frames
//**************************************************************************

// Here we get the number of features each type creates
int VideoData::getNumberFeatures(int block_width, int block_height, int code){

    int num_features;

    switch(code){

    /***********************************************************************************/
    /***********************************************************************************/
    // Edge counts present or absent
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_PRESENCE:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // Edge Counts Totalled
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_COUNTS:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Counts with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case COUNTS_HSV10:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Dominant Colors with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case DOMINANT_HSV10:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Average Colors with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case AVERAGE_HSV10:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Counts with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case COUNTS_HSV20:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Dominant Colors with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case DOMINANT_HSV20:
        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Average Colors with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case AVERAGE_HSV20:
        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // Edge Presence with center of mass tracking
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_PRESENCE_CENTERED:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // If no feature then just add 0
    /***********************************************************************************/
    /***********************************************************************************/
    case NONE:
        // lets make the features be ordered as x by y by color
        num_features = 0;
        break;

    }

    return num_features;
}

// Here we get an array of feature keys for each type of feature set.
void VideoData::getFeatureKeys(int block_width, int block_height, int code, vector < PrimaryFeature * > &bigFeatures){

    int i, j, k;
    int a;
    float bump = 0;
    int num_features;
    CvSize *size;
    CvPoint *pt1 = new CvPoint();
    CvPoint *pt2 = new CvPoint();

    IplImage *colorchart;
    size = new CvSize();
    size->width = 1;
    size->height = 1;
    colorchart = cvCreateImage(*size, IPL_DEPTH_8U, 3);
    pt1->x = 0;
    pt1->y = 0;
    pt2->x = 1;
    pt2->y = 1;
    cvRectangle(colorchart, *pt1, *pt2, cvScalar(0,0,0), CV_FILLED);


    switch(code){

    /***********************************************************************************/
    /***********************************************************************************/
    // Edge counts present or absent
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_PRESENCE:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;
            }
        }
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // Edge counts present or absent tracking center of mass
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_PRESENCE_CENTERED:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;
            }
        }
        break;


    /***********************************************************************************/
    /***********************************************************************************/
    // Edge Counts Totalled
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_COUNTS:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;
            }
        }
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Counts with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case COUNTS_HSV10:

        // lets make the features be ordered as x by y by color
        bump = 181.0 / 8.0;
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 0;

                k++;
                for(a= 0; a < 8; a++){

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255;

                    k++;
                }
            }
        }
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Dominant Colors with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case DOMINANT_HSV10:

        cvReleaseImage(&colorchart);
        size->width = 500;
        size->height = 200;
        colorchart = cvCreateImage(*size, IPL_DEPTH_8U, 3);

        pt1->x = 0;
        pt1->y = 0;
        pt2->x = 500;
        pt2->y = 200;
        cvRectangle(colorchart, *pt1, *pt2, cvScalar(0,0,0), CV_FILLED);

        pt1->x = 0;
        pt1->y = 0;
        pt2->x = 100;
        pt2->y = 100;
        cvRectangle(colorchart, *pt1, *pt2, cvScalar(0,0,255), CV_FILLED);

        pt1->x = 100;
        pt1->y = 0;
        pt2->x = 200;
        pt2->y = 100;
        cvRectangle(colorchart, *pt1, *pt2, cvScalar(0,0,0), CV_FILLED);


        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        bump = 181.0 / 8.0;
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 0;

                k++;
                for(a= 0; a < 8; a++){

                    if(k < 5 && i == 0 && j == 0){

                        pt1->x = k * 100;
                        pt1->y = 0;
                        pt2->x = k * 100 + 100;
                        pt2->y = 100;
                        cvRectangle(colorchart, *pt1, *pt2, cvScalar(a*bump,255,255), CV_FILLED);

                    }else if(i == 0 && j == 0){

                        pt1->x = (k-5) * 100;
                        pt1->y = 100;
                        pt2->x = (k-5) * 100 + 100;
                        pt2->y = 200;
                        cvRectangle(colorchart, *pt1, *pt2, cvScalar(a*bump,255,255), CV_FILLED);
                    }

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255;

                    k++;
                }
            }
        }

        cvCvtColor(colorchart, colorchart, CV_HSV2BGR);
        cvSaveImage("colorchart.png", colorchart);

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Average Colors with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case AVERAGE_HSV10:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        bump = 181.0 / 8.0;
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 0;

                k++;
                for(a= 0; a < 8; a++){

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255;

                    k++;
                }
            }
        }
        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Counts with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case COUNTS_HSV20:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        bump = 181.0 / 9.0;
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 0;

                k++;
                for(a= 0; a < 9; a++){

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255/2;


                    k++;
                }
                for(a= 0; a < 9; a++){

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255;

                    k++;
                }
            }
        }
        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Dominant Colors with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case DOMINANT_HSV20:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        bump = 181.0 / 9.0;
        k = 0;


        cvReleaseImage(&colorchart);
        size->width = 500;
        size->height = 400;
        colorchart = cvCreateImage(*size, IPL_DEPTH_8U, 3);

        pt1->x = 0;
        pt1->y = 0;
        pt2->x = 500;
        pt2->y = 400;
        cvRectangle(colorchart, *pt1, *pt2, cvScalar(0,0,0), CV_FILLED);

        pt1->x = 0;
        pt1->y = 0;
        pt2->x = 100;
        pt2->y = 100;
        cvRectangle(colorchart, *pt1, *pt2, cvScalar(0,0,255), CV_FILLED);

        pt1->x = 100;
        pt1->y = 0;
        pt2->x = 200;
        pt2->y = 100;
        cvRectangle(colorchart, *pt1, *pt2, cvScalar(0,0,0), CV_FILLED);



        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 0;

                k++;
                for(a= 0; a < 9; a++){

                    if(i == 0 && j == 0){

                        pt1->x = (k%5) * 100;
                        pt1->y = (k / 5) * 100;
                        pt2->x = (k%5) * 100 + 100;
                        pt2->y = (k/5 + 1) * 100;
                        cvRectangle(colorchart, *pt1, *pt2, cvScalar(a*bump,255,255/2), CV_FILLED);

                    }

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255 / 2;

                    k++;
                }
                for(a= 0; a < 9; a++){


                    if(i == 0 && j == 0){

                        pt1->x = (k%5) * 100;
                        pt1->y = (k / 5) * 100;
                        pt2->x = (k%5) * 100 + 100;
                        pt2->y = (k/5 + 1) * 100;
                        cvRectangle(colorchart, *pt1, *pt2, cvScalar(a*bump,255,255), CV_FILLED);

                    }

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255;

                    k++;
                }
            }
        }

        cvCvtColor(colorchart, colorchart, CV_HSV2BGR);
        cvSaveImage("colorchart.png", colorchart);

        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Average Colors with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case AVERAGE_HSV20:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        bump = 181.0 / 9.0;
        k = 0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 255;

                k++;

                bigFeatures[k] = new PrimaryFeature();
                bigFeatures[k]->height = block_height;
                bigFeatures[k]->width = block_width;
                bigFeatures[k]->x = j * block_width;
                bigFeatures[k]->y = i * block_height;
                bigFeatures[k]->hue = 0;
                bigFeatures[k]->saturation = 0;
                bigFeatures[k]->value = 0;

                k++;
                for(a= 0; a < 9; a++){

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255/2;

                    k++;
                }
                for(a= 0; a < 9; a++){

                    bigFeatures[k] = new PrimaryFeature();
                    bigFeatures[k]->height = block_height;
                    bigFeatures[k]->width = block_width;
                    bigFeatures[k]->x = j * block_width;
                    bigFeatures[k]->y = i * block_height;
                    bigFeatures[k]->hue = a * bump;
                    bigFeatures[k]->saturation = 255;
                    bigFeatures[k]->value = 255;

                    k++;
                }
            }
        }
        break;

    }

    cvReleaseImage(&colorchart);
    delete size;
    delete pt1;
    delete pt2;

}

// This is a single image version of the larger version
// it overloads the general getFeatures function
void VideoData::getFeatures(IplImage *source_frame, int block_width, int block_height, int code, vector < int > &bigCounts){

    vector < IplImage* > sf;
    sf.resize(1);
    sf[0] = source_frame;

    getFeatures(sf, block_width, block_height, code, bigCounts);
}

// This method takes in a cluster of frames and returns a pointer to an array of counts of feature values
// It also copies the source frames into the destination frames mapping for visualization of the features
// It expects data in 8U BGR format
void VideoData::getFeatures(vector < IplImage* > &source_frames, int block_width, int block_height, int code, vector < int > &bigCounts){


    int i, j, k;
    int a, b, c;
    float bump;
    int width, height;
    vector < vector < vector < int > > > counts;
    vector < int > local_counts;
    int num_features;
    int h, s, v;
    int x, y;
    int maxval, maxindex;
    IplImage *temp, *smooth, *value;
    unsigned char *rowptr;
    int minThreshold = 100;
    int maxThreshold = 300;
    int num_frames = source_frames.size();

    width = cvGetSize(source_frames[0]).width;
    height = cvGetSize(source_frames[0]).height;

    temp = cvCreateImage(cvGetSize(source_frames[0]), source_frames[0]->depth, 3);
    smooth = cvCreateImage(cvGetSize(source_frames[0]), source_frames[0]->depth, 3);
    value = cvCreateImage(cvGetSize(source_frames[0]), source_frames[0]->depth, 1);



    switch(code){
    /***********************************************************************************/
    /***********************************************************************************/
    // Edge counts present or absent
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_PRESENCE:

        // lets make the features be ordered as x by y by color

        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                counts[i][j].resize(1);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 1; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        local_counts.resize(1);
        for(i=0; i < 1; i++){
            local_counts[i] = 0;
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/


        for(a=0; a < num_frames; a++){

            cvCopy(source_frames[a], temp);
            cvSmooth(temp, smooth);
            cvCvtColor(smooth, temp, CV_BGR2YCrCb);
            cvSplit(temp, value, NULL, NULL, NULL);
            cvCanny(value, value, minThreshold, maxThreshold);

            cvMerge(value, value, value, NULL, temp);

            for(i=0; i < ((height / block_height) + 1); i++){

                for(j=0; j < ((width / block_width) + 1); j++){

                    /*****************************************/
                    // Reset the local counts for each block
                    /*****************************************/
                    for(b=0; b < 1; b++){
                        local_counts[b] = 0;
                    }
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) value->imageData) + y * value->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}

                            if(rowptr[x] > 0){
                                local_counts[0] = 1;
                            }
                        }
                    }

                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) value->imageData) + y * value->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}
                        }
                    }

                    if(local_counts[0] > 0){

                        counts[i][j][0] = 10;
                    }
                }
            }
        }

        // Finalize the results
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigCounts[k] = counts[i][j][0];
                k++;
            }
        }

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // Edge counts present or absent tracking on center of mass
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_PRESENCE_CENTERED:

        // lets make the features be ordered as x by y by color

        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                counts[i][j].resize(1);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 1; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        local_counts.resize(1);
        for(i=0; i < 1; i++){
            local_counts[i] = 0;
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        for(a=0; a < num_frames; a++){

            cvCopy(source_frames[a], temp);
            cvSmooth(temp, smooth);
            cvCvtColor(smooth, temp, CV_BGR2YCrCb);
            cvSplit(temp, value, NULL, NULL, NULL);
            cvCanny(value, value, minThreshold, maxThreshold);

            cvMerge(value, value, value, NULL, temp);

            for(i=0; i < ((height / block_height) + 1); i++){

                for(j=0; j < ((width / block_width) + 1); j++){

                    /******************************************/
                    // Reset the local counts for each block
                    /******************************************/
                    for(b=0; b < 1; b++){
                        local_counts[b] = 0;
                    }
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) value->imageData) + y * value->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}

                            if(rowptr[x] > 0){
                                local_counts[0] = 1;
                            }
                        }
                    }

                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) value->imageData) + y * value->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}
                        }
                    }

                    if(local_counts[0] > 0){

                        counts[i][j][0] = 10;
                    }
                }
            }
        }
        // Finalize the results
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigCounts[k] = counts[i][j][0];
                k++;
            }
        }

        break;

    /***********************************************************************************/
    /***********************************************************************************/
    // Edge Counts Totalled
    /***********************************************************************************/
    /***********************************************************************************/
    case EDGE_COUNTS:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                counts[i][j].resize(1);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 1; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        for(a=0; a < num_frames; a++){

            cvCopy(source_frames[a], temp);
            cvSmooth(temp, smooth);
            cvCvtColor(smooth, temp, CV_BGR2YCrCb);
            cvSplit(temp, value, NULL, NULL, NULL);
            cvCanny(value, value, minThreshold, maxThreshold);

            for(i=0; i < ((height / block_height) + 1); i++){

                for(j=0; j < ((width / block_width) + 1); j++){

                    /******************************************/
                    // Reset the local counts for each block
                    /******************************************/
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) value->imageData) + y * value->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}

                            if(rowptr[x] > 0){
                                counts[i][j][0]++;
                            }
                        }
                    }
                }
            }
        }
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                bigCounts[k] = counts[i][j][0];
                k++;
            }
        }

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Counts with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case COUNTS_HSV10:

        // lets make the features be ordered as x by y by color

        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                counts[i][j].resize(10);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 10; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        bump = 181.0 / 8.0;
        for(a=0; a < num_frames; a++){

            cvCvtColor(source_frames[a], temp, CV_BGR2HSV);

            for(i=0; i < height; i++){

                rowptr = (((unsigned char*) temp->imageData) + i * temp->widthStep);
                for(j=0; j < width; j++){

                    h = rowptr[j * 3 + 0];
                    s = rowptr[j * 3 + 1];
                    v = rowptr[j * 3 + 2];

                    if( (s < (255 / 4)) && (v > ((3 * 255) / 4)) ){

                            // White pixel
                            counts[i / block_height][j / block_width][0]++;
                    }else if( v < (255 / 4) ){

                            // Black pixel
                            counts[i / block_height][j / block_width][1]++;
                    }else{

                            // Color pixels (just carve the hue space up 8 ways
                            k = h / bump;
                            if(h > 178 || h < 2){
                                k = 0;
                            }
                            counts[i / block_height][j / block_width][k+2]++;
                    }
                }
            }
        }
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                for(a= 0; a < 10; a++){
                    bigCounts[k] = counts[i][j][a];
                    k++;
                }
            }
        }

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Dominant Colors with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case DOMINANT_HSV10:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/

        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                counts[i][j].resize(10);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 10; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        local_counts.resize(10);
        for(i=0; i < 10; i++){
            local_counts[i] = 0;
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        bump = 181.0 / 8.0;
        for(a=0; a < num_frames; a++){

            cvCvtColor(source_frames[a], temp, CV_BGR2HSV);

            for(i=0; i < ((height / block_height) + 1); i++){

                for(j=0; j < ((width / block_width) + 1); j++){

                    /******************************************/
                    // Reset the local counts for each block
                    /******************************************/
                    for(b=0; b < 10; b++){
                        local_counts[b] = 0;
                    }
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) temp->imageData) + y * temp->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}


                            h = rowptr[x * 3 + 0];
                            s = rowptr[x * 3 + 1];
                            v = rowptr[x * 3 + 2];


                            if( (s < (255 / 4)) && (v > ((3 * 255) / 4)) ){

                                    // White pixel
                                    local_counts[0]++;
                            }else if( v < (255 / 4) ){

                                    // Black pixel
                                    local_counts[1]++;
                            }else{

                                    // Color pixels (just carve the hue space up 8 ways
                                    k = floor(((float) h) / bump);
                                    if(h > 178 || h < 2){
                                        k = 0;
                                    }
                                    if(k > 7 || k < 0){
                                        cout << "Error : " << k << " , " << h << "\n";
                                        cout << bump << "\n";
                                        cout << ((float) h) << "\n";
                                        cout << (2.0 * bump) << "\n";
                                        cout << ((float) h) / (2.0 * bump) << "\n";
                                        while(1){}
                                    }
                                    local_counts[k+2]++;
                            }
                        }
                    }
                    /*********************************************************/
                    // Find the maximally represented value in the block
                    /*********************************************************/
                    maxindex = 0;
                    maxval = local_counts[0];
                    for(b=1; b<10; b++){
                        if(local_counts[b] >= maxval){
                            maxval = local_counts[b];
                            maxindex = b;
                        }
                    }
                    // Increment the found color feature
                    counts[i][j][maxindex]++;

                    /*********************************************************/
                    // Then color the output frames
                    /*********************************************************/
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}

                            if(maxindex == 0){

                            }else if(maxindex == 1){

                            }else{

                            }
                        }
                    }
                }
            }

        }
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                for(a= 0; a < 10; a++){
                    bigCounts[k] = counts[i][j][a];
                    k++;
                }
            }
        }

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Average Colors with 10 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case AVERAGE_HSV10:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 10;
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i ++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i ++){
            for(j=0; j < ((width / block_width) + 1); j ++){
                counts[i][j].resize(10);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 10; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        bump = 181.0 / 8.0;
        for(a=0; a < num_frames; a++){

            cvCvtColor(source_frames[a], temp, CV_BGR2HSV);

            for(i=0; i < ((height / block_height) + 1); i++){

                for(j=0; j < ((width / block_width) + 1); j++){

                    /******************************************/
                    // Reset the local counts for each block
                    /******************************************/
                    h = 0;
                    s = 0;
                    v = 0;
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) temp->imageData) + y * temp->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}


                            h = h + (int) rowptr[x * 3 + 0];
                            s = s + (int) rowptr[x * 3 + 1];
                            v = v + (int) rowptr[x * 3 + 2];
                        }
                    }
                    /*********************************************************/
                    // Average the color values in the block
                    /*********************************************************/
                    h = h / (block_width * block_height);
                    s = s / (block_width * block_height);
                    v = v / (block_width * block_height);

                    /*********************************************************/
                    // Then color the output frames
                    /*********************************************************/
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}

                            if( (s < (255 / 4)) && (v > ((3 * 255) / 4)) ){

                                    // White pixel
                                    counts[i][j][0]++;
                            }else if( v < (255 / 4) ){

                                    // Black pixel
                                    counts[i][j][1]++;
                            }else{

                                    // Color pixels (just carve the hue space up 8 ways
                                    k = h / bump;
                                    if(h > 178 || h < 2){
                                        k = 0;
                                    }
                                    counts[i][j][k+2]++;
                            }
                        }
                    }
                }
            }

        }
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                for(a= 0; a < 10; a++){
                    bigCounts[k] = counts[i][j][a];
                    k++;
                }
            }
        }

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Counts with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case COUNTS_HSV20:

        // lets make the features be ordered as x by y by color

        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                counts[i][j].resize(20);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 20; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        bump = 181.0 / 9.0;
        for(a=0; a < num_frames; a++){

            cvCvtColor(source_frames[a], temp, CV_BGR2HSV);

            for(i=0; i < height; i++){

                rowptr = (((unsigned char*) temp->imageData) + i * temp->widthStep);
                for(j=0; j < width; j++){

                    h = rowptr[j * 3 + 0];
                    s = rowptr[j * 3 + 1];
                    v = rowptr[j * 3 + 2];

                    if( (s < (255 / 4)) && (v > ((5 * 255) / 6)) ){

                            // White pixel
                            counts[i / block_height][j / block_width][0]++;
                    }else if( v < (255 / 6) ){

                            // Black pixel
                            counts[i / block_height][j / block_width][1]++;
                    }else if(v <= (255 / 2)){

                            // Color pixels (just carve the hue space up 8 ways
                            k = h / bump;
                            if(h > 178 || h < 2){
                                k = 0;
                            }
                            counts[i / block_height][j / block_width][k+2]++;
                    }else{

                            // Color pixels (just carve the hue space up 8 ways
                            k = h / bump;
                            if(h > 178 || h < 2){
                                k = 0;
                            }
                            counts[i / block_height][j / block_width][k+2+9]++;
                    }
                }
            }
        }
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                for(a= 0; a < 20; a++){
                    bigCounts[k] = counts[i][j][a];
                    k++;
                }
            }
        }

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Dominant Colors with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case DOMINANT_HSV20:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i ++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i ++){
            for(j=0; j < ((width / block_width) + 1); j ++){
                counts[i][j].resize(20);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 20; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        local_counts.resize(20);
        for(i=0; i < 20; i++){
            local_counts[i] = 0;
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        bump = 181.0 / 9.0;
        for(a=0; a < num_frames; a++){

            cvCvtColor(source_frames[a], temp, CV_BGR2HSV);

            for(i=0; i < ((height / block_height) + 1); i++){

                for(j=0; j < ((width / block_width) + 1); j++){

                    /******************************************/
                    // Reset the local counts for each block
                    /******************************************/
                    for(b=0; b < 20; b++){
                        local_counts[b] = 0;
                    }
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) temp->imageData) + y * temp->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}


                            h = rowptr[x * 3 + 0];
                            s = rowptr[x * 3 + 1];
                            v = rowptr[x * 3 + 2];


                            if( (s < (255 / 4)) && (v > ((5 * 255) / 6)) ){

                                    // White pixel
                                    local_counts[0]++;
                            }else if( v < (255 / 6) ){

                                    // Black pixel
                                    local_counts[1]++;
                            }else if( v < (255/2) ){

                                    // Color pixels (just carve the hue space up 8 ways
                                    k = floor(((float) h) / bump);
                                    if(h > 178 || h < 2){
                                        k = 0;
                                    }
                                    local_counts[k+2]++;
                            }else{

                                    // Color pixels (just carve the hue space up 8 ways
                                    k = floor(((float) h) / bump);
                                    if(h > 178 || h < 2){
                                        k = 0;
                                    }
                                    local_counts[k+2+9]++;
                            }
                        }
                    }
                    /*********************************************************/
                    // Find the maximally represented value in the block
                    /*********************************************************/
                    maxindex = 0;
                    maxval = local_counts[0];
                    for(b=1; b<20; b++){
                        if(local_counts[b] >= maxval){
                            maxval = local_counts[b];
                            maxindex = b;
                        }
                    }
                    // Increment the found color feature
                    counts[i][j][maxindex]++;

                    /*********************************************************/
                    // Then color the output frames
                    /*********************************************************/
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}

                            if(maxindex == 0){

                            }else if(maxindex == 1){

                            }else if( maxindex < 11 ){

                            }else{

                            }
                        }
                    }
                }
            }
        }
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                for(a= 0; a < 20; a++){
                    bigCounts[k] = counts[i][j][a];
                    k++;
                }
            }
        }

        break;
    /***********************************************************************************/
    /***********************************************************************************/
    // HSV Average Colors with 20 colors
    /***********************************************************************************/
    /***********************************************************************************/
    case AVERAGE_HSV20:

        // lets make the features be ordered as x by y by color
        num_features = ((width / block_width) + 1) * ((height / block_height) + 1);
        num_features = num_features * 20;
        /********************************************************/
        // Initialize the array of feature counts
        /********************************************************/
        counts.resize((height / block_height) + 1);
        for(i=0; i < ((height / block_height) + 1); i ++){
            counts[i].resize((width / block_width) + 1);
        }
        for(i=0; i < ((height / block_height) + 1); i ++){
            for(j=0; j < ((width / block_width) + 1); j ++){
                counts[i][j].resize(20);
            }
        }
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){
                for(k=0; k < 20; k++){
                    counts[i][j][k] = 0;
                }
            }
        }
        /********************************************************/
        // Run through the images counting features
        /********************************************************/
        bump = 181.0 / 9.0;
        for(a=0; a < num_frames; a++){

            cvCvtColor(source_frames[a], temp, CV_BGR2HSV);

            for(i=0; i < ((height / block_height) + 1); i++){

                for(j=0; j < ((width / block_width) + 1); j++){

                    /******************************************/
                    // Reset the local counts for each block
                    /******************************************/
                    h = 0;
                    s = 0;
                    v = 0;
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        rowptr = (((unsigned char*) temp->imageData) + y * temp->widthStep);
                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}


                            h = h + (int) rowptr[x * 3 + 0];
                            s = s + (int) rowptr[x * 3 + 1];
                            v = v + (int) rowptr[x * 3 + 2];
                        }
                    }
                    /*********************************************************/
                    // Average the color values in the block
                    /*********************************************************/
                    h = h / (block_width * block_height);
                    s = s / (block_width * block_height);
                    v = v / (block_width * block_height);

                    /*********************************************************/
                    // Then color the output frames
                    /*********************************************************/
                    for(b=0; b < block_height; b++){
                        y = block_height * i + b;
                        if(y >= height){break;}

                        for(c=0; c < block_width; c++){
                            x = j * block_width + c;
                            if(x >= width){break;}

                            if( (s < (255 / 4)) && (v > ((3 * 255) / 6)) ){

                                    // White pixel
                                    counts[i][j][0]++;
                            }else if( v < (255 / 6) ){

                                    // Black pixel
                                    counts[i][j][1]++;
                            }else if( v < (255/2) ){

                                    // Color pixels (just carve the hue space up 8 ways
                                    k = h / bump;
                                    if(h > 178 || h < 2){
                                        k = 0;
                                    }
                                    counts[i][j][k+2]++;
                            }else{

                                    // Color pixels (just carve the hue space up 8 ways
                                    k = h / bump;
                                    if(h > 178 || h < 2){
                                        k = 0;
                                    }
                                    counts[i][j][k+2+9]++;
                            }
                        }
                    }
                }
            }
        }
        k=0;
        for(i=0; i < ((height / block_height) + 1); i++){
            for(j=0; j < ((width / block_width) + 1); j++){

                for(a= 0; a < 20; a++){
                    bigCounts[k] = counts[i][j][a];
                    k++;
                }
            }
        }

        break;

    }

    cvReleaseImage(&temp);
    cvReleaseImage(&value);
    cvReleaseImage(&smooth);
}

