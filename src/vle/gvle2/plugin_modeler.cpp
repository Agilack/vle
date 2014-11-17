/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "plugin_modeler.h"

/**
 * @brief PluginModeler::PluginModeler
 *        Default constructor of PluginModeler object
 *
 */
PluginModeler::PluginModeler()
{
    mMainTabWidget = 0;
}

/**
 * @brief PluginModeler::~PluginModeler
 *        Default destructor
 *
 */
PluginModeler::~PluginModeler()
{
    if (mMainTabWidget)
        delete mMainTabWidget;
}

/**
 * @brief PluginModeler::useCustomMainTab
 *        Return a bool that inform if plugin use a custom Main tab
 *
 */
bool PluginModeler::useCustomMainTab()
{
    return false;
}

/**
 * @brief PluginModeler::getMainTabWidget
 *        Get the default Main tab for plugin
 *
 */
QWidget *PluginModeler::getMainTabWidget()
{
    if (mMainTabWidget == 0)
        mMainTabWidget = new QWidget();

    return mMainTabWidget;
}

/**
 * @brief PluginModeler::getEditClass
 *        Get the widget of an already opened class
 *
 */
QWidget *PluginModeler::getEditClass(sourceCpp *src)
{
    (void) src;
    return 0;
}

/**
 * @brief PluginModeler::closeEditClass
 *        Close and delete a previously opened class widget
 *
 */
void PluginModeler::closeEditClass(sourceCpp *src)
{
    (void) src;
    return;
}
