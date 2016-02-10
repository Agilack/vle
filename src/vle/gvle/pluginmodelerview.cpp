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

#include <QMessageBox>
#include "pluginmodelerview.h"
#include "ui_pluginmodelerview.h"
#include "pluginmodelersummary.h"

#include <QDebug>

namespace vle {
namespace gvle {

/**
 * @brief pluginModelerView::pluginModelerView
 *        Default constructor
 *
 */
pluginModelerView::pluginModelerView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pluginModelerView)
{
    ui->setupUi(this);

    mLoader  = 0;
    mModeler = 0;
    mPackage = 0;

    mListSources.clear();

    ui->tabWidget->setTabsClosable(true);
    QObject::connect(ui->tabWidget,   SIGNAL(tabCloseRequested(int)),
                     this,            SLOT  (onCloseTab(int)));
}

/**
 * @brief pluginModelerView::~pluginModelerView
 *        Default destructor
 *
 */
pluginModelerView::~pluginModelerView()
{
    // First, remove all the tabs
    while(ui->tabWidget->count())
    {
        // Get the first remaining tab from QTabWidget
        QWidget *w = ui->tabWidget->widget(0);
        // Remove it ...
        ui->tabWidget->removeTab(0);
        // and them delete ir
        delete w;
    }
    delete ui;

    // Unload and delete plugin instance
    if (mLoader)
    {
        mLoader->unload();
        delete mLoader;
    }

    while(mListSources.count())
        mPackage->closeSourceCpp( mListSources.takeFirst() );
}

/**
 * @brief pluginModelerView::getName
 *        Get the modeler name (from plugin)
 *
 */
QString pluginModelerView::getName()
{
    if (mModeler)
        return mModeler->getname();
    else
        return QString();
}

/**
 * @brief pluginModelerView::getFilename
 *        Get the plugin file name
 *
 */
QString pluginModelerView::getFilename()
{
    return mPluginFile;
}

void pluginModelerView::undo()
{
    QWidget *w = ui->tabWidget->currentWidget();
    if (mModeler) {
        mModeler->undo(w);
    }
}

void pluginModelerView::redo()
{
    QWidget *w = ui->tabWidget->currentWidget();
    if (mModeler) {
        mModeler->redo(w);
    }
}

void pluginModelerView::save()
{
    QWidget *w = ui->tabWidget->currentWidget();
    if (mModeler) {
        mModeler->save(w);
    }
}

bool pluginModelerView::allowClose()
{
    for (int i = 1; i < ui->tabWidget->count(); i++)
    {
        QWidget* w = ui->tabWidget->widget(i);
        if (mModeler) {
             if (not mModeler->allowClose(w)) {
                 return false;
             }
         }
    }
    return true;
}

void pluginModelerView::setLogger(Logger *logger)
{
    mLogger = logger;
    mLogger->log("pluginModelerView::setLogger");
}

void pluginModelerView::setPackage(vlePackage *package)
{
    mPackage = package;
}

/**
 * @brief pluginModelerView::setPlugin
 *        Set and load the associated plugin
 *
 */
void pluginModelerView::setPlugin(QString filename)
{
    mPluginFile = filename;

    mLoader = new QPluginLoader(mPluginFile);

    // Try to load instance
    QObject *plugin = mLoader->instance();
    if ( ! mLoader->isLoaded())
        return;

    mModeler = qobject_cast<PluginModeler *>(plugin);
    if (! mModeler)
        return;

    QObject::connect(mModeler, SIGNAL(nameChanged(QWidget *, QString)),
                     this,     SLOT  (onTabRename(QWidget *, QString)));

    QObject::connect(mModeler, SIGNAL(saveClass(QString)),
                     this,     SLOT  (onSaveClass(QString)));

    setMainWidget();
}

/**
 * @brief pluginModelerView::addClass
 *        Insert a class to the list of classes managed by current plugin
 *
 */
void pluginModelerView::addClass(QString name, QString filename)
{
    if (mModeler->useCustomMainTab())
        return;
    QWidget *w = ui->tabWidget->widget(0);
    pluginModelerSummary *ws =  qobject_cast<pluginModelerSummary *>(w);
    ws->addClassList(name, filename);
}

/**
 * @brief pluginModelerView::showSummary
 *        Switch the display to Summary tab
 *
 */
void pluginModelerView::showSummary()
{
    ui->tabWidget->setCurrentIndex(0);
}

/**
 * @brief pluginModelerView::setMainWidget
 *        Add the first (main) tab, global plugin informations
 *
 */
void pluginModelerView::setMainWidget()
{
    QWidget *mainTab;

    if (mModeler == 0)
        return;

    if (mModeler->useCustomMainTab())
        mainTab = mModeler->getMainTabWidget();
    else
        mainTab = new pluginModelerSummary();

    vle::utils::Package* mStdPackage= mPackage->getStdPackage();

    mModeler->setPackage(mPackage->getStdPackage());
    mModeler->setLogger(mLogger);

    QObject::connect(mainTab, SIGNAL(openClass(QString)),
                     this,    SLOT  (onOpenTab(QString)));
    QObject::connect(mainTab, SIGNAL(newClass()),
                     this,    SLOT(addNewTab()));
    QObject::connect(mainTab, SIGNAL(duplicateClass(QString, QString)),
                     this,    SLOT(onDuplicateClass(QString, QString)));
    QObject::connect(mainTab, SIGNAL(duplicateReq(QString)),
                     this,    SLOT(onDuplicateRequest(QString)));

    // Insert the summary tab
    int n = ui->tabWidget->insertTab(0, mainTab, tr("Summary"));
    // Summary can't be closed, remove the icon
    QTabBar *tabBar = ui->tabWidget->findChild<QTabBar *>();
    tabBar->setTabButton(n, QTabBar::RightSide, 0);
    // Then, select the summary as active tab
    ui->tabWidget->setCurrentIndex(n);

    mainTab->show();
}

/**
 * @brief pluginModelerView::onOpenTab (slot)
 *        Called when a Summary (or main) tab emit an Open signal
 *
 */
void pluginModelerView::onOpenTab(QString fileName)
{
    if (mModeler == 0) {
        return;
    }
    sourceCpp *src = 0;
    for (int i = 0; i < mListSources.count(); i++) {
        src = mListSources.at(i);
        if (src->getFilename() == fileName)
            break;
        src = 0;
    }
    if (src == 0) {
        src = mPackage->openSourceCpp(fileName);
        mListSources.append(src);
        vle::utils::Package* pkg = mPackage->getStdPackage();
        QFileInfo selectedFileInfo = QFileInfo(fileName);
        QString bname = selectedFileInfo.baseName();
        QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
        QFile file(basepath + "/metadata/src/" + bname + ".sm");
        QString srcClassName;
        if (file.exists()) {
            QDomDocument* mDocSm = new QDomDocument("vle_project_metadata");
            QXmlInputSource source(&file);
            QXmlSimpleReader reader;
            mDocSm->setContent(&source, &reader);
            QDomElement docElem = mDocSm->documentElement();

            QDomNode srcPluginNode =
                mDocSm->elementsByTagName("srcPlugin").item(0);
            srcClassName = srcPluginNode.attributes().namedItem("class").nodeValue();
        }
        QWidget *tab = mModeler->openEditClass(fileName);
        addEditTab(tab, srcClassName);
    } else {
        QWidget *w = mModeler->getEditClass(fileName);
        ui->tabWidget->setCurrentWidget(w);
    }
}

/**
 * @brief pluginModelerView::onCloseTab (slot)
 *        Called when user click on tab icon to close
 *
 */
void pluginModelerView::onCloseTab (int index)
{
    QWidget *w = ui->tabWidget->widget(index);

    vle::utils::Package* pkg = mPackage->getStdPackage();

    if (mModeler) {
        if (mModeler->allowClose(w)) {
            QString editedClass = mModeler->getClassName(w);
            QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();
            QString filePath = basepath + "/src/" + editedClass + ".cpp";
            mModeler->closeEditClass(filePath);
            ui->tabWidget->removeTab(index);
            for (int i = 0; i < mListSources.count(); i++) {
                if (filePath == mListSources.at(i)->getFilename()) {
                    mListSources.removeAt(i);
                    break;
                }
            }
        }
    }
}

/**
 * @brief pluginModelerView::addNewTab
 *        Add a tab used to create a new class
 *
 */
void pluginModelerView::addNewTab()
{
    if (mModeler == 0)
        return;

    QWidget *tab = mModeler->openNewClass();
    addEditTab(tab, tr("NewClass") );
}

/**
 * @brief pluginModelerView::addEditTab
 *        Add a tab to edit an existing class
 *
 */
void pluginModelerView::addEditTab(QWidget *widget, QString name)
{
    int n = ui->tabWidget->addTab(widget, name);
    ui->tabWidget->setCurrentIndex(n);
    widget->show();
}

/**
 * @brief pluginModelerView::onSaveClass (slot)
 *        Called when a class is ready to save on disk
 *
 */
void pluginModelerView::onSaveClass(QString name)
{
    QString fileName = mPackage->getSrcFilePath(name + ".cpp");
    sourceCpp *src = mPackage->openSourceCpp(fileName);

    bool isNew = src->isNew();
    src->write( mModeler->getData(name) );

    // If a new file has been created
    if (isNew)
    {
        // Insert to the class list
        //addClass(name, classFile.fileName());
        addClass(name, src->getFilename());
        // Inform main window
        emit refreshFiles();
    }
}

/**
 * @brief pluginModelerView::onTabRename (slot)
 *        Called when a class is renamed by plugin
 *
 */
void pluginModelerView::onTabRename(QWidget *tab, QString name)
{
    int n = ui->tabWidget->indexOf(tab);
    ui->tabWidget->setTabText(n, name);
}

/**
 * @brief pluginModelerView::onDuplicateRequest (slot)
 *        Called by summary tab when a class duplication is required
 *
 */
void pluginModelerView::onDuplicateRequest(QString className)
{
    QObject *senderObject = QObject::sender();
    pluginModelerSummary *tab = qobject_cast<pluginModelerSummary *>(senderObject);
    if (tab == 0)
        return;

    // Note: It is unsafe to make a copy when the class is opened for editing

    // First, search if the class is opened
    sourceCpp *src = 0;
    for (int i = 0; i < mListSources.count(); i++)
    {
        src = mListSources.at(i);

        sourceCppTemplate *tpl = src->getTemplate();
        if (tpl == 0)
            continue;

        if (tpl->getTagValue("class") == className)
            break;
        src = 0;
    }

    // Class not found, allow to start duplication
    if (src == 0)
    {
        tab->startDuplicate();
    }
    // Class is opened, inform user that duplication is forbidden
    else
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Class is opened for editing, please close the tab first !"));
        msgBox.addButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

/**
 * @brief pluginModelerView::onDuplicateClass (slot)
 *        Called when a class duplication is needed
 *
 */
void pluginModelerView::onDuplicateClass(QString srcFile, QString dst)
{
    if (mModeler == 0)
        return;
    QObject *senderObject = QObject::sender();
    pluginModelerSummary *tab = qobject_cast<pluginModelerSummary *>(senderObject);
    if (tab == 0)
        return;

    QFile classFile( mPackage->getSrcFilePath(dst + ".cpp") );
    if (classFile.exists())
    {
        QMessageBox msgBox;
        msgBox.setText(tr("Class file already exists !"));
        msgBox.addButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    QFileInfo srcFileInfo = QFileInfo(srcFile);

    mModeler->cloneSrc(srcFileInfo.baseName(), dst);
    addClass(dst, mPackage->getSrcFilePath(dst + ".cpp"));

}

}}//namespaces
