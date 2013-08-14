#ifndef TESTDATA
#define TESTDATA

#include "TopicModel.h"
#include "LearningModel.h"
#include "Z_Chain.h"
#include "VideoData.h"
#include <cmath>

class TestData{


public:

	int K, T, M, S;

	double* expectedAleph1;
	double* expectedTau1;
	double* expectedNu1;

	double* expectedAleph2;
	double* expectedTau2;
	double* expectedNu2;

	double* expectedAlpha;
	double* expectedBeta;
	double* expectedGamma;

	double* expectedDeltaOmega00;
	double* expectedDeltaOmega01;
	double* expectedDeltaOmega10;
	double* expectedDeltaOmega11;

	double* expectedDeltaTheta00;
	double* expectedDeltaTheta10;

	double* expectedDeltaSigma0;
	double* expectedDeltaSigma1;

	double expectedLogLikely;

	TopicModel* model;
	LearningModel* learner;
	VideoData* video;
	HillClimber* hill_climbing;
	Z_Chain* chain1;
	Z_Chain* chain2;
	Z_Chain* chain3;

	TestData(int topics, int features, int timesteps);
	~TestData();

	void set_model(TopicModel* m);
	void set_video(VideoData* v);
	void set_learning_model(LearningModel* l);
	void test();


};





















#endif

