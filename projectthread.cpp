#include "projectthread.h"
#include "mainwindow.h"

ProjectThread::ProjectThread(ProjectThread *p, MainWindow *q)
{
    if(q){

        QThread(q);
    }else{

        QThread(p);
    }

    parent = p;
    mainParent = q;



    projectName = "";

}

ProjectThread::~ProjectThread()
{
}

QString ProjectThread::getProjectName(){

    if(parent){

        return parent->getProjectName();
    }

    return mainParent->getProjectName();
}
