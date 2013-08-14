#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "clickableimage.h"
#include "videodialog.h"
#include "projectwidget.h"


namespace Ui {
class VideoWidget;
}

class VideoWidget : public ProjectWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(ProjectWidget *p=0, MainWindow *q=0);
    ~VideoWidget();
    void setText(QString s);
    QString windowTitle();
    void update(QString filename, int featureType);

public slots:
    void makeAlive(QString s);
    void setValue(QString s);
    void generateClick();
    void setVideo(QString s);
    void setProgress(int k);
    void reset();

    void tickUp(int x);
    void setTickMax(int x);

signals:
    void valueSet(QString s);
    void generateVideo();

private:
    Ui::VideoWidget *ui;
    ClickableImage *thumbnail;
    VideoDialog *video;
    QString title;
    QString videoName;
    bool generating;

    int tickMax;
    int tickCount;

};

#endif // VIDEOWIDGET_H
