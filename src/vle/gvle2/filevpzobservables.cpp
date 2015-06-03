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
#include <QDebug>
#include "filevpzobservables.h"
#include "ui_filevpzobservables.h"

FileVpzObservables::FileVpzObservables(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzObservables), mVpz(0)
{
    ui->setupUi(this);
}

FileVpzObservables::~FileVpzObservables()
{
    delete ui;
}

void FileVpzObservables::setVpz(vleVpz *vpz)
{
    mVpz = vpz;
    reload();
}

void FileVpzObservables::reload()
{
    reloadViews();
    reloadObservables();
}

void FileVpzObservables::reloadViews()
{
    if (!mVpz) {
        throw vle::utils::InternalError(
            " gvle2: error in FileVpzExpView::reloadViews");
    }
    std::vector<std::string> outputNames;
    mVpz->viewOutputNames(outputNames);
    ui->viewsList->clear();
    std::vector<std::string>::iterator itb = outputNames.begin();
    std::vector<std::string>::iterator ite = outputNames.end();
    for ( ; itb != ite; itb++) {
        ui->viewsList->addItem(QString(itb->c_str()));
    }
}

void FileVpzObservables::reloadObservables()
{
    if (!mVpz) {
        throw vle::utils::InternalError(
            " gvle2: error in FileVpzExpView::reloadObservables");
    }

    ui->obsTree->clear();
    ui->obsTree->setColumnCount(1);

    std::vector<std::string> observableNames;
    mVpz->viewObservableNames(observableNames);
    QList<QTreeWidgetItem *> obsItems;
    std::vector<std::string>::iterator itb = observableNames.begin();
    std::vector<std::string>::iterator ite = observableNames.end();
    for ( ; itb != ite; itb++) {

        QTreeWidgetItem *newObsItem = new QTreeWidgetItem();
        QString name = QString(itb->c_str());
        newObsItem->setText(0, name);
        newObsItem->setData(0, Qt::UserRole, FileVpzObservables::EObsObs);
        newObsItem->setIcon(0, *(new QIcon(":/icon/resources/bricks.png")));

        refresh(name, newObsItem);
        obsItems.append(newObsItem);
    }
    ui->obsTree->insertTopLevelItems(0, obsItems);
}

void FileVpzObservables::refresh(const QString& obsName,
                                QTreeWidgetItem *obsItem)
{
    if (obsItem == 0) {
        QList<QTreeWidgetItem *>listWidgetItem =
            ui->obsTree->findItems(obsName, Qt::MatchExactly, 0);
        if (listWidgetItem.count() <= 0) {
            return;
        }
        obsItem = listWidgetItem.at(0);
    }

    while (obsItem->childCount())
    {
        QTreeWidgetItem *child = obsItem->takeChild(0);
        delete child;
    }

    QDomNodeList portList = mVpz->obsPortsListFromDoc(obsName);
    for (unsigned int j = 0; j < portList.length(); j++) {
        QDomNode port = portList.at(j);

        QTreeWidgetItem *newPortItem = new QTreeWidgetItem();
        newPortItem->setText(0, mVpz->attributeValue(port, "name"));
        newPortItem->setData(0, Qt::UserRole+0,
                             FileVpzObservables::EObsPort);
        newPortItem->setIcon(0, *(new QIcon(":/icon/resources/cog.png")));

        obsItem->addChild(newPortItem);

        QDomNodeList OutList = port.childNodes();
        for (unsigned int k = 0; k < OutList.length(); k++) {
            QDomNode attachedView = OutList.at(k);

            QString viewName = mVpz->attributeValue(attachedView, "name");
            QTreeWidgetItem *newValueItem = new QTreeWidgetItem();
            newValueItem->setText(0, viewName);
            newValueItem->setData(0, Qt::UserRole, FileVpzObservables::EObsOut);
            newPortItem->addChild(newValueItem);
        }
    }
}
