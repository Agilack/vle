/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef PLUGIN_MODELER_H
#define PLUGIN_MODELER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QWidget>
#include <vle/gvle2/logger.h>
#include <vle/gvle2/sourcecpp.h>
#include <vle/gvle2/vlevpz.h>
#include <vle/gvle2/vpzexpcond.h>

class PluginModeler: public QObject
{
    Q_OBJECT
public:
    PluginModeler();
    ~PluginModeler();
    virtual QString  getname()   = 0;
    virtual QString  getData(QString className) = 0;
public:
    virtual bool     useCustomMainTab();
    virtual QWidget *getMainTabWidget();
    virtual QWidget *openNewClass()  = 0;
    virtual QWidget *openEditClass(sourceCpp *src) = 0;
    virtual QWidget *getEditClass(sourceCpp *src);
    virtual void     closeEditClass(sourceCpp *src);
    virtual QWidget *addEditModel(vleVpzModel *model) = 0;
    virtual void     initExpCond(vpzExpCond *exp, sourceCpp *src) = 0;
    virtual void  setSettings(QSettings *s) = 0;
    virtual void  setLogger(Logger *logger) = 0;
protected:
    QWidget *mMainTabWidget;
};

Q_DECLARE_INTERFACE(PluginModeler, "fr.inra.vle.gvle2.PluginModeler/1.0")

#endif // PLUGIN_MODELER_H
