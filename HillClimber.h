#ifndef HILLCLIMBER
#define HILLCLIMBER

#include "TopicModel.h"
#include "Z_Chain.h"
#include "VideoData.h"
#include "iostream"
#include <QObject>
#include "vector"
#include "datacollection.h"
#include "TopicModel.h"

using namespace std;


class HillClimber : QObject{

    Q_OBJECT

public:

    //*********************************************************
    // Data
    //*********************************************************

    // K = Number topics
    // M = Number primary features
    // B = Number of videos for batch learning
    int K, M, B;
    // The number of time steps for each of the B videos
    vector < int > T;

    // Local pointers to major data items. Must be updated often
    TopicModel *model;
    DataCollection *videos;

    // Learning rate parameters
	double momentum, omega_rate, theta_rate;
	double Theta_MIN;

    // Gradients for all the variables
    // Probably these should be multidimensional
    vector < vector < double > > gradient_omega;
    vector < vector < double > > gradient_theta;
    vector < double > gradient_sigma;
    vector < double > gradient_omega0;
    vector < double > gradient_theta0;
	double gradient_sigma0;
    vector < double > gradient_P0;

    // Memory terms for momentum based hill climbing
    vector < vector < double > > last_omega;
    vector < vector < double > > last_theta;
    vector < double > last_sigma;
    vector < double > last_omega0;
    vector < double > last_theta0;
	double last_sigma0;
    vector < double > last_P0;

    // Arrays of terms for calculating dynamically updated gradients
    vector < vector < double > > del_term_omega;
    vector < vector < double > > del_term_theta;
    vector < double > del_term_sigma;
    vector < double > del_term_omega0;
    vector < double > del_term_theta0;
	double del_term_sigma0;

    //*********************************************************
    // Methods
    //*********************************************************

    HillClimber();
    ~HillClimber();

    void initialize(TopicModel *model, DataCollection *videos);

    // These methods calculate the change in gradient
    // by flipping a, b or just a or just b depending
    double delta_omega(vector < vector < bool > > &Z, vector < int > &sample, int a, int b);
    double delta_theta(vector < vector < bool > > &Z, vector < int > &sample, int a, int b);
    double delta_sigma(vector < vector < bool > > &Z, vector < int > &sample, int a);
    double delta_omega0(vector < vector < bool > > &Z, vector < int > &sample, int a);
    double delta_theta0(vector < vector < bool > > &Z, vector < int > &sample, int b);
    double delta_sigma0(vector < vector < bool > > &Z, vector < int > &sample);

    // These methods calculate a starting gradient value
    // for each variable
    double base_omega(vector < vector < bool > > &Z, int a, int b, int batch);
    double base_theta(vector < vector < bool > > &Z, int a, int b, int batch);
    double base_sigma(vector < vector < bool > > &Z, int a, int batch);
    double base_omega0(vector < vector < bool > > &Z, int a, int batch);
    double base_theta0(int b, int batch);
    double base_sigma0(int batch);

    // Climb functions
    void climb_step(Z_Chain &chain);
    void dynamic_climb_step(Z_Chain &chain);
    double climb();

    // Rate setters
	void setThetaRate(double rate);
	void setMomentum(double rate);
	void setOmegaRate(double rate);

    void setFixedTheta(bool x);
    void setFixedOmega(bool x);
    void setFixedSigma(bool x);
    void setFixedSpontaneous(bool x);

signals:

    void climbTick(int);


 private:

    bool fixedTheta;
    bool fixedOmega;
    bool fixedSigma;
    bool fixedSpontaneous;

};

#endif
