#include "Z_Chain.h"
#include <stdlib.h>
#include "iostream"
#include <cmath>
#include "VideoData.h"
#include "datacollection.h"
#include <QDebug>

using namespace std;

Z_Chain::Z_Chain(int topics, int samples, vector < int > &localTimes){

    K = topics;
    S = samples;
    hillSamples = BASE_HILLSAMPLES;
    B = localTimes.size();

    indexStart = 0;
    max_prob = 0.0;
    max_index = 0;

    b_sample.resize(S);
    k_sample.resize(S);
    t_sample.resize(S);
    probs.resize(S);

    T.resize(B);

    Z0.resize(B);
    ZStart.resize(B);

    for(int b=0; b < B; b++){

        T[b] = localTimes[b];
        Z0[b].resize(T[b]);
        ZStart[b].resize(T[b]);

        for(int t=0; t < T[b]; t++){

            Z0[b].resize(K);
            ZStart[b].resize(K);
        }
    }

    randomize();
}

Z_Chain::Z_Chain(){


}

void Z_Chain::initialize(int topics, int samples, vector < int > &localTimes){

    bool needToResize = false;

    if(K != topics || S != samples || localTimes.size() != B){

        needToResize = true;
    }else{

        for(int b=0; b < B; b++){

            if(T[b] != localTimes[b]){

                needToResize = true;
                break;
            }
        }
    }

    if(needToResize){

        K = topics;
        S = samples;
        B = localTimes.size();

        indexStart = 0;
        max_prob = 0.0;
        max_index = 0;

        b_sample.resize(S);
        k_sample.resize(S);
        t_sample.resize(S);
        probs.resize(S);

        T.resize(B);

        Z0.resize(B);
        ZStart.resize(B);

        for(int b=0; b < B; b++){

            T[b] = localTimes[b];
            Z0[b].resize(T[b]);
            ZStart[b].resize(T[b]);

            for(int t=0; t < T[b]; t++){

                Z0[b][t].resize(K);
                ZStart[b][t].resize(K);
            }
        }
    }

    randomize();
}

Z_Chain::~Z_Chain(){

}

bool Z_Chain::checkRepeats(int number_sampled){

    int num_off = 1;

    vector< vector < vector < bool > > > Z_temp;
    Z_temp.resize(B);


    for(int i=0; i < B; i++){

        Z_temp[i].resize(T[i]);
        for(int t=0; t < T[i]; t++){

            Z_temp[i][t].resize(K);

            for(int k=0; k < K; k++){

                Z_temp[i][t][k] = false;
            }
        }
    }


	if(number_sampled > 1){

        Z_temp[b_sample[number_sampled]][t_sample[number_sampled]][k_sample[number_sampled]] = true;

        for(int i=(number_sampled-1); i >= 0; i--){

            if(Z_temp[b_sample[i]][t_sample[i]][k_sample[i]]){
				num_off--;
				if(num_off == 0){

					return true;
				}
                Z_temp[b_sample[i]][t_sample[i]][k_sample[i]] = false;
			}else{
				num_off++;
                Z_temp[b_sample[i]][t_sample[i]][k_sample[i]] = true;
			}
		}
    }

	return false;

}



void Z_Chain::randomize(){

    int k, t, b;
    indexStart = 0;

    for(b=0; b < B; b++){
        for(t=0; t < T[b]; t++){
            for(k=0; k < K; k++){

                if(((double) rand()) / ((double) RAND_MAX) > 0.5){
                    Z0[b][t][k] = true;
                    ZStart[b][t][k] = true;
                }else{
                    Z0[b][t][k] = false;
                    ZStart[b][t][k] = false;
                }
            }
        }
    }
}




void Z_Chain::getBest(int batchNumber, vector < vector < bool > > &returnVector){

    int b, t, k;
    int s = 0;

    returnVector.resize(T[batchNumber]);
    for(int t=0; t < T[batchNumber]; t++){

        returnVector[t].resize(K);
        for(int k=0; k < K; k++){

            returnVector[t][k] = Z0[batchNumber][t][k];
        }
    }


    s = 0;
    while(s <= max_index){

        b = b_sample[s];
        t = t_sample[s];
        k = k_sample[s];

        if(b == batchNumber){
            returnVector[t][k] = !returnVector[t][k];
        }
        s++;
    }
}


void Z_Chain::setZStart(){


    int b, t, k;
    int s = 0;

    for(int b=0; b < B; b++){
        for(t=0; t < T[b]; t++){
            for(k=0; k < K; k++){

                ZStart[b][t][k] = Z0[b][t][k];
            }
        }
    }

    s = 0;
    while(s < indexStart){

        b = b_sample[s];
        t = t_sample[s];
        k = k_sample[s];

        ZStart[b][t][k] = !ZStart[b][t][k];
        s++;
    }
}


void Z_Chain::normalize(){

	int s;
	double total = 0.0;
	double total_nzero = 0.0;

    double bestChain = 0;
    double currentChain = 0;


	for(s = 0; s < S; s++){
		probs[s] = probs[s] - max_prob;
	}
	max_prob = 0.0;

	for(s = 0; s < S; s++){
		if(probs[s] > -250.0){
			probs[s] = exp(probs[s]);
		}else{
			probs[s] = 0.0;
		}
    }
    for(s = 0; s < S; s++){
        total = total + probs[s];
    }


	if(total < 0.00001){
		cout << "Sampled for zero probability!\n";
		cout << "Specifically : " << total << "\n";
		cin >> s;
	}
	cout << "The total is " << total << "\n";

    for(s = 0; s < S; s++){
        probs[s] = probs[s] / total;
    }

	// Find the max probability
	max_prob = 0.0;
	max_index = 0;
	for(s=0; s < S; s++){
		if(probs[s] > max_prob){
			max_prob = probs[s];
			max_index = s;
		}
	}

	// Zero out all the probabilities that are less than 1 / (10000 * S) of max
    // Now lets just take the top 100 or rather things greater than (1/100) * MAX

    double maxSize = 10000.0 * S;
    total_nzero = 0;

	for(s=0; s < S; s++){
        if(maxSize * probs[s] > max_prob && false){
			probs[s] = 0.0;
			total_nzero = total_nzero + 1.0;
        }
	}
	total_nzero = S - total_nzero;
    qDebug() << "Percent nonzero : " << total_nzero * 100.0 / ((double) S) << "\n";
    qDebug() << "Number nonzero : " << total_nzero << "\n";

    // Then renormalize

    total = 0.0;
    for(s = 0; s < S; s++){

        total = total + probs[s];

        currentChain = currentChain + probs[s];
        if(s >= hillSamples)
            currentChain = currentChain - probs[s-hillSamples];

        if(s >= hillSamples-1){

            if(currentChain > bestChain){

                bestChain = currentChain;
                indexStart = s - hillSamples + 1;
            }
        }
    }

    max_prob = 0;
    for(s = 0; s < S; s++){
        probs[s] = probs[s] / total;
        if(probs[s] > max_prob){
            max_prob = probs[s];
            max_index = s;
        }
        qDebug() << s << " : " << probs[s];

    }

    qDebug() << "index start : " << indexStart << "Totaling : " << bestChain;
    qDebug() << "Max prob ( " << max_index << " ) : " << max_prob;


    setZStart();
}
