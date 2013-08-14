#ifndef LEARNINGMODEL
#define LEARNINGMODEL


#include "Sampler.h"
#include "Z_Chain.h"
#include "VideoData.h"
#include "TopicModel.h"
#include "HillClimber.h"
#include "iostream"
#include <QObject>
#include <QString>
#include "projectthread.h"
#include "vector"

using namespace std;

class DataCollection;

class LearningModel : public ProjectThread{


    Q_OBJECT

public:

    static const int S0 = 5000;
    static const int hill0 = 20;

    //*********************************************
    // Data
    //*********************************************

    int K, M, B;
    vector < int > T;
    int S;
    int hillSize;

	double TRATE0;
	double ORATE0;
	double MRATE0;

    double thetaRate;
    double omegaRate;
    double momentumRate;

	double STEP;
	double STEP_CONDITION;
	int KEEPLAST;

    Sampler sampler;
    DataCollection videos;
    TopicModel model;
    HillClimber hill_climbing;

    double max_steps;

    bool initialized;
    QString dataName;

    //*********************************************
    // Methods
    //*********************************************
    LearningModel(ProjectThread *p=0, MainWindow *q=0);
	~LearningModel();

    double log_likelihood();
    vector < double > log_linear_likelihood();
	void init_default();
    void drawTopic(int k);
    void drawTopics();
    bool initialize();
    QString getDataName();

    void setDataName(QString name);
    TopicModel *getTopicModel();
    void reset();
    bool isInitialized();

    void runLoop(int iterations);
    void bestZChain(int batchIndex, vector < vector < bool > > &returnVector);

    int getS();

public slots:

    void E_Step();
    void M_Step();
//    void setVideo(DataCollection* vid);
//    void updateDataSet();

	void setThetaRate(double rate);
	void setOmegaRate(double rate);
	void setMomentum(double rate);
	void setKeepLast(double rate);
	void setStepCondition(double rate);
    void setStep(double rate);
    void setMaxSteps(double steps);

//    void setM(int m);
    void setK(int k);
    void setS(int s);
    void setHillSize(int s);

    void run();
    void pauseRun();
    void randomize();
    void loadModel(QString s);
    void saveModel(QString s);
    void loadModel();
    void saveModel();

//    int getSaveTotal();
//    int getLoadTotal();

    void setFixedTheta(bool x);
    void setFixedOmega(bool x);
    void setFixedSigma(bool x);
    void setFixedSpontaneous(bool x);

signals:

    void eTick(int);
    void eTickMax(int);
    void mTick(int);
    void mTickMax(int);
    void saveTick(int);
    void loadTick(int);
    void setStatus(QString);

    void changeLL(double);

    void changeM(int);
    void changeT(int);
    void changeS(int);
    void changeThetaRate(double);
    void changeOmegaRate(double);
    void changeMomentum(double);

    void topicsDrawn();
    void changeModel(TopicModel *);
    void pausing();

private:

    // These are the thread variables
    bool activeFlag;
    double ll_new;
    double ll_last;
    double change;
    double best;

    QString projectName;
    QString modelName;

    bool fixedTheta;
    bool fixedOmega;
    bool fixedSigma;
    bool fixedSpontaneous;
};


#endif
