/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "sourcecpp.h"

/**
 * @brief sourceCpp::sourceCpp
 *        Default constructor
 *
 */
sourceCpp::sourceCpp()
{
    mModeler  = 0;
    mPackage  = 0;
    mTemplate = 0;
}

/**
 * @brief sourceCpp::sourceCpp
 *        Constructor with a source filename specified
 *
 */
sourceCpp::sourceCpp(QString filename, vlePackage *package)
{
    mFileName = filename;
    mModeler  = 0;
    mPackage  = package;
    mTemplate = 0;
}

/**
 * @brief sourceCpp::~sourceCpp
 *        Default destructor
 *
 */
sourceCpp::~sourceCpp()
{
    if (mTemplate)
        delete mTemplate;
}

/**
 * @brief sourceCpp::getFilename
 *        Getter for the filename property
 *
 */
QString sourceCpp::getFilename()
{
    return mFileName;
}

/**
 * @brief sourceCpp::getTemplate
 *        Get a template object for this source file
 *
 */
sourceCppTemplate *sourceCpp::getTemplate()
{
    if (mTemplate == 0)
    {
        mTemplate = new sourceCppTemplate(this);
    }
    return mTemplate;
}

PluginModeler *sourceCpp::getModeler()
{
    if (mModeler)
        return mModeler;

    if (mPackage == 0)
        return 0;

    sourceCppTemplate *tpl = getTemplate();
    if (tpl == 0)
        return 0;

    QString name = tpl->getPluginName();

    PluginModeler *modeler = mPackage->getModelerByPlugin(name);

    mModeler = modeler;

    return modeler;
}
/* EOF */
