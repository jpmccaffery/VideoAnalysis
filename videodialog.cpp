#include "videodialog.h"
#include "ui_videodialog.h"

VideoDialog::VideoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoDialog),
    vplayer(new Phonon::VideoWidget(this)),
    media(new Phonon::MediaObject(this)),
    mseconds(0),
    seconds(0),
    totalSeconds(0),
    manualChange(true)
{
    ui->setupUi(this);
    ui->verticalLayout->insertWidget(0, vplayer, 1);

    Phonon::createPath(media, vplayer);

    media->setTickInterval(500);

    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui->restartButton, SIGNAL(clicked()), this, SLOT(restart()));
    connect(media, SIGNAL(tick(qint64)), this, SLOT(updateSlider(qint64)));

    connect(media, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(updateTimer(Phonon::State, Phonon::State)));
    connect(ui->timeSlider, SIGNAL(valueChanged(int)), this, SLOT(moveBySlider(int)));
}

VideoDialog::~VideoDialog()
{
    delete ui;
}

void VideoDialog::setVideo(QString src){

    media->setCurrentSource(src);
}

void VideoDialog::play(){

    media->play();
}

void VideoDialog::pause(){

    media->pause();
}

void VideoDialog::restart(){

    media->stop();
    media->play();
    media->pause();
    mseconds = 0;
    seconds = 0;
    ui->timeSlider->setValue(0);
    ui->timeElapsed->setText(QString::number(0));
}

void VideoDialog::updateSlider(qint64 tickTime){

    manualChange = false;

    mseconds = tickTime;
    seconds = (tickTime / 1000.0);

    ui->timeElapsed->setText(QString::number(seconds));
    ui->timeSlider->setValue(seconds);

    manualChange = true;
}

void VideoDialog::updateTimer(Phonon::State, Phonon::State){

    totalSeconds = (media->totalTime() / 1000.0);
    ui->totalTime->setText(" /  " + QString::number(totalSeconds));
    ui->timeSlider->setMaximum(totalSeconds);
}

void VideoDialog::moveBySlider(int t){

    if(manualChange){
        media->seek(t * 1000);
        seconds = t;
        mseconds = t * 1000;
        ui->timeElapsed->setText(QString::number(seconds));
    }
}
