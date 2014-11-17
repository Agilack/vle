/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef FILEVPZEXPCOND_H
#define FILEVPZEXPCOND_H

#include <QWidget>
#include <QMenu>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include "vlevpz.h"

namespace Ui {
class FileVpzExpCond;
}

class FileVpzExpCond : public QWidget
{
    Q_OBJECT
    
public:
    enum ExpCondTreeType { ECondNone, ECondCondition, ECondParameter };

public:
    explicit FileVpzExpCond(QWidget *parent = 0);
    ~FileVpzExpCond();
    void setVpz(vleVpz *vpz);
    void reload();

public slots:
    void onConditionTreeRefresh();
    void onConditionTreeMenu(const QPoint pos);
    void onConditionChanged(QTreeWidgetItem *item, int column);
    void onConditionTreeSelected();
    void onCondParamTreeSelected();
    void onCondParamListDblClick(QListWidgetItem *item);
    void onParamTableChange();
    void onCondParamTableDblClick(int row, int col);
    void onCondParamTableMenu(const QPoint pos);
    void onCondParamTreeMenu(const QPoint pos);
    void onCondParamListMenu(const QPoint pos);
    void onParamTextChange();
    void onCondParamCancel();
    void onCondParamSave();

private:
    void condParamTreeSelectByValue(vpzExpCondValue *reqValue, QTreeWidgetItem *base = 0);
    void condUpdateTree(QList <vpzExpCondValue *> *valueList, QList<QTreeWidgetItem *> *widList);
    void condValueShowDetail(vpzExpCondValue *value);
    void paramTreeUpdateList(vpzExpCondValue *value);
    void paramTreeUpdateTable(vpzExpCondValue *value);
    void paramTreeUpdateText(vpzExpCondValue *value);

private:
    Ui::FileVpzExpCond *ui;
    vleVpz  *mVpz;
    QTreeWidgetItem *mCondEdition;
};

#endif // FILEVPZEXPCOND_H
