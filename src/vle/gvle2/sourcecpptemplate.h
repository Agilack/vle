/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SOURCECPPTEMPLATE_H
#define SOURCECPPTEMPLATE_H

#include <QMap>
#include <QString>

class sourceCpp;

#ifndef Q_MOC_RUN
#include <vle/utils/Template.hpp>
#endif

class sourceCppTemplate
{
public:
    sourceCppTemplate(sourceCpp *parent = 0);
    bool    isValid();
    QString getPluginName();
    bool    tagArrayLoad(QString name);
    QString getTagValue(QString name);
    int     getTagArrayCount();
    QString getTagArrayName(int pos);
    QString getTagArrayValue(int pos);
private:
    void tagLoad();
private:
    sourceCpp  *mSource;
    bool        mTagLoaded;
    QString     mTagPlugin;
    QString     mTagPackage;
    QMap<QString,QString> mTagConf;
    QMap<QString,QString> mTagConfValue;
};

#endif // SOURCECPPTEMPLATE_H
