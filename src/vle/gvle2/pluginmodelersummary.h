/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef PLUGINMODELERSUMMARY_H
#define PLUGINMODELERSUMMARY_H

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class pluginModelerSummary;
}

class pluginModelerSummary : public QWidget
{
    Q_OBJECT
    
public:
    explicit pluginModelerSummary(QWidget *parent = 0);
    ~pluginModelerSummary();
    void addClassList(QString name, QString filename);

signals:
    void openClass(QString filename);

private slots:
    void onSelectClass(QListWidgetItem *item);

private:
    Ui::pluginModelerSummary *ui;
};

#endif // PLUGINMODELERSUMMARY_H
