#include "videowidget.h"
#include "ui_videowidget.h"
#include "VideoData.h"

VideoWidget::VideoWidget(ProjectWidget *p, MainWindow *q) :
    ProjectWidget(p, q),
    ui(new Ui::VideoWidget),
    video(new VideoDialog(this)),
    generating(false),
    videoName("")
{

    thumbnail = new ClickableImage(this);

    ui->setupUi(this);
    ui->comboBox->addItem("None");
    ui->comboBox->addItem("HSV10 Dom ");
    ui->comboBox->addItem("Edges Bin ");

    connect(ui->comboBox, SIGNAL(activated(QString)), this, SLOT(setValue(QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(generateClick()));
    connect(thumbnail, SIGNAL(clicked()), video, SLOT(show()));

    ui->thumbBox->insertWidget(0, thumbnail, 1);
    title = "None";

    tickMax = 100;
    tickCount = 0;

    setEnabled(false);

}

VideoWidget::~VideoWidget()
{
    delete ui;
}


void VideoWidget::setText(QString s){

    ui->label->setText(s);
}

void VideoWidget::makeAlive(QString s){

    setEnabled(true);

    ui->comboBox->setEnabled(true);
    thumbnail->setEnabled(true);
    ui->pushButton->setEnabled(false);
}


void VideoWidget::setValue(QString s){

    QString x, y;
    int a;

    y = ui->label->text();
    a = y.indexOf(": ");
    x = y.mid(0, a+2);
    x = x + s;

    ui->label->setText(x);
    title = s;
    if(s != "None"){
        ui->pushButton->setEnabled(true);
        emit valueSet(s);
    }
}


void VideoWidget::generateClick(){

    generating = true;
    emit generateVideo();
}

void VideoWidget::setVideo(QString s){

    qDebug() << "Setting the video : " << s;
    if(generating){
        videoName = s;
        video->setVideo(getProjectName() + "/videos/" + s + ".avi");
        thumbnail->setImage(getProjectName() + "/thumbs/" + s + "_thumbnail.png");
        ui->progressBar->setValue(100);
        generating = false;
    }
}

void VideoWidget::setProgress(int k){

    if(generating){
        ui->progressBar->setValue(k);
    }
}

void VideoWidget::reset(){

    ui->comboBox->setCurrentIndex(0);
    ui->comboBox->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->progressBar->setValue(0);

    thumbnail->setEnabled(false);
    thumbnail->setImage(getProjectName() + "/thumbs/baseThumb.png");
}

void VideoWidget::update(QString filename, int featureType){

    makeAlive("");

    ui->comboBox->setCurrentIndex(0);
    if(featureType == VideoData::EDGE_PRESENCE){
        ui->comboBox->setCurrentIndex(2);
    }
    if(featureType == VideoData::DOMINANT_HSV10){
        ui->comboBox->setCurrentIndex(1);
    }
    ui->comboBox->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->progressBar->setValue(100);

    thumbnail->setEnabled(true);
    generating = true;
    setVideo(filename);
    generating = false;

}

void VideoWidget::setTickMax(int x){

    tickMax = x;
}
void VideoWidget::tickUp(int x){

    double z;

    tickCount = x;
    if(x < 0){

        ui->progressBar->setValue(0);
    }else{

        z = (double) tickCount / (double) tickMax;
        z = z * 100.0;
        ui->progressBar->setValue((int) z);
    }
}
