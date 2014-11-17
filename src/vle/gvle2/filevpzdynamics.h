/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef FILEVPZDYNAMICS_H
#define FILEVPZDYNAMICS_H

#include <QWidget>
#include "vlevpz.h"

namespace Ui {
class FileVpzDynamics;
}

class FileVpzDynamics : public QWidget
{
    Q_OBJECT
    
public:
    explicit FileVpzDynamics(QWidget *parent = 0);
    ~FileVpzDynamics();
    void setVpz(vleVpz *vpz);
    void reload();

public slots:
    void onSelected(int cr, int cc, int pr, int pc);
    void onSelectPackage(int index);
    void onSelectLibrary(int index);
    void onAddButton();
    void onCloneButton();
    void onRemoveButton();
    void onSaveButton();

protected:
    void updatePackageList(QString name = "");
    void updateLibraryList(QString package = "", QString name = "");

private:
    vleVpzDynamic *getSelected();

private:
    Ui::FileVpzDynamics *ui;
    vleVpz  *mVpz;
    bool     mPackageLoaded;
};

#endif // FILEVPZDYNAMICS_H
