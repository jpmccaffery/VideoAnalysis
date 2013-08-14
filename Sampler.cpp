#include "Sampler.h"
#include "stdlib.h"
#include "iostream"
#include "time.h"
#include "QDebug"
#include "vector"
#include "datacollection.h"

using namespace std;

Sampler::Sampler(int numberSamples) :
    QObject(NULL)
{

    K = 0;
    S = numberSamples;
    B = 0;

    T.resize(B);
    Z_temp.resize(B);

    chain = Z_Chain(K, S, T);
}

Sampler::~Sampler(){

}

void Sampler::initialize(TopicModel &model, DataCollection &videos){

    bool needToUpdate = false;

    if(K != model.K || B != videos.getBatchSize()){

        needToUpdate = true;
    }else{

        for(int b=0; b < B; b++){

            if(T[b] != videos.getData(b)->T){
                needToUpdate = true;
                break;
            }
        }
    }

    if(needToUpdate){

        K = model.K;
        B = videos.getBatchSize();

        T.resize(B);
        for(int b=0; b < B; b++){

            T[b] = videos.getData(b)->T;
        }

        Z_temp.resize(B);
        for(int b=0; b < B; b++){
            Z_temp[b].resize(T[b]);
            for(int t=0; t < T[b]; t++){

                Z_temp[b][t].resize(K);
                for(int k=0; k < K; k++){
                    Z_temp[b][t][k] = false;
                }
            }
        }
    }
}

void Sampler::sample(TopicModel &model, DataCollection &videos){


    int current_b = 0;
    int current_k = 0;
	int current_t = 0;
	int number_sampled = 0;
    int t;
    int count = 0;
	double p, prand;
	double accepted = 0.0;
	double rejected = 0.0;
	double lag_time = clock();
    bool stuck = false;

	int total_rejected = 0;
	int total_accepted = 0;
	int total_timeout = 0;

    int tickStepSize = S / 100;

    int total_consecutive_timeout = 0;
    int total_consecutive_nonmax = 0;

    //************************************************
    // Begin Sampling Process
    //************************************************

    this->initialize(model, videos);
    chain.initialize(model.K, S, T);

    for(int b=0; b < B; b++){
        for(int t=0; t < T[b]; t++){
            for(int k=0; k < K; k++){

                Z_temp[b][t][k] = chain.Z0[b][t][k];
            }
        }
    }


    model.setAleph(Z_temp);
    model.setTau(Z_temp);
    model.setNu(Z_temp);

	cout << "Sampling\n";

    emit sampleTickMax(S);
    emit sampleTick(0);

    count = -1;
	while(number_sampled < S){


		if((clock() - lag_time) > max_wait){
			stuck = true;
		}else{
			stuck = false;
		}

        if(current_t == 0 && current_k == 0 && current_b == 0){
			count++;
		}
        if(Z_temp[current_b][current_t][current_k]){

            p = model.getAlpha(current_b, current_t, current_k, false);
            p = p + (model.getBeta(current_b, current_t, current_k, false, Z_temp[current_b]));
            p = p + (model.getGamma(current_b, current_t, current_k, false, videos.getData(current_b)));

            prand = ((double) rand()) / ((double) RAND_MAX);

			if(isnan(p) || isinf(p)){
				cout << "P is nan\n";
                cout << "Alpha : " << model.getAlpha(current_b, current_t, current_k, false) << "\n";
                cout << "Beta : " << (model.getBeta(current_b, current_t, current_k, false, Z_temp[current_b])) << "\n";
                cout << "Gamma : " << (model.getGamma(current_b, current_t, current_k, false, videos.getData(current_b))) << "\n";
			}


			if(prand < exp(p) || stuck){


				// If we got this legitimately, reset the wait clock
                Z_temp[current_b][current_t][current_k] = false;

                chain.k_sample[number_sampled] = current_k;
                chain.t_sample[number_sampled] = current_t;
                chain.b_sample[number_sampled] = current_b;

                if(!chain.checkRepeats(number_sampled)){

					if(stuck){
						total_timeout++;
                        total_consecutive_timeout++;
					}else{
						total_accepted++;
                        total_consecutive_timeout = 0;
					}

                    chain.k_sample[number_sampled] = current_k;
                    chain.t_sample[number_sampled] = current_t;
                    chain.b_sample[number_sampled] = current_b;
                    if(number_sampled > 0){
                        chain.probs[number_sampled] = chain.probs[number_sampled-1] + p;
					}else{
                        chain.probs[number_sampled] = 0;
					}

                    if(chain.probs[number_sampled] > chain.max_prob){
                        chain.max_prob = chain.probs[number_sampled];
                        total_consecutive_nonmax = 0;
                    }else{
                        total_consecutive_nonmax++;
                    }

                    model.updateProbs(current_b, current_t, current_k, false);

					number_sampled++;
					accepted = accepted + 1;

                    if(number_sampled % tickStepSize == 0){

                        emit sampleTick(number_sampled);
                    }

                    lag_time = clock();

				}else{
					rejected = rejected + 1;
                    Z_temp[current_b][current_t][current_k] = true;
				}
			}else{

				rejected = rejected + 1;
				total_rejected++;
			}

            current_k++;
            if(current_k == K){
                current_k = 0;
                current_t++;
                if(current_t == T[current_b]){
                    current_t = 0;
                    current_b++;
                    if(current_b == B){
                        current_b = 0;
                    }
                }
            }

		}else{
					
            p = model.getAlpha(current_b, current_t, current_k, true);
            p = p + (model.getBeta(current_b, current_t, current_k, true, Z_temp[current_b]));
            p = p + (model.getGamma(current_b, current_t, current_k, true, videos.getData(current_b)));
			prand = ((double) rand()) / ((double) RAND_MAX);

			if(isnan(p) || isinf(p)){
				cout << "P is nan\n";
                cout << "Alpha : " << model.getAlpha(current_b, current_t, current_k, false) << "\n";
                cout << "Beta : " << (model.getBeta(current_b, current_t, current_k, false, Z_temp[current_b])) << "\n";
                cout << "Gamma : " << (model.getGamma(current_b, current_t, current_k, false, videos.getData(current_b))) << "\n";

			}


			if(prand < exp(p) || stuck){

				// If we got this legitimately, reset the wait clock
                Z_temp[current_b][current_t][current_k] = true;

                chain.k_sample[number_sampled] = current_k;
                chain.t_sample[number_sampled] = current_t;
                chain.b_sample[number_sampled] = current_b;

                if(!chain.checkRepeats(number_sampled)){

					if(stuck){
                        total_timeout++;
                        total_consecutive_timeout++;
					}else{
						total_accepted++;
                        total_consecutive_timeout = 0;
					}

                    chain.k_sample[number_sampled] = current_k;
                    chain.t_sample[number_sampled] = current_t;
                    chain.b_sample[number_sampled] = current_b;

					if(number_sampled > 0){
                        chain.probs[number_sampled] = chain.probs[number_sampled-1] + p;
					}else{
                        chain.probs[number_sampled] = 0;
					}
			
                    if(chain.probs[number_sampled] > chain.max_prob){
                        chain.max_prob = chain.probs[number_sampled];
                        total_consecutive_nonmax = 0;
                    }else{
                        total_consecutive_nonmax++;
                    }

                    model.updateProbs(current_b, current_t, current_k, true);

					number_sampled++;
					accepted = accepted + 1;

                    if(number_sampled % tickStepSize == 0){

                        emit sampleTick(number_sampled);
                    }

                    lag_time = clock();

				}else{

                    Z_temp[current_b][current_t][current_k] = false;
					rejected = rejected + 1;
				}
	
			}else{
				rejected = rejected + 1;
				total_rejected++;
			}

            current_k++;
            if(current_k == K){
                current_k = 0;
                current_t++;
                if(current_t == T[current_b]){
                    current_t = 0;
                    current_b++;
                    if(current_b == B){
                        current_b = 0;
                    }
                }
            }
		}

        // Check to see if it's completely stalled. In this case the last tickstepsize are probably near zero
        if(total_consecutive_timeout > tickStepSize || total_consecutive_nonmax > tickStepSize){

            for(int ka = number_sampled; ka < S; ka++){

                chain.k_sample[ka] = 0;
                chain.t_sample[ka] = 0;
                chain.b_sample[ka] = 0;
                chain.probs[ka] = -400 + chain.max_prob;
            }

            emit sampleTick(-1);
            break;
        }
    }

	// Now the normalize function also converts from log to absolute likelihoods

    chain.normalize();
}
