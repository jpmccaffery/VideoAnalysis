#ifndef VIDEODATA
#define VIDEODATA

#include <string>
#include <iostream>
#include "DataManager.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "QString"
#include "QObject"
#include "projectwidget.h"

class TopicModel;
class PrimaryFeature;

using namespace std;

class VideoData : public ProjectWidget{

    Q_OBJECT

public:

    //*********************************************
    // Constants
    //*********************************************
    static const int NONE = -1;
    static const int EDGE_COUNTS = 0;
    static const int EDGE_PRESENCE = 1;
    static const int COUNTS_HSV10 = 2;
    static const int DOMINANT_HSV10 = 3;
    static const int AVERAGE_HSV10 = 4;
    static const int COUNTS_HSV20 = 5;
    static const int DOMINANT_HSV20 = 6;
    static const int AVERAGE_HSV20 = 7;
    static const int EDGE_PRESENCE_CENTERED = 8;

    static void generateThumbnail(QString filename, QString filetype);

    //*********************************************
    // Data
    //*********************************************
    vector < CvScalar > COLORS;

    vector < vector < double > > X;
    vector < vector < int > > frameCounts;
    vector < double > N;
    QString fname, ftype;

    // Lets just call T the number of timesteps in the model including time = 0
    int M, T, totalFrames;
    int binSize;
    int blockWidth, blockHeight;
    int FEATURE1, FEATURE2, FEATURE3, FEATURE4;
    int width, height;

    //*********************************************
    // Methods
    //*********************************************
    VideoData(ProjectWidget *p=0, MainWindow *q=0);
    ~VideoData();

    // Getters
    QString windowTitle();
    QString getFeatureName(int featureType);
    QString getFeatureFileName(int featureType);
    QString getFType();
    QString getFName();
    QString getDataName();
    int getGenerateTotal();

    // Setters
    void setDataName(QString name);

    // Utility Functions
    int getTotalFrameCount(CvCapture *movie);
    void setFrameSize(CvCapture *movie, CvSize &size);
    void blendAlpha(vector < IplImage* > &frames, IplImage *dest);
    void blend64(vector < IplImage* > frames);
    void cvMulS(IplImage* src, CvScalar scale, IplImage* dest);

    // Data Generators
    int stripZeroFeatures(vector < vector < vector < int > > > &tempX, vector < vector < PrimaryFeature * > > &tempFeatures);
    void setX();
    void setN();

    // Video Generators
    void generateFeatureVideo(int feature);
    void generateCountVideo(QString saveName, vector < vector < bool > > chain);
    void generateCoarseHighlights(TopicModel *model, int slowDown, vector < vector < bool > > &chain);
    void generateFineHighlights(TopicModel *model, int slowDown, vector < vector < bool > > &chain);

    // Image Generators
    IplImage* drawTopic(vector < double > &weights, QString s, bool wantReturnImage);
    void drawKeyWeighted(int feature, IplImage *image, int bwidth, int bheight, double weight, double scale = 1.0);

    // Save / Load Functions
    int getSaveTotal();
    int getLoadTotal();
    int getFastSaveTotal();

    // Preprocessing Functions
    void subtractBackground(QString subtractName);
    void centerOnMass(QString fname, QString outname);

    // Video count generator stuff
    int getNumberFeatures(int block_width, int block_height, int code);
    void getFeatureKeys(int block_width, int block_height, int code, vector < PrimaryFeature * > &bigFeatures);
    void getFeatures(IplImage *source_frame, int block_width, int block_height, int code, vector < int > &bigCounts);
    void getFeatures(vector < IplImage* > &source_frames, int block_width, int block_height, int code, vector < int > &bigCounts);



public slots:

    // Initialization functions
    void setFName(QString name);
    void setFType(QString name);

    void setBlockHeight(int h);
    void setBlockWidth(int w);
    void setBinSize(int s);

    void setFeature1(int k);
    void setFeature1(QString feature);
    void setFeature2(int k);
    void setFeature2(QString feature);
    void setFeature3(int k);
    void setFeature3(QString feature);
    void setFeature4(int k);
    void setFeature4(QString feature);

    void generateCounts();

    void generateFeatureVideo1();
    void generateFeatureVideo2();
    void generateFeatureVideo3();
    void generateFeatureVideo4();

    void reset();
    void save(QString qname);
    void fastSave(QString qname);
    void load(QString qname);
//    void slowLoad(QString qname);
    void load();
    void save();
    void slowLoad();
    void fastSave();

signals:

    void MSet(int m);
    void TSet(int t);
    void generateCountsTick();
    void generateCountsDone();
    void featureVideoTick(int k);
    void featureVideoDone(QString);
    void analysisComplete(VideoData *);
    void loadSaveTick(int);
    void tick(int);


    void workTick(int k);
    void workTickMax(int k);

    void globalTick(int k);
    void globalTickMax(int k);

    void featureTick(int index, int k);
    void featureTickMax(int index, int k);

private:

    QString dataName;
    QWidget *wParent;

    vector < PrimaryFeature* > features;

};

#endif
