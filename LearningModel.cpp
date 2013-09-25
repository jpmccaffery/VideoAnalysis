#include "LearningModel.h"
#include "HillClimber.h"
#include "VideoData.h"
#include <list>
#include <QDebug>
#include "projectthread.h"
#include "datacollection.h"

LearningModel::LearningModel(ProjectThread *p, MainWindow *q) :
    ProjectThread(p, q),
    sampler(S0),
    videos(0, q)
{

    K = -1;
    M = -1;
    S = S0;

    thetaRate = -1;
    omegaRate = -1;
    momentumRate = -1;
    max_steps = -1;

    STEP_CONDITION = 1;
    KEEPLAST = 10;
    STEP = 2;

    activeFlag = false;
    ll_current.first = 0;
    ll_current.second = 0;
    ll_new = 0;
    ll_last = 0;
    change = 0;
    best = 10;

    fixedTheta = false;
    fixedOmega = false;
    fixedSigma = false;
    fixedSpontaneous = true;

    dataName = "";
    initialized = false;

}

LearningModel::~LearningModel(){

}

//***********************************************
// Get statistics
//***********************************************


pair < double, double > LearningModel::log_likelihood(){

    return model.log_likelihood(sampler.chain, videos);
}

vector < double > LearningModel::log_linear_likelihood(){

    return model.log_linear_likelihood(sampler.chain, videos);
}

int LearningModel::getS(){

    return S;
}

//***********************************************
// Run the model's learning methods
//***********************************************

void LearningModel::run(){

    runLoop(-1);
}


void LearningModel::runLoop(int iterations){

    int i, j, count;
    list<double> likelihoods;
    double average = 0.0;
    int times_failed = 0;

    if(!initialize()){

        emit setStatus("Uninitialized");
        qDebug() << "Failed to initialize";

        return;
    }

    i = 0;
    count = 0;
    activeFlag = true;
    while(activeFlag){


        ll_last = ll_new;


        emit setStatus("Drawing");
        for(j=0; j < K+1; j++){

            drawTopic(j);
        }
        emit topicsDrawn();

        E_Step();
        M_Step();

        ll_current = log_likelihood();
        ll_new = ll_current.first + ll_current.second;

        emit changeLL(ll_current.first, ll_current.second);

        change = ll_new - ll_last;


        if(ll_new >= best || best > 1){

            if(i%10 == 0){
                best = ll_new;
//                saveModel("waving_model.db");
            }
        }


        if(i > 0){

            // Update the list of the last 100 changes and the
            // average of the last 100
            average = average * ((double) likelihoods.size());
            average = average + change;
            likelihoods.push_front(change);

            if(likelihoods.size() > KEEPLAST){

                average = average - likelihoods.back();
                likelihoods.pop_back();
            }
            average = average / ((double) likelihoods.size());

            // check the condition and reset if necessary
            if(average < STEP_CONDITION && likelihoods.size() == KEEPLAST){


                if(ll_new > best || best > 1){

                    best = ll_new;
                    qDebug() << "Transition save";
                }
                thetaRate = thetaRate / STEP;
                omegaRate = omegaRate / STEP;

                emit changeThetaRate(thetaRate);
                emit changeOmegaRate(omegaRate);

                hill_climbing.setOmegaRate(omegaRate);
                hill_climbing.setThetaRate(thetaRate);

                likelihoods.clear();
                average = 0;
                times_failed++;
                i = -1;


            }else if(likelihoods.size() == KEEPLAST){

                times_failed = 0;
            }
        }

        if(times_failed > 20){
            break;
        }
        i++;

        count++;
        if(iterations > 0 && count > iterations){
            activeFlag = false;
        }
    }

    emit pausing();
    emit setStatus("Ready");
}



void LearningModel::E_Step(){

    emit setStatus("E-Step");

    if(!initialize()){

        emit setStatus("Uninitialized");
        qDebug() << "Failed to initialize";

        return;
    }

    sampler.sample(model, videos);

    vector < vector < bool > > bestChain;

    sampler.chain.getBest(0, bestChain);
    videos.getData(0)->generateCountVideo("withZChain.avi", bestChain);


}


void LearningModel::M_Step(){

    int steps;

    double max = max_steps;
    double total = 0;

    emit setStatus("M-Step");
    emit mTick(0);

    for(steps=0; steps < max_steps; steps++){

        qDebug() << "Loop begin";

        hill_climbing.dynamic_climb_step(sampler.chain);
        hill_climbing.climb();

        total = total + 1;
        qDebug() << "Loop end 1";
        emit mTick((total / max) * 100.0);
        qDebug() << "Loop end 2";
    }


    emit mTick(100);
}


void LearningModel::pauseRun(){

    activeFlag = false;
}


//***************************************************
// Output Functions
//***************************************************

void LearningModel::drawTopics(){

    for(int j=0; j < K+1; j++){

        drawTopic(j);
    }
    emit topicsDrawn();
}

void LearningModel::drawTopic(int k){

    QString saveName = getProjectName() + "/thumbs/" + dataName + "_topic_";

    qDebug() << "Saving Topic : " << saveName;
    if(k <= K){

        if(k == 0){

            videos.getData(0)->drawTopic(model.Theta0, saveName + "0.png", false);
        }else{

            saveName = saveName + QString::number(k) + ".png";
            videos.getData(0)->drawTopic(model.Theta[k-1], saveName, false);
        }

    }else{

        qDebug() << "No such topic : " << k;
    }
}

bool LearningModel::initialize(){

    bool isReady = false;

    if(!videos.getInitialized()){

        qDebug() << "Video data not prepared";
        return isReady;

    }if(K < 0){
        qDebug() << "K failed : " << K;
        return isReady;
    }
    if(thetaRate < 0){
        qDebug() << "Theta failed : " << thetaRate;
        return isReady;
    }
    if(omegaRate < 0){
        qDebug() << "Omegafailed : " << omegaRate;
        return isReady;
    }
    if(momentumRate < 0){
        qDebug() << "Momentum failed : " << momentumRate;
        return isReady;
    }

    M = videos.getData(0)->M;
    B = videos.getBatchSize();
    T.resize(B);
    for(int i=0; i < B; i++){
        T[i] = videos.getData(i)->T;
    }

    model.initialize(K, M, T);

    model.setFixedOmega(fixedOmega);
    model.setFixedTheta(fixedTheta);
    model.setFixedSigma(fixedSigma);
    model.setFixedSpontaneous(fixedSpontaneous);

    connect(&model, SIGNAL(changeK(int)), this, SIGNAL(changeK(int)));
    connect(this, SIGNAL(changeK(int)), this, SLOT(setK(int)));
    connect(&model, SIGNAL(changeLL(double)), this, SIGNAL(changeLL(double)));
    connect(&model, SIGNAL(changeStatus(QString)), this, SIGNAL(setStatus(QString)));
    connect(&model, SIGNAL(loadTick(int)), this, SIGNAL(saveTick(int)));
    connect(&model, SIGNAL(saveTick(int)), this, SIGNAL(saveTick(int)));

    hill_climbing.initialize(&model, &videos);
    hill_climbing.setFixedTheta(fixedTheta);
    hill_climbing.setFixedOmega(fixedOmega);
    hill_climbing.setFixedSigma(fixedSigma);
    hill_climbing.setFixedSpontaneous(fixedSpontaneous);

    sampler.initialize(model, videos);

    model.setProbs(sampler.chain.Z0);

    connect(&sampler, SIGNAL(sampleTick(int)), this, SIGNAL(eTick(int)));
    connect(&sampler, SIGNAL(sampleTickMax(int)), this, SIGNAL(eTickMax(int)));

    qDebug() << "Model Initialized";
    isReady = true;
    initialized = true;
    return isReady;
}

//************************************************************
// Data Setters
//************************************************************

void LearningModel::setS(int s){

    S = s;
}

void LearningModel::setHillSize(int s){

    hillSize = s;
}

void LearningModel::setK(int k){

    if(K != k){
        K = k;
    }
}

void LearningModel::setThetaRate(double rate){

    thetaRate = rate;
    hill_climbing.setThetaRate(rate);
}

void LearningModel::setOmegaRate(double rate){

    omegaRate = rate;
    hill_climbing.setOmegaRate(rate);
}

void LearningModel::setMomentum(double rate){

    momentumRate = rate;
    hill_climbing.setMomentum(rate);
}

void LearningModel::setStepCondition(double rate){

    STEP_CONDITION = rate;
}

void LearningModel::setStep(double rate){

    STEP = rate;
}

void LearningModel::setMaxSteps(double steps){

    max_steps = steps;
}


void LearningModel::setKeepLast(double rate){

    KEEPLAST = rate;
}

void LearningModel::randomize(){

    int j;
    model.randomize();

    for(j=0; j < K+1; j++){

        drawTopic(j);
    }
    emit topicsDrawn();
}

void LearningModel::loadModel(){

    loadModel(getProjectName() + "/data/" + dataName + "_model.db");

}

void LearningModel::saveModel(){

    saveModel(getProjectName() + "/data/" + dataName + "_model.db");

}

void LearningModel::loadModel(QString s){

    initialize();
    model.load(s);

    initialized = true;
}


void LearningModel::saveModel(QString s){

    model.save(s);
}


QString LearningModel::getDataName(){

    qDebug() << dataName;
    return dataName;
}

void LearningModel::setDataName(QString name){

    dataName = name;
}

TopicModel* LearningModel::getTopicModel(){

    return &model;
}


void LearningModel::setFixedTheta(bool x){

    fixedTheta = x;
    hill_climbing.setFixedTheta(x);
    model.setFixedTheta(x);
}

void LearningModel::setFixedOmega(bool x){

    fixedOmega = x;
    hill_climbing.setFixedOmega(x);
    model.setFixedOmega(x);
}

void LearningModel::setFixedSigma(bool x){

    fixedSigma= x;
    hill_climbing.setFixedSigma(x);
    model.setFixedSigma(x);
}


void LearningModel::setFixedSpontaneous(bool x){

    fixedSpontaneous= x;
    hill_climbing.setFixedSpontaneous(x);
    model.setFixedSpontaneous(x);
}


bool LearningModel::isInitialized(){

    return initialized;
}


void LearningModel::bestZChain(int batchIndex, vector < vector < bool > > &returnVector){

    E_Step();
    sampler.chain.getBest(batchIndex, returnVector);
}
