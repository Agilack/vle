/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef FILEVPZVIEW_H
#define FILEVPZVIEW_H

#include <QGraphicsScene>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include "ui_filevpzrtool.h"
#include "vlevpz.h"
#include "widgetvpzproperty.h"
#include "filevpzdynamics.h"
#include "filevpzexpcond.h"
#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

#define ROW_NAME 0
#define ROW_DYN  1
#define ROW_EXP  2

namespace Ui {
class fileVpzView;
class fileVpzRTool;
}

class fileVpzView : public QWidget
{
    Q_OBJECT

public:
    enum ExpCondTreeType { ECondNone, ECondCondition, ECondParameter };

public:
    explicit fileVpzView(QWidget *parent = 0);
    ~fileVpzView();
    void setVpz(vleVpz *v);
    vleVpz* vpz();
    bool isUsed(int *reason);
    void usedBySim(bool isUsed);
    void save();
    QWidget *getTool();
    void treeInsertModel(vleVpzModel *model, QTreeWidgetItem *base);
    void diagSelectModel(vleVpzModel *base, bool force = FALSE);
    void treeUpdateModel(vleVpzModel *model, QString oldName, QString newName);

public slots:
    void onTreeModelSelected();
    void onViewTreeMenu(const QPoint pos);
    void onFocusChanged(vleVpzModel *model);
    void onModelDblClick(vleVpzModel *model);
    void onPropertyChanged(QTableWidgetItem *item);
    void onPropertySelected(int cr, int cc, int pr, int pc);
    void onPropertyMode(bool isSelected);
    void onTabClose(int index);
    void onAddModelerTab(vleVpzModel *model);

private:
    bool             mUseSim;
    Ui::fileVpzView *ui;
    Ui::fileVpzRtool*uiTool;
    FileVpzDynamics *mDynamicsTab;
    FileVpzExpCond  *mExpCondTab;
    vleVpz          *mVpz;
    QWidget         *mWidgetTool;
    QList<QTreeWidgetItem *> mViewsItems;
    vleVpzModel     *mCurrentModel;

private:
    QGraphicsScene   mScene;
};

#endif // FILEVPZVIEW_H
