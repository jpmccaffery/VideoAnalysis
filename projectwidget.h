#ifndef PROJECTWIDGET_H
#define PROJECTWIDGET_H

#include <QWidget>


class MainWindow;

class ProjectWidget : public QWidget
{
    Q_OBJECT

public:
    ProjectWidget(ProjectWidget *p = 0, MainWindow *q = 0);
    ~ProjectWidget();
    QString getProjectName();

private:
    ProjectWidget *parent;
    MainWindow *mainParent;
    QString projectName;

};

#endif // PROJECTWIDGET_H
