#include "DataManager.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>

using namespace std;

DataManager::DataManager(ProjectWidget *p, MainWindow *q){

    ProjectWidget(p,q);
	MAXROWS = 200;
	
}

DataManager::~DataManager(){


	
}


/*****************************************************/
// Single elements
/*****************************************************/

void DataManager::saveInt(QString dbname, QString tableName, QString keyName, int X){

    vector < vector < int > > y;
    y.resize(1);
    y[0].resize(2);
    y[0][0] = 0;
    y[0][1] = X;

    vector < QString > keys;
    keys.resize(2);
    keys[0] = "id";
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "INT";

    saveDump(dbname, tableName, keys, types, y);
}

void DataManager::saveLong(QString dbname, QString tableName, QString keyName, long X){


    vector < vector < long > > y;
    y.resize(1);
    y[0].resize(2);
    y[0][0] = 0;
    y[0][1] = X;

    vector < QString > keys;
    keys.resize(2);
    keys[0] = "id";
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "LONG";

    saveDump(dbname, tableName, keys, types, y);
}

void DataManager::saveDouble(QString dbname, QString tableName, QString keyName, double X){

    vector < vector < double > > y;
    y.resize(1);
    y[0].resize(2);
    y[0][0] = 0;
    y[0][1] = X;

    vector < QString > keys;
    keys.resize(2);
    keys[0] = "id";
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "DOUBLE";

    saveDump(dbname, tableName, keys, types, y);
}

/*
void DataManager::saveString(QString dbname, QString tableName, QString keyName, QString X){

    vector < vector < QString > > y;
    y.resize(1);
    y[0].resize(2);
    y[0][0] = 0;
    y[0][1] = X;

    vector < QString > keys;
    keys.resize(2);
    keys[0] = "id";
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "TEXT";

    saveDump(dbname, tableName, keys, types, y);
}
*/

/*****************************************************/
// One dimensional arrays with one index
/*****************************************************/

void DataManager::saveArray(QString dbname, QString tableName, QString indexName1,
                            QString keyName, vector < int > &X){

    vector < vector < int > > y;
    y.resize(X.size());
    for(int i=0; i < X.size(); i++){

        y[i].resize(2);
        y[i][0] = 0;
        y[i][1] = X[i];
    }

    vector < QString > keys;
    keys.resize(2);
    keys[0] = indexName1;
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "INT";

    saveDump(dbname, tableName, keys, types, y);
}


void DataManager::saveArray(QString dbname, QString tableName, QString indexName1,
                            QString keyName, vector < long > &X){

    vector < vector < long > > y;
    y.resize(X.size());
    for(int i=0; i < X.size(); i++){

        y[i].resize(2);
        y[i][0] = 0;
        y[i][1] = X[i];
    }

    vector < QString > keys;
    keys.resize(2);
    keys[0] = indexName1;
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "LONG";

    saveDump(dbname, tableName, keys, types, y);
}


void DataManager::saveArray(QString dbname, QString tableName, QString indexName1,
                            QString keyName, vector < double > &X){

    vector < vector < double > > y;
    y.resize(X.size());
    for(int i=0; i < X.size(); i++){

        y[i].resize(2);
        y[i][0] = 0;
        y[i][1] = X[i];
    }

    vector < QString > keys;
    keys.resize(2);
    keys[0] = indexName1;
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "DOUBLE";

    saveDump(dbname, tableName, keys, types, y);
}

/*
void DataManager::saveArray(QString dbname, QString tableName, QString indexName1,
                            QString keyName, vector < QString > X){

    vector < vector < QString > > y;
    y.resize(X.size());
    for(int i=0; i < X.size(); i++){

        y[i].resize(2);
        y[i][0] = 0;
        y[i][1] = X[i];
    }

    vector < QString > keys;
    keys.resize(2);
    keys[0] = indexName1;
    keys[1] = keyName;

    vector < QString > types;
    types.resize(2);
    types[0] = "INT";
    types[1] = "TEXT";

    saveDump(dbname, tableName, keys, types, y);
}
*/

/*********************************************************/
// Two dimensional arrays with two indexes
/*********************************************************/
void DataManager::saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
                                vector < vector < int > > &X){

    int size = 0;
    int k = 0;

    Q_ASSERT(keyNames.size() == 3);

    for(int i=0; i < X.size(); i++){
        size = size + X[i].size();
    }

    vector < vector < int > > y;
    y.resize(size);

    for(int i=0; i < X.size(); i++){
        for(int j=0; j < X[i].size(); j++){

            y[k].resize(3);
            y[k][0] = i;
            y[k][1] = j;
            y[k][2] = X[i][j];
            k++;
        }
    }

    vector < QString > types;
    types.resize(3);
    types[0] = "INT";
    types[1] = "INT";
    types[2] = "INT";

    saveDump(dbname, tableName, keyNames, types, y);
}



void DataManager::saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
                                vector < vector < long > > &X){

    int size = 0;
    int k = 0;

    Q_ASSERT(keyNames.size() == 3);

    for(int i=0; i < X.size(); i++){
        size = size + X[i].size();
    }

    vector < vector < long > > y;
    y.resize(size);

    for(int i=0; i < X.size(); i++){
        for(int j=0; j < X[i].size(); j++){

            y[k].resize(3);
            y[k][0] = i;
            y[k][1] = j;
            y[k][2] = X[i][j];
            k++;
        }
    }

    vector < QString > types;
    types.resize(3);
    types[0] = "INT";
    types[1] = "INT";
    types[2] = "LONG";

    saveDump(dbname, tableName, keyNames, types, y);
}



void DataManager::saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
                                vector < vector < double > > &X){

    int size = 0;
    int k = 0;

    Q_ASSERT(keyNames.size() == 3);

    for(int i=0; i < X.size(); i++){
        size = size + X[i].size();
    }

    vector < vector < double > > y;
    y.resize(size);

    for(int i=0; i < X.size(); i++){
        for(int j=0; j < X[i].size(); j++){

            y[k].resize(3);
            y[k][0] = i;
            y[k][1] = j;
            y[k][2] = X[i][j];
            k++;
        }
    }

    vector < QString > types;
    types.resize(3);
    types[0] = "INT";
    types[1] = "INT";
    types[2] = "DOUBLE";

    saveDump(dbname, tableName, keyNames, types, y);
}


/*
void DataManager::saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
                                vector < vector < QString > > &X){

    int size = 0;
    int k = 0;

    Q_ASSERT(keyNames.size() == 3);

    for(int i=0; i < X.size(); i++){
        size = size + X[i].size();
    }

    vector < vector < QString > > y;
    y.resize(size);

    for(int i=0; i < X.size(); i++){
        for(int j=0; j < X[i].size(); j++){

            y[k].resize(3);
            y[k][0] = i;
            y[k][1] = j;
            y[k][2] = X[i][j];
            k++;
        }
    }

    vector < QString > types;
    types.resize(3);
    types[0] = "INT";
    types[1] = "INT";
    types[2] = "TEXT";

    saveDump(dbname, tableName, keys, types, y);
}
*/

/*********************************************************/
// Overloaded functions so that we only write the
// templated one once
/*********************************************************/

void DataManager::saveDump(QString dbname, QString tableName, vector < QString > &keyNames, vector < QString > &typeNames, vector < vector < int > > &X){

    saveDump<int>(dbname, tableName, keyNames, typeNames, X);
}

void DataManager::saveDump(QString dbname, QString tableName, vector < QString > &keyNames, vector < QString > &typeNames, vector < vector < long > > &X){

    saveDump<long>(dbname, tableName, keyNames, typeNames, X);
}

void DataManager::saveDump(QString dbname, QString tableName, vector < QString > &keyNames, vector < QString > &typeNames, vector < vector < double > > &X){

    saveDump<double>(dbname, tableName, keyNames, typeNames, X);
}
/*
void DataManager::saveDump(QString dbname, QString tableName, vector < QString > &keyNames, vector < QString > &typeNames, vector < vector < QString > > &X){

    saveDump<QString>(dbname, tableName, keyNames, typeNames, X);
}
*/

/*********************************************************/
// Two dimensional arrays with two indices
// This is the general case
/*********************************************************/
// saveDump just pushes out a 2D array with keynames as is. No preprocessing is done.
template <class T>
void DataManager::saveDump(QString dbname, QString tableName, vector < QString > &keyNames, vector < QString > &typeNames, vector < vector < T > > &X){

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);
    QString statement;

    int rows = X.size();
    int rowsLeft = X.size();
    int rowsUsed = 0;

    Q_ASSERT(X[0].size() == keyNames.size());

    // Delete the existing table
    statement = "DROP TABLE " + tableName + ";";
    query.exec(statement);

    // Build the new table
    statement = "CREATE TABLE " + tableName;
    statement = statement + "(" + keyNames[0] + " " + typeNames[0];
    for(int i=1; i < keyNames.size(); i++){
        statement = statement + ", " + keyNames[i] + " " + typeNames[i];
    }
    statement = statement + ");";

    query.exec(statement);

    statement = "INSERT INTO " + tableName + " VALUES";
    for(int i=0; i < X.size(); i++){

        statement = statement + " (" + QString::number(X[i][0]);
        for(int j=1; j < X[i].size(); j++){

            statement = statement + ", " + QString::number(X[i][j]);
        }

        rowsUsed++;
        if(rowsUsed%400 == 0){

            statement = statement + ");";
            query.exec(statement);
            statement = "INSERT INTO " + tableName + " VALUES";
            rowsLeft = rowsLeft - 400;
            emit saveTick(400);

        }else if(rowsUsed < rows){
            statement = statement + "),";
        }else{
            statement = statement + ");";
        }
    }
    query.exec(statement);
    emit saveTick(rowsLeft);

    db.close();
}


//*****************************************************************
// Load functions
//*****************************************************************

// These are too hard to update. We may come back to these at some later date


void DataManager::loadIntArray(QString dbname, QString tableName, QString keyName, vector < int > &X){

    int rows;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT COUNT(" + keyName+ ") FROM " + tableName + ";";

    query.prepare(statement);
    query.exec();
    query.next();
    rows = query.value(0).toInt();

    X.resize(rows);
    statement = "SELECT " + keyName + " FROM " + tableName;

    query.prepare(statement);
    query.exec();
    for(int i=0; i < rows; i++){

        query.next();
        X[i] = query.value(0).toInt();
        if((i+1)%1000 == 0){

            emit loadTick(1000);
        }
    }

    emit loadTick(rows%1000);

    db.close();
}

void DataManager::loadLongArray(QString dbname, QString tableName, QString keyName, vector < long > &X){

    int rows;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT COUNT(" + keyName+ ") FROM " + tableName + ";";

    query.prepare(statement);
    query.exec();
    query.next();
    rows = query.value(0).toInt();

    X.resize(rows);
    statement = "SELECT " + keyName + " FROM " + tableName;

    query.prepare(statement);
    query.exec();
    for(int i=0; i < rows; i++){

        query.next();
        X[i] = query.value(0).toLongLong();

        if((i+1)%1000 == 0){

            emit loadTick(1000);
        }
    }
    emit loadTick(rows%1000);

    db.close();
}

void DataManager::loadDoubleArray(QString dbname, QString tableName, QString keyName, vector < double > &X){

    int rows;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT COUNT(" + keyName+ ") FROM " + tableName + ";";

    query.prepare(statement);
    query.exec();
    query.next();
    rows = query.value(0).toInt();

    X.resize(rows);
    statement = "SELECT " + keyName + " FROM " + tableName;

    query.prepare(statement);
    query.exec();
    for(int i=0; i < rows; i++){

        query.next();
        X[i] = query.value(0).toDouble();

        if((i+1)%1000 == 0){
            emit loadTick(1000);
        }
    }
    emit loadTick(rows%1000);


    db.close();
}
/*
void DataManager::loadStringArray(QString dbname, QString tableName, QString keyName, vector < QString > &X){

    int rows;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT COUNT(" + keyName+ ") FROM " + tableName + ";";

    query.prepare(statement);
    query.exec();
    query.next();
    rows = query.value(0).toInt();

    X.resize(rows);
    statement = "SELECT " + keyName + " FROM " + tableName;

    query.prepare(statement);
    query.exec();
    for(int i=0; i < rows; i++){

        query.next();
        array[i] = query.value(0).toString();
        if((i+1)%1000 == 0)
            emit loadTick(1000);
    }
    emit loadTick(rows%1000);

    db.close();
}
*/
int DataManager::loadInt(QString dbname, QString tableName){

    int x;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";
    int i;

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT * FROM " + tableName + ";";
    query.prepare(statement);
    query.exec();

    query.next();
    x = query.value(1).toInt();

    emit loadTick(1);
    db.close();
    return x;
}

long DataManager::loadLong(QString dbname, QString tableName){

    long x;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT * FROM " + tableName;
    query.prepare(statement);
    query.exec();

    query.next();
    x = query.value(1).toLongLong();

    emit loadTick(1);
    db.close();
    return x;
}

double DataManager::loadDouble(QString dbname, QString tableName){

    double x;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT * FROM " + tableName;
    query.prepare(statement);
    query.exec();

    query.next();
    x = query.value(1).toDouble();

    emit loadTick(1);
    db.close();
    return x;
}

/*
QString DataManager::loadString(QString dbname, QString tableName){

    QString x;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString statement = "";
    int i;

    db.setDatabaseName(dbname);
    db.open();
    QSqlQuery query(db);

    statement = "SELECT * FROM " + tableName;
    query.prepare(statement);
    query.exec();

    query.next();
    x = query.value(1).toString();

    emit loadTick(1);
    db.close();
    return x;
}
*/
