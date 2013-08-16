#include "TopicModel.h"
#include "iostream"
#include <stdlib.h>
#include <cmath>
#include "fstream"
#include <QDebug>
#include "datacollection.h"
#include <fstream>

using namespace std;


//Here the k=0 topic always indicates the null topic and 
//Omega[0][] is a meaningless quantity. Theta[0][] is the everpresent
//background topic. Omega[k][0] is the base rate probability of topic k.
//Sigma k is the background topic's mixing coefficient.

TopicModel::TopicModel(ProjectThread *p, MainWindow *q) :
    ProjectThread(p, q)
{


	cout << "Weak Initialization.\n";

	K = 0;
	M = 0;
    B = 0;

	Sigma0 = 0.0;
    ll.first = 0.0;
    ll.second = 0.0;
	Theta_MIN = 0.0;
}

//We'll let t = 0 be the null video frames that don't mean anything.
//I don't think we're doing this one anymore
TopicModel::TopicModel(int topics, int features) :
    ProjectThread(0, 0)
{

    cout << "Constructing Model.\n";

    K = topics;
    M = features;
    B = 0;

    Theta_MIN = (1.0 / ( ((double) M) * ((double) M) * ((double) M) ) );
    ll.first = -1000000.0;
    ll.second = -1000000.0;

    Omega0.resize(K);
    Omega.resize(K);
    for(int i=0; i < K; i++){
        Omega[i].resize(K);
    }

    Theta0.resize(M);
    Theta.resize(K);
    for(int i=0; i < K; i++){
        Theta[i].resize(M);
    }

    Sigma0 = 0;
    Sigma.resize(K);

    P0.resize(K);

    randomize();

}

TopicModel::~TopicModel(){


}

void TopicModel::initialize(int topics, int features, vector<int> &localTimes){

    cout << "Constructing Model.\n";

    bool needToUpdate = false;

    if(K != topics || M != features || B != localTimes.size()){

        needToUpdate = true;
    }else{

        for(int i=0; i < B; i++){
            if(T[i] != localTimes[i]){
                needToUpdate = true;
                break;
            }
        }
    }

    if(needToUpdate){

        K = topics;
        M = features;
        B = localTimes.size();

        Theta_MIN = (1.0 / ( ((double) M) * ((double) M) * ((double) M) ) );
        ll.first = -1000000.0;
        ll.second = -1000000.0;

        T.resize(B);
        for(int i=0; i < B; i++){

            T[i] = localTimes[i];
        }

        Omega0.resize(K);
        Omega.resize(K);
        for(int i=0; i < K; i++){
            Omega[i].resize(K);
        }

        Theta0.resize(M);
        Theta.resize(K);
        for(int i=0; i < K; i++){
            Theta[i].resize(M);
        }

        Sigma0 = 0;
        Sigma.resize(K);

        P0.resize(K);

        Aleph.resize(B);
        for(int b=0; b < B; b++){

            Aleph[b].resize(K);
            for(int k=0; k < K; k++){

                Aleph[b][k].resize(T[b]);
            }
        }

        Tau.resize(B);
        for(int b=0; b < B; b++){

            Tau[b].resize(M);
            for(int j=0; j < M; j++){

                Tau[b][j].resize(T[b]);
            }
        }

        Nu.resize(B);
        for(int b=0; b < B; b++){

            Nu[b].resize(T[b]);
        }

        randomize();
    }
}

//********************************************
// Load / Save Functions
//********************************************

int TopicModel::getSaveTotal(){

    int total = 0;

    total = total + 5;
    total = total + (K+1);
    total = total + (K+1)*(K+1);
    total = total + (K+1)*M;
    total = total + (K+1);

    return total;
}

int TopicModel::getLoadTotal(){

    int total = 0;

    total = total + 5;
    total = total + (K+1);
    total = total + (K+1)*(K+1);
    total = total + (K+1)*M;
    total = total + (K+1);

    return total;
}

void TopicModel::saveTopic(QString qname, int k){

//  saveTopicData(qname, k);
    fastSaveTopicData(qname, k);
}

void TopicModel::loadTopic(QString qname, int k){

//  loadTopicData(qname, k);
    fastLoadTopicData(qname, k);
}

void TopicModel::save(QString qname){

    saveData(qname);
}

void TopicModel::load(QString qname){

    emit changeStatus("Loading");
    loadData(qname);
    emit changeStatus("Ready");
}

void TopicModel::saveTopicData(QString qname, int k){

    DataManager *datamanager = new DataManager();

    sleep(1);

    connect(datamanager, SIGNAL(saveTick(int)), this, SIGNAL(saveTick(int)));

    emit changeStatus("Saving");

    /*************************************/
    // Save parameters
    /*************************************/

    datamanager->saveInt(qname, "M", "M", M);

    /*************************************/
    // Save Theta
    /*************************************/

    if(k == 0){
        datamanager->saveArray(qname, "Theta", "feature", "probability", Theta0);
    }else{
        datamanager->saveArray(qname, "Theta", "feature", "probability", Theta[k-1]);
    }

    emit changeStatus("Ready");
    disconnect(datamanager, SIGNAL(loadTick(int)), this, SIGNAL(loadTick(int)));
    delete datamanager;
}


void TopicModel::fastSaveTopicData(QString qname, int k){

    int i, j;
    ofstream fileOut;

    fileOut.open(qname.toStdString().c_str());
    sleep(1);

    emit changeStatus("Saving");

    /*************************************/
    // Save parameters
    /*************************************/

    fileOut << M << endl;

    /*************************************/
    // Save Theta
    /*************************************/

    if(k == 0){
        for(j=0; j < M; j++){
            fileOut << Theta0[j] << endl;
        }
    }else{
        for(j=0; j < M; j++){
            fileOut << Theta[k-1][j] << endl;
        }
    }

    fileOut.close();
    emit changeStatus("Ready");
}

void TopicModel::saveData(QString qname){

    int i, j;
    vector < double > storage1;
    vector < vector < double > > storage2;
    vector < vector < vector < double > > > storage3;
    DataManager *datamanager = new DataManager();

    sleep(1);

    connect(datamanager, SIGNAL(saveTick(int)), this, SIGNAL(saveTick(int)));

    emit changeStatus("Saving");

    /*************************************/
    // Save parameters
    /*************************************/

    datamanager->saveInt(qname, "K", "K", K);
    datamanager->saveInt(qname, "M", "M", M);
    datamanager->saveDouble(qname, "Theta_MIN", "Theta_MIN", Theta_MIN);
    datamanager->saveDouble(qname, "Log_Likelihood", "likelihood", (ll.first + ll.second));;

    cout << "Saving Omega values...\n";
    /*************************************/
    // Save P0
    /*************************************/
    storage1.resize(K+1);

    storage1[0] = -1;
    for(i=1; i < K+1; i++){

        storage1[i] = P0[i-1];
    }

    datamanager->saveArray(qname, "P0", "category", "probability", storage1);

    /*************************************/
    // Save Omega
    /*************************************/
    storage2.resize(K+1);

    for(i=0; i < K+1; i++){

        storage2[i].resize(K+1);

        for(j=0; j < K+1; j++){

            if(j == 0){
                storage2[i][j] = -1.0;
            }else if(i == 0){
                storage2[i][j] = Omega0[j-1];
            }else{
                storage2[i][j] = Omega[j-1][i-1];
            }
        }
    }

    vector < QString > keyNames;
    keyNames.resize(3);
    keyNames[0] = "cause";
    keyNames[1] = "effect";
    keyNames[2] = "probability";

    datamanager->saveArray(qname, "Omega", keyNames, storage2);

    /*************************************/
    // Save Theta
    /*************************************/

    storage2.resize(K+1);

    for(i=0; i < K+1; i++){

        storage2[i].resize(M);

        for(j=0; j < M; j++){

            if(i == 0){
                storage2[i][j] = Theta0[j];
            }else{
                storage2[i][j] = Theta[i-1][j];
            }
        }
    }

    keyNames.resize(3);
    keyNames[0] = "category";
    keyNames[1] = "feature";
    keyNames[2] = "probability";

    datamanager->saveArray(qname, "Theta", keyNames, storage2);

    /*************************************/
    // Save Sigma
    /*************************************/

    storage1.resize(K+1);

    storage1[0] = Sigma0;

    for(i=1; i < K+1; i++){

        storage1[i] = Sigma[i-1];
    }

    datamanager->saveArray(qname, "Sigma", "category", "proportion", storage1);

    /*************************************/
    // Cleanup
    /*************************************/

    emit changeStatus("Ready");
    disconnect(datamanager, SIGNAL(loadTick(int)), this, SIGNAL(loadTick(int)));
    delete datamanager;
}


void TopicModel::loadTopicData(QString qname, int k){

    int j;
    vector < double > storage;
    DataManager *datamanager = new DataManager();

    sleep(1);

    connect(datamanager, SIGNAL(saveTick(int)), this, SIGNAL(saveTick(int)));

    emit changeStatus("Saving");

    /*************************************/
    // Load parameters
    /*************************************/

    if(M != datamanager->loadDouble(qname, "M")){
        qDebug() << "Load failure: mismatched number of features.";
    }

    /*************************************/
    // Load Theta
    /*************************************/

    datamanager->loadDoubleArray(qname, "Theta", "probability", storage);

    if(k == 0){
        for(j=0; j < M; j++){

            Theta0[j] = storage[j];
        }
    }else{
        for(j=0; j < M; j++){

            Theta[k-1][j] = storage[j];
        }
    }

    emit changeStatus("Ready");
    disconnect(datamanager, SIGNAL(loadTick(int)), this, SIGNAL(loadTick(int)));
    delete datamanager;
}


void TopicModel::fastLoadTopicData(QString qname, int k){

    int i, j;
    ifstream fileIn;

    fileIn.open(qname.toStdString().c_str());

    sleep(1);

    emit changeStatus("Loading");

    /*************************************/
    // Load parameters
    /*************************************/

    fileIn >> i;

    if(i != M || k > this->K){

        qDebug() << "Mismatched features " << i << endl;
    }

    /*************************************/
    // Load Theta
    /*************************************/


    if(k == 0){
        for(j=0; j < M; j++){

            fileIn >> Theta0[j];
        }
    }else{
        for(j=0; j < M; j++){

            fileIn >> Theta[k-1][j];
        }
    }

    fileIn.close();
    emit changeStatus("Ready");
}


void TopicModel::loadData(QString qname){

    int i, j;
    vector < double > storage1;
    vector < vector < double > > storage2;

	DataManager *datamanager = new DataManager();

    connect(datamanager, SIGNAL(loadTick(int)), this, SIGNAL(loadTick(int)));

    /*************************************/
    // load parameters
	/*************************************/

    K = datamanager->loadDouble(qname, "K");
    M = datamanager->loadDouble(qname, "M");
    Theta_MIN = datamanager->loadDouble(qname, "Theta_MIN");

    ll.first = 0;
    ll.second = 0;
//    ll = datamanager->loadDouble(qname, "Log_Likelihood");;

	/****************************************/
	// Allocate memory
	/****************************************/

    Omega0.resize(K);
    Omega.resize(K);
    for(int i=0; i < K; i++){
        Omega[i].resize(K);
    }

    Theta0.resize(M);
    Theta.resize(K);
    for(int i=0; i < K; i++){
        Theta[i].resize(M);
    }

	Sigma0 = 0;
    Sigma.resize(K);

    P0.resize(K);


	/*************************************/
	// Load P0
	/*************************************/

    datamanager->loadDoubleArray(qname, "P0", "probability", storage1);

	for(i=1; i < K+1; i++){

            if(storage1[i] > 10 || storage1[i] < -10){
				cout << "P0 Storage error : " << i << "\n";
				cin >> i;
			}
            P0[i-1] = storage1[i];
    }

	/*************************************/
	// Load Omega
	/*************************************/

    datamanager->loadDoubleArray(qname, "Omega", "probability", storage1);
	
	for(i=0; i < K+1; i++){

		for(j=0; j < K+1; j++){
	
			if(j == 0){
                j = 0;
			}else if(i == 0){
                Omega0[j-1] = storage1[i*(K+1) + j];
			}else{
                Omega[j-1][i-1] = storage1[i*(K+1) + j];
			}

		}
    }

	/*************************************/
	// Load Theta
	/*************************************/

    datamanager->loadDoubleArray(qname, "Theta", "probability", storage1);

	for(i=0; i < K+1; i++){

		for(j=0; j < M; j++){

			if(i == 0){
                Theta0[j] = storage1[i*M + j];
			}else{
                Theta[i-1][j] = storage1[i*M + j];
			}
		}
    }

	/*************************************/
	// Load Sigma
	/*************************************/

    datamanager->loadDoubleArray(qname, "Sigma", "proportion", storage1);

	for(i=0; i < K+1; i++){

		if(i == 0){
            Sigma0 = storage1[i];
		}else{
            Sigma[i-1] = storage1[i];
		}
    }

	/*************************************/
    // Cleanup
	/*************************************/

    emit changeK(K);
    emit changeLL(ll);
    emit changeStatus("Waiting");

    disconnect(datamanager, SIGNAL(loadTick(int)), this, SIGNAL(loadTick(int)));

    delete datamanager;
}



void TopicModel::randomize(){

	double d = 0.0;
	double d0 = 0.0;
	int i, j, k;
	int a;
	double r;
	int index1, index2;
	// This parameter governs how much the values will be shuffled
	int shuffle = 10;

//	srand(time(NULL));

	// There is only one Sigma0
	Sigma0 = ((double) rand()) / ((double) RAND_MAX);
	for(k = 0; k < K; k++){

		Sigma[k] = ((double) rand()) / ((double) RAND_MAX);
		P0[k] = ((double) rand()) / ((double) RAND_MAX);

		Omega0[k] = ((double) rand()) / ((double) RAND_MAX);
		for(i = 0; i < K; i++){
			
            Omega[k][i] = ((double) rand()) / ((double) RAND_MAX);
        }

		// Set the Thetas to uniform and then shuffle them
		r = 1.0 / ((double) M);
		for(j = 0; j < M; j++){

            Theta[k][j] = r;
		}
		for(a=0; a < shuffle * M; a++){

			index1 = rand() % M;
			index2 = rand() % M;

			r = ((double) rand()) / ((double) RAND_MAX);
            r = r * min(Theta[k][index1], Theta[k][index2]);
			j = rand() % 2;
            if(j && (Theta[k][index2] - r) > Theta_MIN){
                Theta[k][index1] = Theta[k][index1] + r;
                Theta[k][index2] = Theta[k][index2] - r;
            }else if((Theta[k][index1] - r) > Theta_MIN){
                Theta[k][index1] = Theta[k][index1] - r;
                Theta[k][index2] = Theta[k][index2] + r;
			}
		}
		r = 0.0;
		for(j=0; j < M; j++){
            r = r + Theta[k][j];
		}
		cout << "Total : " << (r-1.0) << "\n";

	}

	// Now do the Theta0 terms separately
	r = 1.0 / ((double) M);
	for(j = 0; j < M; j++){

		Theta0[j] = r;
	}
	for(a=0; a < shuffle * M; a++){

		index1 = rand() % M;
		index2 = rand() % M;

		r = ((double) rand()) / ((double) RAND_MAX);
		r = r * min(Theta0[index1], Theta0[index2]);
		j = rand() % 2;
		if(j && (Theta0[index2] - r) > Theta_MIN){
			Theta0[index1] = Theta0[index1] + r;
			Theta0[index2] = Theta0[index2] - r;
		}else if((Theta0[index1] - r) > Theta_MIN){
			Theta0[index1] = Theta0[index1] - r;
			Theta0[index2] = Theta0[index2] + r;
		}		
	}

	r = 0.0;
	for(j=0; j < M; j++){
		r = r + Theta0[j];
    }

    if(fixedSigma){

        double mixingRatio = 0.1;

        Sigma0 = 1.0;
        for(k=0; k < K; k++){
            Sigma[k] = mixingRatio;
        }
    }

    if(fixedSpontaneous){


        for(k=0; k < K; k++){

            Omega0[k] = 0.001;
        }
    }
}



void TopicModel::randomizeOmega(){

    int i, k;

//	srand(time(NULL));

	// There is only one Sigma0
	Sigma0 = ((double) rand()) / ((double) RAND_MAX);
	for(k = 0; k < K; k++){


		Sigma[k] = ((double) rand()) / ((double) RAND_MAX);
		P0[k] = ((double) rand()) / ((double) RAND_MAX);

		Omega0[k] = ((double) rand()) / ((double) RAND_MAX);
		for(i = 0; i < K; i++){
			
            Omega[k][i] = ((double) rand()) / ((double) RAND_MAX);
		}
    }
}

/**********************************************************/
// All of the likelihoods are being switched to log
// likelihoods. This is slower, but should be more
// robust
/**********************************************************/

double TopicModel::getAlpha(int b, int t, int position, bool flip_positive){


	double alpha;

	if(t > 0){	
        alpha = Aleph[b][position][t-1] / (1.0 - Aleph[b][position][t-1]);
	}else{
		alpha = P0[position] / (1.0 - P0[position]);
	}

	if(!flip_positive){
		alpha = 1.0 / alpha;
	}

	if((alpha < 0) || (isnan(alpha))){
		cout << "Sampling Error : Alpha\n";
		cin >> alpha;
	}


	return log(alpha);

}



// Continue Refactoring from here
// -------------------------------------------------------------

// This represents the term P(Z^t+1 | Z^t)
// When moving to a batch zchain we'll just have to be sure
// to grab the right Z_t
double TopicModel::getBeta(int b, int t, int position, bool flip_positive, vector < vector < bool > > &Z_t){

	double beta = 1.0;
	double logbeta = 0.0;
	int k;
	
    if(t == (T[b]-1)){
		return 0.0;
	}
	
	if(flip_positive){


		for(k=0; k< K; k++){

            if(Z_t[t+1][k]){
                logbeta = logbeta + log((Omega[k][position] + (1.0 - Omega[k][position]) * Aleph[b][k][t]) / Aleph[b][k][t]);
			}else{
                logbeta = logbeta + log(1.0 - Omega[k][position]);
			}
		}

	}else{


		for(k=0; k< K; k++){

            if(Z_t[t+1][k]){
                logbeta = logbeta + log((Aleph[b][k][t] - Omega[k][position]) / ((1.0 - Omega[k][position]) * Aleph[b][k][t]));
			}else{
                logbeta = logbeta - log(1.0 - Omega[k][position]);
			}
		}
	}


	if((beta < 0) || (isnan(logbeta))){
		cout << "Sampling Error : Beta\n";
		cin >> beta;
	}

	return logbeta;
}

// I think this does something like return the log of the gamma term and all the sampling is
// done on a logarithmic scale. Probabilities are fixed in the chain normalization
double TopicModel::getGamma(int b, int t, int position, bool flip_positive, VideoData *video){

	double gamma = 1.0;
    int j;
	double* X;
	double* N;
	double multiplier, base;

	/********************************************************/
	// This is the new logarithmic calculator (slower)
	/********************************************************/

	gamma = 0.0;
	if(flip_positive){	
		
		if(t > 0){

            multiplier = (Nu[b][t] / (Nu[b][t] + Sigma[position]));
			for(j=0; j<M; j++){
			
                if(video->X[t][j] > 0){

                    base = multiplier * ((Tau[b][j][t] + Sigma[position] * Theta[position][j]) / Tau[b][j][t]);
                    gamma = gamma + log(base) * video->X[t][j];
                }
			}
		}				

	}else{

		if(t > 0){

            multiplier = (Nu[b][t] / (Nu[b][t] - Sigma[position]));
			for(j=0; j<M; j++){
			
                if(video->X[t][j] > 0){

                    base = multiplier * ((Tau[b][j][t] - Sigma[position] * Theta[position][j]) / Tau[b][j][t]);
                    gamma = gamma + log(base) * video->X[t][j];

				}
			}
		}

	}


    /********************************************************/
    // This is the old multiplicative calculator (faster)
    /********************************************************/


/*
    if(flip_positive){

        if(t > 0){

//			gamma = gamma * pow((Nu[t] / (Nu[t] + Sigma[position])), N[t]);
            multiplier = (Nu[t] / (Nu[t] + Sigma[position]));
            for(j=0; j<M; j++){

                if(X[t*M + j] > 0){

//					cout << "X : " << X[t*M+j] << "\n";
//					cout << "Power : " << pow( (Tau[t * M + j] + Sigma[position] * Theta[position*M + j]) / Tau[t * M + j], X[t * M + j]) << "\n";
//					cout << "Tau : " << Tau[t * M + j] << "\n";

                    base = multiplier * ((Tau[t * M + j] + Sigma[position] * Theta[position * M + j]) / Tau[t * M + j]);
                    gamma = gamma * pow( base, X[t * M + j]);

                    cout << "Gamma : " << gamma << "\n";

                }
                cin >> a;
            }
        }

    }else{

        if(t > 0){

            multiplier = (Nu[t] / (Nu[t] - Sigma[position]));
            for(j=0; j<M; j++){

                if(X[t*M + j] > 0){
//					cout << "X : " << X[t*M+j] << "\n";

                    base = multiplier * ((Tau[t * M + j] - Sigma[position] * Theta[position * M + j]) / Tau[t * M + j]);
                    gamma = gamma * pow( base, X[t * M + j]);
//					cout << "Power : " << pow( (Tau[t * M + j] - Sigma[position] * Theta[position*M + j]) / Tau[t * M + j], X[t * M + j]) << "\n";
//					cout << "Tau : " << Tau[t * M + j] << "\n";
                    cout << "Gamma : " << gamma << "\n";
                }
                cin >> a;
            }
        }

    }

*/

	if((isnan(gamma)) || (isinf(gamma))){
		cout << "Sampling Error : Gamma  =  "  << gamma << "\n";
		cin >> gamma;
	}

	return gamma;

}


//*****************************************************************
// Probability Update Functions
//*****************************************************************
void TopicModel::setProbs(vector< vector < vector < bool > > > &Z){

    int i;

    for(i = 0; i < Z.size(); i++){
        setAleph(i, Z[i]);
        setTau(i, Z[i]);
        setNu(i, Z[i]);
    }

}

void TopicModel::setAleph(vector < vector < vector < bool > > > &Z){

    for(int b=0; b < B; b++){
        setAleph(b, Z[b]);
    }
}

void TopicModel::setTau(vector < vector < vector < bool > > > &Z){

    for(int b=0; b < B; b++){
        setTau(b, Z[b]);
    }
}

void TopicModel::setNu(vector < vector < vector < bool > > > &Z){

    for(int b=0; b < B; b++){
        setNu(b, Z[b]);
    }
}

void TopicModel::setAleph(int b, vector < vector < bool > > &Z){


    int k, t, i;

    for(k=0; k < K; k++){
        for(t=0; t < T[b]; t++){

            Aleph[b][k][t] = 1.0 - Omega0[k];
            for(i = 0; i < K; i++){

                if(Z[t][i]){
                    Aleph[b][k][t] = Aleph[b][k][t] * (1.0 - Omega[k][i]);
                }
            }
            Aleph[b][k][t] = 1.0 - Aleph[b][k][t];
        }

    }
}


void TopicModel::setTau(int b, vector < vector < bool > > &Z){


    int j, t, k;

    for(t=0; t < T[b]; t++){
        for(j=0; j < M; j++){

            Tau[b][j][t] = Sigma0 * Theta0[j];
            for(k = 0; k < K; k++){

                if(Z[t][k]){
                    Tau[b][j][t] = Tau[b][j][t] + Sigma[k] * Theta[k][j];
                }
            }

            if(Tau[b][j][t] < 0.00000000000000000000001){
                cout << "Sigma0 * Theta0[ " << j << "] = " << Sigma0 * Theta0[j] << "\n";
                cout << "Sigma0 = " << Sigma0 << "\n";
                cout << "Theta0[ " << j << "] = " << Theta0[j] << "\n";
                for(k = 0; k < K; k++){

                    if(Z[t][k]){
                        cout << "Adding " << Sigma[k] * Theta[k][j] << "\n";
                    }
                }
                cin >> j;
            }
        }
    }
}


void TopicModel::setNu(int b, vector < vector < bool > > &Z){


    int j, t, k;

    for(t=0; t < T[b]; t++){

        Nu[b][t] = Sigma0;
        for(k = 0; k < K; k++){

            if(Z[t][k]){
                Nu[b][t] = Nu[b][t] + Sigma[k];
            }
        }
        if(Nu[b][t] < 0.000000000001 || isnan(Nu[b][t]) || isinf(Nu[b][t])){
            cout << "Nu error [ " << t << " ] = " << Nu[b][t] << "\n";
            cin >> j;
        }

    }
}


void TopicModel::updateProbs(int batch, int time, int topic, bool adding){


    int j, k;
    int b = batch;

	if(adding){

        Nu[b][time] = Nu[b][time] + Sigma[topic];
		for(k=0; k < K; k++){
            Aleph[b][k][time] = Omega[k][topic] + (1.0 - Omega[k][topic]) * Aleph[b][k][time];
		}
		for(j=0; j < M; j++){
            Tau[b][j][time] = Tau[b][j][time] + Sigma[topic] * Theta[topic][j];
		}
	}else{

        Nu[b][time] = Nu[b][time] - Sigma[topic];
		for(k=0; k < K; k++){
            Aleph[b][k][time] = (Aleph[b][k][time] - Omega[k][topic]) / (1.0 - Omega[k][topic]);
		}
		for(j=0; j < M; j++){
            Tau[b][j][time] = Tau[b][j][time] - Sigma[topic] * Theta[topic][j];
		}
	}
}

//************************************************
// Likelihood Functions
//************************************************
pair < double, double > TopicModel::log_likelihood(Z_Chain &chain, DataCollection &videos){

    int i, j, k;
    int k0, b0, t0;
    int t;
    int a, b;
    int count;
    double causal_total;
    double visual_total;
    double causal_likelihood = 0.0;
    double visual_likelihood = 0.0;
    vector < vector < vector < bool > > > Z;


    Z.resize(B);
    for(b=0; b < B; b++){

        t = T[b];
        Z[b].resize(t);
        for(int i=0; i < t; i++){

            Z[b][i].resize(K);
        }
    }

    for(b=0; b < B; b++){
        for(t=0; t < T[b]; t++){
            for(k = 0; k < K; k++){

                Z[b][t][k] = chain.ZStart[b][t][k];
            }
        }
    }

    setProbs(Z);

    for(i=chain.indexStart; i< (chain.hillSamples + chain.indexStart); i++){

        causal_total = 0.0;
        visual_total = 0.0;

        b0 = chain.b_sample[i];
        t0 = chain.t_sample[i];
        k0 = chain.k_sample[i];


        if(Z[b0][t0][k0]){

            Z[b0][t0][k0] = false;
            updateProbs(b0, t0, k0, false);
        }else{

            Z[b0][t0][k0] = true;
            updateProbs(b0, t0, k0, true);
        }

        count = 1;

        // Do the P0 terms

        for(b=0; b < B; b++){

            for(k=0; k < K; k++){

                if(Z[b][0][k]){
                    causal_total = causal_total + log( P0[k] );
                }else{
                    causal_total = causal_total + log( 1.0 - P0[k] );
                }
            }

            for(t=1; t < T[b]; t++){

                // Do the Aleph terms
                for(k=0; k < K; k++){

                    if(Z[b][t][k]){
                        causal_total = causal_total + log( Aleph[b][k][t] );
                    }else{
                        causal_total = causal_total + log( 1.0 - Aleph[b][k][t] );
                    }
                }
                // Do the Tau terms
                for(j=0; j < M; j++){

                    if(videos.getData(b)->X[t][j] > 0){

                        visual_total = visual_total + videos.getData(b)->X[t][j] * log( Tau[b][j][t] );
                    }
                }

                if(videos.getData(b)->N[t] > 0){

                    visual_total = visual_total - videos.getData(b)->N[t] * log( Nu[b][t] );
                }

            }

            causal_likelihood = causal_likelihood + chain.probs[i] * causal_total;
            visual_likelihood = visual_likelihood + chain.probs[i] * visual_total;

            if(isnan(causal_likelihood) || isnan(visual_likelihood)){
                cout << "Is Nan\n";
                cin >> a;
            }
        }

    }

    ll.first = causal_likelihood;
    ll.second = visual_likelihood;
    return ll;
}


vector < double > TopicModel::log_linear_likelihood(Z_Chain &chain, DataCollection &videos){


    int i, j, k;
    int k0, b0, t0;
    int t;
    int a, b;
    int count;
    double total;
    double likelihood = 0.0;
    vector < vector < vector < bool > > > Z;
    vector < double > likelihoods;
    double max;

    likelihoods.resize(chain.S);


    Z.resize(B);
    for(b=0; b < B; b++){

        t = T[b];
        Z[b].resize(t);
        for(int i=0; i < t; i++){

            Z[b][i].resize(K);
        }
    }

    for(b=0; b < B; b++){
        for(t=0; t < T[b]; t++){
            for(k = 0; k < K; k++){

                Z[b][t][k] = chain.ZStart[b][t][k];
            }
        }
    }


    setProbs(Z);

    for(i=0; i < chain.S; i++){

        total = 0.0;


        b0 = chain.b_sample[i];
        t0 = chain.t_sample[i];
        k0 = chain.k_sample[i];


        if(Z[b0][t0][k0]){

            Z[b0][t0][k0] = false;
            updateProbs(b0, t0, k0, false);
        }else{

            Z[b0][t0][k0] = true;
            updateProbs(b0, t0, k0, true);
        }

        count = 1;

        // Do the P0 terms
        for(b=0; b < B; b++){

            for(k=0; k < K; k++){

                if(Z[b][0][k]){
                    total = total + log( P0[k] );
                }else{
                    total = total + log( 1.0 - P0[k] );
                }
            }

            for(t=1; t < T[b]; t++){

                // Do the Aleph terms
                for(k=0; k < K; k++){

                    if(Z[b][t][k]){
                        total = total + log( Aleph[b][k][t] );
                    }else{
                        total = total + log( 1.0 - Aleph[b][k][t] );
                    }
                }
                // Do the Tau terms
                for(j=0; j < M; j++){

                    if(videos.getData(b)->X[t][j] > 0){

                        total = total + videos.getData(b)->X[t][j] * log( Tau[b][j][t] );
                    }
                }

                if(videos.getData(b)->N[t] > 0){

                    total = total - videos.getData(b)->N[t] * log( Nu[b][t] );
                }
            }

            likelihood = likelihood + chain.probs[i] * total;

            likelihoods[i] = total;
            if(i == 0){
                max = total;
            }
            if(likelihoods[i] > max){
                max = likelihoods[i];
            }

            if(isnan(likelihood)){
                cout << "Is Nan\n";
                cin >> a;
            }
        }

    }

//    ll = likelihood;
    return likelihoods;
}

pair < double, double > TopicModel::getLL(){

    return ll;
}

//*******************************************
// Fixed value setters
//*******************************************
void TopicModel::setFixedOmega(bool t){

    fixedOmega = t;
}


void TopicModel::setFixedTheta(bool t){

    fixedTheta = t;
}

void TopicModel::setFixedSigma(bool t){

    fixedSigma = t;
}

void TopicModel::setFixedSpontaneous(bool t){

    fixedSpontaneous = t;
}

//*************************************************************//
// Some helper summary functions
//*************************************************************//

double TopicModel::getAvgSigma(){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    average = Sigma0;
    N = N + 1;
    for(k = 0; k < K; k++){


        average = average + Sigma[k];
        N = N + 1;
        average = average + P0[k];
        N = N + 1;
    }
    return (average / N);
}


double TopicModel::getAvgOmega(){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    for(k = 0; k < K; k++){

        average = average + Omega0[k];
        N = N + 1;
        for(i = 0; i < K; i++){

            average = average + Omega[k][i];
            N = N + 1;
        }
    }
    return (average / N);
}


double TopicModel::getAvgTheta0(){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // Now do the Theta0 terms separately
    for(j = 0; j < M; j++){

        average = average + Theta0[j];
        N = N + 1;
    }
    return (average / N);
}




double TopicModel::getAvgTheta(){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    for(k = 0; k < K; k++){

        for(j = 0; j < M; j++){

            average = average + Theta[k][j];
            N = N + 1;
        }
    }
    return (average / N);
}




double TopicModel::getPercentSigmaOver(double x){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    if(Sigma0 > x){
        average = average + 1;
    }
    N = N + 1;
    for(k = 0; k < K; k++){


        if(Sigma[k] > x){
            average = average + 1;
        }
        N = N + 1;
        if(P0[k] > x){
            average = average + 1;
        }
        N = N + 1;
    }
    return (average / N);
}


double TopicModel::getPercentOmegaOver(double x){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    for(k = 0; k < K; k++){

        if(Omega0[k] > x){
            average = average + 1;
        }
        N = N + 1;
        for(i = 0; i < K; i++){

            if(Omega[k][i] > x){
                average = average + 1;
            }
            N = N + 1;
        }
    }
    return (average / N);
}


double TopicModel::getPercentTheta0Over(double x){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // Now do the Theta0 terms separately
    for(j = 0; j < M; j++){

        if(Theta0[j] > x){
            average = average + 1;
        }
        N = N + 1;
    }
    return (average / N);
}


double TopicModel::getPercentThetaOver(double x){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    for(k = 0; k < K; k++){

        for(j = 0; j < M; j++){

            if(Theta[k][j] > x){
                average = average + 1;
            }
            N = N + 1;
        }
    }
    return (average / N);
}



double TopicModel::getPercentSigmaUnder(double x){

    int k;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    if(Sigma0 < x){
        average = average + 1;
    }
    N = N + 1;
    for(k = 0; k < K; k++){


        if(Sigma[k] < x){
            average = average + 1;
        }
        N = N + 1;
        if(P0[k] < x){
            average = average + 1;
        }
        N = N + 1;
    }
    return (average / N);
}


double TopicModel::getPercentOmegaUnder(double x){

    int i, k;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    for(k = 0; k < K; k++){

        if(Omega0[k] < x){
            average = average + 1;
        }
        N = N + 1;
        for(i = 0; i < K; i++){

            if(Omega[k][i] < x){
                average = average + 1;
            }
            N = N + 1;
        }
    }
    return (average / N);
}


double TopicModel::getPercentThetaUnder(double x){

    int i, j, k;
    int a;

    double average = 0.0;
    double N = 0.0;

    // There is only one Sigma0
    for(k = 0; k < K; k++){

        for(j = 0; j < M; j++){

            if(Theta[k][j] < x){
                average = average + 1;
            }
            N = N + 1;
        }
    }

    // Now do the Theta0 terms separately
    for(j = 0; j < M; j++){

        if(Theta0[j] < x){
            average = average + 1;
        }
        N = N + 1;
    }
    return (average / N);
}

int TopicModel::getK(){

    return K;
}

int TopicModel::getM(){

    return M;
}

int TopicModel::getB(){

    return B;
}
