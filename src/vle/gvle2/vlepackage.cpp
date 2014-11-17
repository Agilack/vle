/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QPluginLoader>
#include "vlepackage.h"
#include "plugin_modeler.h"
#include "sourcecpp.h"

vlePackage::vlePackage()
{
    mStdPackage = 0;
}

vlePackage::vlePackage(QString path)
{
    mStdPackage = 0;
    mDir.setPath(path);

    QDir dir(mDir);
    dir.cdUp();
    QDir::setCurrent( dir.path() );

    if (mDir.exists("src"))
        refreshModelerClass();
}

QString vlePackage::getName()
{
    return mDir.dirName();
}

QString vlePackage::getSrcFilePath(QString name)
{
    QDir dir(mDir);
    dir.cd("src");
    return dir.filePath(name);
}

void vlePackage::setStdPackage(vle::utils::Package *pkg)
{
    mStdPackage = pkg;

    mStdPackage->select( mDir.dirName().toStdString() );
}

void vlePackage::addModeler(QString filename)
{
    if (mModelers.key(filename).isEmpty())
    {
        QPluginLoader *loader = new QPluginLoader(filename);
        QObject *plugin = loader->instance();
        if ( ! loader->isLoaded())
        {
            delete loader;
            return;
        }

        PluginModeler *modeler = qobject_cast<PluginModeler *>(plugin);
        if (modeler)
        {
            mModelers.insert(modeler->getname(), filename);
        }
        delete loader;
    }
}

PluginModeler *vlePackage::getModelerByClass(QString className)
{
    // Search the plugin that own the specified class
    QString pluginName = getModelerClassPlugin(className);

    return getModelerByPlugin(pluginName);
}

PluginModeler *vlePackage::getModelerByPlugin(QString pluginName)
{
    // Search the library file of the plugin
    QString pluginFile = mModelers.value(pluginName);

    QPluginLoader *loader = new QPluginLoader(pluginFile);
    QObject *plugin = loader->instance();

    PluginModeler *modeler = qobject_cast<PluginModeler *>(plugin);

    delete loader;

    return modeler;
}

int  vlePackage::getModelerCount()
{
    return mModelers.count();
}

QString vlePackage::getModelerName(int pos)
{
    QList<QString> k = mModelers.keys();
    return k.at(pos);
}

int  vlePackage::getModelerClassCount()
{
    return mModelerClass.count();
}

QString vlePackage::getModelerClass(int pos)
{
    QList<QString> k = mModelerClass.keys();

    return k.at(pos);
}

QString vlePackage::getModelerClassPlugin(QString className)
{
    return mModelerClass.value(className);
}

void vlePackage::refreshModelerClass()
{
    QDir srcDir(mDir);
    srcDir.cd("src");

    QFileInfoList list = srcDir.entryInfoList();
    QListIterator<QFileInfo> files( list );
    while( files.hasNext() )
    {
        QFileInfo fileInfo = files.next();
        if ( ! fileInfo.isFile())
            continue;
        sourceCpp srcFile( fileInfo.filePath() );
        if (srcFile.getTemplate()->isValid())
        {
            QString className  = srcFile.getTemplate()->getTagValue("class");
            QString pluginName = srcFile.getTemplate()->getPluginName();
            mModelerClass.insert(className, pluginName);
        }
    }
}
