#include "projectwidget.h"
#include "ui_projectwidget.h"
#include "mainwindow.h"

ProjectWidget::ProjectWidget(ProjectWidget *p, MainWindow *q)
{
    if(q){

        QWidget(q);
    }else{

        QWidget(p);
    }

    parent = p;
    mainParent = q;



    projectName = "";

}

ProjectWidget::~ProjectWidget()
{
}

QString ProjectWidget::getProjectName(){

    if(parent){

        return parent->getProjectName();
    }

    return mainParent->getProjectName();
}
