#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDebug"
#include "QDir"
#include "videowidget.h"
#include "DataManager.h"
#include "QProcess"
#include "stdlib.h"
#include "libsvm/svm.h"
#include "fstream"

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    totalTicks = 0;
    currentTicks = 0;

    projectName = "test";
    setWindowTitle(projectName);

    qDebug() << windowTitle();

    vwidget1 = new VideoWidget(0, this);
    vwidget2 = new VideoWidget(0, this);
    vwidget3 = new VideoWidget(0, this);
    vwidget4 = new VideoWidget(0, this);

    ui->setupUi(this);

    model = new LearningModel(0, this);
    loadWidget = new VideoLoadWidget(&(model->videos), 0, this);
    leftWindow = new LearningWindow(model, this);
    centralWindow = new TopicWindow(model, 0, this);

    centralWindow->setK(5);

    vwidget1->setText("Feature 1 : None");
    vwidget2->setText("Feature 2 : None");
    vwidget3->setText("Feature 3 : None");
    vwidget4->setText("Feature 4 : None");
    ui->gridLayout_3->addWidget(vwidget1, 0, 0);
    ui->gridLayout_3->addWidget(vwidget2, 0, 1);
    ui->gridLayout_3->addWidget(vwidget3, 1, 0);
    ui->gridLayout_3->addWidget(vwidget4, 1, 1);

    ui->layoutRight->insertWidget(0, loadWidget, 1);
    ui->layoutLeft->insertWidget(0, leftWindow, 1);
    ui->layoutMiddle->insertWidget(0, centralWindow, 1);

    vwidget1->makeAlive("");

    // Connect each video dock to the next one for enabling
    connect(vwidget1, SIGNAL(valueSet(QString)), vwidget2, SLOT(makeAlive(QString)));
    connect(vwidget2, SIGNAL(valueSet(QString)), vwidget3, SLOT(makeAlive(QString)));
    connect(vwidget3, SIGNAL(valueSet(QString)), vwidget4, SLOT(makeAlive(QString)));

    // Connect each dock to the video data set for when we set the feature types
    connect(vwidget1, SIGNAL(valueSet(QString)), &(model->videos), SLOT(setFeature1(QString)));
    connect(vwidget2, SIGNAL(valueSet(QString)), &(model->videos), SLOT(setFeature2(QString)));
    connect(vwidget3, SIGNAL(valueSet(QString)), &(model->videos), SLOT(setFeature3(QString)));
    connect(vwidget4, SIGNAL(valueSet(QString)), &(model->videos), SLOT(setFeature4(QString)));

    connect(vwidget1, SIGNAL(generateVideo()), &(model->videos), SLOT(generateFeatureVideo1()));
    connect(vwidget2, SIGNAL(generateVideo()), &(model->videos), SLOT(generateFeatureVideo2()));
    connect(vwidget3, SIGNAL(generateVideo()), &(model->videos), SLOT(generateFeatureVideo3()));
    connect(vwidget4, SIGNAL(generateVideo()), &(model->videos), SLOT(generateFeatureVideo4()));

    connect(&(model->videos), SIGNAL(featureTick(int, int)), this, SLOT(featureTickHandler(int, int)));
    connect(&(model->videos), SIGNAL(featureTickMax(int, int)), this, SLOT(featureTickMaxHandler(int, int)));

    connect(&(model->videos), SIGNAL(featureVideoDone(QString)), vwidget1, SLOT(setVideo(QString)));
    connect(&(model->videos), SIGNAL(featureVideoDone(QString)), vwidget2, SLOT(setVideo(QString)));
    connect(&(model->videos), SIGNAL(featureVideoDone(QString)), vwidget3, SLOT(setVideo(QString)));
    connect(&(model->videos), SIGNAL(featureVideoDone(QString)), vwidget4, SLOT(setVideo(QString)));

    connect(&(model->videos), SIGNAL(globalTickMax(int)), this, SLOT(setTickMax(int)));
    connect(&(model->videos), SIGNAL(globalTick(int)), this, SLOT(tickUp(int)));

    connect(&(model->videos), SIGNAL(workTickMax(int)), loadWidget, SLOT(setTickMax(int)));
    connect(&(model->videos), SIGNAL(workTick(int)), loadWidget, SLOT(tickUp(int)));

    connect(model, SIGNAL(tickMax(int)), this, SLOT(masterTickMax(int)));
    connect(model, SIGNAL(tick(int)), this, SLOT(masterTick(int)));

    connect(loadWidget, SIGNAL(generating()), &(model->videos), SLOT(generateCounts()));

    connect(loadWidget, SIGNAL(nameSet(QString, QString)), &(model->videos), SLOT(setFName(QString, QString)));
    connect(loadWidget, SIGNAL(namesSet(QStringList, QStringList)), &(model->videos), SLOT(setFNames(QStringList, QStringList)));

    connect(loadWidget, SIGNAL(updateBinSize(int)), &(model->videos), SLOT(setBinSize(int)));
    connect(loadWidget, SIGNAL(updateBlockWidth(int)), &(model->videos), SLOT(setBlockWidth(int)));
    connect(loadWidget, SIGNAL(updateBlockHeight(int)), &(model->videos), SLOT(setBlockHeight(int)));

    connect(loadWidget, SIGNAL(videoReset()), this, SLOT(resetVideos()));

    connect(leftWindow, SIGNAL(changeK(int)), centralWindow, SLOT(setK(int)));

    model->videos.setBlockHeight(3);
    model->videos.setBlockWidth(3);
    model->videos.setBinSize(5);

    connect(leftWindow, SIGNAL(topicsDrawn()), centralWindow, SLOT(update()));


    // Create the project directory
    QDir *directory = new QDir("./");
    if(!directory->entryList().contains(projectName)){

        QString s;
        s = "cp ./baseThumb.png ./" + projectName + "/thumbs/";

        directory->mkdir(projectName);
        directory->cd(projectName);
        directory->mkdir("thumbs");
        directory->mkdir("data");
        directory->mkdir("videos");
        system(s.toStdString().c_str());
        directory->cd("..");
    }
    delete directory;

    //    connect(ui->menuFile, SIGNAL(triggered(QAction*)), this, SLOT(testSlot(QAction*)));


    model->setDataName("testModel");
//    saveConf();
    loadConf();


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetVideos(){

    vwidget1->reset();
    vwidget2->reset();
    vwidget3->reset();
    vwidget4->reset();

    vwidget1->makeAlive("");
    updateGeometry();
}


void MainWindow::on_actionLoad_Project_triggered()
{

    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(loadProject(QString)));

    newDialog->setFilter("Dissertation (*.conf)");
    newDialog->setDirectory("./");
    newDialog->show();
}

void MainWindow::loadProject(QString s){


    s = s.right(s.length() - s.lastIndexOf("/") - 1);
    s = s.left(s.lastIndexOf("."));
    qDebug() << s;

    if(s != projectName){

        projectName = s;

        loadConf();
    }



}


void MainWindow::on_actionNew_Project_triggered()
{

    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(newProject(QString)));

    newDialog->setFilter("Dissertation (*.conf)");
    newDialog->setDirectory("./");
    newDialog->show();
}

void MainWindow::newProject(QString s){


    s = s.right(s.length() - s.lastIndexOf("/") - 1);

    QString temp;

    // Create the project directory
    QDir directory("./");
    if(!directory.entryList().contains(s)){

        directory.mkdir(s);
        directory.cd(s);
        directory.mkdir("thumbs");
        directory.mkdir("data");
        directory.mkdir("videos");

        temp = "cp ./" + projectName + "/thumbs/baseThumb.png ./" + s + "/thumbs/";
        system(temp.toStdString().c_str());

        temp = "touch ./" + s + "/" + s + ".conf";
        system(temp.toStdString().c_str());

        directory.cd("..");
    }

    projectName = s;

    resetVideos();
    loadWidget->reset();    //This will reset the videodata
    leftWindow->reset();    //This will reset the learning model
    centralWindow->reset();

    model->videos.reset();
    model->videos.setBatchName(projectName);
    model->setDataName(projectName);
}

void MainWindow::saveConf(){

    QFile file(getProjectName() + "/" + getProjectName() + ".conf");
    file.open(QIODevice::WriteOnly);

    QTextStream stream(&file);

    stream << "VideoData : ";
    stream << model->videos.getBatchName() << "\n";

    stream << "ModelName : ";
    stream << model->getDataName() << "\n";

    stream << "BatchSize : ";
    stream << model->videos.getBatchSize() << "\n";

    stream.flush();
    file.close();


}

void MainWindow::loadConf(){


    QFile file(getProjectName() + "/" + getProjectName() + ".conf");
    file.open(QIODevice::ReadOnly);

    QString a;
    QDir dir(getProjectName() + "/data/");

    QTextStream stream(&file);

    a = stream.readLine();
    model->videos.setBatchName(a.split(" : ").last());

    a = stream.readLine();
    model->setDataName(a.split(" : ").last());

    a = stream.readLine();
    model->videos.setBatchSize(a.toInt());


    currentTicks = 0;
    totalTicks = 0;
    setTickMax(1);
    tickUp(0);


    // Load the video data and model

    if(model->videos.checkForData()){
        totalTicks = totalTicks + model->videos.getLoadTotal();
    }else{
        model->videos.reset();
        loadWidget->setEnabled(true);
    }

    if(dir.entryList().contains(model->getDataName() + "_model.db")){
        qDebug() << "Modeldata found";
        totalTicks = totalTicks + model->model.getLoadTotal();
    }else{
        model->initialize();
    }

    if(model->videos.checkForData()){

        model->videos.load();
        loadWidget->setFName(model->videos.getFName());
        loadWidget->setFType(model->videos.getFType());
        loadWidget->update();
        loadWidget->setBlockWidth(model->videos.getBlockWidth());
        loadWidget->setBlockHeight(model->videos.getBlockHeight());
        loadWidget->setBinSize(model->videos.getBinSize());
        loadWidget->setEnabled(false);
        loadWidget->setM(model->videos.getM());
    }
    if(dir.entryList().contains(model->getDataName() + "_model.db")){

        model->loadModel();
    }

    // Load the video widget thumbnails where used
    updateVideoWidgets();
    leftWindow->update();
    centralWindow->update();
    loadWidget->update();



    tickUp(-1);


    file.close();

}

void MainWindow::on_actionSave_triggered()
{

    qDebug() << "Saving";
    saveConf();
    save();
}

QString MainWindow::getProjectName(){

    return projectName;
}

// I don't think we're using this anymore
void MainWindow::on_actionVideoExport_triggered()
{

    currentTicks = 0.0;
    totalTicks = 1.0;
    tickUp(0);

    totalTicks = model->videos.getSaveTotal();
    model->videos.save(getProjectName() + "/data/testData2.db");

    tickUp(-1);
}

void MainWindow::tickUp(int k){

    currentTicks = k;
    ui->progressBar->setValue((currentTicks / totalTicks) * 100.0);

    if(k == -1){
        ui->progressBar->setValue(100);
    }

}

void MainWindow::setTickMax(int k){

    totalTicks = max(1, k);

}

//I don't think we're using this anymore
void MainWindow::on_actionModelExport_triggered()
{
    currentTicks = 0.0;
    totalTicks = 1.0;
    tickUp(0);

    totalTicks = model->model.getSaveTotal();
    model->saveModel(getProjectName() + "/data/testModel2.db");

    tickUp(-1);
}


void MainWindow::on_actionExport_Project_triggered()
{


    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(exportProject(QString)));

    newDialog->setFilter("Dissertation (*.conf)");
    newDialog->setDirectory("./");
    newDialog->show();


}

void MainWindow::exportProject(QString pName){

    QString s;

    pName = pName.right(pName.length() - pName.lastIndexOf("/") - 1);

    s = "cp ./" + projectName + " -r ./" + pName;
    system(s.toStdString().c_str());

    s = "rm ./" + pName + "/" + projectName + ".conf";
    system(s.toStdString().c_str());

    s = projectName;
    projectName = pName;

    save();

    projectName = s;

}


void MainWindow::save(){

    saveConf();

    totalTicks = model->model.getSaveTotal() + model->videos.getSaveTotal();
    currentTicks = 0;
    tickUp(0);

    model->saveModel();
    model->videos.save();

    tickUp(-1);
}

void MainWindow::updateVideoWidgets(){


    QDir directory("./" + projectName + "/thumbs/");
    QString name;

    name = model->videos.getFName() + "_" + model->videos.getFeatureFileName(model->videos.getFeature1());
    if(directory.entryList().contains(name + "_thumbnail.png")){
        vwidget1->update(name, model->videos.getFeature1());
    }else{
        vwidget1->reset();
        vwidget1->makeAlive("");
    }

    name = model->videos.getFName() + "_" + model->videos.getFeatureFileName(model->videos.getFeature2());
    if(directory.entryList().contains(name + "_thumbnail.png")){
        vwidget2->update(name, model->videos.getFeature2());
    }else{

        vwidget2->reset();
    }

    name = model->videos.getFName() + "_" + model->videos.getFeatureFileName(model->videos.getFeature3());
    if(directory.entryList().contains(name + "_thumbnail.png")){
        vwidget3->update(name, model->videos.getFeature3());
    }else{
        vwidget3->reset();
    }

    name = model->videos.getFName() + "_" + model->videos.getFeatureFileName(model->videos.getFeature4());
    if(directory.entryList().contains(name + "_thumbnail.png")){
        vwidget4->update(name, model->videos.getFeature4());
    }else{
        vwidget4->reset();
    }
}

void MainWindow::on_actionVideoLoad_triggered()
{


    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(loadVideo(QString)));
    connect(newDialog, SIGNAL(filesSelected(QStringList)), this, SLOT(loadVideos(QStringList)));

    newDialog->setFileMode(QFileDialog::ExistingFiles);
    newDialog->setDirectory("./" + projectName + "/data/");
    newDialog->setFilter("Text Data (*.data)");
    newDialog->show();

}


//It's really important that data files follow the _data.db naming convention.
//If they don't then it won't load correctly.
void MainWindow::loadVideo(QString s){

    QString command;

    command = "cp " + s + " " + "./" + projectName + "/data/";

    system(command.toStdString().c_str());

    s = s.right(s.length() - s.lastIndexOf("/") - 1);

    if(s.lastIndexOf("_") > 0){
        s = s.left(s.lastIndexOf("_"));
    }else{
        s = s.left(s.lastIndexOf("."));
    }


    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);


    model->videos.setDataName(s);
    totalTicks = totalTicks + model->videos.getLoadTotal();
    model->videos.load();

    loadWidget->setFName(model->videos.getFName());
    loadWidget->setFType(model->videos.getFType());
    loadWidget->update();
    loadWidget->setBlockWidth(model->videos.getBlockWidth());
    loadWidget->setBlockHeight(model->videos.getBlockHeight());
    loadWidget->setBinSize(model->videos.getBinSize());
    loadWidget->setEnabled(false);
    loadWidget->setM(model->videos.getM());

    // Load the video widget thumbnails where used
    updateVideoWidgets();
    leftWindow->update();
    centralWindow->update();
    loadWidget->update();

    tickUp(-1);
}


//It's really important that data files follow the _data.db naming convention.
//If they don't then it won't load correctly.
void MainWindow::loadVideos(QStringList s){

    QString command;

    int batchSize = s.size();
    int i;
    QString a;
    QStringList list;

    for(i=0; i < batchSize; i++){

        a = s.at(i);

        command = "cp " + a + " " + "./" + projectName + "/data/";

        system(command.toStdString().c_str());

        a = a.right(a.length() - a.lastIndexOf("/") - 1);
        a = a.left(a.lastIndexOf("_"));

        list.push_front(a);
    }


    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);

    model->videos.setDataNames(list);
    totalTicks = totalTicks + model->videos.getLoadTotal();
    model->videos.load();

    loadWidget->setFName(model->videos.getFName());
    loadWidget->setFType(model->videos.getFType());
    loadWidget->update();
    loadWidget->setBlockWidth(model->videos.getBlockWidth());
    loadWidget->setBlockHeight(model->videos.getBlockHeight());
    loadWidget->setBinSize(model->videos.getBinSize());
    loadWidget->setEnabled(false);
    loadWidget->setM(model->videos.getM());

    // Load the video widget thumbnails where used
    updateVideoWidgets();
    leftWindow->update();
    centralWindow->update();
    loadWidget->update();

    tickUp(-1);
}




void MainWindow::on_actionSaveVideo_triggered()
{
    currentTicks = 0;
    totalTicks = model->videos.getSaveTotal();
    tickUp(0);
    model->videos.save();
    tickUp(-1);
}


void MainWindow::on_actionSaveModel_triggered()
{
    currentTicks = 0;
    totalTicks = model->model.getSaveTotal();
    tickUp(0);
    model->saveModel();
    tickUp(-1);
}

void MainWindow::on_actionModelLoad_triggered()
{

    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(loadModel(QString)));

    newDialog->setDirectory("./" + projectName + "/data/");
    newDialog->setFilter("Database (*.db)");
    newDialog->show();

}

void MainWindow::loadModel(QString s){


    QString command;

    command = "cp " + s + " " + "./" + projectName + "/data/";

    system(command.toStdString().c_str());

    s = s.right(s.length() - s.lastIndexOf("/") - 1);
    s = s.left(s.lastIndexOf("_"));

    model->setDataName(s);

    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);

    totalTicks = totalTicks + model->model.getLoadTotal();
    model->loadModel();

    // Load the video widget thumbnails where used
    leftWindow->update();
    centralWindow->update();
    tickUp(-1);

}

void MainWindow::on_actionSaveData_As_triggered()
{
    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(saveDataAs(QString)));

    newDialog->setFilter("Database (*.db)");
    newDialog->setDirectory("./" + projectName + "/data/");
    newDialog->show();
}

void MainWindow::saveDataAs(QString s){

    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);

    totalTicks = totalTicks + model->videos.getSaveTotal();

    model->videos.save(s);

    tickUp(-1);
}


void MainWindow::on_actionSaveModel_As_triggered()
{
    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(saveModelAs(QString)));

    newDialog->setFilter("Database (*.db)");
    newDialog->setDirectory("./" + projectName + "/data/");
    newDialog->show();
}

void MainWindow::saveModelAs(QString s){

    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);

    totalTicks = totalTicks + model->model.getSaveTotal();

    model->saveModel(s);

    tickUp(-1);
}



void MainWindow::on_actionNewVideo_triggered()
{

    this->loadWidget->reset();
}

void MainWindow::on_actionNewModel_triggered()
{

    this->leftWindow->reset();
}





void MainWindow::on_actionFastSaveData_As_triggered()
{
    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(fastSaveDataAs(QString)));

    newDialog->setFilter("Text Data (*.data)");
    newDialog->setDirectory("./" + projectName + "/data/");
    newDialog->show();
}

void MainWindow::fastSaveDataAs(QString s){

    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);

    totalTicks = totalTicks + model->videos.getFastSaveTotal();

    model->videos.fastSave(s);

    tickUp(-1);
}



void MainWindow::on_actionFastSaveData_triggered()
{
    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);

    totalTicks = totalTicks + model->videos.getFastSaveTotal();

    model->videos.fastSave();

    tickUp(-1);

}



void MainWindow::on_actionSlowLoadData_triggered()
{


    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(loadVideo(QString)));
    connect(newDialog, SIGNAL(filesSelected(QStringList)), this, SLOT(loadVideos(QStringList)));

    newDialog->setFileMode(QFileDialog::ExistingFiles);
    newDialog->setDirectory("./" + projectName + "/data/");
    newDialog->setFilter("Text Data (*.db)");
    newDialog->show();

}


//It's really important that data files follow the _data.db naming convention.
//If they don't then it won't load correctly.
void MainWindow::slowLoadData(QString s){

    QString command;
    QByteArray ba;
    char *name;

    command = "cp " + s + " " + "./" + projectName + "/data/";

    ba = command.toLocal8Bit();
    name = ba.data();
    system(name);

    s = s.right(s.length() - s.lastIndexOf("/") - 1);

    if(s.lastIndexOf("_") > 0){
        s = s.left(s.lastIndexOf("_"));
    }else{
        s = s.left(s.lastIndexOf("."));
    }


    currentTicks = 0;
    totalTicks = 1;
    tickUp(0);


    model->videos.setDataName(s);
    totalTicks = totalTicks + model->videos.getLoadTotal();
    model->videos.slowLoad();

    loadWidget->setFName(model->videos.getFName());
    loadWidget->setFType(model->videos.getFType());
    loadWidget->update();
    loadWidget->setBlockWidth(model->videos.getBlockWidth());
    loadWidget->setBlockHeight(model->videos.getBlockHeight());
    loadWidget->setBinSize(model->videos.getBinSize());
    loadWidget->setEnabled(false);
    loadWidget->setM(model->videos.getM());

    // Load the video widget thumbnails where used
    updateVideoWidgets();
    leftWindow->update();
    centralWindow->update();
    loadWidget->update();

    tickUp(-1);
}









//********************************************************************************************
//
// Batch testing code. Not always useful
//
//********************************************************************************************

void MainWindow::on_actionBatch_Test_triggered()
{

    int numberClasses = 6;
    vector < QString > classNames;
    vector < QString > baseClassNames;
    vector < QString > modelNames;


//    model->videos.generateFeatureHighlights(model->getTopicModel(), 5, model->bestZChain(0), 1);
//    return;


    qDebug() << "First Batch";

    classNames.resize(numberClasses);
    baseClassNames.resize(numberClasses);
    modelNames.resize(numberClasses);
    classNames[0] = "waving";
    classNames[1] = "clapping";
    classNames[2] = "running";
    classNames[3] = "walking";
    classNames[4] = "jogging";
    classNames[5] = "boxing";

    batchLearning(classNames);


    for(int i=0; i < numberClasses; i++){

        modelNames[i] = classNames[i] + "_model.db";
        baseClassNames[i] = classNames[i];
    }
    setWindowTitle("First Batch (svm)");
    repaint();


    for(int i=0; i < numberClasses; i++){

        classNames[i] = baseClassNames[i] + "_training";
    }

    prepareData_linearized(classNames, modelNames);


    for(int i=0; i < numberClasses; i++){

        classNames[i] = baseClassNames[i];
    }
    setWindowTitle("Second Batch (regular)");
    repaint();
    prepareData_linearized(classNames, modelNames);


    for(int i=0; i < numberClasses; i++){

        classNames[i] = baseClassNames[i] + "_svm";
    }
    setWindowTitle("Third Batch (Training)");
    repaint();
    prepareData_linearized(classNames, modelNames);


    confusionMatrix();
    return;

}

void MainWindow::prepareData(vector < QString > classNames, vector < QString > modelNames){



    int i, j, k;

    QString s;
    int numberClasses;

    numberClasses = classNames.size();

    vector < QDir > classPaths;
    classPaths.resize(numberClasses);
    for(i=0; i < numberClasses; i++){
        classPaths[i].setPath("./" + projectName + "/data/" + classNames[i] + "/");
    }

    vector < vector < vector < double > > > classLikelihoods;
    classLikelihoods.resize(numberClasses);

    vector < vector < int > > classifications;
    classifications.resize(numberClasses);

    vector < vector < bool > > correctness;
    correctness.resize(numberClasses);

    vector < int > batchSizes;
    batchSizes.resize(numberClasses);


    // So that's ll[i][j][k]
    // i = class type
    // j = entry in test data
    // k = model tested on

    for(i=0; i < numberClasses; i++){

        batchSizes[i] = classPaths[i].entryList().size() - 2;
        classLikelihoods[i].resize(batchSizes[i]);
        classifications[i].resize(batchSizes[i]);
        correctness[i].resize(batchSizes[i]);

        for(j=0; j < batchSizes[i]; j++){

            classLikelihoods[i][j].resize(numberClasses);
        }
    }

    for(i=0; i < numberClasses; i++){


        loadModel("./" + projectName + "/data/" + modelNames[i]);

        for(j=0; j < numberClasses; j++){

            for(k=0; k < batchSizes[j]; k++){


                s = classPaths[j].entryList().at(k+2);
                s = "./" + projectName + "/data/" + classNames[j] + "/" + s;
                loadVideo(s);

                leftWindow->updateLL();
                classLikelihoods[j][k][i] = leftWindow->getLL();
            }
        }
    }



    QFile file("./" + projectName + "/svm/svm_" + classNames[0] + "_data");
    file.open(QIODevice::WriteOnly);

    QTextStream fileStream(&file);

    for(i=0; i < numberClasses; i++){

        for(j=0; j < batchSizes[i]; j++){

            fileStream << i;

            for(k=0; k < numberClasses; k++){

                fileStream << " " << k << ":" << classLikelihoods[i][j][k];

            }
            if(! ( i == (numberClasses-1) && j == (batchSizes[i]-1)) ){

                fileStream << "\n";
            }
        }
    }
    fileStream.flush();
    file.close();

}



void MainWindow::prepareData_linearized(vector < QString > classNames, vector < QString > modelNames){



    int i, j, k, m;

    QString s;
    int numberClasses;

    vector < vector < vector < double > > > maxLikelihoods;
    double partialLikelihood = 0;
    double divisor = 0;


    numberClasses = classNames.size();

    vector < QDir > classPaths;
    classPaths.resize(numberClasses);
    for(i=0; i < numberClasses; i++){
        classPaths[i].setPath("./" + projectName + "/data/" + classNames[i] + "/");
    }

    vector < vector < vector < vector < double > > > > classLikelihoods;
    classLikelihoods.resize(numberClasses);
    maxLikelihoods.resize(numberClasses);

    vector < double > tempLikelihood;
    tempLikelihood.resize(model->getS());

    vector < vector < int > > classifications;
    classifications.resize(numberClasses);

    vector < vector < bool > > correctness;
    correctness.resize(numberClasses);

    vector < int > batchSizes;
    batchSizes.resize(numberClasses);


    // So that's ll[i][j][k]
    // i = class type
    // j = entry in test data
    // k = model tested on
    // m = sample term for the sum

    for(i=0; i < numberClasses; i++){

        batchSizes[i] = classPaths[i].entryList().size() - 2;
        classLikelihoods[i].resize(batchSizes[i]);
        maxLikelihoods[i].resize(batchSizes[i]);
        classifications[i].resize(batchSizes[i]);
        correctness[i].resize(batchSizes[i]);

        for(j=0; j < batchSizes[i]; j++){

            classLikelihoods[i][j].resize(numberClasses);
            maxLikelihoods[i][j].resize(numberClasses);

            for(k=0; k < numberClasses; k++){

                classLikelihoods[i][j][k].resize(model->getS());
            }
        }
    }


    for(i=0; i < numberClasses; i++){


        loadModel("./" + projectName + "/data/" + modelNames[i]);

        for(j=0; j < numberClasses; j++){

            for(k=0; k < batchSizes[j]; k++){


                s = classPaths[j].entryList().at(k+2);
                s = "./" + projectName + "/data/" + classNames[j] + "/" + s;
                loadVideo(s);

                leftWindow->updateLL();
                tempLikelihood = model->log_linear_likelihood();


                maxLikelihoods[j][k][i] = tempLikelihood[0];

                for(m=0; m < model->getS(); m++){

                    classLikelihoods[j][k][i][m] = tempLikelihood[m];

                    if(tempLikelihood[m] > maxLikelihoods[j][k][i]){

                        maxLikelihoods[j][k][i] = tempLikelihood[m];
                    }
                }
            }
        }
    }


    for(i=0; i < numberClasses; i++){

        for(j=0; j < numberClasses; j++){

            for(k=0; k < batchSizes[j]; k++){

                for(m=0; m < model->getS(); m++){

                    // So here we're dividing all the terms for a given j,k,i by a constant to make them exponentiable
                    classLikelihoods[j][k][i][m] = classLikelihoods[j][k][i][m] - maxLikelihoods[j][k][i];
                }
            }
        }
    }


    for(i=0; i < numberClasses; i++){

        for(j=0; j < numberClasses; j++){

            for(k=0; k < batchSizes[j]; k++){

                classLikelihoods[j][k][i][0] = exp(classLikelihoods[j][k][i][0]);
                for(m=1; m < model->getS(); m++){

                    // Then we add all the P terms for a given j, k, i to get the marginalization
                    classLikelihoods[j][k][i][0] = classLikelihoods[j][k][i][0] + exp(classLikelihoods[j][k][i][m]);
                }

                // Then log likelihood it back down and multiply back in the scaling constant
                classLikelihoods[j][k][i][0] = log(classLikelihoods[j][k][i][0]) + maxLikelihoods[j][k][i];
            }
        }
    }



    qDebug() << "Writing to file";


    QFile file("./" + projectName + "/svm/svm_" + classNames[0] + "_data");
    file.open(QIODevice::WriteOnly);

    QTextStream fileStream(&file);

    for(i=0; i < numberClasses; i++){

        for(j=0; j < batchSizes[i]; j++){

            fileStream << i;

            for(k=0; k < numberClasses; k++){

                fileStream << " " << k << ":" << classLikelihoods[i][j][k][0];

            }
            if(! ( i == (numberClasses-1) && j == (batchSizes[i]-1)) ){

                fileStream << "\n";
            }
        }
    }
    fileStream.flush();
    file.close();

}




void MainWindow::batchLearning(vector < QString > classNames){


    int numberClasses = classNames.size();
    int trainingSize = 20;
    int svmTrainingSize = 20;
    QDir directory(".");
    QString status;

    int randSpot, randCap;

    QProcess process;

/*

    // This section flips all the videos vertically
    // Built for a specific data set

    directory.setNameFilters(QStringList("*.avi"));

    // flip the data
    for(int i=0; i < numberClasses; i++){

        status = "Flipping Data : " + classNames[i] + " (" + QString::number(i+1) + " / " + QString::number(numberClasses) + ")";
        setWindowTitle(status);
        this->repaint();

        directory.setPath(projectName + "/videos/" + classNames[i] + "/");
        loadWidget->reset();
        loadWidget->changeAddress("/home/jack");

        for(int j=0; j < directory.entryList().size(); j++){

            model->videos.getData(0)->flipVideo(directory.entryList().at(j), classNames[i]);
        }
    }

*/

    directory.setNameFilters(QStringList("*.avi"));

/*

    // preprocess the data
    for(int i=0; i < numberClasses; i++){

        QStringList dataList;


        status = "Generating Data : " + classNames[i] + " (" + QString::number(i+1) + " / " + QString::number(numberClasses) + ")";
        setWindowTitle(status);
        this->repaint();

        directory.setPath(projectName + "/videos/" + classNames[i] + "/");
        qDebug() << directory.path();
        qDebug() << directory.entryList();

        loadWidget->reset();

        for(int j=0; j < directory.entryList().size(); j++){

            dataList.append("./" + projectName + "/videos/" + classNames[i] + "/" + directory.entryList().at(j));
        }


        loadWidget->changeAddresses(dataList);

        model->videos.setFeature1("Edges Bin ");

        loadWidget->generate();


        loadWidget->makeDead();

        process.execute("mkdir ./" + projectName + "/data/" + classNames[i]);
        process.waitForFinished(-1);

        fastSaveDataAs("./" + projectName + "/data/" + classNames[i] + "/" + classNames[i]);
    }


*/

    directory.setNameFilters(QStringList("*.data"));


    for(int i=3; i < numberClasses; i++){

        QStringList trainingFiles;

        status = "Training Model : " + classNames[i] + " (" + QString::number(i+1) + " / " + QString::number(numberClasses) + ")";
        setWindowTitle(status);
        repaint();

        process.execute("mkdir ./" + projectName + "/data/" + classNames[i] + "_training");
        process.waitForFinished(-1);

        directory.setPath(projectName + "/data/" + classNames[i]);


        trainingFiles.clear();

        // Build training set
        for(int j=0; j < trainingSize; j++){

            QString sourcePath, targetPath, objectName;

            qDebug() << directory.entryList();

            randCap = directory.entryList().size();
            randSpot = rand() % randCap;

            objectName = directory.entryList().at(randSpot);
            sourcePath = "./" + projectName + "/data/" + classNames[i] + "/";
            targetPath = "./" + projectName + "/data/" + classNames[i] + "_training/";

            process.execute("cp " + sourcePath + objectName + " " + targetPath);
            process.waitForFinished(-1);

            process.execute("rm " + sourcePath + objectName);
            process.waitForFinished(-1);

            trainingFiles.append(targetPath + objectName);

            directory.refresh();
            qDebug() << directory.entryList();


        }

        loadVideos(trainingFiles);

        leftWindow->reset();
        leftWindow->setFixedSigma(true);

//        model->reset();

        leftWindow->setRunning();
        model->setDataName(classNames[i]);
        model->runLoop(60);

        saveModelAs("./" + projectName + "/data/" + classNames[i] + "_model.db");

    }



    for(int i=0; i < numberClasses; i++){


        directory.setPath(projectName + "/data/" + classNames[i]);

        process.execute("mkdir ./" + projectName + "/data/" + classNames[i] + "_svm");
        process.waitForFinished(-1);

        // Build training set
        for(int j=0; j < svmTrainingSize; j++){

            QString sourcePath, targetPath, objectName;

            qDebug() << directory.currentPath();
            qDebug() << directory.entryList();


            randCap = directory.entryList().size();
            randSpot = rand() % randCap;

            objectName = directory.entryList().at(randSpot);
            sourcePath = "./" + projectName + "/data/" + classNames[i] + "/";
            targetPath = "./" + projectName + "/data/" + classNames[i] + "_svm/";

            process.execute("cp " + sourcePath + objectName + " " + targetPath);
            process.waitForFinished(-1);

            process.execute("rm " + sourcePath + objectName);
            process.waitForFinished(-1);

            directory.refresh();

        }
    }
}


void MainWindow::confusionMatrix(){


    svm_model *smodel;
    svm_node *datapoint;
    double *datapointbuffer;
    double prediction;
    double category;
    QString path = "./" + projectName + "/svm/testScaled";
    char colon;
    int index;
    int count;

    //goes class, prediction so [0][2] is how many times 0 was classified as 2;

    double average[6][6];
    double total[6][6];
    char newline = '\n';

    vector < int > classMaps;
    classMaps.resize(6);



    classMaps[0] = 3;
    classMaps[1] = 4;
    classMaps[2] = 2;
    classMaps[3] = 5;
    classMaps[4] = 1;
    classMaps[5] = 0;



    ifstream infile;

    infile.open(path.toStdString().c_str());

    path = "./" + projectName + "/svm/model";
    smodel = svm_load_model(path.toStdString().c_str());
    datapointbuffer = new double[6];




    if(!infile.is_open()){

        qDebug() << "failed to open file";
        return;
    }

    for(int i=0; i<6; i++){
        for(int j=0; j<6; j++){

            average[i][j] = 0;
            total[i][j] = 0;
        }
    }


    for(int i=0; i < 1000; i++){

        for(int j=0; j < 6; j++){
            datapointbuffer[j] = -1;
        }
        count = 0;

        if(!(infile >> category)){
            break;
        }

        infile >> colon;

        while(1){

//            qDebug() << "data";
//            qDebug() << colon;
            index = colon-48;
//            qDebug() << index;

            infile >> colon;


            infile >> datapointbuffer[index];
            count++;

            colon = infile.get();

//            qDebug() << colon;
            colon = infile.get();

//            qDebug() << colon;
            if(colon == newline){

                datapoint = new svm_node[count+1];
                count = 0;
                for(int j=0; j < 6; j++){

                    if(datapointbuffer[j] > -1){
                        datapoint[count].index = j;
                        datapoint[count].value = datapointbuffer[j];
                        count++;
                    }
                }

                datapoint[count].index = -1;
                break;
            }
        }

/*
        for(int j=0; j < count+1; j++){
            qDebug() << datapoint[j].index << " :  " << datapoint[j].value;
        }
*/
        prediction = svm_predict(smodel, datapoint);
        qDebug() << "is " << category << " : " << " prediction " << prediction ;

        total[(int)category][0] = total[(int)category][0] + 1;
        average[(int)category][(int)prediction] = average[(int)category][(int)prediction] + 1;


        delete [] datapoint;
    }


    for(int i=0; i<6; i++){

        path = QString::number(i) + " : ";

        for(int j=0; j<6; j++){

            average[classMaps[i]][classMaps[j]] = average[classMaps[i]][classMaps[j]] / total[classMaps[i]][0];
            path = path + QString::number(average[classMaps[i]][classMaps[j]]) + "\t\t ";
        }
        qDebug() << path;
    }


    delete [] datapointbuffer;

}

void MainWindow::featureTickHandler(int index, int k){

    switch(index){

    case 0:
        vwidget1->tickUp(k);
        break;
    case 1:
        vwidget2->tickUp(k);
        break;
    case 2:
        vwidget3->tickUp(k);
        break;
    case 3:
        vwidget4->tickUp(k);
        break;
    }
}

void MainWindow::featureTickMaxHandler(int index, int k){

    switch(index){

    case 0:
        vwidget1->setTickMax(k);
        break;
    case 1:
        vwidget2->setTickMax(k);
        break;
    case 2:
        vwidget3->setTickMax(k);
        break;
    case 3:
        vwidget4->setTickMax(k);
        break;
    }
}




