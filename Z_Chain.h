#ifndef ZCHAIN
#define ZCHAIN
#include "VideoData.h"
#include "map"

class DataCollection;

class Z_Chain{


public:

    static const int BASE_HILLSAMPLES = 200;



    // this will go [Batchsize][timestep][topic]
//    vector< vector < vector < bool > > > Z0;
//    map < vector < vector < vector < bool > > >, int > samples;

    //***************************************************
    // Data
    //***************************************************

    vector < vector < vector < bool > > > Z0;
    vector < vector < vector < bool > > > ZStart;

    vector < int > b_sample;
    vector < int > k_sample;
    vector < int > t_sample;
    vector < double > probs;

    int K, B, S;
    vector< int > T;

    int hillSamples;
    int indexStart;
	double max_prob;
	int max_index;


    //***************************************************
    // Methods
    //***************************************************

    Z_Chain(int topics, int samples, vector < int > &localTimes);
    Z_Chain();
	~Z_Chain();
	
    void initialize(int topics, int samples, vector < int > &localTimes);
	void randomize();
	void normalize();
    void setZStart();

	bool checkRepeats(int number_sampled);
    void getBest(int batchIndex, vector < vector < bool > > &returnVector);

};

#endif
