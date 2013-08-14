#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QFileDialog"
#include "videoloadwidget.h"
#include "VideoData.h"
#include "LearningModel.h"
#include "learningwindow.h"
#include <topicwindow.h>
#include "datacollection.h"

class VideoWidget;

namespace Ui{

    class MainWindow;
}

class VideoWidget;


class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString getWindowTitle();
    QString getProjectName();

public slots:

    void resetVideos();
    void newProject(QString);
    void loadProject(QString);
    void saveConf();
    void loadConf();
    void exportProject(QString pName);
    void save();
    void setTickMax(int x);
    void tickUp(int x);

    void featureTickHandler(int index, int k);
    void featureTickMaxHandler(int index, int k);

private slots:
    void on_actionLoad_Project_triggered();
    void on_actionNew_Project_triggered();

    void on_actionSave_triggered();

    void on_actionVideoExport_triggered();

    void on_actionModelExport_triggered();

    void on_actionExport_Project_triggered();

    void on_actionVideoLoad_triggered();
    void loadVideo(QString s);
    void loadVideos(QStringList s);
    void loadModel(QString s);
    void saveDataAs(QString s);
    void saveModelAs(QString s);
    void fastSaveDataAs(QString s);
    void slowLoadData(QString s);

    void on_actionSaveModel_triggered();

    void on_actionSaveModel_As_triggered();

    void on_actionSaveVideo_triggered();

    void on_actionModelLoad_triggered();

    void on_actionSaveData_As_triggered();

    void on_actionBatch_Test_triggered();

    void on_actionNewVideo_triggered();

    void on_actionNewModel_triggered();

    void on_actionFastSaveData_triggered();

    void on_actionFastSaveData_As_triggered();

    void on_actionSlowLoadData_triggered();

private:
    Ui::MainWindow *ui;
    QFileDialog *fileDialog;
    VideoWidget *vwidget1, *vwidget2, *vwidget3, *vwidget4;
    VideoLoadWidget *loadWidget;

    LearningModel *model;

    LearningWindow *leftWindow;
    TopicWindow *centralWindow;

    QString projectName;
    double totalTicks;
    double currentTicks;

    void updateVideoWidgets();
    void prepareData(vector < QString > classNames, vector < QString > modelNames);
    void prepareData_linearized(vector < QString > classNames, vector < QString > modelNames);
    void batchLearning(vector < QString > classNames);

    void confusionMatrix();
};

#endif // MAINWINDOW_H
