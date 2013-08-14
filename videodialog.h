#ifndef VIDEODIALOG_H
#define VIDEODIALOG_H

#include <QDialog>
#include "phonon/videowidget.h"
#include "phonon/MediaObject"

namespace Ui {
class VideoDialog;
}

class VideoDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit VideoDialog(QWidget *parent = 0);
    ~VideoDialog();
    
private:
    Ui::VideoDialog *ui;
    Phonon::VideoWidget *vplayer;
    Phonon::MediaObject *media;
    double mseconds;
    int seconds;
    int totalSeconds;
    bool manualChange;

public slots:
    void setVideo(QString src);
    void play();
    void pause();
    void restart();
    void updateSlider(qint64 tickTime);
    void updateTimer(Phonon::State, Phonon::State);
    void moveBySlider(int t);

};

#endif // VIDEODIALOG_H
