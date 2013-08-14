#include "HillClimber.h"
#include <cmath>
#include <stdlib.h>
#include "QDebug"
#include "datacollection.h"

using namespace std;

HillClimber::HillClimber()
{

    K = -1;
    M = -1;
    B = -1;
    T.resize(0);

    model = 0;
    videos = 0;

    fixedTheta = false;
    fixedOmega =false;
    fixedSigma = false;
    fixedSpontaneous = true;

    omega_rate = -1;
    theta_rate = -1;
    momentum = -1;

    del_term_omega.resize(0);
    del_term_theta.resize(0);
    del_term_sigma.resize(0);
    del_term_omega0.resize(0);
    del_term_theta0.resize(0);

    gradient_omega.resize(0);
    gradient_theta.resize(0);
    gradient_sigma.resize(0);
    gradient_omega0.resize(0);
    gradient_theta0.resize(0);
    gradient_P0.resize(0);

    last_omega.resize(0);
    last_theta.resize(0);
    last_sigma.resize(0);
    last_omega0.resize(0);
    last_theta0.resize(0);
    last_P0.resize(0);

}

void HillClimber::initialize(TopicModel *m, DataCollection *v){

    bool needToResize = false;

    if(model != m){
        model = m;
        needToResize = true;
    }
    if(videos != v){
        videos = v;
        needToResize = true;
    }

    // We only need to resize all the gradients if the parameters have changed
    if(M != model->getM() || K != model->getK()){
        needToResize = true;
    }
    if(B != videos->getBatchSize()){
        needToResize = true;
    }else{
        for(int i=0; i < B; i++){

            if(T[i] != videos->getData(i)->T){
                needToResize = true;
                break;
            }
        }
    }

    if(needToResize){

        M = model->getM();
        K = model->getK();
        B = videos->getBatchSize();
        T.resize(B);
        for(int i=0; i < B; i++){
            T[i] = videos->getData(i)->T;
        }


        Theta_MIN = (1.0 / (((double) M) * ((double) M) * ((double) M)));

        // Size the del_terms and gradients
        del_term_omega.resize(K);
        del_term_theta.resize(K);
        del_term_sigma.resize(K);
        del_term_omega0.resize(K);
        del_term_theta0.resize(M);
        for(int i=0; i < K; i++){
            del_term_omega[i].resize(K);
            del_term_theta[i].resize(M);
        }

        gradient_omega.resize(K);
        gradient_theta.resize(K);
        gradient_sigma.resize(K);
        gradient_omega0.resize(K);
        gradient_theta0.resize(M);
        gradient_P0.resize(K);
        for(int i=0; i < K; i++){
            gradient_omega[i].resize(K);
            gradient_theta[i].resize(M);
        }

        last_omega.resize(K);
        last_theta.resize(K);
        last_sigma.resize(K);
        last_omega0.resize(K);
        last_theta0.resize(M);
        last_P0.resize(K);
        for(int i=0; i < K; i++){
            last_omega[i].resize(K);
            last_theta[i].resize(M);
        }

        // Initialize all the values
        gradient_sigma0 = 0.0;
        last_sigma0 = 0.0;

        for(int i = 0; i < K; i++){
            for(int k = 0; k < K; k++){
                del_term_omega[i][k] = 0.0;
                gradient_omega[i][k] = 0.0;
                last_omega[i][k] = 0.0;
            }
            for(int j = 0; j < M; j++){
                del_term_theta[i][j] = 0.0;
                gradient_theta[i][j] = 0.0;
                last_theta[i][j] = 0.0;
            }
        }

        for(int k = 0; k < K; k++){
            del_term_sigma[k] = 0.0;
            del_term_omega0[k] = 0.0;
            gradient_sigma[k] = 0.0;
            gradient_omega0[k] = 0.0;
            gradient_P0[k] = 0.0;

            last_sigma[k] = 0.0;
            last_omega0[k] = 0.0;
            last_P0[k] = 0.0;
        }
        for(int j = 0; j < M; j++){
            del_term_theta0[j] = 0.0;
            gradient_theta0[j] = 0.0;
            last_theta0[j] = 0.0;

        }
    }
}


HillClimber::~HillClimber(){


    del_term_omega.resize(0);
    del_term_theta.resize(0);
    del_term_sigma.resize(0);
    del_term_omega0.resize(0);
    del_term_theta0.resize(0);

    gradient_omega.resize(0);
    gradient_theta.resize(0);
    gradient_sigma.resize(0);
    gradient_omega0.resize(0);
    gradient_theta0.resize(0);
    gradient_P0.resize(0);

    last_omega.resize(0);
    last_theta.resize(0);
    last_sigma.resize(0);
    last_omega0.resize(0);
    last_theta0.resize(0);
    last_P0.resize(0);
}

void HillClimber::setThetaRate(double rate){

	theta_rate = rate;
}

void HillClimber::setOmegaRate(double rate){

	omega_rate = rate;
}

void HillClimber::setMomentum(double rate){

	momentum = rate;
}


// This hill climber requires topic vector samples that change by
// exactly one variable each sample

void HillClimber::dynamic_climb_step(Z_Chain &chain){

    int i, j, k;
    int t, s, b;
    vector < int > passSample;
    vector < vector < vector < bool > > > Z;

    passSample.resize(3);

    qDebug() << "Begin dynamic step 1";
    qDebug() << "Begin bool copy";

    Z.resize(B);
    for(i=0; i < B; i++){

        Z[i].resize(T[i]);
        for(int j=0; j < T[i]; j++){

            Z[i][j].resize(K);
        }
//         Z[i] = new bool[T[i] * K];
    }

    double max = chain.S + 1 + K + 1 + K + 1;
    double total = 0;


    for(b=0; b < B; b++){
        for(t=0; t < T[b]; t++){
            for(k=0; k < K; k++){
                Z[b][t][k] = chain.ZStart[b][t][k];
            }
        }
    }

    qDebug() << "Begin set probabilities";

    model->setProbs(chain.ZStart);

//	cout << "3a sigma0 " << model->Sigma0 << "\n";

//	cout << "Omegas\n";


//	cout << gradient_omega << "\n";

    qDebug() << "Begin omegas";

    // This section sets the base gradients of the model parameters
    if(!fixedOmega){

        for(i=0; i<K; i++){

            for(k=0; k<K; k++){

                del_term_omega[i][k] = 0;
                gradient_omega[i][k] = 0.0;

                for(b=0; b < B; b++){
                    del_term_omega[i][k] = del_term_omega[i][k] + base_omega(chain.ZStart[b], i, k, b);
                }
            }
        }


        for(i=0; i<K; i++){

            del_term_omega0[i] = 0;
            gradient_omega0[i] = 0.0;

            for(b=0; b < B; b++){

                del_term_omega0[i] = del_term_omega0[i] + base_omega0(chain.ZStart[b], i, b);
            }
        }

    }

//	cout << "Thetas\n";

    qDebug() << "Begin thetas";

    if(!fixedTheta){

        for(k=0; k<K; k++){

            for(j=0; j < M; j++){

                del_term_theta[k][j] = 0;
                gradient_theta[k][j] = 0.0;

                for(b=0; b < B; b++){

                    del_term_theta[k][j] = del_term_theta[k][j] + base_theta(chain.ZStart[b], k, j, b);
                }
            }
        }

        for(j=0; j< M; j++){

            del_term_theta0[j] = 0;
            gradient_theta0[j] = 0.0;

            for(b=0; b < B; b++){

                del_term_theta0[j] = del_term_theta0[j] + base_theta0(j, b);
            }
        }
    }

//	cout << "Sigmas\n";

    if(!fixedSigma){

        for(k=0; k<K; k++){

            del_term_sigma[k] = 0;
            gradient_sigma[k] = 0.0;

            for(b=0; b < B; b++){

                del_term_sigma[k] = del_term_sigma[k] + base_sigma(chain.ZStart[b], k, b);
            }
        }

        del_term_sigma0 = 0;
        gradient_sigma0 = 0.0;

        for(b=0; b < B; b++){

            del_term_sigma0 = del_term_sigma0 + base_sigma0(b);
        }
    }
    // End of the gradient initializations

    total = total + 1;
    emit climbTick(total / max);


    // Next we have to iterate over the samples in the sampled Z_chain
    qDebug() << "Begin dynamic step 2";

    for(s=chain.indexStart; s< (chain.hillSamples + chain.indexStart); s++){

        b = chain.b_sample[s];

        passSample[0] = chain.t_sample[s];
        passSample[1] = chain.k_sample[s];
        passSample[2] = chain.b_sample[s];

        if(!fixedOmega){

            for(i=0; i<K; i++){

                for(k=0; k<K; k++){

                    del_term_omega[i][k] = del_term_omega[i][k] + delta_omega(Z[b], passSample, i, k);
                    gradient_omega[i][k] = gradient_omega[i][k] + chain.probs[s] * del_term_omega[i][k];
                }
            }

            for(i=0; i<K; i++){

                del_term_omega0[i] = del_term_omega0[i] + delta_omega0(Z[b], passSample, i);
                gradient_omega0[i] = gradient_omega0[i] + chain.probs[s] * del_term_omega0[i];
            }
        }

        if(!fixedTheta){

            for(k=0; k<K; k++){

                for(j=0; j < M; j++){

                    del_term_theta[k][j] = del_term_theta[k][j] + delta_theta(Z[b], passSample, k, j);
                    gradient_theta[k][j] = gradient_theta[k][j] + chain.probs[s] * del_term_theta[k][j];
                }
            }

            for(j=0; j < M; j++){

                del_term_theta0[j] = del_term_theta0[j] + delta_theta0(Z[b], passSample, j);
                gradient_theta0[j] = gradient_theta0[j] + chain.probs[s] * del_term_theta0[j];

                if(isnan(gradient_theta0[j]) || isnan(del_term_theta0[j])){
                    cout << "Gradient comp error\n";
                    cout << "s = " << s << "\n";
                    cout << "j = " << j << "\n";
                    cout << "chainprobs = " << chain.probs[s] << "\n";
                    cout << "delta = " << delta_theta0(Z[b], passSample, j) << "\n";
                    cin >> j;
                }
            }
        }

//		cout << "Sigmas\n";

        if(!fixedSigma){

            for(k=0; k<K; k++){

                del_term_sigma[k] = del_term_sigma[k] + delta_sigma(Z[b], passSample, k);
                gradient_sigma[k] = gradient_sigma[k] + chain.probs[s] * del_term_sigma[k];
            }

            del_term_sigma0 = del_term_sigma0 + delta_sigma0(Z[b], passSample);
            gradient_sigma0 = gradient_sigma0 + chain.probs[s] * del_term_sigma0;
        }


        // Finished updating. Now change samples

        if(Z[b][chain.t_sample[s]][chain.k_sample[s]]){

            Z[b][chain.t_sample[s]][chain.k_sample[s]] = false;
            model->updateProbs(chain.b_sample[s], chain.t_sample[s], chain.k_sample[s], false);
        }else{

            Z[b][chain.t_sample[s]][chain.k_sample[s]] = true;
            model->updateProbs(chain.b_sample[s], chain.t_sample[s], chain.k_sample[s], true);
        }


        // This is inefficient just to see if it works

/*

        if(!fixedOmega){
            for(i=0; i<K; i++){

                for(k=0; k<K; k++){

                    check1 = del_term_omega[i*K+k];
                    check2 = 0;
                    for(j=0; j < B; j++){
                        check2 = check2 + base_omega(Z[b], i, k, j);
                    }
                    if(check1 != check2){

                        qDebug() << "Dynamic programming error";
                        qDebug() << "Updtated : " << check1 << " vs " << check2;

                    }

//                    gradient_omega[i*K+k] = gradient_omega[i*K+k] - chain->probs[s] * del_term_omega[i*K+k];
//                    gradient_omega[i*K+k] = gradient_omega[i*K+k] + chain->probs[s] * base_omega(Z[b], i, k, b);
                }
            }

            if(!fixedSpontaneous){

                for(i=0; i<K; i++){


                    check1 = del_term_omega0[i];
                    check2 = 0;
                    for(j=0; j < B; j++){
                        check2 = check2 + base_omega0(Z[b], i, j);
                    }

                    if(check1 != check2){

                        qDebug() << "Dynamic programming error";

                    }

//                gradient_omega0[i] = gradient_omega0[i] - chain->probs[s] * del_term_omega0[i];
//                gradient_omega0[i] = gradient_omega0[i] + chain->probs[s] * base_omega0(Z[b], i, b);
            }
        }
    }

*/

    //****************************************************************************************
    // After setting the next value of Z[tk], we can optionally check
    // that the del_terms came out right by ma(*Nu)ally calculating them
    // with base_terms
    //****************************************************************************************

/*

        if(s == 0){
            cout << "\nChecking the Hill Climbing... (" << s << "/" << chain->S << ")\t";
        }
        errors = 0;
    //	cout << "Omegas\n";

        for(i=0; i<K; i++){

            for(k=0; k<K; k++){

                if(abs(del_term_omega[i*K+k] - base_omega(Z, i, k)) > 0.1){

                    cout << "\nError : sample " << s << "\n";
                    cout << "Omega : i = " << i << ", k = " << k << "\n";
                    cout << del_term_omega[i*K+k] << "\tExpected :\t" << base_omega(Z, i, k);
                    cout << "\nThe sample was t = " << chain->t_sample[s] << " and k = " << chain->k_sample[s];
                    cout << "\nThe term that made the error was : " << delta << "\n";
                    errors++;
                    chain->display(Z);
                    cin >> i;
                }
            }
        }


    //	cout << "Thetas\n";
        for(k=0; k<K; k++){

            for(j=0; j<(M-1); j++){

                if(abs(del_term_theta[k*M+j] - base_theta(Z, k, j)) > 0.1){

                    cout << "\nError : sample " << s << "\n";
                    cout << "Theta : k = " << k << ", j = " << j << "\n";
                    cout << del_term_theta[k*M+j] << "\tExpected :\t" << base_theta(Z, k, j);
                    errors++;
                }
            }
        }

    //	cout << "Sigmas\n";
        for(k=0; k<K; k++){

            if(abs(del_term_sigma[k] - base_sigma(Z, k)) > 0.1){

                cout << "\nError : sample " << s << "\n";
                cout << "Sigma : k = " << k << "\n";
                cout << del_term_sigma[k] << "\tExpected :\t" << base_sigma(Z, k);
                errors++;
            }
        }
        if(errors == 0){

            cout << "[OK]\n\n";
        }else{

            cout << "\n\n";
        }


*/

//		cout << "P0 Terms\n";
        for(k=0; k<K; k++){

            if(Z[b][0][k]){
                gradient_P0[k] = gradient_P0[k] + chain.probs[s] * (1.0 / model->P0[k]);
            }else{
                gradient_P0[k] = gradient_P0[k] - chain.probs[s] * (1.0 / (1.0 - model->P0[k]));
            }
        }

        total = total + 1;
        emit climbTick(total / max);
    }

//	cout << "3b sigma0 " << model->Sigma0 << "\n";
}


// This is slower than the dynamic climb step but requires less restrictions
// on the form of the topic vector samples

void HillClimber::climb_step(Z_Chain &chain){

    int i, j, k;
    int t, s, b;
    vector < vector < vector < bool > > > Z;
    vector < int > passSample;

    passSample.resize(3);

    Z.resize(B);
    for(i=0; i < B; i++){

        Z[i].resize(T[i]);
        for(int j=0; j < T[i]; j++){

            Z[i][j].resize(K);
        }
//         Z[i] = new bool[T[i] * K];
    }

    double max = chain.S + 1 + K + 1 + K + 1;
    double total = 0;


    for(b=0; b < B; b++){
        for(t=0; t < T[i]; t++){
            for(k=0; k < K; k++){
                Z[b][t][k] = chain.ZStart[b][t][k];
            }
        }
    }


    model->setProbs(chain.ZStart);

//	cout << "3a sigma0 " << model->Sigma0 << "\n";

//	cout << "Omegas\n";


//	cout << gradient_omega << "\n";

    if(!fixedOmega){

        for(i=0; i<K; i++){

            for(k=0; k<K; k++){

                del_term_omega[i][k] = 0;
                gradient_omega[i][k] = 0.0;
/*
                for(b=0; b < B; b++){
                    del_term_omega[i*K+k] = del_term_omega[i*K+k] + base_omega(chain->Z0[b], i, k, b);
                }

*/
            }
        }


        for(i=0; i<K; i++){

            del_term_omega0[i] = 0;
            gradient_omega0[i] = 0.0;
/*
            for(b=0; b < B; b++){

                del_term_omega0[i] = del_term_omega0[i] + base_omega0(chain->Z0[b], i, b);
            }
*/
        }

    }



//	cout << "Thetas\n";

    if(!fixedTheta){

        for(k=0; k<K; k++){

            for(j=0; j < M; j++){

                del_term_theta[k][j] = 0;
                gradient_theta[k][j] = 0.0;
/*
                for(b=0; b < B; b++){

                    del_term_theta[k*M+j] = del_term_theta[k*M+j] + base_theta(chain->Z0[b], k, j, b);
                }
*/
            }
        }

        for(j=0; j< M; j++){

            del_term_theta0[j] = 0;
            gradient_theta0[j] = 0.0;
/*
            for(b=0; b < B; b++){

                del_term_theta0[j] = del_term_theta0[j] + base_theta0(chain->Z0[b], j, b);
            }
*/
        }
    }

//	cout << "Sigmas\n";

    if(!fixedSigma){

        for(k=0; k<K; k++){

            del_term_sigma[k] = 0;
            gradient_sigma[k] = 0.0;
/*
            for(b=0; b < B; b++){

                del_term_sigma[k] = del_term_sigma[k] + base_sigma(chain->Z0[b], k, b);
            }
*/
        }

        del_term_sigma0 = 0;
        gradient_sigma0 = 0.0;
/*
        for(b=0; b < B; b++){

            del_term_sigma0 = del_term_sigma0 + base_sigma0(chain->Z0[b], b);
        }
*/
    }


    total = total + 1;
    emit climbTick(total / max);

    for(s=chain.indexStart; s< (chain.hillSamples + chain.indexStart); s++){


        b = chain.b_sample[s];

        passSample[0] = chain.t_sample[s];
        passSample[1] = chain.k_sample[s];
        passSample[2] = chain.b_sample[s];

        if(!fixedOmega){

            for(i=0; i<K; i++){

                for(k=0; k<K; k++){

                    for(b=0; b < B; b++){
                        gradient_omega[i][k] = gradient_omega[i][k] + base_omega(chain.ZStart[b], i, k, b);
                    }
                    del_term_omega[i][k] = del_term_omega[i][k] + delta_omega(Z[b], passSample, i, k);
                    gradient_omega[i][k] = gradient_omega[i][k] + chain.probs[s] * del_term_omega[i][k];
                }
            }

            for(i=0; i<K; i++){

                del_term_omega0[i] = del_term_omega0[i] + delta_omega0(Z[b], passSample, i);
                gradient_omega0[i] = gradient_omega0[i] + chain.probs[s] * del_term_omega0[i];
            }
        }

//		cout << "Thetas\n";

        if(!fixedTheta){

            for(k=0; k<K; k++){

                for(j=0; j < M; j++){

                    del_term_theta[k][j] = del_term_theta[k][j] + delta_theta(Z[b], passSample, k, j);
                    gradient_theta[k][j] = gradient_theta[k][j] + chain.probs[s] * del_term_theta[k][j];
                }
            }

            for(j=0; j < M; j++){

                del_term_theta0[j] = del_term_theta0[j] + delta_theta0(Z[b], passSample, j);
                gradient_theta0[j] = gradient_theta0[j] + chain.probs[s] * del_term_theta0[j];


    //			cout << "Deltermtheta = " << del_term_theta0[j] << "\n";
                if(isnan(gradient_theta0[j]) || isnan(del_term_theta0[j])){
                    cout << "Gradient comp error\n";
                    cout << "s = " << s << "\n";
                    cout << "j = " << j << "\n";
                    cout << "chainprobs = " << chain.probs[s] << "\n";
                    cout << "delta = " << delta_theta0(Z[b], passSample, j) << "\n";
                    cin >> j;
                }
            }
        }

//		cout << "Sigmas\n";

        if(!fixedSigma){

            for(k=0; k<K; k++){

                del_term_sigma[k] = del_term_sigma[k] + delta_sigma(Z[b], passSample, k);
                gradient_sigma[k] = gradient_sigma[k] + chain.probs[s] * del_term_sigma[k];
            }

            del_term_sigma0 = del_term_sigma0 + delta_sigma0(Z[b], passSample);
            gradient_sigma0 = gradient_sigma0 + chain.probs[s] * del_term_sigma0;
        }

        // Finished updating. Now change samples

        if(Z[b][chain.t_sample[s]][chain.k_sample[s]]){

            Z[b][chain.t_sample[s]][chain.k_sample[s]] = false;
            model->updateProbs(chain.b_sample[s], chain.t_sample[s], chain.k_sample[s], false);
        }else{

            Z[b][chain.t_sample[s]][chain.k_sample[s]] = true;
            model->updateProbs(chain.b_sample[s], chain.t_sample[s], chain.k_sample[s], true);
        }


        // This is inefficient just to see if it works

/*

        if(!fixedOmega){
            for(i=0; i<K; i++){

                for(k=0; k<K; k++){

                    check1 = del_term_omega[i*K+k];
                    check2 = 0;
                    for(j=0; j < B; j++){
                        check2 = check2 + base_omega(Z[b], i, k, j);
                    }
                    if(check1 != check2){

                        qDebug() << "Dynamic programming error";
                        qDebug() << "Updtated : " << check1 << " vs " << check2;

                    }

//                    gradient_omega[i*K+k] = gradient_omega[i*K+k] - chain->probs[s] * del_term_omega[i*K+k];
//                    gradient_omega[i*K+k] = gradient_omega[i*K+k] + chain->probs[s] * base_omega(Z[b], i, k, b);
                }
            }

            if(!fixedSpontaneous){

                for(i=0; i<K; i++){


                    check1 = del_term_omega0[i];
                    check2 = 0;
                    for(j=0; j < B; j++){
                        check2 = check2 + base_omega0(Z[b], i, j);
                    }

                    if(check1 != check2){

                        qDebug() << "Dynamic programming error";

                    }

//                gradient_omega0[i] = gradient_omega0[i] - chain->probs[s] * del_term_omega0[i];
//                gradient_omega0[i] = gradient_omega0[i] + chain->probs[s] * base_omega0(Z[b], i, b);
            }
        }
    }

*/

    //****************************************************************************************
    // After setting the next value of Z[tk], we can optionally check
    // that the del_terms came out right by ma(*Nu)ally calculating them
    // with base_terms
    //****************************************************************************************

/*

        if(s == 0){
            cout << "\nChecking the Hill Climbing... (" << s << "/" << chain->S << ")\t";
        }
        errors = 0;
    //	cout << "Omegas\n";

        for(i=0; i<K; i++){

            for(k=0; k<K; k++){

                if(abs(del_term_omega[i*K+k] - base_omega(Z, i, k)) > 0.1){

                    cout << "\nError : sample " << s << "\n";
                    cout << "Omega : i = " << i << ", k = " << k << "\n";
                    cout << del_term_omega[i*K+k] << "\tExpected :\t" << base_omega(Z, i, k);
                    cout << "\nThe sample was t = " << chain->t_sample[s] << " and k = " << chain->k_sample[s];
                    cout << "\nThe term that made the error was : " << delta << "\n";
                    errors++;
                    chain->display(Z);
                    cin >> i;
                }
            }
        }


    //	cout << "Thetas\n";
        for(k=0; k<K; k++){

            for(j=0; j<(M-1); j++){

                if(abs(del_term_theta[k*M+j] - base_theta(Z, k, j)) > 0.1){

                    cout << "\nError : sample " << s << "\n";
                    cout << "Theta : k = " << k << ", j = " << j << "\n";
                    cout << del_term_theta[k*M+j] << "\tExpected :\t" << base_theta(Z, k, j);
                    errors++;
                }
            }
        }

    //	cout << "Sigmas\n";
        for(k=0; k<K; k++){

            if(abs(del_term_sigma[k] - base_sigma(Z, k)) > 0.1){

                cout << "\nError : sample " << s << "\n";
                cout << "Sigma : k = " << k << "\n";
                cout << del_term_sigma[k] << "\tExpected :\t" << base_sigma(Z, k);
                errors++;
            }
        }
        if(errors == 0){

            cout << "[OK]\n\n";
        }else{

            cout << "\n\n";
        }


*/

//		cout << "P0 Terms\n";
        for(k=0; k<K; k++){

            if(Z[b][0][k]){
                gradient_P0[k] = gradient_P0[k] + chain.probs[s] * (1.0 / model->P0[k]);
            }else{
                gradient_P0[k] = gradient_P0[k] - chain.probs[s] * (1.0 / (1.0 - model->P0[k]));
            }
        }

        total = total + 1;
        emit climbTick(total / max);
    }

    qDebug() << "End dynamic step";

//	cout << "3b sigma0 " << model->Sigma0 << "\n";
}



// Once we've established the gradient, we need to update the model parameters.
// Sigmas and Omegas are ok. Just constrain them to be between 0 and 1.
// For the Thetas we need to stay inside them summing to 1. May be best to just
// Scale them so that they make the correct relative updates without bouncing
// We'll let the other two climb freely.
double HillClimber::climb(){

	int i, k, j;
	int a;
    double test;
	int overshot = 0;
    double dist = 0.0;
    double in_use, test_sum;
	double mean;
    double x;

    double gradScale;
	double total = 0.0;

    // I don't think we're even using this anymore
    vector < bool > collapse_dimension;
    collapse_dimension.resize(M);

    vector < double > gradient_total;
    gradient_total.resize(M);

    qDebug() << "Begin climbing";

	// Do the zero parameters
//	test = Sigma0 + omega_rate * gradient_sigma0 + momentum * last_sigma0;

    if(!fixedSigma){

        test = omega_rate * gradient_sigma0 + momentum * last_sigma0;
        if(test > 0){

            test = model->Sigma0 + omega_rate * 0.1;
        }else{

            test = model->Sigma0 - omega_rate * 0.1;
        }

        if(test < 0.001){
            dist = (model->Sigma0 - 0.001) * (model->Sigma0 - 0.001);
            last_sigma0 = (0.001 - model->Sigma0);
            model->Sigma0 = 0.001;
            overshot++;
        }else if(test > 0.999){
            dist = (model->Sigma0 - 0.999) * (model->Sigma0 - 0.999);
            last_sigma0 = (0.999 - model->Sigma0);
            model->Sigma0 = 0.999;
            overshot++;
        }else{
            dist = (model->Sigma0 - test) * (model->Sigma0 - test);
            last_sigma0 = (test - model->Sigma0);
            model->Sigma0 = test;
        }
    }

    // Set up the scale parameter for the omegas

    if(!fixedOmega){

        total = 0.0;
        for(k=0; k < K; k++){

            test = omega_rate * gradient_omega0[k] + momentum * last_omega0[k];

            if((model->Omega0[k] + test) < 0.001){

                test = model->Omega0[k] - 0.001;
                total = total + test * test;
            }else if((model->Omega0[k] + test) > 0.999){

                test = 0.999 - model->Omega0[k];
                total = total + test * test;
            }else{

                total = total + test * test;
            }
        }


        for(i=0; i < K; i++){

            for(k=0; k < K; k++){

                test = omega_rate * gradient_omega[i][k] + momentum * last_omega[i][k];
                if((model->Omega[i][k] + test) < 0.001){

                    test = model->Omega[i][k] - 0.001;
                    total = total + test * test;
                }else if((model->Omega[i][k] + test) > 0.999){

                    test = 0.999 - model->Omega[i][k];
                    total = total + test * test;
                }else{

                    total = total + test * test;
                }
            }
        }

        gradScale = sqrt(omega_rate) / sqrt(total);

    //	cout << "Gradscale " << gradScale << "\n";


        if(!fixedSpontaneous){

            for(k=0; k < K; k++){

        //		Omega0[k] = 0.001;


        //		test = Omega0[k] + omega_rate * gradient_omega0[k] + momentum * last_omega0[k];

                test = model->Omega0[k] + gradScale * (omega_rate * gradient_omega0[k] + momentum * last_omega0[k]);

                if(test < 0.001){
                    dist = (model->Omega0[k] - 0.001) * (model->Omega0[k] - 0.001);
                    last_omega0[k] = (0.001 - model->Omega0[k]) / gradScale;
                    model->Omega0[k] = 0.001;
                    overshot++;
                }else if(test > 0.999){
                    dist = (model->Omega0[k] - 0.999) * (model->Omega0[k] - 0.999);
                    last_omega0[k] = (0.999 - model->Omega0[k]) / gradScale;
                    model->Omega0[k] = 0.999;
                    overshot++;
                }else{
                    dist = (model->Omega0[k] - test) * (model->Omega0[k] - test);
                    last_omega0[k] = (test - model->Omega0[k]) / gradScale;
                    model->Omega0[k] = test;
                }

            }
        }
    }

    // Do all the non zero, non theta parameters

	for(i=0; i < K; i++){


        if(!fixedSigma){

            test = model->Sigma[i] + omega_rate * gradient_sigma[i] + momentum * last_sigma[i];
            test = omega_rate * gradient_sigma[i] + momentum * last_sigma[i];

            if(test > 0){

                test = model->Sigma[i] + omega_rate * 0.1;
            }else{

                test = model->Sigma[i] - omega_rate * 0.1;
            }

    //		Sigma[i] = 0.99;


            if(test < 0.001){
                dist = (model->Sigma[i] - 0.001) * (model->Sigma[i] - 0.001);
                last_sigma[i] = (0.001 - model->Sigma[i]);
                model->Sigma[i] = 0.001;
                overshot++;
            }else if(test > 0.999){
                dist = (model->Sigma[i] - 0.999) * (model->Sigma[i] - 0.999);
                last_sigma[i] = (0.999 - model->Sigma[i]);
                model->Sigma[i] = 0.999;
                overshot++;
            }else{
                dist = (model->Sigma[i] - test) * (model->Sigma[i] - test);
                last_sigma[i] = (test - model->Sigma[i]);
                model->Sigma[i] = test;
            }
        }

//		test = P0[i] + omega_rate * gradient_P0[i] + momentum * last_P0[i];

		test = omega_rate * gradient_P0[i] + momentum * last_P0[i];
		if(test > 0){
            test = model->P0[i] + omega_rate * 0.1;
		}else{

            test = model->P0[i] - omega_rate * 0.1;
		}

		if(test < 0.001){
            dist = (model->P0[i] - 0.001) * (model->P0[i] - 0.001);
            last_P0[i] = (0.001 - model->P0[i]);
            model->P0[i] = 0.001;
			overshot++;
		}else if(test > 0.999){
            dist = (model->P0[i] - 0.999) * (model->P0[i] - 0.999);
            last_P0[i] = (0.999 - model->P0[i]);
            model->P0[i] = 0.999;
			overshot++;
		}else{
            dist = (model->P0[i] - test) * (model->P0[i] - test);
            last_P0[i] = (test - model->P0[i]);
            model->P0[i] = test;
		}


		for(k=0; k < K; k++){

//			test = Omega[i*K+k] + omega_rate * gradient_omega[i*K+k] + momentum * last_omega[i*K+k];

            if(!fixedOmega){

                test = model->Omega[i][k] + gradScale * (omega_rate * gradient_omega[i][k] + momentum * last_omega[i][k]);

                if(test < 0.001){
                    dist = (model->Omega[i][k] - 0.001) * (model->Omega[i][k] - 0.001);
                    last_omega[i][k] = (0.001 - model->Omega[i][k]) / gradScale;
                    model->Omega[i][k] = 0.001;
                    overshot++;
                }else if(test > 0.999){
                    dist = (model->Omega[i][k] - 0.999) * (model->Omega[i][k] - 0.999);
                    last_omega[i][k] = (0.999 - model->Omega[i][k]) / gradScale;
                    model->Omega[i][k] = 0.999;
                    overshot++;
                }else{
                    dist = (model->Omega[i][k] - test) * (model->Omega[i][k] - test);
                    last_omega[i][k] = (test - model->Omega[i][k]) / gradScale;
                    model->Omega[i][k] = test;
                }
            }

		}

    }



	//****************************************************************
	// This is the old theta hill climbing code with reduced (M-1)
	// Dimensionality
	//*****************************************************************
	/*

	//	cout << "Updating Theta 0\n";	
	//Do the theta0 parameters. Just like the theta regulars
	temp_rate = 2.0;
	theta_total = 5.0;

	while(theta_total > 0.999){
	momentum parameter
		temp_rate = temp_rate / 2.0;
		theta_total = 0.0;

		for(j=0; j < (M-1); j++){

			test = Theta0[j] + temp_rate * (theta_rate * gradient_theta0[j] + momentum * last_theta0[j]);
			if(test < 0.001){
				theta_total = theta_total + 0.001;
			}else if(test > 0.999){
				theta_total = theta_total + 0.999;
			}else{
				theta_total = theta_total + test;
			}
			if(theta_total > 0.999){
				j = M+1;
			}
		}

	}

	cout << "Theta0 temp rate : " << temp_rate << "\n";

	//Actually update the theta values.
	theta_total = 0.0;
	for(j=0; j < (M-1); j++){

		test = Theta0[j] + temp_rate * (theta_rate * gradient_theta0[j] + momentum * last_theta0[j]);
		if(test < 0.001){
			dist = (Theta0[j] - 0.001) * (Theta0[j] - 0.001);
			theta_total = theta_total + 0.001;
			last_theta0[j] = (Theta0[j] - 0.001);
			Theta0[j] = 0.001;
		}else if(test > 0.999){
			dist = (Theta0[j] - 0.999) * (Theta0[j] - 0.999);
			theta_total = theta_total + 0.999;
			last_theta0[j] = (Theta0[j] - 0.999);
			Theta0[j] = 0.999;
		}else{
			dist = (Theta0[j] - test) * (Theta0[j] - test);
			theta_total = theta_total + test;
			last_theta0[j] = (Theta0[j] - test);
			Theta0[j] = test;
		}
	}

	dist = (Theta0[M-1] - 1.0 + theta_total) * (Theta0[M-1] - 1.0 + theta_total);
	Theta0[M-1] = 1.0 - theta_total;



	// For each visual category choose an appropriate learning rate
	// For the feature distribution.
	for(k=0; k < K; k++){

		temp_rate = 2.0;
		theta_total = 5.0;

		// cout << "Updating Theta " << k << "\n";

		while(theta_total > 0.999){
		
			temp_rate = temp_rate / 2.0;
			theta_total = 0.0;

			for(j=0; j < (M-1); j++){

				test = Theta[k*M+j] + temp_rate * (theta_rate * gradient_theta[k*M + j] + momentum * last_theta[k*M + j]);
				if(test < 0.001){
					theta_total = theta_total + 0.001;
				}else if(test > 0.999){
					theta_total = theta_total + 0.999;
				}else{
					theta_total = theta_total + test;
				}
				if(theta_total > 0.999){
					j = M+1;
				}
			}

		}


		cout << "Theta" << (k+1) << " temp rate : " << temp_rate << "\n";
		//Actually update the theta values.
		theta_total = 0.0;
		for(j=0; j < (M-1); j++){

			cout << "Theta ( " << (k+1) << " , " << j << ") change : " << (theta_rate * gradient_theta[k*M + j] + momentum * last_theta[k*M + j]) << "\n";

			test = Theta[k*M+j] + temp_rate * (theta_rate * gradient_theta[k*M + j] + momentum * last_theta[k*M + j]);
			if(test < 0.001){
				dist = (Theta[k*M+j] - 0.001) * (Theta[k*M+j] - 0.001);
				theta_total = theta_total + 0.001;
				last_theta[k*M+j] = (Theta[k*M+j] - 0.001);
				Theta[k*M+j] = 0.001;
			}else if(test > 0.999){
				dist = (Theta[k*M+j] - 0.999) * (Theta[k*M+j] - 0.999);
				theta_total = theta_total + 0.999;
				last_theta[k*M+j] = (Theta[k*M+j] - 0.999);
				Theta[k*M+j] = 0.999;
			}else{
				dist = (Theta[k*M+j] - test) * (Theta[k*M+j] - test);
				theta_total = theta_total + test;
				last_theta[k*M+j] = (Theta[k*M+j] - test);
				Theta[k*M+j] = test;
			}
		}

		dist = (Theta[k*M+M-1] - 1.0 + theta_total) * (Theta[k*M+M-1] - 1.0 + theta_total);
		Theta[k*M+ M-1] = 1.0 - theta_total;

	}

	*/
	//****************************************************************
	// This concludes the old theta hill climbing code attempt 1
	//*****************************************************************

	//****************************************************************
	// This begins the old theta hill climbing code attempt 2
	//*****************************************************************
/*
	gradScale = 1.0;
	mean = 0.0;
	in_use = M;
	for(j=0; j < M; j++){

		collapse_dimension[j] = false;
		gradient_total[j] = theta_rate * gradient_theta0[j] + momentum * last_theta0[j];
		mean = mean + gradient_total[j];
	}
	mean = mean / in_use;

	changes = true;
	while(changes){
		
		changes = false;
		for(j=0; j < M; j++){


			if(!collapse_dimension[j]){
				if(((abs(Theta0[j] - Theta_MIN) / Theta_MIN) < 0.001) && ((gradient_total[j] - mean) < 0.0)){

//					cout << "Collapsing Theta[ " << j << " ] = " << Theta0[j] << "\n";
//					cout << "ThetaMIN = " << Theta_MIN << "\n";
//					cin >> a;
					mean = mean * in_use;
					mean = mean - gradient_total[j];
					in_use = in_use - 1.0;
					mean = mean / in_use;
					gradient_total[j] = 0.0;
					collapse_dimension[j] = true;
					changes = true;
				}
			}
		}
	}

	// This scales the gradient down so that something goes to zero
	scaled = -1;
	changes = true;
	while(changes){
		
		changes = false;
		for(j=0; j < M; j++){

			if(!collapse_dimension[j]){
				if((Theta0[j] + gradScale * (gradient_total[j] - mean)) < Theta_MIN){
					if(abs(Theta0[j] + gradScale * (gradient_total[j] - mean) - Theta_MIN) / Theta_MIN > 0.001){

						gradScale = ((Theta_MIN - Theta0[j]) / (gradient_total[j] - mean));
						changes = true;
						scaled = j;
					}
				}
			}
		}
	}

	if(gradScale < 0.99){
//		cout << "Grad Scale : " << gradScale << "\n";
	}

	test_sum = 0.0;
	for(j=0; j < M; j++){

		if(!collapse_dimension[j]){

			test_sum = test_sum + gradScale * (gradient_total[j] - mean);	
		}
	}

	if(abs(test_sum - 0.0) > 0.00001){
		cout << " Uh Oh, changes sum to " << test_sum << "\n";
		cin >> a;
	}


	test_sum = 0.0;
	for(j=0; j < M; j++){

		if(!collapse_dimension[j]){

			Theta0[j] = Theta0[j] + gradScale * (gradient_total[j] - mean);
			last_theta0[j] = gradScale * (gradient_total[j] - mean);
			dist = dist + last_theta0[j] * last_theta0[j];
		}else{

			last_theta0[j] = 0.0;
		}
		test_sum = test_sum + Theta0[j];
	}

	if(abs(test_sum - 1.0) > 0.001){
		cout << " Uh Oh, the theta0's sum to " << test_sum << "\n";
		cin >> a;
	}


	// Now do the non-zero thetas.

	for(k = 0; k < K; k++){

		mean = 0.0;
		in_use = M;
		for(j=0; j < M; j++){

			collapse_dimension[j] = false;
			gradient_total[j] = theta_rate * gradient_theta[k*M+j] + momentum * last_theta[k*M+j];
			mean = mean + gradient_total[j];
		}
		mean = mean / in_use;

		changes = true;
		while(changes){
		
			changes = false;
			for(j=0; j < M; j++){


				if(!collapse_dimension[j]){
					if(((abs(Theta[k*M+j] - Theta_MIN) / Theta_MIN) < 0.001) && ((gradient_total[j] - mean) < 0.0)){

						mean = mean * in_use;
						mean = mean - gradient_total[j];
						in_use = in_use - 1.0;
						mean = mean / in_use;
						gradient_total[j] = 0.0;
						collapse_dimension[j] = true;
						changes = true;
					}
				}
			}
		}

		test_sum = 0.0;
		gradScale = 1.0;
		// This scales the gradient down so that something goes to zero
		changes = true;
		while(changes){
		
			changes = false;
			for(j=0; j < M; j++){

				if(!collapse_dimension[j]){
					if((Theta[k*M+j] + gradScale * (gradient_total[j] - mean)) < Theta_MIN){
						if(abs(Theta[k*M+j] + gradScale * (gradient_total[j] - mean) - Theta_MIN) / Theta_MIN > 0.001){

							gradScale = ((Theta_MIN - Theta[k*M+j]) / (gradient_total[j] - mean));
							changes = true;
						}
					}
				}
			}
		}

		if(gradScale < 0.99){
//			cout << "Grad Scale : " << gradScale << "\n";
		}

		test_sum = 0.0;
		for(j=0; j < M; j++){

			if(!collapse_dimension[j]){

				test_sum = test_sum + gradScale * (gradient_total[j] - mean);	
			}
		}

		if(abs(test_sum - 0.0) > 0.00001){
			cout << " Uh Oh, changes sum to " << test_sum << "\n";
			cin >> a;
		}


		test_sum = 0.0;
		for(j=0; j < M; j++){

			if(!collapse_dimension[j]){

				Theta[k*M+j] = Theta[k*M+j] + gradScale * (gradient_total[j] - mean);
				last_theta[k*M+j] = gradScale * (gradient_total[j] - mean);
				dist = dist + last_theta[k*M+j] * last_theta[k*M+j];
				if(Theta[k*M+j] < -0.00001 || Theta[k*M+j] > 1.0){
					cout << "Theta[ " << k << " , " << j << " ] = " << Theta[k*M+j] << "\n";
					cin >> a;
				}
			}else{

				last_theta[k*M+j] = 0.0;
			}
			test_sum = test_sum + Theta[k*M+j];
		}

		if(abs(test_sum - 1.0) > 0.001){
			cout << " Uh Oh, the theta's sum to " << test_sum << "\n";
			cin >> j;
		}
	}
*/

	//****************************************************************
	// This concludes the old theta hill climbing code attempt 2
	//*****************************************************************

    if(!fixedTheta){

        mean = 0.0;
        in_use = M;
        for(j=0; j < M; j++){

            collapse_dimension[j] = false;
            gradient_total[j] = gradient_theta0[j] + momentum * last_theta0[j];

            mean = mean + gradient_total[j];
        }
        mean = mean / in_use;

        // Update Theta's projected into the sum to 1 plane
        gradScale = 0.0;
        for(j=0; j < M; j++){

            gradient_total[j] = gradient_total[j] - mean;
            gradScale = gradScale + gradient_total[j] * gradient_total[j];
        }
        gradScale = sqrt(gradScale);
        if(gradScale > 0){
            gradScale = theta_rate / gradScale;
        }else{
            gradScale = 0;
        }
        for(j=0; j < M; j++){

            gradient_total[j] = gradScale * gradient_total[j];
        }

        // Just keep projecting until it works. jesus.
        while(true){

            test_sum = 0.0;
            for(j=0; j < M; j++){

                test_sum = test_sum + gradient_total[j];

            }

            if(abs(test_sum - 0.0) > 0.0001){
                cout << " 1. Uh Oh, changes sum to " << test_sum << "\n";
                cin >> a;
            }




            // Find out which ones overshot (or rather didn't
            in_use = 0;
            mean = 0;
            for(j=0; j < M; j++){

                x = model->Theta0[j] + gradient_total[j];
                if(((x - Theta_MIN) / Theta_MIN) > 0.000001){
                    mean = mean + model->Theta0[j] + gradient_total[j];
                    in_use = in_use + 1;
                }
            }
            mean = mean / in_use;

            // Now project this point into the bounded region
            // It ought to work
            for(j=0; j < M; j++){

                x = model->Theta0[j] + gradient_total[j];
                if(((x - Theta_MIN) / Theta_MIN) > 0.000001){

                    gradient_total[j] = gradient_total[j] + ((1.0 - (((double) M) - in_use) * Theta_MIN) / in_use) - mean;
                }else{

                    gradient_total[j] = Theta_MIN - model->Theta0[j];
                }
            }
            // And that's it. It's just that easy.

            in_use = 0;
            for(j=0; j < M; j++){

                x = model->Theta0[j] + gradient_total[j];
                if(((x - Theta_MIN) / Theta_MIN) < -0.000001){

                    in_use = in_use + 1;
                }
                if(x > 1){

                    in_use = in_use + 1;
                }
            }
            if(in_use < 0.5){

                for(j=0; j < M; j++){

                    last_theta0[j] = gradient_total[j];
                    model->Theta0[j] = model->Theta0[j] + gradient_total[j];
                }
                break;
            }

        }
        test_sum = 0.0;
        for(j=0; j < M; j++){

            test_sum = test_sum + last_theta0[j];

        }

        if(abs(test_sum - 0.0) > 0.0001){
            cout << " 2. Uh Oh, changes sum to " << test_sum << "\n";
            cin >> a;
        }


        test_sum = 0.0;
        for(j=0; j < M; j++){

            test_sum = test_sum + model->Theta0[j];
        }

        if(abs(test_sum - 1.0) > 0.001){
            cout << " 3. Uh Oh, the theta0's sum to " << test_sum << "\n";
            cin >> a;
        }
    }

    // Now do the non-zero thetas.

    for(k = 0; k < K; k++){

        if(!fixedTheta){

            mean = 0.0;
            in_use = M;
            for(j=0; j < M; j++){

                collapse_dimension[j] = false;
                gradient_total[j] = gradient_theta[k][j] + momentum * last_theta[k][j];
                mean = mean + gradient_total[j];
            }
            mean = mean / in_use;

            // Update Theta's projected into the sum to 1 plane
            for(j=0; j < M; j++){

                gradient_total[j] = gradient_total[j] - mean;
                gradScale = gradScale + gradient_total[j] * gradient_total[j];
            }
            gradScale = sqrt(gradScale);
            if(gradScale > 0){
                gradScale = theta_rate / gradScale;
            }else{
                gradScale = 0;
            }

            for(j=0; j < M; j++){

                gradient_total[j] = gradScale * gradient_total[j];
            }

            // Just keep projecting until it works. jesus.
            while(true){

                test_sum = 0.0;
                for(j=0; j < M; j++){

                    test_sum = test_sum + gradient_total[j];

                }

                if(abs(test_sum - 0.0) > 0.0001){
                    cout << " 4. Uh Oh, changes sum to " << test_sum << "\n";
                    cin >> a;
                }




                // Find out which ones overshot (or rather didn't
                in_use = 0;
                mean = 0;
                for(j=0; j < M; j++){

                    x = model->Theta[k][j] + gradient_total[j];
                    if(((x - Theta_MIN) / Theta_MIN) > 0.000001){
                        mean = mean + model->Theta[k][j] + gradient_total[j];
                        in_use = in_use + 1;
                    }
                }
                mean = mean / in_use;

                // Now project this point into the bounded region
                // It ought to work
                for(j=0; j < M; j++){

                    x = model->Theta[k][j] + gradient_total[j];
                    if(((x - Theta_MIN) / Theta_MIN) > 0.000001){

                        gradient_total[j] = gradient_total[j] + ((1.0 - (((double) M) - in_use) * Theta_MIN) / in_use) - mean;
                    }else{

                        gradient_total[j] = Theta_MIN - model->Theta[k][j];
                    }
                }
                // And that's it. It's just that easy.

                in_use = 0;
                for(j=0; j < M; j++){

                    x = model->Theta[k][j] + gradient_total[j];
                    if(((x - Theta_MIN) / Theta_MIN) < -0.000001){

                        in_use = in_use + 1;
                    }
                    if(x > 1){

                        in_use = in_use + 1;
                    }
                }
                if(in_use < 0.5){

                    for(j=0; j < M; j++){

                        last_theta[k][j] = gradient_total[j];
                        model->Theta[k][j] = model->Theta[k][j] + gradient_total[j];
                    }
                    break;
                }

            }

            test_sum = 0.0;
            for(j=0; j < M; j++){

                test_sum = test_sum + last_theta[k][j];

            }

            if(abs(test_sum - 0.0) > 0.0001){
                cout << " 5. Uh Oh, changes sum to " << test_sum << "\n";
                cin >> a;
            }


            test_sum = 0.0;
            for(j=0; j < M; j++){

                test_sum = test_sum + model->Theta[k][j];
            }

            if(abs(test_sum - 1.0) > 0.001){
                cout << " 6. Uh Oh, the theta0's sum to " << test_sum << "\n";
                cin >> a;
            }


            //Conclude the fixedTheta if statement
        }

    }


    qDebug() << "End climbing";


//	cout << "Overshot " << overshot << " times in hill climbing.\n";
	dist = sqrt(dist);
	return dist;
}



// Lets assume that X contains data for a t=0 step that goes unused

double HillClimber::base_omega(vector < vector < bool > > &Z, int a, int b, int batch){

    int i;
	int t;
	double base = 0.0;

    i = batch;

//	cout << "Inside gradient " << gradient_omega << "\n";

    for(t=1; t < T[i]; t++){

        if(Z[t-1][b]){

            if(Z[t][a]){

                base = base + ((1.0 - model->Aleph[i][a][t-1]) / ((1.0 - model->Omega[a][b]) * model->Aleph[i][a][t-1]));

            }else{

//				base = base - ((1.0 - (*Aleph)[(t-1)*K + a]) / ((1.0 - Omega[a*K + b]) * (*Aleph)[(t-1)*K + a]));
                base = base - (1.0 / (1.0 - model->Omega[a][b]));
            }

        }else{

            base = base + 0.0;
        }
//		cout << "Inside gradient " << gradient_omega << " , Loop : " << t << "\n";
    }

//	cout << "outside gradient " << gradient_omega << "\n";

    return base;
}

double HillClimber::base_omega0(vector < vector < bool > > &Z, int a, int batch){

    int i;
	int t;
	double base = 0.0;

    i = batch;

/*******************************************/
//  I have no idea why this line was in but it's probably correct.
//	base = base - (1.0 / (1.0 - Omega0[a]));
/*******************************************/

    for(t=1; t < T[i]; t++){

        if(Z[t][a]){

            base = base + ((1.0 - model->Aleph[i][a][t-1]) / ((1.0 - model->Omega0[a]) * model->Aleph[i][a][t-1]));

        }else{

//			base = base - ((1.0 - (*Aleph)[(t-1)*K + a]) / ((1.0 - Omega0[a]) * (*Aleph)[(t-1)*K + a]));
            base = base - (1.0 / (1.0 - model->Omega0[a]));
        }

    }

	return base;
}

double HillClimber::base_theta(vector < vector < bool > > &Z, int a, int b, int batch){

    int i;
	int t;
	double base = 0.0;
	double x = 0.0;

    i = batch;

    for(t=1; t < T[i]; t++){

        if(Z[t][a]){

            x = (videos->getData(i)->X[t][b] / model->Tau[i][b][t]);
//			This is for the old version
//			x = x - (V->X[t*M + M-1] / (*Tau)[t*M + M-1]);
            x = x * model->Sigma[a];

			base = base + x;
		}else{

			base = base + 0.0;
		}
	}

	return base;
}


double HillClimber::base_theta0(int b, int batch){

    int i;
	int t;
	double base = 0.0;
	double x = 0.0;
	int a;

    i = batch;

    for(t=1; t < T[i]; t++){

        x = (videos->getData(i)->X[t][b] / model->Tau[i][b][t]);
//		This is for the old version
//		x = x - (V->X[t*M + M-1] / (*Tau)[t*M + M-1]);
        x = x * model->Sigma0;

        base = base + x;

		if(isnan(base)){
			cout << "x = " << x << "\n";
            cout << "X[tb] = " << videos->getData(i)->X[t][b] << "\n";
            cout << "(*Tau) = " << model->Tau[i][b][t] << "\n";
			cout << "t = " << t << "\n";
			cout << "b = " << b << "\n";
            cout << "sigma0 = " << model->Sigma0 << "\n";
			cin >> a;
		}
	}

	return base;
}


double HillClimber::base_sigma(vector < vector < bool > > &Z, int a, int batch){

    int i, j;
	int t;
	double base = 0.0;
	double x = 0.0;

    i = batch;

//	cout << "\nBase Sigma_" << a << "\n";
    for(t=1; t < T[i]; t++){

        if(Z[t][a]){

			x = 0.0;
			for(j=0; j < M; j++){
			
                x = x + ((videos->getData(i)->X[t][j] * model->Theta[a][j]) / model->Tau[i][j][t]);
			}
            x = x - (videos->getData(i)->N[t] / model->Nu[i][t]);

			base = base + x;
//			cout << "Sigma_t :\t" << x << "\n";

		}else{

			base = base + 0.0;
//			cout << "Sigma_t :\t" << 0.0 << "\n";
		}
	}
//	cout << "\n";


	return base;
}



double HillClimber::base_sigma0(int batch){

    int i, j;
    int t;
	double base = 0.0;
	double x = 0.0;

    i = batch;

//	cout << "\nBase Sigma_" << a << "\n";
    for(t=1; t < T[i]; t++){

		x = 0.0;
		for(j=0; j < M; j++){
		
            x = x + ((videos->getData(i)->X[t][j] * model->Theta0[j]) / model->Tau[i][j][t]);
		}
        x = x - (videos->getData(i)->N[t] / model->Nu[i][t]);

		base = base + x;
//		cout << "Sigma_t :\t" << x << "\n";

	}
//	cout << "\n";


	return base;
}

// Just make sure you pass the right Z vector

double HillClimber::delta_omega(vector < vector < bool > > &Z, vector < int > &sample, int a, int b){


	double inferior, superior;
	

    int t = sample[0];
    int k = sample[1];
    int i = sample[2];

	inferior = 0.0;
	superior = 0.0;

	if(t > 0){

		// Establish the inferior term

		if(a != k){
			inferior = 0.0;

		}else{
		
            if(!Z[t-1][b]){

				inferior = 0.0;
			}else{
                inferior = (1.0 - model->Omega[a][b]) * (model->Aleph[i][a][t-1]);
				inferior = 1.0 / inferior;
			}
		}
		
        if(Z[t][a]){

			inferior = (-1.0) * inferior;
		}
	}


    if(t < T[i]-1){

		// Establish the superior term

		// If Z^t+1_a = 1
        if(Z[t+1][a]){

			// If Z^t_k : 0 -> 1
            if(!Z[t][k]){

				if(k == b){

                    superior = (1.0 - model->Omega[a][k]) * model->Aleph[i][a][t] + model->Omega[a][k];
                    superior = (1.0 - model->Aleph[i][a][t]) / superior;

				}else{

                    if(Z[t][b]){

                        superior = (1.0 - model->Omega[a][k]) * model->Aleph[i][a][t] + model->Omega[a][k];
                        superior = (1.0 - model->Aleph[i][a][t]) / superior;

                        superior = model->Omega[a][k] * superior;
                        superior = superior / (1.0 - model->Omega[a][b]);
                        superior = superior / model->Aleph[i][a][t];
			
						superior = -1.0 * superior;

					}else{

						superior = 0.0;
					}
				}



			}else{

				if(k == b){

                    superior = (1.0 - model->Omega[a][k]) * model->Aleph[i][a][t];
                    superior = (1.0 - model->Aleph[i][a][t]) / superior;
					superior = -1.0 * superior;

				}else{


                    if(Z[t][b]){

                        superior = (1.0 - model->Omega[a][b]) * model->Aleph[i][a][t];
                        //**************************************************************
                        // READ THIS
                        // This stupid thing below. The second parenthesis was missing. Not
                        // sure if it was put back in the correct spot
                        //**************************************************************
                        superior = (1.0 - model->Aleph[i][a][t]) / superior;

                        superior = superior * model->Omega[a][k];
                        superior = superior / (model->Aleph[i][a][t] - model->Omega[a][k]);

					}else{

						superior = 0.0;
					}

				}

			}

		// Else Z^t+1_a = 0
		}else{


			// If Z^t_k : 0 -> 1
            if(!Z[t][k]){

				if(k == b){

                    superior = (-1.0) / (1.0 - model->Omega[a][b]);

				}else{
					
					superior = 0.0;
				}


			// Otherwise Z^t_k : 1 -> 0
			}else{

				if(k == b){

                    superior = 1.0 / (1.0 - model->Omega[a][b]);
				}else{

					superior = 0.0;
				}

			}
		}

	}

	return (inferior+superior);

}




double HillClimber::delta_omega0(vector < vector < bool > > &Z, vector < int > &sample, int a){


	double inferior, superior;
	
    int t = sample[0];
    int k = sample[1];
    int b = sample[2];

	inferior = 0.0;
	superior = 0.0;


	if(t > 0){

		// Establish the inferior term

		if(a != k){
			inferior = 0.0;

		}else{
	
            inferior = (1.0 - model->Omega0[a]) * (model->Aleph[b][a][t-1]);
			inferior = 1.0 / inferior;
		}
		
        if(Z[t][a]){

			inferior = (-1.0) * inferior;
		}
	}


    if(t < T[b]-1){

		// Establish the superior term

		// If Z^t+1_a = 1
        if(Z[t+1][a]){

			// If Z^t_k : 0 -> 1
            if(!Z[t][k]){

                superior = (1.0 - model->Omega[a][k]) * model->Aleph[b][a][t] + model->Omega[a][k];
                //**************************************************************
                // READ THIS
                // This stupid thing below. The second parenthesis was missing. Not
                // sure if it was put back in the correct spot
                //**************************************************************
                superior = (1.0 - model->Aleph[b][a][t]) / superior;

                superior = model->Omega[a][k] * superior;
                superior = superior / (1.0 - model->Omega0[a]);
                superior = superior / model->Aleph[b][a][t];
	
				superior = -1.0 * superior;

			}else{

				
                superior = (1.0 - model->Omega0[a]) * model->Aleph[b][a][t];
                superior = (1.0 - model->Aleph[b][a][t]) / superior;

                superior = superior * model->Omega[a][k];
                superior = superior / (model->Aleph[b][a][t] - model->Omega[a][k]);

			}

		// Else Z^t+1_a = 0
		}else{


			// If Z^t_k : 0 -> 1
            if(!Z[t][k]){
				
				superior = 0.0;

			// Otherwise Z^t_k : 1 -> 0
			}else{

				superior = 0.0;
			}
		}

	}

	return (inferior+superior);

}


double HillClimber::delta_theta(vector < vector < bool > > &Z, vector < int > &sample, int a, int b){


	double delta_theta = 0.0;

    int t = sample[0];
    int k = sample[1];
    int i = sample[2];

	if(t > 0){


        if(!Z[t][k]){

			if(k == a){

                delta_theta = videos->getData(i)->X[t][b] / (model->Tau[i][b][t] + model->Sigma[a] * model->Theta[a][b]);
//				Old version
//				delta_theta = delta_theta - V->X[t*M + M-1] / ((*Tau)[t*M + M-1] + Sigma[a] * Theta[a*M + M-1]);
                delta_theta = delta_theta * model->Sigma[a];

			}else{
				
                if(Z[t][a]){

                    delta_theta = (videos->getData(i)->X[t][b] * model->Theta[k][b]) / (model->Tau[i][b][t] * (model->Tau[i][b][t] + model->Sigma[k] * model->Theta[k][b]));
//					Old version
//					delta_theta = delta_theta - (V->X[t*M + M-1] * Theta[k*M + M-1]) / ((*Tau)[t*M + M-1] * ((*Tau)[t*M + M-1] + Sigma[k] * Theta[k*M + M-1]));
                    delta_theta = delta_theta * model->Sigma[a] * model->Sigma[k];
					delta_theta = (-1.0) * delta_theta;
				}else{

					delta_theta = 0.0;
				}

			}


		}else{

			if(k == a){

                delta_theta = videos->getData(i)->X[t][b] / model->Tau[i][b][t];
//				cout << "t = " << t << ", M = " << M << ", b = " << b << "\n";
//				Old version
//				delta_theta = delta_theta - (V->X[t*M + M-1] / (*Tau)[t*M + M-1]);
                delta_theta = delta_theta * model->Sigma[a];
				delta_theta = (-1.0) * delta_theta;

			}else{

                if(Z[t][a]){

                    delta_theta = (videos->getData(i)->X[t][b] * model->Theta[k][b]) / (model->Tau[i][b][t] * (model->Tau[i][b][t] - model->Sigma[k] * model->Theta[k][b]));
//					Old version
//					delta_theta = delta_theta - (V->X[t*M + M-1] * Theta[k*M + M-1]) / ((*Tau)[t*M + M-1] * ((*Tau)[t*M + M-1] - Sigma[k] * Theta[k*M + M-1]));
                    delta_theta = delta_theta * model->Sigma[a] * model->Sigma[k];

				}else{

					delta_theta = 0.0;
				}
			}


		}

	}
	return delta_theta;

}



double HillClimber::delta_theta0(vector < vector < bool > > &Z, vector < int > &sample, int b){


	double delta_theta = 0.0;

    int t = sample[0];
    int k = sample[1];
    int i = sample[2];


	if(t > 0){

        if(!Z[t][k]){

				
            delta_theta = (videos->getData(i)->X[t][b] * model->Theta[k][b]) / (model->Tau[i][b][t] * (model->Tau[i][b][t] + model->Sigma[k] * model->Theta[k][b]));
//			Old version
//			delta_theta = delta_theta - (V->X[t*M + M-1] * Theta[k*M + M-1]) / ((*Tau)[t*M + M-1] * ((*Tau)[t*M + M-1] + Sigma[k] * Theta[k*M + M-1]));
            delta_theta = delta_theta * model->Sigma0 * model->Sigma[k];
			delta_theta = (-1.0) * delta_theta;


		}else{

            delta_theta = (videos->getData(i)->X[t][b] * model->Theta[k][b]) / (model->Tau[i][b][t] * (model->Tau[i][b][t] - model->Sigma[k] * model->Theta[k][b]));
//			Old version
//			delta_theta = delta_theta - (V->X[t*M + M-1] * Theta[k*M + M-1]) / ((*Tau)[t*M + M-1] * ((*Tau)[t*M + M-1] - Sigma[k] * Theta[k*M + M-1]));
            delta_theta = delta_theta * model->Sigma0 * model->Sigma[k];

		}

	}
	return delta_theta;

}





double HillClimber::delta_sigma(vector < vector < bool > > &Z, vector < int > &sample, int a){


	double delta_sigma = 0.0;
	int j = 0;

    int t = sample[0];
    int k = sample[1];
    int b = sample[2];

//	cout << "Changing sigma for sample t = " << t << ", k = " << k << "\n";

	if(t > 0){


        if(!Z[t][k]){

			if(k == a){

				for(j=0; j<M; j++){

                    delta_sigma = delta_sigma + (videos->getData(b)->X[t][j] * model->Theta[a][j]) / (model->Tau[b][j][t] + model->Sigma[a] * model->Theta[a][j]);
				}
                delta_sigma = delta_sigma - (videos->getData(b)->N[t] / (model->Nu[b][t] + model->Sigma[a]));

//				cout << "Delta Sigma, t  = " << 0 << ", k==a, Ztk = 1\n";
//				cout << delta_sigma << "\n";


			}else{
				
                if(Z[t][a]){

					for(j=0; j<M; j++){

                        delta_sigma = delta_sigma + ((videos->getData(b)->X[t][j] * model->Theta[a][j] * model->Theta[k][j]) / (model->Tau[b][j][t] * (model->Tau[b][j][t] + model->Sigma[k] * model->Theta[k][j])));
					}
                    delta_sigma = delta_sigma - (videos->getData(b)->N[t] / (model->Nu[b][t] * (model->Nu[b][t] + model->Sigma[k])));
                    delta_sigma = (-1.0) * model->Sigma[k] * delta_sigma;

//				cout << "Delta Sigma, t  = " << 0 << ", k!=a, Ztk = 1\n";
//				cout << delta_sigma << "\n";

				}else{

					delta_sigma = 0.0;
				}

			}


		}else{

			if(k == a){

				for(j=0; j<M; j++){

                    delta_sigma = delta_sigma + (videos->getData(b)->X[t][j] * model->Theta[a][j]) / model->Tau[b][j][t];
				}
                delta_sigma = delta_sigma - (videos->getData(b)->N[t] / model->Nu[b][t]);
				delta_sigma = (-1.0) * delta_sigma;

//				cout << "Delta Sigma, t  = " << 0 << ", k==a, Ztk = 0\n";
//				cout << delta_sigma << "\n";

			}else{

                if(Z[t][a]){

					for(j=0; j<M; j++){

                        delta_sigma = delta_sigma + ((videos->getData(b)->X[t][j] * model->Theta[a][j] * model->Theta[k][j]) / (model->Tau[b][j][t] * (model->Tau[b][j][t] - model->Sigma[k] * model->Theta[k][j])));
					}
                    delta_sigma = delta_sigma - (videos->getData(b)->N[t] / (model->Nu[b][t] * (model->Nu[b][t] - model->Sigma[k])));
                    delta_sigma = model->Sigma[k] * delta_sigma;


//					cout << "Delta Sigma, t  = " << 0 << ", k!=a, Ztk = 0\n";
//					cout << delta_sigma << "\n";

				}else{

					delta_sigma = 0.0;
				}
			}
		}
	}

	return delta_sigma;

}





double HillClimber::delta_sigma0(vector < vector < bool > > &Z, vector < int > &sample){


	double delta_sigma = 0.0;
	int j = 0;

    int t = sample[0];
    int k = sample[1];
    int b = sample[2];

//	cout << "Changing sigma for sample t = " << t << ", k = " << k << "\n";

	if(t > 0){


        if(!Z[t][k]){


			for(j=0; j<M; j++){

                delta_sigma = delta_sigma + ((videos->getData(b)->X[t][j] * model->Theta0[j] * model->Theta[k][j]) / (model->Tau[b][j][t] * (model->Tau[b][j][t] + model->Sigma[k] * model->Theta[k][j])));
			}
            delta_sigma = delta_sigma - (videos->getData(b)->N[t] / (model->Nu[b][t] * (model->Nu[b][t] + model->Sigma[k])));
            delta_sigma = (-1.0) * model->Sigma[k] * delta_sigma;

		}else{

	
			for(j=0; j<M; j++){

                delta_sigma = delta_sigma + ((videos->getData(b)->X[t][j] * model->Theta0[j] * model->Theta[k][j]) / (model->Tau[b][j][t] * (model->Tau[b][j][t] - model->Sigma[k] * model->Theta[k][j])));
			}
            delta_sigma = delta_sigma - (videos->getData(b)->N[t] / (model->Nu[b][t] * (model->Nu[b][t] - model->Sigma[k])));
            delta_sigma = model->Sigma[k] * delta_sigma;

		}

	}

	return delta_sigma;

}


void HillClimber::setFixedTheta(bool x){

    fixedTheta = x;
}

void HillClimber::setFixedOmega(bool x){

    fixedOmega = x;
}

void HillClimber::setFixedSigma(bool x){

    fixedSigma = x;
}

void HillClimber::setFixedSpontaneous(bool x){

    fixedSpontaneous= x;
}
