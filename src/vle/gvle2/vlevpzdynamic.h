/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef VLEVPZDYNAMIC_H
#define VLEVPZDYNAMIC_H

#include <QList>
#include <QString>
#ifndef Q_MOC_RUN
#include <vle/utils/Path.hpp>
#include <vle/utils/ModuleManager.hpp>
#endif

class vleVpzDynamic
{
public:
    vleVpzDynamic(QString name = "", QString lib = "", QString package = "");
    vleVpzDynamic(vleVpzDynamic *dynamic);

    void setName(QString name);
    void setLibrary(QString name);
    void setPackage(QString name);
    QString getName();
    QString getLibrary();
    QString getPackage();
    QList <QString> *getPackageList();
    QList <QString> *getLibraryList(QString package);
    bool    isAltered();
    bool    isValid(bool again = false);

private:
    bool    mIsAltered;
    bool    mValidPackage;
    bool    mValidLibrary;
    QString mName;
    QString mLibrary;
    QString mPackage;
    QList <QString> mPackageList;
    QList <QString> mLibraryList;
};

#endif // VLEVPZDYNAMIC_H
