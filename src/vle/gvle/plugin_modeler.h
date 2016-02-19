/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef gvle_PLUGIN_MODELER_H
#define gvle_PLUGIN_MODELER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QWidget>
#include <QString>
#include <vle/gvle/logger.h>
#include <vle/gvle/vlepackage.h>

#ifndef Q_MOC_RUN
#include <vle/utils/Package.hpp>
#endif

namespace vle {
namespace gvle {

class sourceCpp;
class vlePackage;

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
    virtual QWidget* getMainTabWidget();
    virtual QWidget* openNewClass()  = 0;
    virtual QWidget* openEditClass(QString fileName) = 0;
    virtual QWidget* getEditClass(QString fileName) = 0;
    virtual void     closeEditClass(QString fileName) = 0;
    virtual void     rename(QString oldName, QString newName) = 0;
    virtual void     setSettings(QSettings *s) = 0;
    virtual void     setLogger(Logger *logger) = 0;
    virtual void     setPackage(vle::utils::Package *pkg) = 0;
    virtual void     undo(QWidget *w) = 0;
    virtual void     redo(QWidget *w) = 0;
    virtual void     save(QWidget *w) = 0;
    virtual bool     allowClose(QWidget *w) = 0;
    virtual QString  getClassName(QWidget *w) = 0;
    virtual void     cloneSrc(QString from, QString to) = 0;

protected:
    QWidget *mMainTabWidget;
};

}}//namespaces

Q_DECLARE_INTERFACE(vle::gvle::PluginModeler, "fr.inra.vle.gvle.PluginModeler")

#endif // PLUGIN_MODELER_H
