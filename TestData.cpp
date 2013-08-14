
#include "TestData.h"

TestData::TestData(int topics, int features, int timesteps){

	int t, j;

	K = topics;
	T = timesteps;
	M = features;
	S = T*K;

	// Initialize the video data------------------

//	video = new VideoData(K, T-1);

	video->X[0*M + 0] = -1;
	video->X[0*M + 1] = -1;
	video->X[1*M + 0] = 1;
	video->X[1*M + 1] = 3;
	video->X[2*M + 0] = 1;
	video->X[2*M + 1] = 10;
	video->X[3*M + 0] = 10;
	video->X[3*M + 1] = 1;

	video->N[0] = -1;
	video->N[1] = 4;
	video->N[2] = 11;
	video->N[3] = 11;

	

	// Initialize the topic model-----------------

    model = new TopicModel(K, M);

	model->P0[0] = 0.3;
	model->P0[1] = 0.6;

	model->Sigma0 = 0.7;
	model->Sigma[0] = 0.8;
	model->Sigma[1] = 0.9;
	
	model->Omega0[0] = 0.1;
	model->Omega0[1] = 0.1;
	model->Omega[0*K + 0] = 0.1;
	model->Omega[0*K + 1] = 0.9;
	model->Omega[1*K + 0] = 0.8;
	model->Omega[1*K + 1] = 0.1;

	model->Theta0[0] = 0.15;
	model->Theta0[1] = 0.85;
	model->Theta[0*M + 0] = 0.8;
	model->Theta[0*M + 1] = 0.2;
	model->Theta[1*M + 0] = 0.1;
	model->Theta[1*M + 1] = 0.9;

	// Initialize the learning model------------------------

//	learner = new LearningModel(K, M, T, S);
    learner = new LearningModel(NULL);
	// Initialize the first chain----------------------------


    chain1 = new Z_Chain(K, S, S);

    chain1->Z0[0][0*K+0] = 0;
    chain1->Z0[0][0*K+1] = 1;
    chain1->Z0[0][1*K+0] = 1;
    chain1->Z0[0][1*K+1] = 1;
    chain1->Z0[0][2*K+0] = 0;
    chain1->Z0[0][2*K+1] = 0;
    chain1->Z0[0][3*K+0] = 1;
    chain1->Z0[0][3*K+1] = 1;

	chain1->t_sample[0*K+0] = 0;
	chain1->k_sample[0*K+0] = 0;
	chain1->t_sample[0*K+1] = 0;
	chain1->k_sample[0*K+1] = 1;
	chain1->t_sample[1*K+0] = 1;
	chain1->k_sample[1*K+0] = 0;
	chain1->t_sample[1*K+1] = 1;
	chain1->k_sample[1*K+1] = 1;
	chain1->t_sample[2*K+0] = 2;
	chain1->k_sample[2*K+0] = 0;
	chain1->t_sample[2*K+1] = 2;
	chain1->k_sample[2*K+1] = 1;
	chain1->t_sample[3*K+0] = 3;
	chain1->k_sample[3*K+0] = 0;
	chain1->t_sample[3*K+1] = 3;
	chain1->k_sample[3*K+1] = 1;

	// Initialize the Hill Climber------------------------

    hill_climbing = new HillClimber(model, chain1);


	// Initialize the third chain----------------------------

    chain3 = new Z_Chain(K, 3, 3);

    chain3->Z0[0][0*K+0] = 1; // this is the only change from Z1
    chain3->Z0[0][0*K+1] = 1;
    chain3->Z0[0][1*K+0] = 1;
    chain3->Z0[0][1*K+1] = 1;
    chain3->Z0[0][2*K+0] = 0;	// 0 1
    chain3->Z0[0][2*K+1] = 0;	// 1 1
    chain3->Z0[0][3*K+0] = 1;	// 0 0
    chain3->Z0[0][3*K+1] = 1;	// 0 1

	chain3->t_sample[0] = 0;	// 0 1
	chain3->k_sample[0] = 0;	// 1 1
	chain3->probs[0] = 1.0;
	chain3->t_sample[1] = 2;	// 1 0
	chain3->k_sample[1] = 0;	// 0 1
	chain3->probs[1] = 0.0;
	chain3->t_sample[2] = 3;
	chain3->k_sample[2] = 1;	// 0 1
								// 1 1
								// 1 0
								// 0 0
	chain3->probs[2] = 0.0;


	// Initialize the second chain------------------

    chain2 = new Z_Chain(K, 1, 1);

    chain2->Z0[0][0*K+0] = 0;
    chain2->Z0[0][0*K+1] = 1;
    chain2->Z0[0][1*K+0] = 1;
    chain2->Z0[0][1*K+1] = 0;
    chain2->Z0[0][2*K+0] = 1;
    chain2->Z0[0][2*K+1] = 0;
    chain2->Z0[0][3*K+0] = 1;
    chain2->Z0[0][3*K+1] = 0;

	// Initialize the expected outputs------------------
	
	expectedAleph1 = new double[T*K];

	expectedAleph1[0*K + 0] = 0.91;
	expectedAleph1[0*K + 1] = 0.19;
	expectedAleph1[1*K + 0] = 0.919;
	expectedAleph1[1*K + 1] = 0.838;
	expectedAleph1[2*K + 0] = 0.1;
	expectedAleph1[2*K + 1] = 0.1;
	expectedAleph1[3*K + 0] = 0.919;
	expectedAleph1[3*K + 1] = 0.838;


	expectedTau1 = new double[T*M];

	expectedTau1[0*M + 0] = 0.195;
	expectedTau1[0*M + 1] = 1.405;
	expectedTau1[1*M + 0] = 0.835;
	expectedTau1[1*M + 1] = 1.565;
	expectedTau1[2*M + 0] = 0.105;
	expectedTau1[2*M + 1] = 0.595;
	expectedTau1[3*M + 0] = 0.835;
	expectedTau1[3*M + 1] = 1.565;

	expectedNu1 = new double[T];
	
	expectedNu1[0] = 1.6;
	expectedNu1[1] = 2.4;
	expectedNu1[2] = 0.7;
	expectedNu1[3] = 2.4;



	// Initialize the expected outputs------------------
	
	expectedAleph2 = new double[T*K];

	expectedAleph2[0*K + 0] = 0.91;
	expectedAleph2[0*K + 1] = 0.19;
	expectedAleph2[1*K + 0] = 0.19;
	expectedAleph2[1*K + 1] = 0.82;
	expectedAleph2[2*K + 0] = 0.19;
	expectedAleph2[2*K + 1] = 0.82;
	expectedAleph2[3*K + 0] = 0.19;
	expectedAleph2[3*K + 1] = 0.82;


	expectedTau2 = new double[T*M];

	expectedTau2[0*M + 0] = 0.195;
	expectedTau2[0*M + 1] = 1.405;
	expectedTau2[1*M + 0] = 0.755;
	expectedTau2[1*M + 1] = 0.755;
	expectedTau2[2*M + 0] = 0.755;
	expectedTau2[2*M + 1] = 0.755;
	expectedTau2[3*M + 0] = 0.755;
	expectedTau2[3*M + 1] = 0.755;

	expectedNu2 = new double[T];
	
	expectedNu2[0] = 1.6;
	expectedNu2[1] = 1.5;
	expectedNu2[2] = 1.5;
	expectedNu2[3] = 1.5;


	// Initialize the expected sampling terms------------------
	
	expectedAlpha = new double[T*K];

	expectedAlpha[0*K + 0] = 0.429;
	expectedAlpha[0*K + 1] = 0.6667;
	expectedAlpha[1*K + 0] = 0.099;
	expectedAlpha[1*K + 1] = 4.263;
	expectedAlpha[2*K + 0] = 11.35;
	expectedAlpha[2*K + 1] = 5.17;
	expectedAlpha[3*K + 0] = 9.0;
	expectedAlpha[3*K + 1] = 9.0;


	expectedBeta = new double[T*K];

	expectedBeta[0*K + 0] = 4.4541;
	expectedBeta[0*K + 1] = 0.0575;
	expectedBeta[1*K + 0] = 5.556;
	expectedBeta[1*K + 1] = 11.111;
	expectedBeta[2*K + 0] = 15.58;
	expectedBeta[2*K + 1] = 17.29;
	expectedBeta[3*K + 0] = 1.0;
	expectedBeta[3*K + 1] = 1.0;

	expectedGamma = new double[T*K];
	
	expectedGamma[0*K + 0] = 1.0;
	expectedGamma[0*K + 1] = 1.0;
	expectedGamma[1*K + 0] = 0.8555;
	expectedGamma[1*K + 1] = 0.6565;
	expectedGamma[2*K + 0] = 0.0176;
	expectedGamma[2*K + 1] = 1.1251;
	expectedGamma[3*K + 0] = 0.00003747;
	expectedGamma[3*K + 1] = 27.13;


	// Initialize the dynamic Omega terms------------------
	
	expectedDeltaOmega00 = new double[T*K];

	expectedDeltaOmega00[0*K + 0] = 0.0 	+	0.098;
	expectedDeltaOmega00[0*K + 1] = 0.0 	+	0.0;
	expectedDeltaOmega00[1*K + 0] = 0.0 	+	1.1111;
	expectedDeltaOmega00[1*K + 1] = 0.0 	+	0.0;
	expectedDeltaOmega00[2*K + 0] = 1.209 	+	4.7368;
	expectedDeltaOmega00[2*K + 1] = 0.0 	+	0.0;
	expectedDeltaOmega00[3*K + 0] = 0.0		+	0.0;
	expectedDeltaOmega00[3*K + 1] = 0.0 	+	0.0;

	expectedDeltaOmega01 = new double[T*K];

	expectedDeltaOmega01[0*K + 0] = 0.0 	+	-0.1076;
	expectedDeltaOmega01[0*K + 1] = 0.0 	+	-0.989;
	expectedDeltaOmega01[1*K + 0] = -10.989 +	0.0;
	expectedDeltaOmega01[1*K + 1] = 0.0 	+	10.0;
	expectedDeltaOmega01[2*K + 0] = 10.8814 +	0.0;
	expectedDeltaOmega01[2*K + 1] = 0.0 	+	0.989;
	expectedDeltaOmega01[3*K + 0] = 0.0 	+	0.0;
	expectedDeltaOmega01[3*K + 1] = 0.0 	+	0.0;

	expectedDeltaOmega10 = new double[T*K];

	expectedDeltaOmega10[0*K + 0] = 0.0 	+	0.9666;
	expectedDeltaOmega10[0*K + 1] = 0.0 	+	0.0;
	expectedDeltaOmega10[1*K + 0] = 0.0 	+	5.0;
	expectedDeltaOmega10[1*K + 1] = 0.0 	+	0.0;
	expectedDeltaOmega10[2*K + 0] = 0.0 	+	1.0976;
	expectedDeltaOmega10[2*K + 1] = 5.9666 	+	0.0;
	expectedDeltaOmega10[3*K + 0] = 0.0 	+ 	0.0;
	expectedDeltaOmega10[3*K + 1] = 0.0 	+ 	0.0;

	expectedDeltaOmega11 = new double[T*K];

	expectedDeltaOmega11[0*K + 0] = 0.0 	+ 	-4.522;
	expectedDeltaOmega11[0*K + 1] = 0.0 	+ 	-4.7368;
	expectedDeltaOmega11[1*K + 0] = 0.0 	+ 	0.0;
	expectedDeltaOmega11[1*K + 1] = -5.848 	+	1.1111;
	expectedDeltaOmega11[2*K + 0] = 0.0 	+ 	0.0;
	expectedDeltaOmega11[2*K + 1] = 1.326	+ 	4.7368;
	expectedDeltaOmega11[3*K + 0] = 0.0 	+ 	0.0;
	expectedDeltaOmega11[3*K + 1] = 0.0 	+ 	0.0;

	// Initialize the dynamic Theta terms------------------
	
	expectedDeltaTheta00 = new double[T*K];

	expectedDeltaTheta00[0*K + 0] = 0.0;
	expectedDeltaTheta00[0*K + 1] = 0.0;
	expectedDeltaTheta00[1*K + 0] = 0.5755;
	expectedDeltaTheta00[1*K + 1] = -1.5295;
	expectedDeltaTheta00[2*K + 0] = -9.522;
	expectedDeltaTheta00[2*K + 1] = 0.0;
	expectedDeltaTheta00[3*K + 0] = -9.0697;
	expectedDeltaTheta00[3*K + 1] = 0.6090;

	expectedDeltaTheta10 = new double[T*K];

	expectedDeltaTheta10[0*K + 0] = 0.0;
	expectedDeltaTheta10[0*K + 1] = 0.0;
	expectedDeltaTheta10[1*K + 0] = 3.3411;
	expectedDeltaTheta10[1*K + 1] = 0.6474;
	expectedDeltaTheta10[2*K + 0] = 0.0;
	expectedDeltaTheta10[2*K + 1] = -1.7903;
	expectedDeltaTheta10[3*K + 0] = 35.3100;
	expectedDeltaTheta10[3*K + 1] = -10.2034;


	// Initialize the dynamic Sigma terms------------------
	
	expectedDeltaSigma0 = new double[T*K];

	expectedDeltaSigma0[0*K + 0] = 0.0;
	expectedDeltaSigma0[0*K + 1] = 0.0;
	expectedDeltaSigma0[1*K + 0] = 0.3252;
	expectedDeltaSigma0[1*K + 1] = -0.4730;
	expectedDeltaSigma0[2*K + 0] = -3.6105;
	expectedDeltaSigma0[2*K + 1] = 0.0;
	expectedDeltaSigma0[3*K + 0] = -5.1253;
	expectedDeltaSigma0[3*K + 1] = -1.4555;

	expectedDeltaSigma1 = new double[T*K];

	expectedDeltaSigma1[0*K + 0] = 0.0;
	expectedDeltaSigma1[0*K + 1] = 0.0;
	expectedDeltaSigma1[1*K + 0] = -0.2438;
	expectedDeltaSigma1[1*K + 1] = -0.1783;
	expectedDeltaSigma1[2*K + 0] = 0.0;
	expectedDeltaSigma1[2*K + 1] = 0.0435;
	expectedDeltaSigma1[3*K + 0] = 1.7044;
	expectedDeltaSigma1[3*K + 1] = 2.8106;

	// Initialize the expected log likelihood for chain1
	expectedLogLikely = -28.4128;
}


TestData::~TestData(){




	delete video;
	delete model;
	delete learner;
	delete chain1;


}

void TestData::set_model(TopicModel* m){

	delete model;
	model = m;
}

void TestData::set_video(VideoData* v){

	delete video;
	video = v;
}


void TestData::set_learning_model(LearningModel* l){

	delete learner;
	learner = l;
}	

void TestData::test(){

	int i, j, k;
	int t;
	int errors;
	double x, y, z;

    model->setProbs(&(chain1->Z0));

	errors = 0;
	cout << "\nChain 1 Test--------------\n";
	cout << "Aleph Test...\t\t";
	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(model->Aleph[0][k][t] - expectedAleph1[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << "\n";
                cout << model->Aleph[0][k][t] << "\t expected : \t" << expectedAleph1[t*K+k] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Tau Test...\t\t";
	for(t=0; t < T; t++){
		for(j=0; j < M; j++){

            if(abs(model->Tau[0][j][t] - expectedTau1[t*M+j]) > 0.01){
				cout << "\nError : t = " << t << ", j = " << j << "\n";
                cout << model->Tau[0][j][t] << "\t expected : \t" << expectedTau1[t*M+j] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Nu Test...\t\t";
	for(t=0; t < T; t++){

        if(abs(model->Nu[0][t] - expectedNu1[t]) > 0.01){
			cout << "\nError : t = " << t << "\n";
            cout << model->Nu[0][t] << "\t expect : \t" << expectedNu1[t];
			errors++;
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}


	cout << "\n";
    model->setProbs(&(chain2->Z0));

	errors = 0;
	cout << "Chain 2 Test--------------\n";
	cout << "Aleph Test...\t\t";
	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(model->Aleph[0][k][t] - expectedAleph2[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << "\n";
                cout << model->Aleph[0][k][t] << "\t expected : \t" << expectedAleph2[t*K+k] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Tau Test...\t\t";
	for(t=0; t < T; t++){
		for(j=0; j < M; j++){

            if(abs(model->Tau[0][j][t] - expectedTau2[t*M+j]) > 0.01){
				cout << "\nError : t = " << t << ", j = " << j << "\n";
                cout << model->Tau[0][j][t] << "\t expected : \t" << expectedTau2[t*M+j] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Nu Test...\t\t";
	for(t=0; t < T; t++){

        if(abs(model->Nu[0][t] - expectedNu2[t]) > 0.01){
			cout << "\nError : t = " << t << "\n";
            cout << model->Nu[0][t] << "\t expect : \t" << expectedNu2[t];
			errors++;
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}
	cout << "\n";
	
    model->updateProbs(0, 1, 1, true);
    model->updateProbs(0, 2, 0, false);
    model->updateProbs(0, 3, 1, true);

	errors = 0;
	cout << "Backward Chain Test--------------\n";
	cout << "Aleph Test...\t\t";
	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(model->Aleph[0][k][t] - expectedAleph1[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << "\n";
                cout << model->Aleph[0][k][t] << "\t expected : \t" << expectedAleph1[t*K+k] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Tau Test...\t\t";
	for(t=0; t < T; t++){
		for(j=0; j < M; j++){

            if(abs(model->Tau[0][j][t] - expectedTau1[t*M+j]) > 0.01){
				cout << "\nError : t = " << t << ", j = " << j << "\n";
                cout << model->Tau[0][j][t] << "\t expected : \t" << expectedTau1[t*M+j] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Nu Test...\t\t";
	for(t=0; t < T; t++){

        if(abs(model->Nu[0][t] - expectedNu1[t]) > 0.01){
			cout << "\nError : t = " << t << "\n";
            cout << model->Nu[0][t] << "\t expect : \t" << expectedNu1[t];
			errors++;
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	cout << "\n";

    model->updateProbs(0, 1, 1, false);
    model->updateProbs(0, 3, 1, false);
    model->updateProbs(0, 2, 0, true);



	errors = 0;
	cout << "Forward Chain Test--------------\n";
	cout << "Aleph Test...\t\t";
	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(model->Aleph[0][k][t] - expectedAleph2[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << "\n";
                cout << model->Aleph[0][k][t] << "\t expected : \t" << expectedAleph2[t*K+k] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Tau Test...\t\t";
	for(t=0; t < T; t++){
		for(j=0; j < M; j++){

            if(abs(model->Tau[0][j][t] - expectedTau2[t*M+j]) > 0.01){
				cout << "\nError : t = " << t << ", j = " << j << "\n";
                cout << model->Tau[0][j][t] << "\t expected : \t" << expectedTau2[t*M+j] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Nu Test...\t\t";
	for(t=0; t < T; t++){

        if(abs(model->Nu[0][t] - expectedNu2[t]) > 0.01){
			cout << "\nError : t = " << t << "\n";
            cout << model->Nu[0][t] << "\t expect : \t" << expectedNu2[t];
			errors++;
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	cout << "\n";


    model->setProbs(&(chain1->Z0));

	errors = 0;
	cout << "Sample Parameter Test--------------\n";
	cout << "Alpha Test...\t\t";
	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(model->getAlpha(0, t, k, !chain1->Z0[0][t*K+k]) - expectedAlpha[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << "\n";
                cout << model->getAlpha(0, t, k, !chain1->Z0[0][t*K+k]) << "\t expected : \t" << expectedAlpha[t*K+k] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Beta Test...\t\t";
	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(model->getBeta(0, t, k, !chain1->Z0[0][t*K+k], (chain1->Z0[0] + t*K)) - expectedBeta[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << "\n";
                cout << "Aleph " << t << " , 0 = " << model->Aleph[0][0][t] << "\n";
                cout << "Aleph " << t << " , 1 = " << model->Aleph[0][1][t] << "\n";
                cout << model->getBeta(0, t, k, !chain1->Z0[0][t*K+k], (chain1->Z0[0] + t*K)) << "\t expected : \t" << expectedBeta[t*K+k] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	errors = 0;
	cout << "Gamma Test...\t\t";
	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(model->getGamma(0, t, k, !chain1->Z0[0][t*K+k], video) - expectedGamma[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << "\n";
                cout << model->getGamma(0, t, k, !chain1->Z0[0][t*K+k], video) << "\t expected : \t" << expectedGamma[t*K+k] << "\n";
				errors++;
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}

	cout << "\n";

	errors = 0;
	cout << "Delta Gradient Test--------------\n";
	cout << "Delta Sigma Test...\t";
    model->setProbs(&(chain1->Z0));

	for(i=0; i<K; i++){
		for(t=0; t < T; t++){
			for(k=0; k < K; k++){

				if(i == 0){
                    if(abs(hill_climbing->delta_sigma(chain1->Z0[0], (t*K+k), i) - expectedDeltaSigma0[t*K+k]) > 0.01){
						cout << "\nError : t = " << t << ", k = " << k << ", i = " << i << "\n";
                        cout << hill_climbing->delta_sigma(chain1->Z0[0], (t*K+k), i) << "\t expected : \t" << expectedDeltaSigma0[t*K+k] << "\n";
						errors++;
					}
				}else if(i == 1){
                    if(abs(hill_climbing->delta_sigma(chain1->Z0[0], (t*K+k), i) - expectedDeltaSigma1[t*K+k]) > 0.01){
						cout << "\nError : t = " << t << ", k = " << k << ", i = " << i << "\n";
                        cout << hill_climbing->delta_sigma(chain1->Z0[0], (t*K+k), i) << "\t expected : \t" << expectedDeltaSigma1[t*K+k] << "\n";
						errors++;
					}
				}
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}


	errors = 0;
	cout << "Delta Theta Test...\t";
    model->setProbs(&(chain1->Z0));

	for(i=0; i<K; i++){
		for(j=0; j < (M-1); j++){
			for(t=0; t < T; t++){
				for(k=0; k < K; k++){

					if(i == 0){
                        if(abs(hill_climbing->delta_theta(chain1->Z0[0], (t*K+k), i, j) - expectedDeltaTheta00[t*K+k]) > 0.01){
							cout << "\nError : t = " << t << ", k = " << k << ", i = " << i << "\n";
                            cout << hill_climbing->delta_theta(chain1->Z0[0], (t*K+k), i, j) << "\t expected : \t" << expectedDeltaTheta00[t*K+k] << "\n";
							errors++;
						}
					}else if(i == 1){
                        if(abs(hill_climbing->delta_theta(chain1->Z0[0], (t*K+k), i, j) - expectedDeltaTheta10[t*K+k]) > 0.01){
							cout << "\nError : t = " << t << ", k = " << k << ", i = " << i << "\n";
                            cout << hill_climbing->delta_theta(chain1->Z0[0], (t*K+k), i, j) << "\t expected : \t" << expectedDeltaTheta10[t*K+k] << "\n";
							errors++;
						}
					}
				}
			}
		}
	}
	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}




	errors = 0;
	cout << "Delta Omega Test...\t";
    model->setProbs(&(chain1->Z0));

	for(t=0; t < T; t++){
		for(k=0; k < K; k++){

            if(abs(hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 0, 0) - expectedDeltaOmega00[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << ", a,b = 0,0\n";
                cout << hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 0, 0) << "\t expected : \t" << expectedDeltaOmega00[t*K+k] << "\n";
				errors++;
			}

            if(abs(hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 0, 1) - expectedDeltaOmega01[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << ", a,b = 0,1\n";
                cout << hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 0, 1) << "\t expected : \t" << expectedDeltaOmega01[t*K+k] << "\n";
				errors++;
			}

            if(abs(hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 1, 0) - expectedDeltaOmega10[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << ", a,b = 1,0\n";
                cout << hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 1, 0) << "\t expected : \t" << expectedDeltaOmega10[t*K+k] << "\n";
				errors++;
			}

            if(abs(hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 1, 1) - expectedDeltaOmega11[t*K+k]) > 0.01){
				cout << "\nError : t = " << t << ", k = " << k << ", a,b = 1,1\n";
                cout << hill_climbing->delta_omega(chain1->Z0[0], (t*K+k), 1, 1) << "\t expected : \t" << expectedDeltaOmega11[t*K+k] << "\n";
				errors++;
			}

		}
	}

	if(errors > 0){
		cout << errors << " errors identified\n";
	}else{
		cout << "[OK]\n";
	}


	cout << "\n";




	cout << "Log Likelihood Test--------------\n";
	cout << "Test...\t\t\t";
//	x = model->log_likelihood(chain3, video);
	if(abs(x - expectedLogLikely) > 0.01){
		cout << x << "\n\t expected : \t" << expectedLogLikely << "\n";
	}else{
		cout << "[OK]\n";
	}	
	cout << "\n";

}
