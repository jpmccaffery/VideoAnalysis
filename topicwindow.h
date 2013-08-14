#ifndef TOPICWINDOW_H
#define TOPICWINDOW_H

#include <QWidget>
#include <clickableimage.h>
#include <QString>
#include <topicdialog.h>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <TopicModel.h>
#include "projectwidget.h"
#include "LearningModel.h"

namespace Ui {
class TopicWindow;
}

class TopicWindow : public ProjectWidget
{
    Q_OBJECT
    
public:
    explicit TopicWindow(LearningModel *m, ProjectWidget *p = 0, MainWindow *q = 0);
    ~TopicWindow();

    void reset();
    
private:
    Ui::TopicWindow *ui;
    ClickableImage **thumbNails;
    int K;
    QString fname;
    TopicDialog *dialog;
    QLabel **labels;
    QVBoxLayout **layouts;
    QWidget **widgets;
    LearningModel *model;

public slots:

    void update();
    void setK(int k);
    void setName(QString s);
    void showDialog(int i);

};

#endif // TOPICWINDOW_H
