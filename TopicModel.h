#ifndef TOPICMODEL
#define TOPICMODEL

#include "VideoData.h"
#include "Z_Chain.h";
#include <QObject>
#include <QString>
#include "projectthread.h"
#include "vector"

class DataCollection;

class TopicModel : public ProjectThread{

    Q_OBJECT

public:

    //**********************************************************
    // Data
    //**********************************************************

    int K, M, B;
	double Theta_MIN;
    pair < double, double > ll;
	
    vector < vector < double > > Omega;
    vector < double > Omega0;
    vector < vector < double > > Theta;
    vector < double > Theta0;
    vector < double > Sigma;
	double Sigma0;
    vector < double > P0;	//The probabilities of each Z0

    // We'll do these as X[video][t][whatever]

    vector< vector< double > > Nu;
    vector< vector< vector < double > > > Aleph;
    vector< vector< vector < double > > > Tau;
    // This is the T values for each video
    vector< double > T;

    //**********************************************************
    // Methods
    //**********************************************************

    TopicModel(int topics, int features);
    TopicModel(ProjectThread *p=0, MainWindow *q=0);
    ~TopicModel();
    void initialize(int topics, int features, vector < int > &localTimes);

    void randomize();
    void randomizeOmega();

    //**********************************************************
    // Probability Update Functions
    //**********************************************************
    void setAleph(vector < vector < vector < bool > > > &Z);
    void setTau(vector < vector < vector < bool > > > &Z);
    void setNu(vector < vector < vector < bool > > > &Z);
    void setAleph(int i, vector < vector < bool > > &Z);
    void setTau(int i, vector < vector < bool > > &Z);
    void setNu(int i, vector < vector < bool > > &Z);
    void setProbs(vector < vector < vector < bool > > > &Z);
    void updateProbs(int batch, int time, int topic, bool adding);

    pair < double, double > log_likelihood(Z_Chain &chain, DataCollection &videos);
    vector < double > log_linear_likelihood(Z_Chain &chain, DataCollection &videos);
    pair < double, double > getLL();

    double getAlpha(int b, int t, int position, bool flip_positive);
    double getBeta(int b, int t, int position, bool flip_positive, vector < vector < bool > > &Z_t);
    double getGamma(int b, int t, int position, bool flip_positive, VideoData *video);

    //**********************************************************
    // Helper Functions
    //**********************************************************
    double getAvgSigma();
	double getAvgOmega();
	double getAvgTheta();
	double getAvgTheta0();
	double getPercentSigmaOver(double x);
	double getPercentOmegaOver(double x);
	double getPercentThetaOver(double x);
	double getPercentTheta0Over(double x);
	double getPercentSigmaUnder(double x);
	double getPercentOmegaUnder(double x);
	double getPercentThetaUnder(double x);
	double getPercentTheta0Under(double x);

    int getK();
    int getM();
    int getB();

    //********************************************************
    // Save / Load Functions
    //********************************************************

    int getSaveTotal();
    int getLoadTotal();

private:

    bool fixedTheta;
    bool fixedOmega;
    bool fixedSigma;
    bool fixedSpontaneous;


    void saveData(QString qname);
    void saveTopicData(QString qname, int k);
    void fastSaveTopicData(QString qname, int k);
    void loadData(QString qname);
    void loadTopicData(QString qname, int k);
    void fastLoadTopicData(QString qname, int k);

public slots:

    void save(QString qname);
    void load(QString qname);
    void saveTopic(QString qname, int k);
    void loadTopic(QString qname, int k);


    void setFixedTheta(bool t);
    void setFixedOmega(bool t);
    void setFixedSigma(bool t);
    void setFixedSpontaneous(bool t);

signals:

    void saveTick(int);
    void loadTick(int);
    void changeK(int);
    void changeLL(pair < double, double >);
    void changeStatus(QString);

};


#endif
