#include "videoloadwidget.h"
#include "ui_videoloadwidget.h"
#include "QDebug"
#include "QPixmap"
#include "VideoData.h"
#include "projectwidget.h"
#include "mainwindow.h"
#include "videodialog.h"
#include "datacollection.h"

VideoLoadWidget::VideoLoadWidget(DataCollection *d, ProjectWidget *p, MainWindow *q) :
    ProjectWidget(p, q),
    ui(new Ui::VideoLoadWidget),
    path(""),
    fname(""),
    ftype(""),
    fileDialog(new QFileDialog(this)),
    saveDialog(new QFileDialog(this)),
    loadDialog(new QFileDialog(this)),
    videoDialog(new VideoDialog(this)),
    thumb(new ClickableImage(this))
{

    ui->setupUi(this);
    ui->mainBox->insertWidget(1, thumb, 1);

    fileDialog->setWindowTitle("Load Video");
    fileDialog->setFilter("Video (*.avi *.mpg *.mpeg)");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setDirectory("./" + getProjectName() + "/");

    saveDialog->setWindowTitle("Save Video Data");
    saveDialog->setFilter("Data (*.db)");
    saveDialog->setAcceptMode(QFileDialog::AcceptSave);
    saveDialog->setDirectory("./" + getProjectName() + "/data/");

    loadDialog->setWindowTitle("Load Video Data");
    loadDialog->setFilter("Data (*.db)");
    loadDialog->setDirectory("./" + getProjectName() + "/data/");

    data = d;

    connect(ui->browseButton, SIGNAL(clicked()), fileDialog, SLOT(open()));
    connect(fileDialog, SIGNAL(fileSelected(QString)), this, SLOT(changeAddress(QString)));
    connect(fileDialog, SIGNAL(filesSelected(QStringList)), this, SLOT(changeAddresses(QStringList)));
    connect(ui->generateButton, SIGNAL(clicked()), this, SLOT(generate()));
    connect(ui->generateButton, SIGNAL(clicked()), this, SLOT(makeDead()));
    connect(thumb, SIGNAL(clicked()), this, SLOT(popupVideo()));
    connect(ui->binBox, SIGNAL(valueChanged(int)), data, SIGNAL(setBinSize(int)));
    connect(ui->widthBox, SIGNAL(valueChanged(int)), data, SIGNAL(setBlockWidth(int)));
    connect(ui->heightBox, SIGNAL(valueChanged(int)), data, SIGNAL(setBlockHeight(int)));

    connect(ui->resetButton, SIGNAL(clicked()), this, SLOT(reset()));

    tickCount = 0;
    tickMax = 100;
}

VideoLoadWidget::~VideoLoadWidget()
{
    delete ui;
}


void VideoLoadWidget::changeAddress(QString s){

    int a, b;

    char *name;
    QByteArray ba;
    QString systemout = "";

    a = s.lastIndexOf("/");
    b = s.lastIndexOf(".");

    //fileDialog->hide();

    fname = s.mid(a+1, (b-(a+1)));
    ftype = s.mid(b+1,-1);
    path = s;
    ui->label->setText(fname + "." + ftype);


    systemout = "cp " + s + " ./" + getProjectName() + "/";
    ba = systemout.toLocal8Bit();
    name = ba.data();
    system(name);

    emit nameSet(fname, ftype);

    VideoData::generateThumbnail("./" + getProjectName() + "/" + fname + "." + ftype, "./" + getProjectName() + "/thumbs/" + fname + "_thumbnail.png");
    thumb->setImage("./" + getProjectName() + "/thumbs/" + fname + "_thumbnail.png");
}


void VideoLoadWidget::changeAddresses(QStringList list){


    int a, b;
    int batchSize;
    int i;

    QStringList names, types;
    QString s;

    char *name;
    QByteArray ba;
    QString systemout = "";


    batchSize = list.size();

    for(i=0; i < batchSize; i++){

        s = list.at(i);

        a = s.lastIndexOf("/");
        b = s.lastIndexOf(".");

        //fileDialog->hide();

        fname = s.mid(a+1, (b-(a+1)));
        ftype = s.mid(b+1,-1);


        if(i == 0){
            path = s;
            ui->label->setText(fname + "." + ftype);
        }

        systemout = "cp " + s + " ./" + getProjectName() + "/";
        ba = systemout.toLocal8Bit();
        name = ba.data();
        system(name);

        names.push_front(fname);
        types.push_front(ftype);

    }

    fname = names.at(0);
    ftype = types.at(0);

    emit namesSet(names, types);
    VideoData::generateThumbnail("./" + getProjectName() + "/" + fname + "." + ftype, "./" + getProjectName() + "/thumbs/" + fname + "_thumbnail.png");
    thumb->setImage("./" + getProjectName() + "/thumbs/" + fname + "_thumbnail.png");
}


void VideoLoadWidget::generate(){

    emit generating();
}

void VideoLoadWidget::popupVideo(){


    QString src = fname + "." + ftype;

    if(fname != ""){
        videoDialog->show();
        videoDialog->setVideo(getProjectName() + "/" + src);
    }
}

void VideoLoadWidget::setM(int m){

//    thumb->resizeEvent();
//    qDebug() << thumb->sizeHint();
//    qDebug() << thumb->minimumSizeHint();
//    qDebug() << thumb->sizePolicy();
//    thumb->updateGeometry();
    ui->MCount->setText(QString::number(m));

//    thumb->sizeHint();
}


void VideoLoadWidget::resizeEvent(QResizeEvent *e){

    thumb->shrink(thumb->width(), thumb->height());

}


void VideoLoadWidget::reset(){

    data->reset();

    ui->widthBox->setEnabled(true);
    ui->binBox->setEnabled(true);
    ui->heightBox->setEnabled(true);
    ui->generateButton->setEnabled(true);
    ui->browseButton->setEnabled(true);

    ui->widthBox->setValue(3);
    ui->binBox->setValue(5);
    ui->heightBox->setValue(3);
    ui->progressBar->setValue(0);
    thumb->reset();

    ui->MCount->setText("0");

    emit videoReset();

    update();

}

void VideoLoadWidget::makeDead(){


    ui->widthBox->setEnabled(false);
    ui->binBox->setEnabled(false);
    ui->heightBox->setEnabled(false);
    ui->generateButton->setEnabled(false);
    ui->browseButton->setEnabled(false);
//    ui->loadButton->setEnabled(false);
//    ui->saveButton->setEnabled(false);
}

void VideoLoadWidget::setEnabled(bool flag){

    ui->binBox->setEnabled(flag);
    ui->browseButton->setEnabled(flag);
    ui->generateButton->setEnabled(flag);
    ui->heightBox->setEnabled(flag);
    ui->widthBox->setEnabled(flag);

}

void VideoLoadWidget::update(){

//    qDebug() << "Updating";

    QDir directory("./" + getProjectName() + "/");
    if(directory.entryList().contains(fname + "." + ftype)){

        ui->label->setText("/" + fname + "." + ftype);
    }else{

        ui->label->setText("No video loaded");
    }

    directory.setPath("./" + getProjectName() + "/thumbs/");
//    qDebug() << "./" + getProjectName() + "/thumbs/";
//    qDebug() << directory.entryList();
//    qDebug() << fname;
    if(directory.entryList().contains(fname + "_thumbnail.png")){

//        qDebug() << "Loading thumbnail";
        thumb->setImage("./" + getProjectName() + "/thumbs/" + fname + "_thumbnail.png");
    }else{
        thumb->setImage("./" + getProjectName() + "/thumbs/baseThumb.png");

    }
}

void VideoLoadWidget::setFName(QString s){

    fname = s;
}

void VideoLoadWidget::setFType(QString s){

    ftype = s;
}

void VideoLoadWidget::setBlockWidth(int k){

    ui->widthBox->setValue(k);
}
void VideoLoadWidget::setBlockHeight(int k){

    ui->heightBox->setValue(k);
}
void VideoLoadWidget::setBinSize(int k){

    ui->binBox->setValue(k);
}
void VideoLoadWidget::setTickMax(int x){

    tickMax = x;
}
void VideoLoadWidget::tickUp(int x){

    double z;

    tickCount = x;
    if(x < 0){

        ui->progressBar->setValue(100);
    }else{

        z = (double) tickCount / (double) tickMax;
        z = z * 100.0;
        ui->progressBar->setValue((int) z);
    }
}
