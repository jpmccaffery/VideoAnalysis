#include "topicwindow.h"
#include "ui_topicwindow.h"
#include <QDebug>
#include <QFont>
#include <QDir>

TopicWindow::TopicWindow(LearningModel *m, ProjectWidget *p, MainWindow *q) :
    ProjectWidget(p, q),
    ui(new Ui::TopicWindow)
{
    ui->setupUi(this);

    K = -1;
    fname = "default";
    thumbNails = 0;
    labels = 0;
    layouts = 0;
    widgets = 0;

    dialog = new TopicDialog(m, this);

    model = m;
}

TopicWindow::~TopicWindow()
{
    delete ui;
}

void TopicWindow::setK(int k){

    int i;
    int rowWidth = 2;
    QFont font("Arial", 14, QFont::Bold);

    dialog->setK(k);

    if(K != k){

//        qDebug() << K;
        if(K > -1){
            for(i=0; i < K+1; i++){

//                ui->gridLayout->removeWidget(thumbNails[i]);
                ui->gridLayout->removeWidget(widgets[i]);
                layouts[i]->removeWidget(thumbNails[i]);
                layouts[i]->removeWidget(labels[i]);
                delete thumbNails[i];
                delete labels[i];
                delete widgets[i];
            }
            delete [] thumbNails;
            delete[] layouts;
            delete [] labels;
        }
        K = k;
        thumbNails = new ClickableImage*[K+1];
        layouts = new QVBoxLayout*[K+1];
        labels = new QLabel*[K+1];
        widgets= new QWidget*[K+1];

        for(i=0; i < K+1; i++){

            widgets[i] = new QWidget(this);

            thumbNails[i] = new ClickableImage(this);
            thumbNails[i]->setName("topic_" + QString::number(i));
            thumbNails[i]->setIndex(i);

            layouts[i] = new QVBoxLayout(widgets[i]);
            labels[i] = new QLabel(this);
            if(i == 0){
                labels[i]->setText("Background");
            }else{
                labels[i]->setText("   Topic " + QString::number(i) + "   ");
            }
            labels[i]->setAlignment(Qt::AlignCenter);
            labels[i]->setFont(font);

            layouts[i]->insertWidget(0, labels[i], 1);
            layouts[i]->insertWidget(0, thumbNails[i], 1);
            layouts[i]->insertSpacing(2, 100);

            labels[i]->setFrameShadow(QFrame::Raised);
            labels[i]->setFrameShape(QFrame::Box);
            labels[i]->setLineWidth(3);

            labels[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);


            connect(thumbNails[i], SIGNAL(clicked(int)), this, SLOT(showDialog(int)));
//            ui->gridLayout->addWidget(thumbNails[i], i/rowWidth, i%rowWidth);
            ui->gridLayout->addWidget(thumbNails[i], 2 * (i/rowWidth), i%rowWidth);
            ui->gridLayout->addWidget(labels[i], 2 * (i/rowWidth) + 1, i%rowWidth);

        }
    }

}

void TopicWindow::setName(QString s){

    fname = s;
}


void TopicWindow::update(){

    int i;
    QString name, path;

    path = "./" + getProjectName() + "/thumbs/";

    QDir *directory = new QDir(path);

    name = model->getDataName() + "_topic_" + QString::number(0) + ".png";
//    qDebug() << name;

    if(directory->entryList().contains(name)){
        thumbNails[0]->setImage(path + name);
    }else{
        name = "baseThumb.png";
        thumbNails[0]->setImage(path + name);
    }

    for(i=0; i < K; i++){

        name = model->getDataName() + "_topic_" + QString::number(i+1) + ".png";
        if(directory->entryList().contains(name)){
            thumbNails[i+1]->setImage(path + name);
        }else{
            name = "baseThumb.png";
            thumbNails[i+1]->setImage(path + name);
        }
    }

    delete directory;
}


void TopicWindow::showDialog(int i){

    if(i == 0){
        dialog->setWindowTitle(fname + " : Background");
    }else{
        dialog->setWindowTitle(fname + " : Topic " + QString::number(i));
    }
    dialog->setImage(getProjectName() + "/thumbs/" + model->getDataName() + "_topic_" + QString::number(i) + ".png");
    dialog->setLocalTopic(i);
    dialog->setLabels();
    dialog->show();
}


void TopicWindow::reset(){

    update();
}
