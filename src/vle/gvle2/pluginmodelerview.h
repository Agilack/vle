/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef PLUGINMODELERVIEW_H
#define PLUGINMODELERVIEW_H

#include <QPluginLoader>
#include <QTextStream>
#include <QWidget>
#include "vlepackage.h"
#include "plugin_modeler.h"
#include "sourcecpp.h"

namespace Ui {
class pluginModelerView;
}

class pluginModelerView : public QWidget
{
    Q_OBJECT
    
public:
    explicit pluginModelerView(QWidget *parent = 0);
    ~pluginModelerView();
    void setPlugin(QString filename);
    void setPackage(vlePackage *package);
    void addClass(QString name, QString filename);
    void showSummary();
    void addNewTab();
    void addEditTab(QWidget *widget, QString name);
    QString getName();
    QString getFilename();

public slots:
    void onTabRename(QWidget *tab, QString name);
    void onSaveClass(QString name);
    void onOpenTab  (QString filename);
    void onCloseTab (int index);

signals:
    void refreshFiles();

protected:
    void setMainWidget();
    
private:
    Ui::pluginModelerView *ui;
    QString                mPluginFile;
    QPluginLoader         *mLoader;
    PluginModeler         *mModeler;
    vlePackage            *mPackage;
    QList<sourceCpp *>     mListSources;
};

#endif // PLUGINMODELERVIEW_H
