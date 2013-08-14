#ifndef DATAMANAGER
#define DATAMANAGER

#include <stdlib.h>
#include <vector>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <sstream>
#include "projectwidget.h"

using namespace std;

class QString;

class DataManager : public ProjectWidget{

    Q_OBJECT

public:

	int MAXROWS;

    // Save single values
    void saveInt(QString dbname, QString tableName, QString keyName, int X);
    void saveLong(QString dbname, QString tableName, QString keyName, long X);
    void saveDouble(QString dbname, QString tableName, QString keyName, double X);
//    void saveString(QString dbname, QString tableName, QString keyName, QString X);

    // Save single dimensional arrays
    void saveArray(QString dbname, QString tableName, QString indexName1,
                                QString keyName, vector < int > &X);

    void saveArray(QString dbname, QString tableName, QString indexName1,
                                QString keyName, vector < long > &X);

    void saveArray(QString dbname, QString tableName, QString indexName1,
                                QString keyName, vector < double > &X);

//    void saveArray(QString dbname, QString tableName, QString indexName1,
//                                QString keyName, vector < QString > &X);

    // Save two dimensional arrays
    void saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
                                    vector < vector < int > > &X);
    void saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
                                    vector < vector < long > > &X);
    void saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
                                    vector < vector < double > > &X);
//    void saveArray(QString dbname, QString tableName, vector < QString > &keyNames,
//                                    vector < vector < QString > > X);


    void saveDump(QString dbname, QString tableName, vector < QString > &keyNames,
                                    vector < QString > &typeNames, vector < vector < int > > &X);

    void saveDump(QString dbname, QString tableName, vector < QString > &keyNames,
                                    vector < QString > &typeNames, vector < vector < long > > &X);

    void saveDump(QString dbname, QString tableName, vector < QString > &keyNames,
                                    vector < QString > &typeNames, vector < vector < double > > &X);

//    void saveDump(QString dbname, QString tableName, vector < QString > &keyNames,
//                                    vector < QString > &typeNames, vector < vector < QString > > X);


    template <class T>
    void saveDump(QString dbname, QString tableName, vector < QString > &keyNames,
                                    vector < QString > &typeNames, vector < vector < T > > &X);


    // Load functions
    void loadIntArray(QString dbname, QString tableName, QString keyName, vector < int > &X);
    void loadLongArray(QString dbname, QString tableName, QString keyName, vector < long > &X);
    void loadDoubleArray(QString dbname, QString tableName, QString keyName, vector < double > &X);
//    void loadStringArray(QString dbname, QString tableName, QString keyName, vector < QString > &X);

    int loadInt(QString dbname, QString tableName);
    long loadLong(QString dbname, QString tableName);
    double loadDouble(QString dbname, QString tableName);
//    QString loadString(QString dbname, QString tableName);



    DataManager(ProjectWidget *p = 0, MainWindow *q = 0);
	~DataManager();

signals:

    void saveTick(int);
    void loadTick(int);



};

#endif
