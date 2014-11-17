/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "pluginmodelersummary.h"
#include "ui_pluginmodelersummary.h"

/**
 * @brief pluginModelerSummary::pluginModelerSummary
 *        Default constructor
 *
 */
pluginModelerSummary::pluginModelerSummary(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pluginModelerSummary)
{
    ui->setupUi(this);
    QObject::connect(ui->classList, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
                     this,          SLOT  (onSelectClass(QListWidgetItem *)));
}

/**
 * @brief pluginModelerSummary::~pluginModelerSummary
 *        Default destructor
 *
 */
pluginModelerSummary::~pluginModelerSummary()
{
    delete ui;
}

/**
 * @brief pluginModelerSummary::addClassList
 *        Add an entry to the class list
 *
 */
void pluginModelerSummary::addClassList(QString name, QString filename)
{
    // Allocate a new item
    QListWidgetItem *item = new QListWidgetItem();
    // This item show the class name, and save the associated file
    item->setText(name);
    item->setData(Qt::UserRole, QVariant(filename));
    // Then, add tis new item to the class list
    ui->classList->addItem(item);
}

/**
 * @brief pluginModelerSummary::onSelectClass (slot)
 *        Called when user select a class into list widget
 *
 */
void pluginModelerSummary::onSelectClass(QListWidgetItem *item)
{
    if (item == 0)
        return;

    // Get the file associated with the selected class
    QVariant vFilename = item->data(Qt::UserRole);
    if ( ! vFilename.isValid())
        return;

    emit openClass(vFilename.toString());
}
