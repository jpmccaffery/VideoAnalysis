#include "learningwindow.h"
#include "ui_learningwindow.h"
#include "QStringList"
#include <QPointF>

LearningWindow::LearningWindow(LearningModel *m, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LearningWindow),
    loadDialog(new QFileDialog(this)),
    saveDialog(new QFileDialog(this))
{
    ui->setupUi(this);
    model = m;
    initialize();

    fixedTheta = fixedTheta0;
    fixedOmega = fixedOmega0;
    fixedSigma = fixedSigma0;
    fixedSpontaneous = fixedSpontaneous0;

    useFile = "";

    plotCurve = new QwtPlotCurve();
    plot = new QwtPlot(this);
    iterations = 0;

    ecount = 0;
    mcount = 0;

    emax = 100;
    mmax = 100;

    x = new double[1];
    y = new double[1];

    x[0] = 0;
    y[0] = 0;

    plotCurve->setRawSamples(x, y, 1);
    plotCurve->attach(plot);

    plot->setAxisMaxMajor(0, 3);
    plot->setAxisMaxMajor(2, 3);

    plot->replot();
    plot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    plot->setMinimumHeight(165);

    ui->verticalLayout->insertWidget(1, plot, 1);

    saveDialog->setNameFilter("Models (*.db)");
    saveDialog->setWindowTitle("Save Topic Model");
    saveDialog->setAcceptMode(QFileDialog::AcceptSave);
    loadDialog->setNameFilter("Models (*.db)");
    loadDialog->setWindowTitle("Load Topic Model");

    // Connect the numeric boxes
    connect(ui->KBox, SIGNAL(valueChanged(int)), this, SLOT(setK(int)));

    connect(model, SIGNAL(changeThetaRate(double)), ui->thetaRateBox, SLOT(setValue(double)));
    connect(ui->thetaRateBox, SIGNAL(valueChanged(double)), model, SLOT(setThetaRate(double)));

    connect(model, SIGNAL(changeOmegaRate(double)), ui->omegaBox, SLOT(setValue(double)));
    connect(ui->omegaBox, SIGNAL(valueChanged(double)), model, SLOT(setOmegaRate(double)));

    connect(model, SIGNAL(changeMomentum(double)), ui->momentumBox, SLOT(setValue(double)));
    connect(ui->momentumBox, SIGNAL(valueChanged(double)), model, SLOT(setMomentum(double)));

    connect(model, SIGNAL(setStatus(QString)), ui->statusLabel, SLOT(setText(QString)));
    connect(model, SIGNAL(changeLL(double, double)), this, SLOT(setLL(double, double)));

    // connect the buttons
    connect(ui->startButton, SIGNAL(clicked()), model, SLOT(start()));
    connect(ui->pauseButton, SIGNAL(clicked()), model, SLOT(pauseRun()));
    connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(updateLL()));

    connect(ui->startButton, SIGNAL(clicked()), this, SLOT(setRunning()));
    connect(model, SIGNAL(pausing()), this, SLOT(setPaused()));

    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearData()));
    connect(ui->randomButton, SIGNAL(clicked()), model, SLOT(randomize()));

    // connect the progress bars
    connect(model, SIGNAL(eTick(int)), this, SLOT(eTickUp(int)));
    connect(model, SIGNAL(mTick(int)), this, SLOT(mTickUp(int)));

    connect(model, SIGNAL(eTickMax(int)), this, SLOT(eTickMax(int)));
    connect(model, SIGNAL(mTickMax(int)), this, SLOT(mTickMax(int)));

    connect(model, SIGNAL(topicsDrawn()), this, SIGNAL(topicsDrawn()));    

    connect(ui->fixedOmega, SIGNAL(clicked(bool)), this, SLOT(setFixedOmega(bool)));
    connect(ui->fixedTheta, SIGNAL(clicked(bool)), this, SLOT(setFixedTheta(bool)));
    connect(ui->fixedSigma, SIGNAL(clicked(bool)), this, SLOT(setFixedSigma(bool)));

    wParent = parent;
}

LearningWindow::~LearningWindow()
{
    delete ui;
    delete [] x;
    delete [] y;
}

QString LearningWindow::windowTitle(){

    return wParent->windowTitle();
}

void LearningWindow::initialize(){

    K = K0;

    ui->momentumBox->setValue(momentum0);
    ui->thetaRateBox->setValue(thetaRate0);
    ui->omegaBox->setValue(omegaRate0);

    ui->KBox->setValue(K0);

    model->setMaxSteps(20);
    model->setK(K0);
    model->setThetaRate(thetaRate0);
    model->setOmegaRate(omegaRate0);
    model->setMomentum(momentum0);

    model->setFixedTheta(fixedTheta0);
    model->setFixedOmega(fixedOmega0);
    model->setFixedSigma(fixedSigma0);
    model->setFixedSpontaneous(fixedSpontaneous0);
}

// This updates the log likelihood list and replots
void LearningWindow::setLL(double causal_t, double visual_t){

    double *newY;
    int i;

    iterations++;

    newY = y;
    y = new double[iterations];
    delete [] x;
    x = new double[iterations];

    for(i=0; i < iterations-1; i++){

        x[i] = i+1;
        y[i] = newY[i];
    }

    x[iterations-1] = iterations;
    y[iterations-1] = causal_t + visual_t;


    if(iterations == 2 && y[0] == 0.0){

        y[0] = causal_t + visual_t;
    }

    plotCurve->setRawSamples(x, y, iterations);
    plot->replot();
    delete [] newY;

    ui->causalLabel->setText("Causal : " + QString::number(causal_t));
    ui->visualLabel->setText("Visual : " + QString::number(visual_t));
    ui->logData->setText(QString::number(causal_t + visual_t));

}

double LearningWindow::getLL(){

    if(y != 0){

        return y[iterations - 1];
    }

    return 0;
}

void LearningWindow::updateLL(){

    pair < double, double > logs;

    model->E_Step();
    logs = model->log_likelihood();
    setLL(logs.first, logs.second);
    model->drawTopics();
}

void LearningWindow::loadModel(){

    model->loadModel(useFile);
}

void LearningWindow::saveModel(){

    model->saveModel(useFile);
}


void LearningWindow::setFile(QString s){

    useFile = s;
}

void LearningWindow::setK(int k){

    if(k != K){
        K = k;
        model->setK(k);
        emit changeK(k);
    }
}

void LearningWindow::setRunning(){

    ui->momentumBox->setEnabled(false);
    ui->omegaBox->setEnabled(false);
    ui->randomButton->setEnabled(false);
    ui->updateButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->thetaRateBox->setEnabled(false);
    ui->clearButton->setEnabled(false);
    ui->fixedOmega->setEnabled(false);
    ui->fixedTheta->setEnabled(false);
    ui->fixedSigma->setEnabled(false);
}


void LearningWindow::setPaused(){

    ui->momentumBox->setEnabled(true);
    ui->omegaBox->setEnabled(true);
    ui->randomButton->setEnabled(true);
    ui->updateButton->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->thetaRateBox->setEnabled(true);
    ui->clearButton->setEnabled(true);
    ui->fixedOmega->setEnabled(true);
    ui->fixedTheta->setEnabled(true);
    ui->fixedSigma->setEnabled(true);
}

void LearningWindow::clearData(){

    clearData(-101);
}

void LearningWindow::clearData(double z){

    double y0;

    if(z == -101){
        y0 = y[iterations -1];
    }else{
        y0 = z;
    }
    delete [] y;
    y = new double[1];
    delete [] x;
    x = new double[1];

    x[0] = 1;
    y[0] = y0;
    iterations = 1;

    plotCurve->setRawSamples(x, y, iterations);
    plot->replot();

    ui->logData->setText(QString::number(y0));
}

void LearningWindow::setFixedTheta(bool x){

    fixedTheta = x;
    model->setFixedTheta(x);
}

void LearningWindow::setFixedOmega(bool x){

    fixedTheta = x;
    model->setFixedOmega(x);
}

void LearningWindow::setFixedSigma(bool x){

    fixedTheta = x;
    model->setFixedSigma(x);
}

void LearningWindow::reset(){

    K = K0;

    ui->KBox->setValue(K0);

    fixedTheta = false;
    fixedOmega = false;
    fixedSigma = true;

    ui->fixedTheta->setChecked(fixedTheta);
    ui->fixedOmega->setChecked(fixedOmega);
    ui->fixedSigma->setChecked(fixedSigma);
    ui->fixedSigma->setChecked(fixedSpontaneous);

    ui->EBar->setValue(0);
    ui->MBar->setValue(0);

    ui->statusLabel->setText("Uninitialized");

    clearData(0.0);
    setPaused();

    unlockK();
    initialize();
}

void LearningWindow::update(){


    setPaused();

    if(model->isInitialized()){

        K = model->K;
        lockK();
    }else{

        K = K0;
        unlockK();
        initialize();
    }

    ui->EBar->setValue(0);
    ui->MBar->setValue(0);

    ui->statusLabel->setText("Uninitialized");

    clearData(0.0);

}

void LearningWindow::lockK(){

    ui->KBox->setEnabled(false);
    ui->updateButton->setEnabled(true);
    ui->randomButton->setEnabled(true);
}


void LearningWindow::unlockK(){

    ui->KBox->setEnabled(true);
    ui->updateButton->setEnabled(false);
    ui->randomButton->setEnabled(false);
}

void LearningWindow::eTickMax(int x){

    emax = x;
}

void LearningWindow::mTickMax(int x){

    mmax = x;
}

void LearningWindow::eTickUp(int x){

    double z;

    if(x < 0){

        ui->EBar->setValue(100);
    }else{

        ecount = x;

        z = (double) ecount / (double) emax;
        z = z * 100.0;

        ui->EBar->setValue((int) z);
    }
}


void LearningWindow::mTickUp(int x){

    double z;

    if(x < 0){

        ui->MBar->setValue(100);
    }else{

        mcount = x;

        z = (double) mcount / (double) mmax;
        z = z * 100.0;

        ui->MBar->setValue((int) z);
    }
}



