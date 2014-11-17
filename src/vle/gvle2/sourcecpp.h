/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SOURCECPP_H
#define SOURCECPP_H

#include <QString>
#include "sourcecpptemplate.h"
#include "vlepackage.h"
#include "plugin_modeler.h"

class PluginModeler;
class vlePackage;

class sourceCpp
{
public:
    sourceCpp();
    ~sourceCpp();
    sourceCpp(QString filename, vlePackage *package = 0);
    sourceCppTemplate *getTemplate();
    PluginModeler     *getModeler();
    QString getFilename();
    void               setPackage(vlePackage *package);

private:
    QString mFileName;
    vlePackage        *mPackage;
    sourceCppTemplate *mTemplate;
    PluginModeler     *mModeler;
};

#endif // SOURCECPP_H
