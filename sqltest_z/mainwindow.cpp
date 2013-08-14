#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDriver>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase database = QSqlDatabase::addDatabase("PSQL");
    QSqlQuery query;
    database.setDatabaseName("jackTest.db");
    database.open();

    query.prepare("INSERT INTO person (id, forename) VALUES (?, ?)");
    query.bindValue(0, 10);
    query.bindValue(1, "Ender");

    query.exec();
}

MainWindow::~MainWindow()
{
    delete ui;
}
