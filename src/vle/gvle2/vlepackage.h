/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef VLEPACKAGE_H
#define VLEPACKAGE_H

#include <QDir>
#include <QMap>
#include <QString>

#include "plugin_modeler.h"

#ifndef Q_MOC_RUN
#include <vle/utils/Package.hpp>
#endif

class PluginModeler;

class vlePackage
{
public:
    vlePackage();
    vlePackage(QString path);
    QString getName();
    void    setStdPackage(vle::utils::Package *pkg);
    QString getSrcFilePath(QString name);
    void    addModeler(QString filename);
    PluginModeler *getModelerByPlugin(QString pluginName);
    PluginModeler *getModelerByClass(QString className);
    int     getModelerCount();
    QString getModelerName(int pos);
    int     getModelerClassCount();
    QString getModelerClass(int pos);
    QString getModelerClassPlugin(QString className);
protected:
    void    refreshModelerClass();
private:
    QDir mDir;
private:
    vle::utils::Package *mStdPackage;
    QMap <QString,QString> mModelers;
    QMap <QString,QString> mModelerClass;
};

#endif // VLEPACKAGE_H
