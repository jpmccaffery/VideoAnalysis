#ifndef SAMPLER
#define SAMPLER

#include "Z_Chain.h"
#include "TopicModel.h"
#include "VideoData.h"
#include <QObject>
#include "vector"

class DataCollection;

class Sampler : public QObject{

    Q_OBJECT

public:

    //***************************************************
    // Data
    //***************************************************

    Z_Chain chain;
    vector < vector < vector < bool > > > Z_temp;

    int B, K, S;
    vector < int > T;

    //***************************************************
    // Methods
    //***************************************************

    Sampler(int numberSamples);
	~Sampler();
	
    void sample(TopicModel &m, DataCollection &v);
    void initialize(TopicModel &m, DataCollection &v);

signals:

    void sampleTick(int);
    void sampleTickMax(int);

private:

    static const double max_wait = 0.2 * 1000000.0;



};

#endif
