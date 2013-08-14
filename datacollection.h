#ifndef DATACOLLECTION_H
#define DATACOLLECTION_H

#include "projectwidget.h"
#include <QString>
#include "vector"

class VideoData;
class TopicModel;

using namespace std;

class DataCollection : public ProjectWidget
{

    Q_OBJECT

public:
    DataCollection(ProjectWidget *p = 0, MainWindow *q = 0);
    ~DataCollection();
    int getBatchSize();
    void setBatchSize(int k);
    VideoData *getData(int k=0);
    bool getInitialized();
    void setBatchName(QString s);
    QString getBatchName();
    bool checkForData();
    int getLoadTotal();
    int getSaveTotal();
    int getFastSaveTotal();

    QString getFName();
    QString getFType();
    int getBlockWidth();
    int getBlockHeight();
    int getBinSize();
    int getM();

    void load();
    void save();
    void fastSave();
    void save(QString s);
    void fastSave(QString s);
    void slowLoad();

    QString getFeatureFileName(int k);
    int getFeature1();
    int getFeature2();
    int getFeature3();
    int getFeature4();

    void generateFeatureHighlights(TopicModel *model, int slowDown, vector < vector < bool > > &chain, int type);

private:
    vector < VideoData* > data;
    int batchSize;
    bool initialized;
    QString FEATURE1, FEATURE2, FEATURE3, FEATURE4;
    int binSize, blockWidth, blockHeight;
    QString batchName;
    double total, counts;
    int currentTick;
    void destroy();

public slots:
    void setDataNames(QStringList fnames);
    void setDataName(QString fname);
    void setFeature1(QString s);
    void setFeature2(QString s);
    void setFeature3(QString s);
    void setFeature4(QString s);
    void generateFeatureVideo1();
    void generateFeatureVideo2();
    void generateFeatureVideo3();
    void generateFeatureVideo4();
    void setBinSize(int k);
    void setBlockHeight(int h);
    void setBlockWidth(int w);
    void setFName(QString name, QString type);
    void setFNames(QStringList names, QStringList types);
    void reset();
    void generateCounts();

    void getWorkTick(int k);
    void getWorkTickMax(int k);

    void getGlobalTick(int k);
    void getGlobalTickMax(int k);

    void getFeatureTick(int index, int k);
    void getFeatureTickMax(int index, int k);


signals:

    void featureVideoDone();
    void dataSetChanged();

    void workTick(int k);
    void workTickMax(int k);

    void globalTick(int k);
    void globalTickMax(int k);

    void featureTick(int index, int k);
    void featureTickMax(int index, int k);

};

#endif // DATACOLLECTION_H
