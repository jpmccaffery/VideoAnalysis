#ifndef TOPICDIALOG_H
#define TOPICDIALOG_H

#include <QDialog>
#include <clickableimage.h>
#include <QLabel>
#include <TopicModel.h>
#include "LearningModel.h"

namespace Ui {
class TopicDialog;
}

class ProjectWidget;

class TopicDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TopicDialog(LearningModel *m, ProjectWidget *p = 0);
    ~TopicDialog();
    void setImage(QString s);
    void setLabels();
    void setLocalTopic(int k);
    void setK(int k);

public slots:
    void saveStarted();
    void loadStarted();
    void saveCompleted(QString);
    void loadCompleted(QString);

private:
    Ui::TopicDialog *ui;
    ClickableImage *picture;
    QLabel **causeNames;
    QLabel **causeValues;
    QLabel **effectNames;
    QLabel **effectValues;
    QLabel *sigmaName;
    QLabel *sigmaValue;
    int K;
    LearningModel *model;
    int localTopic;
    int lastSize;

    ProjectWidget *parent;
};

#endif // TOPICDIALOG_H
