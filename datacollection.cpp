#include "datacollection.h"
#include "QDebug"
#include "VideoData.h"
#include "QDir"

DataCollection::DataCollection(ProjectWidget *p, MainWindow *q) :

    ProjectWidget(p, q)
{
    batchSize = 0;
    initialized = false;
    data.resize(0);
    FEATURE1 = "";
    FEATURE2 = "";
    FEATURE3 = "";
    FEATURE4 = "";

    blockHeight = 0;
    blockWidth = 0;
    binSize = 0;

    batchName = "default";

    counts = 0.0;
    total = 0.0;
    currentTick = 0;
}

DataCollection::~DataCollection(){

    int i;


    if(batchSize > 0){

        for(i=0; i < batchSize; i++){

            disconnect(data[i], SIGNAL(workTick(int)), this, SLOT(getWorkTick(int)));
            disconnect(data[i], SIGNAL(workTickMax(int)), this, SLOT(getWorkTickMax(int)));

            disconnect(data[i], SIGNAL(globalTick(int)), this, SLOT(getGlobalTick(int)));
            disconnect(data[i], SIGNAL(globalTickMax(int)), this, SLOT(getGlobalTickMax(int)));

            disconnect(data[i], SIGNAL(featureTick(int, int)), this, SLOT(getFeatureTick(int, int)));
            disconnect(data[i], SIGNAL(featureTickMax(int, int)), this, SLOT(getFeatureTickMax(int, int)));

            disconnect(data[i], SIGNAL(MSet(int)), this, SLOT(MSet(int)));
            delete data[i];
        }
        data.resize(0);
    }
}


void DataCollection::destroy(){

    int i;

    if(batchSize > 0){

        for(i=0; i < batchSize; i++){


            disconnect(data[i], SIGNAL(workTick(int)), this, SLOT(getWorkTick(int)));
            disconnect(data[i], SIGNAL(workTickMax(int)), this, SLOT(getWorkTickMax(int)));

            disconnect(data[i], SIGNAL(globalTick(int)), this, SLOT(getGlobalTick(int)));
            disconnect(data[i], SIGNAL(globalTickMax(int)), this, SLOT(getGlobalTickMax(int)));

            disconnect(data[i], SIGNAL(featureTick(int, int)), this, SLOT(getFeatureTick(int, int)));
            disconnect(data[i], SIGNAL(featureTickMax(int, int)), this, SLOT(getFeatureTickMax(int, int)));

            disconnect(data[i], SIGNAL(MSet(int)), this, SLOT(MSet(int)));
            delete data[i];
        }
        data.resize(0);
    }
}

void DataCollection::setBatchSize(int k){

    int i;

    destroy();

    batchSize = k;
    data.resize(batchSize);

    for(i=0; i < batchSize; i++){

        data[i] = new VideoData(this);
        data[i]->setFName("");
        data[i]->setFType("");
        data[i]->setBinSize(binSize);
        data[i]->setBlockHeight(blockHeight);
        data[i]->setBlockWidth(blockWidth);
        data[i]->setFeature1(FEATURE1);
        data[i]->setFeature2(FEATURE2);
        data[i]->setFeature3(FEATURE3);
        data[i]->setFeature4(FEATURE4);
        data[i]->setDataName(batchName + QString::number(i));

        connect(data[i], SIGNAL(workTick(int)), this, SLOT(getWorkTick(int)));
        connect(data[i], SIGNAL(workTickMax(int)), this, SLOT(getWorkTickMax(int)));

        connect(data[i], SIGNAL(globalTick(int)), this, SLOT(getGlobalTick(int)));
        connect(data[i], SIGNAL(globalTickMax(int)), this, SLOT(getGlobalTickMax(int)));

        connect(data[i], SIGNAL(featureTick(int, int)), this, SLOT(getFeatureTick(int, int)));
        connect(data[i], SIGNAL(featureTickMax(int, int)), this, SLOT(getFeatureTickMax(int, int)));

        connect(data[i], SIGNAL(MSet(int)), this, SLOT(MSet(int)));
     }
}

int DataCollection::getBatchSize(){

    return batchSize;
}

VideoData *DataCollection::getData(int k){

    if(k < batchSize){

        return data[k];
    }else{

        qDebug() << "Error, not enough datas";
        return 0;
    }
}

bool DataCollection::getInitialized(){

    return initialized;
}

void DataCollection::setFName(QString name, QString type){

    int i;

    destroy();

    batchSize = 1;
    data.resize(1);
    data[0] = new VideoData(this);
    data[0]->setFName(name);
    data[0]->setFType(type);
    data[0]->setBinSize(binSize);
    data[0]->setBlockHeight(blockHeight);
    data[0]->setBlockWidth(blockWidth);
    data[0]->setFeature1(FEATURE1);
    data[0]->setFeature2(FEATURE2);
    data[0]->setFeature3(FEATURE3);
    data[0]->setFeature4(FEATURE4);
    data[0]->setDataName(batchName);

    connect(data[0], SIGNAL(workTick(int)), this, SLOT(getWorkTick(int)));
    connect(data[0], SIGNAL(workTickMax(int)), this, SLOT(getWorkTickMax(int)));

    connect(data[0], SIGNAL(globalTick(int)), this, SLOT(getGlobalTick(int)));
    connect(data[0], SIGNAL(globalTickMax(int)), this, SLOT(getGlobalTickMax(int)));

    connect(data[0], SIGNAL(featureTick(int, int)), this, SLOT(getFeatureTick(int, int)));
    connect(data[0], SIGNAL(featureTickMax(int, int)), this, SLOT(getFeatureTickMax(int, int)));

    connect(data[0], SIGNAL(MSet(int)), this, SLOT(MSet(int)));
}

void DataCollection::setFNames(QStringList names, QStringList types){

    int i;

    destroy();

    if(names.size() != types.size()){

        qDebug() << "Mismatched fnames and ftypes";

    }

    batchSize = names.size();
    data.resize(batchSize);

    for(i=0; i < batchSize; i++){

        data[i] = new VideoData(this);
        data[i]->setFName(names.at(i));
        data[i]->setFType(types.at(i));
        data[i]->setBinSize(binSize);
        data[i]->setBlockHeight(blockHeight);
        data[i]->setBlockWidth(blockWidth);
        data[i]->setFeature1(FEATURE1);
        data[i]->setFeature2(FEATURE2);
        data[i]->setFeature3(FEATURE3);
        data[i]->setFeature4(FEATURE4);
        data[i]->setDataName(batchName + QString::number(i));

        connect(data[i], SIGNAL(workTick(int)), this, SLOT(getWorkTick(int)));
        connect(data[i], SIGNAL(workTickMax(int)), this, SLOT(getWorkTickMax(int)));

        connect(data[i], SIGNAL(globalTick(int)), this, SLOT(getGlobalTick(int)));
        connect(data[i], SIGNAL(globalTickMax(int)), this, SLOT(getGlobalTickMax(int)));

        connect(data[i], SIGNAL(featureTick(int, int)), this, SLOT(getFeatureTick(int, int)));
        connect(data[i], SIGNAL(featureTickMax(int, int)), this, SLOT(getFeatureTickMax(int, int)));

        connect(data[i], SIGNAL(MSet(int)), this, SLOT(MSet(int)));
     }
}


void DataCollection::setFeature1(QString s){

    int i;
    FEATURE1 = s;
    for(i=0; i < batchSize; i++){

        data[i]->setFeature1(s);
    }
}

void DataCollection::setFeature2(QString s){

    int i;
    FEATURE2 = s;
    for(i=0; i < batchSize; i++){

        data[i]->setFeature2(s);
    }
}

void DataCollection::setFeature3(QString s){

    int i;
    FEATURE3 = s;
    for(i=0; i < batchSize; i++){

        data[i]->setFeature3(s);
    }
}

void DataCollection::setFeature4(QString s){

    int i;
    FEATURE4 = s;
    for(i=0; i < batchSize; i++){

        data[i]->setFeature4(s);
    }
}

void DataCollection::generateFeatureVideo1(){

    if(batchSize == 1){

        data[0]->generateFeatureVideo1();
    }else{

        qDebug() << "Too many videos";

    }
}

void DataCollection::generateFeatureVideo2(){

    if(batchSize == 1){

        data[0]->generateFeatureVideo2();
    }else{

        qDebug() << "Too many videos";

    }
}

void DataCollection::generateFeatureVideo3(){

    if(batchSize == 1){

        data[0]->generateFeatureVideo3();
    }else{

        qDebug() << "Too many videos";

    }
}

void DataCollection::generateFeatureVideo4(){

    if(batchSize == 1){

        data[0]->generateFeatureVideo4();
    }else{

        qDebug() << "Too many videos";

    }
}


void DataCollection::setBinSize(int k){

    int i;
    binSize = k;
    for(i=0; i < batchSize; i++){

        data[i]->setBinSize(k);
    }
}


void DataCollection::setBlockHeight(int h){

    int i;
    blockHeight = h;
    for(i=0; i < batchSize; i++){

        data[i]->setBlockHeight(h);
    }
}


void DataCollection::setBlockWidth(int w){

    int i;
    blockWidth = w;
    for(i=0; i < batchSize; i++){

        data[i]->setBlockWidth(w);
    }
}

QString DataCollection::getBatchName(){

    return batchName;
}

void DataCollection::setBatchName(QString s){

    int i;

    batchName = s;

    for(i=0; i < batchSize; i++){

        data[i]->setDataName(batchName + QString::number(i));
     }
}

void DataCollection::reset(){

    destroy();

    FEATURE1 = "";
    FEATURE2 = "";
    FEATURE3 = "";
    FEATURE4 = "";

    batchSize = 0;
    batchName = "default";

    counts = 0.0;
    total = 0.0;

    initialized = false;
}

void DataCollection::generateCounts(){

    int i;

    total = 0.0;
    counts = 0.0;

    for(i=0; i < batchSize; i++){

        total = total + data[i]->getGenerateTotal();
    }

    for(i=0; i < batchSize; i++){

        data[i]->generateCounts();
    }
    initialized = true;
}


bool DataCollection::checkForData(){

    int i;
    QDir dir(getProjectName() + "/data/");

    if(batchSize < 1){
        return false;
    }

    for(i=0; i < batchSize; i++){


        if(!dir.entryList().contains(data[i]->getDataName() + "_data.db")){
            return false;
        }
    }
    return true;
}

int DataCollection::getLoadTotal(){

    int total = 0;
    int i;

    for(i=0; i < batchSize; i++){

        total = total + data[i]->getLoadTotal();
    }

    return total;

}


int DataCollection::getSaveTotal(){

    int total = 0;
    int i;

    for(i=0; i < batchSize; i++){

        total = total + data[i]->getSaveTotal();
    }

    return total;

}

int DataCollection::getFastSaveTotal(){

    int total = 0;
    int i;

    for(i=0; i < batchSize; i++){

        total = total + data[i]->getFastSaveTotal();
    }

    return total;

}

void DataCollection::load(){

    int i;

    for(i=0; i < batchSize; i++){

        data[i]->load();
    }

    initialized = true;
    emit dataSetChanged();
}


void DataCollection::slowLoad(){

    int i;

    for(i=0; i < batchSize; i++){

        data[i]->slowLoad();
    }

    initialized = true;
    emit dataSetChanged();
}


void DataCollection::setDataNames(QStringList s){

    int i;

    destroy();

    batchSize = s.size();
    data.resize(batchSize);

    for(i=0; i < batchSize; i++){

        data[i] = new VideoData(this);
        data[i]->setFName("");
        data[i]->setFType("");
        data[i]->setBinSize(binSize);
        data[i]->setBlockHeight(blockHeight);
        data[i]->setBlockWidth(blockWidth);
        data[i]->setFeature1(FEATURE1);
        data[i]->setFeature2(FEATURE2);
        data[i]->setFeature3(FEATURE3);
        data[i]->setFeature4(FEATURE4);
        data[i]->setDataName(s.at(i));

        connect(data[i], SIGNAL(workTick(int)), this, SLOT(getWorkTick(int)));
        connect(data[i], SIGNAL(workTickMax(int)), this, SLOT(getWorkTickMax(int)));

        connect(data[i], SIGNAL(globalTick(int)), this, SLOT(getGlobalTick(int)));
        connect(data[i], SIGNAL(globalTickMax(int)), this, SLOT(getGlobalTickMax(int)));

        connect(data[i], SIGNAL(featureTick(int, int)), this, SLOT(getFeatureTick(int, int)));
        connect(data[i], SIGNAL(featureTickMax(int, int)), this, SLOT(getFeatureTickMax(int, int)));

        connect(data[i], SIGNAL(MSet(int)), this, SLOT(MSet(int)));
     }
}


void DataCollection::setDataName(QString s){

    destroy();

    batchSize = 1;
    data.resize(batchSize);

    data[0] = new VideoData(this);
    data[0]->setFName("");
    data[0]->setFType("");
    data[0]->setBinSize(binSize);
    data[0]->setBlockHeight(blockHeight);
    data[0]->setBlockWidth(blockWidth);
    data[0]->setFeature1(FEATURE1);
    data[0]->setFeature2(FEATURE2);
    data[0]->setFeature3(FEATURE3);
    data[0]->setFeature4(FEATURE4);
    data[0]->setDataName(s);

    connect(data[0], SIGNAL(workTick(int)), this, SLOT(getWorkTick(int)));
    connect(data[0], SIGNAL(workTickMax(int)), this, SLOT(getWorkTickMax(int)));

    connect(data[0], SIGNAL(globalTick(int)), this, SLOT(getGlobalTick(int)));
    connect(data[0], SIGNAL(globalTickMax(int)), this, SLOT(getGlobalTickMax(int)));

    connect(data[0], SIGNAL(featureTick(int, int)), this, SLOT(getFeatureTick(int, int)));
    connect(data[0], SIGNAL(featureTickMax(int, int)), this, SLOT(getFeatureTickMax(int, int)));

    connect(data[0], SIGNAL(MSet(int)), this, SLOT(MSet(int)));
}

void DataCollection::save(){

    int i;

    for(i=0; i < batchSize; i++){

        data[i]->save();
    }
}

void DataCollection::save(QString s){

    int i;

    if(batchSize == 1){

        data[0]->save(s + "_data.db");
    }else{
        for(i=0; i < batchSize; i++){

            data[i]->save(s + QString::number(i) + "_data.db");
        }
    }
}

void DataCollection::fastSave(){

    int i;

    for(i=0; i < batchSize; i++){

        data[i]->fastSave();
    }
}

void DataCollection::fastSave(QString s){

    int i;

    if(batchSize == 1){

        data[0]->fastSave(s + "_data.data");
    }else{
        for(i=0; i < batchSize; i++){

            data[i]->fastSave(s + QString::number(i) + "_data.data");
        }
    }
}

QString DataCollection::getFName(){

    if(batchSize > 0){

        return data[0]->getFName();
    }else{

        return "";
    }
}

QString DataCollection::getFType(){

    if(batchSize > 0){

        return data[0]->getFType();
    }else{

        return "";
    }
}

int DataCollection::getBlockWidth(){

    return blockWidth;
}


int DataCollection::getBlockHeight(){

    return blockHeight;
}


int DataCollection::getBinSize(){

    return binSize;
}

int DataCollection::getM(){

    if(batchSize > 0){
        return data[0]->M;
    }else{

        return -1;
    }
}


int DataCollection::getFeature1(){

    if(batchSize > 0){

        return data[0]->FEATURE1;
    }else{

        return -1;
    }
}

int DataCollection::getFeature2(){

    if(batchSize > 0){

        return data[0]->FEATURE2;
    }else{

        return -1;
    }
}

int DataCollection::getFeature3(){

    if(batchSize > 0){

        return data[0]->FEATURE3;
    }else{

        return -1;
    }
}


int DataCollection::getFeature4(){

    if(batchSize > 0){

        return data[0]->FEATURE4;
    }else{

        return -1;
    }
}

QString DataCollection::getFeatureFileName(int k){

    if(batchSize > 0){

        return data[0]->getFeatureFileName(k);
    }else{

        return "X";
    }

}

// type 0 is fine
// type 1 is coarse

void DataCollection::generateFeatureHighlights(TopicModel *model, int slowDown, vector < vector < bool > > &chain, int type){

    counts = 0;

    if(batchSize > 0){

        if(type == 0){
            total = data[0]->totalFrames;
            data[0]->generateFineHighlights(model, slowDown, chain);
        }
        else if(type == 1){
            total = data[0]->T;
            data[0]->generateCoarseHighlights(model, slowDown, chain);
        }
    }
}

// Slots

void DataCollection::getWorkTick(int k){

    emit workTick(k);
}

void DataCollection::getWorkTickMax(int k){

    emit workTickMax(k);
}

void DataCollection::getGlobalTick(int k){

    emit globalTick(k);
}

void DataCollection::getGlobalTickMax(int k){

    emit globalTickMax(k);
}

void DataCollection::getFeatureTick(int index, int k){

    emit featureTick(index, k);
}

void DataCollection::getFeatureTickMax(int index, int k){

    emit featureTick(index, k);
}

