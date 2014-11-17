/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef PLUGIN_COND_H
#define PLUGIN_COND_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <vle/gvle2/logger.h>
#include <vle/gvle2/vlevpz.h>

class PluginExpCond
{
public:
    virtual QString  getname()   = 0;
    virtual QWidget *getWidget() = 0;
    virtual void     delWidget() = 0;
    virtual QWidget *getWidgetToolbar() = 0;
    virtual void     delWidgetToolbar() = 0;
    virtual void  setSettings(QSettings *s) = 0;
    virtual void  setLogger(Logger *logger) = 0;
    virtual void  setVpz(vleVpz *vpz) = 0;
    virtual void *getVpz() = 0;
};

Q_DECLARE_INTERFACE(PluginExpCond, "fr.inra.vle.gvle2.PluginExpCond/1.0")

#endif // PLUGIN_COND_H
