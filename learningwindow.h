#ifndef LEARNINGWINDOW_H
#define LEARNINGWINDOW_H

#include <QWidget>
#include <LearningModel.h>
#include <QFileDialog>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <TopicModel.h>

namespace Ui {
class LearningWindow;
}

class LearningWindow : public QWidget
{
    Q_OBJECT
    
public:


    static const int K0 = 5;
    // Must do at least 100 samples
    static const double thetaRate0 = 0.01;
    static const double omegaRate0 = 0.01;
    static const double momentum0 = 0.1;
    static const bool fixedTheta0 = false;
    static const bool fixedOmega0 = false;
    static const bool fixedSigma0 = false;
    static const bool fixedSpontaneous0 = true;

    explicit LearningWindow(LearningModel *m, QWidget *parent = 0);
    ~LearningWindow();
    void initialize();
    QString windowTitle();

    void reset();
    void lockK();
    void unlockK();
    void update();

    double getLL();
    int getS();

private:
    Ui::LearningWindow *ui;

    /******************************************************
      Initial values
      ********************************************************/

    int K;

    QFileDialog *loadDialog;
    QFileDialog *saveDialog;
    QString useFile;

    QwtPlot *plot;
    QwtPlotCurve *plotCurve;

    double *x;
    double *y;
    int iterations;

    LearningModel *model;
    QWidget *wParent;

signals:

    void getVideo();
    void topicsDrawn();
    void changeK(int);

public slots:

    void setLL(double causal_t, double visual_t);
    void setFile(QString s);
    void loadModel();
    void saveModel();
    void setK(int k);
    void setRunning();
    void setPaused();
    void clearData();
    void clearData(double z);
    void updateLL();

    void setFixedTheta(bool);
    void setFixedOmega(bool);
    void setFixedSigma(bool);
//    void setFixedSpontaneous(bool);

    void mTickMax(int x);
    void eTickMax(int x);
    void mTickUp(int x);
    void eTickUp(int x);

private:

    bool fixedTheta;
    bool fixedOmega;
    bool fixedSigma;
    bool fixedSpontaneous;

    int ecount;
    int mcount;

    int emax;
    int mmax;

};

#endif // LEARNINGWINDOW_H
