/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "pluginmodelerview.h"
#include "ui_pluginmodelerview.h"
#include "pluginmodelersummary.h"

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
        delete mListSources.takeFirst();
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

/**
 * @brief pluginModelerView::setPackage
 *        Set the working package
 *
 */
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

    QObject::connect(mainTab, SIGNAL(openClass(QString)),
                     this,    SLOT  (onOpenTab(QString)));

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
void pluginModelerView::onOpenTab(QString filename)
{
    if (mModeler == 0)
        return;

    sourceCpp *src = 0;
    for (int i = 0; i < mListSources.count(); i++)
    {
        src = mListSources.at(i);
        if (src->getFilename() == filename)
            break;
        src = 0;
    }
    if (src == 0)
    {
        src = new sourceCpp(filename);
        mListSources.append(src);

        QWidget *tab = mModeler->openEditClass(src);
        sourceCppTemplate *tpl = src->getTemplate();
        addEditTab(tab, tpl->getTagValue("class") );
    }
    else
    {
        QWidget *w = mModeler->getEditClass(src);
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

    QString className = ui->tabWidget->tabText(index);

    for (int i = 0; i < mListSources.count(); i++)
    {
        sourceCpp *src = mListSources.at(i);
        sourceCppTemplate *tpl = src->getTemplate();
        QString srcClass = tpl->getTagValue("class");
        if (srcClass == className)
        {
            // Uncomment this line tu use the close() methode
            // In this case, the "delete w" must be removed
            //mModeler->closeEditClass(src);

            // Remove item from list ...
            mListSources.removeAt(i);
            // ... and delete it
            delete src;
            break;
        }
    }
    delete w;
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
    QFile classFile( mPackage->getSrcFilePath(name + ".cpp") );
    bool isNew = (classFile.exists() == false);
    classFile.open(QIODevice::WriteOnly);
    QTextStream out(&classFile);
    out << mModeler->getData(name);
    classFile.close();

    // If a new file has been created
    if (isNew)
    {
        // Insert to the class list
        addClass(name, classFile.fileName());
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
