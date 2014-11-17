/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef WIDGETVPZPROPERTY_H
#define WIDGETVPZPROPERTY_H

#include <QWidget>
#include "vlevpz.h"

namespace Ui {
class WidgetVpzPropertyDynamics;
class WidgetVpzPropertyExpCond;
}

class WidgetVpzProperty : public QWidget
{
    Q_OBJECT

public:
    enum propertyMode { ModeSimple, ModeExtend, ModeEdit };

public:
    explicit WidgetVpzProperty(QWidget *parent = 0);
    ~WidgetVpzProperty();
    void setMaximized(bool isMax);
    void setModel(vleVpzModel *model);

protected:
    vleVpzModel *mModel;

private:
    bool isMaximized;
};

class WidgetVpzPropertyDynamics : public WidgetVpzProperty
{
    Q_OBJECT

public:
    explicit WidgetVpzPropertyDynamics(QWidget *parent = 0);
    ~WidgetVpzPropertyDynamics();
    void setModel(vleVpzModel *model);
    void setMaximized(bool isMax);

protected:
    // From QWidget
    void mouseDoubleClickEvent (QMouseEvent *event);

signals:
    void sigActivated(bool isActive);

private slots:
    void onChange(int index);

private:
    Ui::WidgetVpzPropertyDynamics *ui;
    propertyMode mMode;
};

class WidgetVpzPropertyExpCond : public WidgetVpzProperty
{
    Q_OBJECT
public:
    explicit WidgetVpzPropertyExpCond(QWidget *parent = 0);
    ~WidgetVpzPropertyExpCond();
    void setModel(vleVpzModel *model);
    void setMaximized(bool isMax);

protected:
    void mouseDoubleClickEvent (QMouseEvent *event);

private slots:
    void onCheckboxToggle(bool checked);

private:
    Ui::WidgetVpzPropertyExpCond *ui;
};

#endif // WIDGETVPZPROPERTY_H
