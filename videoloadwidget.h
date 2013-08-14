#ifndef VIDEOLOADWIDGET_H
#define VIDEOLOADWIDGET_H

#include <QWidget>
#include "QFileDialog"
#include <VideoData.h>
#include "projectwidget.h"

class MainWindow;
class ClickableImage;
class VideoDialog;
class DataCollection;

namespace Ui {
class VideoLoadWidget;
}

class VideoLoadWidget : public ProjectWidget
{
    Q_OBJECT
    
public:
    explicit VideoLoadWidget(DataCollection *d, ProjectWidget *p = 0, MainWindow *q = 0);
    ~VideoLoadWidget();
    void resizeEvent(QResizeEvent *e);
    QString windowTitle();

    void setEnabled(bool flag);
    void update();

    void setFName(QString s);
    void setFType(QString s);
    void setBlockWidth(int k);
    void setBlockHeight(int k);
    void setBinSize(int k);

public slots:

    void changeAddress(QString s);
    void changeAddresses(QStringList s);
    void generate();
    void popupVideo();
    void setM(int m);
    void reset();
    void makeDead();

    void setTickMax(int x);
    void tickUp(int x);

signals:
    void generating();
    void nameSet(QString, QString);
    void namesSet(QStringList, QStringList);
    void updateBinSize(int);
    void updateBlockWidth(int);
    void updateBlockHeight(int);
    void videoReset();

private:
    Ui::VideoLoadWidget *ui;
    QFileDialog *fileDialog;
    QString path, fname, ftype;
    VideoDialog *videoDialog;
    ClickableImage *thumb, *thumb2, *thumb3, *thumb4;

    QFileDialog *saveDialog;
    QFileDialog *loadDialog;

    DataCollection *data;
    QWidget *wParent;

    int tickMax;
    int tickCount;
};

#endif // VIDEOLOADWIDGET_H
