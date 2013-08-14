#include "topicdialog.h"
#include "ui_topicdialog.h"
#include <QFont>
#include "projectwidget.h"
#include "QDebug"
#include "QFileDialog"

TopicDialog::TopicDialog(LearningModel *m, ProjectWidget *p) :
    QDialog(p),
    ui(new Ui::TopicDialog)
{
    ui->setupUi(this);

    parent = p;
    K = -1;
    localTopic = -1;
    lastSize = 0;
    causeNames = 0;
    causeValues= 0;
    effectNames = 0;
    effectValues= 0;

    model = m;

    picture = new ClickableImage(parent);
    ui->horizontalLayout->insertWidget(1, picture, 1);

    ui->leftForm->setAlignment(Qt::AlignTop);
    ui->rightForm->setAlignment(Qt::AlignTop);

    sigmaName = new QLabel(this);
    sigmaValue = new QLabel(this);

    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveStarted()));
    connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(loadStarted()));


}

TopicDialog::~TopicDialog()
{
    delete ui;
}

void TopicDialog::setImage(QString s){

    picture->setImage(s);
}


void TopicDialog::setLabels(){

    QString topic = QString::number(localTopic);
    int i;
    QFont font = QFont("Arial", 12, QFont::Bold);


    if(lastSize > 0){

        for(i=0; i < lastSize; i++){

            ui->leftForm->removeWidget(causeNames[i]);
            ui->leftForm->removeWidget(causeValues[i]);

            ui->rightForm->removeWidget(effectNames[i]);
            ui->rightForm->removeWidget(effectValues[i]);

            delete causeNames[i];
            delete causeValues[i];

            delete effectNames[i];
            delete effectValues[i];
        }

        delete [] causeNames;
        delete [] causeValues;
        delete [] effectNames;
        delete [] effectValues;

        ui->leftForm->removeWidget(sigmaName);
        ui->leftForm->removeWidget(sigmaValue);


        lastSize = 0;
    }


    if(model == 0){

        causeNames = new QLabel*[1];
        causeValues = new QLabel*[1];
        effectNames = new QLabel*[1];
        effectValues = new QLabel*[1];

        causeNames[0] = new QLabel(this);
        causeNames[0]->setText(" No Model Loaded ");
        causeValues[0] = new QLabel(this);
        causeValues[0]->setText("");
        effectNames[0] = new QLabel(this);
        effectNames[0]->setText(" No Model Loaded ");
        effectValues[0] = new QLabel(this);
        effectValues[0]->setText("");

        ui->leftForm->addWidget(causeNames[0], 0, 0);
        ui->rightForm->addWidget(effectNames[0], 0, 0);

        ui->leftForm->addWidget(causeValues[0], 1, 0);
        ui->rightForm->addWidget(effectValues[0], 1, 0);

        sigmaName->setText("");
        sigmaValue->setText("");

        ui->leftForm->addWidget(sigmaName, 2, 0);
        ui->leftForm->addWidget(sigmaValue, 2, 1);

        lastSize = 1;

    }else if(localTopic < 1){

        causeNames = new QLabel*[1];
        causeValues = new QLabel*[1];
        effectNames = new QLabel*[1];
        effectValues = new QLabel*[1];

        causeNames[0] = new QLabel(this);
        causeNames[0]->setText(" Always Present ");
        causeValues[0] = new QLabel(this);
        causeValues[0]->setText("");
        effectNames[0] = new QLabel(this);
        effectNames[0]->setText(" Always Present ");
        effectValues[0] = new QLabel(this);
        effectValues[0]->setText("");

        ui->leftForm->addWidget(causeNames[0], 0, 0);
        ui->rightForm->addWidget(effectNames[0], 0, 0);

        ui->leftForm->addWidget(causeValues[0], 1, 0);
        ui->rightForm->addWidget(effectValues[0], 1, 0);

        sigmaName->setText("S0");
        sigmaValue->setText(QString::number(model->model.Sigma0));
        ui->leftForm->addWidget(sigmaName, 2, 0);
        ui->leftForm->addWidget(sigmaValue, 2, 1);

        sigmaValue->setFont(font);

        lastSize = 1;

    }else{

        causeNames = new QLabel*[K+1];
        effectNames = new QLabel*[K+1];
        causeValues = new QLabel*[K+1];
        effectValues = new QLabel*[K+1];

        causeNames[0] = new QLabel(this);
        causeNames[0]->setText("0 -> " + topic + "  ");
        effectNames[0] = new QLabel(this);
        effectNames[0]->setText(topic + " -> 0" + "  ");
        causeValues[0] = new QLabel(this);
        causeValues[0]->setText(QString::number(model->model.Omega0[localTopic-1]));
        effectValues[0] = new QLabel(this);
        effectValues[0]->setText(QString::number(-1));

        ui->leftForm->addWidget(causeNames[0], 0, 0);
        ui->leftForm->addWidget(causeValues[0], 0, 1);

        ui->rightForm->addWidget(effectNames[0], 0, 0);
        ui->rightForm->addWidget(effectValues[0], 0, 1);


        for(i=0; i < K; i++){


            causeNames[i+1] = new QLabel(this);
            causeNames[i+1]->setText(QString::number(i+1) + " -> " + topic + "  ");
            effectNames[i+1] = new QLabel(this);
            effectNames[i+1]->setText(topic + " -> " + QString::number(i+1) + "  ");
            causeValues[i+1] = new QLabel(this);
            causeValues[i+1]->setText(QString::number(model->model.Omega[localTopic-1][i]));
            effectValues[i+1] = new QLabel(this);
            effectValues[i+1]->setText(QString::number(model->model.Omega[i][localTopic-1]));

            ui->leftForm->addWidget(causeNames[i+1], i+1, 0);
            ui->leftForm->addWidget(causeValues[i+1], i+1, 1);

            ui->rightForm->addWidget(effectNames[i+1], i+1, 0);
            ui->rightForm->addWidget(effectValues[i+1], i+1, 1);

        }


        for(i=0; i < K+1; i++){

            causeValues[i]->setFont(font);
            effectValues[i]->setFont(font);
        }

        sigmaName->setText("S" + topic);
        sigmaValue->setText(QString::number(model->model.Sigma[localTopic-1]));
        ui->leftForm->addWidget(sigmaName, K+3, 0);
        ui->leftForm->addWidget(sigmaValue, K+3, 1);

        sigmaValue->setFont(font);

        lastSize = K+1;
    }
}

void TopicDialog::setLocalTopic(int k){

    localTopic = k;
}

void TopicDialog::setK(int k){

    K = k;
}

void TopicDialog::saveStarted(){

    qDebug() << "Save started " << localTopic;

    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(saveCompleted(QString)));

    newDialog->setFilter("Database (*.db)");
    newDialog->setDirectory("./");
    newDialog->show();
}

void TopicDialog::loadStarted(){

    qDebug() << "Load started " << localTopic;

    QFileDialog *newDialog = new QFileDialog(this);

    connect(newDialog, SIGNAL(fileSelected(QString)), this, SLOT(loadCompleted(QString)));

    newDialog->setFilter("Database (*.db)");
    newDialog->setDirectory("./");
    newDialog->show();
}

void TopicDialog::saveCompleted(QString s){

    model->model.saveTopic(s, localTopic);
}

void TopicDialog::loadCompleted(QString s){

    model->model.loadTopic(s, localTopic);
}
