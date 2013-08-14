#ifndef PROJECTTHREAD_H
#define PROJECTTHREAD_H

#include <QThread>


class MainWindow;

class ProjectThread: public QThread
{
    Q_OBJECT

public:
    ProjectThread(ProjectThread *p = 0, MainWindow *q = 0);
    ~ProjectThread();
    QString getProjectName();

private:
    ProjectThread *parent;
    MainWindow *mainParent;
    QString projectName;

};

#endif // PROJECTTHREAD_H
