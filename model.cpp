#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include "iostream"
#include <cmath>
#include <time.h>
#include "Sampler.h"
#include "LearningModel.h"
#include "HillClimber.h"
#include "TestData.h"

using namespace std;

// I need to define an order on indices. Lets go with K, M, T
// Except for with Z we'll do T, K and for X we'll do T, M
// Also for the probs Aleph, Tau, Nu, we'll use T, K and T, M and T

//We're going to append a row of junk values (-1) at time == 0;


void run_test(){


	TestData* test;
	test = new TestData(2,2,4);
	test->test();
	
	delete test;

}



int main(int ac, char* av[]){

	int M = 3;
	int T = 20;
	int K = 5;
	int S = 400;
	int i, j, k;
	double best = -200;
	double start_time;
	VideoData* data;
	double ll1 = 0;
	double ll2 = 0;
	char *fname = new char[30];
	double total = 0.0;

	double TRATE0 = 0.000001;
	double ORATE0 = 0.000001;
	double MRATE0 = 0.1;

	double STEP = 1;
	int KEEPLAST = 100;



	start_time = clock();

	if(ac > 1){
		run_test();
		return 0;
	}

//	data = new VideoData("brazil.jpg");
//	data = new VideoData("waving", "avi", 5);
//	data->save("waving_data.db");

	data = new VideoData("waving_data.db");





	srand(time(NULL));
//	srand(10);
/*
	data = new VideoData(M, T-1);
	data->X[1*M + 0] = 10;
	data->X[1*M + 1] = 10;
	data->X[1*M + 2] = 1;
	data->X[2*M + 0] = 10;
	data->X[2*M + 1] = 1;
	data->X[2*M + 2] = 10;
	data->X[3*M + 0] = 10;
	data->X[3*M + 1] = 10;
	data->X[3*M + 2] = 1;
	data->X[4*M + 0] = 10;
	data->X[4*M + 1] = 1;
	data->X[4*M + 2] = 10;
*/
	data->setN();

	T = data->T;

	M = data->M;

	cout << T << "\n";
	cout << M << "\n";

	LearningModel* learner = NULL;
	learner = new LearningModel(K, M, T, S);
	learner->model->randomizeOmega();
//	learner->model->randomize();

/*

	learner->model->P0[0] = 0.5;
	learner->model->P0[1] = 0.5;

	learner->model->Omega0[0] = 0.01;
	learner->model->Omega0[1] = 0.01;

*/

	learner->model->Sigma0 = 0.99;
	learner->model->Sigma[0] = 0.2;
	learner->model->Sigma[1] = 0.2;
	learner->model->Sigma[2] = 0.2;
	learner->model->Sigma[3] = 0.2;
	learner->model->Sigma[4] = 0.2;


/*
	learner->model->Omega0[0] = 0.01;
	learner->model->Omega0[1] = 0.01;
	learner->model->Omega0[2] = 0.01;
	learner->model->Omega0[3] = 0.01;
	learner->model->Omega0[4] = 0.01;
*/
/*
	learner->model->Omega[0*K + 0] = 0.01;
	learner->model->Omega[0*K + 1] = 0.99;
	learner->model->Omega[1*K + 0] = 0.99;
	learner->model->Omega[1*K + 1] = 0.01;


	learner->model->Theta0[0] = 0.99;
	learner->model->Theta0[1] = 0.01;
	learner->model->Theta0[2] = 0.01;


	learner->model->Theta[0*M + 0] = 0.01;
	learner->model->Theta[0*M + 1] = 0.98;
	learner->model->Theta[0*M + 2] = 0.01;
	learner->model->Theta[1*M + 0] = 0.01;
	learner->model->Theta[1*M + 1] = 0.01;
	learner->model->Theta[1*M + 2] = 0.98;
*/

//	learner->model->display();
	learner->set_video(data);
//	learner->model->randomize();
//	learner->model->display();

	int c = clock();

	learner->run();


	delete [] fname;
	delete learner;


	cout << "Total elapsed time : " << ((clock()-start_time) / 1000000.0) << " seconds.\n";

}




