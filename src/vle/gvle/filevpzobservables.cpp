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

namespace vle {
namespace gvle {


FileVpzObservables::FileVpzObservables(QWidget *parent) :
    QWidget(parent), ui(new Ui::FileVpzObservables), mVpm(0)
{
    ui->setupUi(this);

    QTreeWidget *obsTree = ui->obsTree;

    obsTree->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(obsTree,
                     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this, SLOT(onViewTreeMenu(const QPoint&)));
    QObject::connect(obsTree,
                     SIGNAL(itemChanged (QTreeWidgetItem *, int)),
                     this, SLOT(onItemChanged(QTreeWidgetItem *, int)));
}

int FileVpzObservables::itemType(const QTreeWidgetItem * item) const
{
    QList<QVariant> qVList = item->data(0, Qt::UserRole).toList();
    QVariant vItemType = qVList.at(0);
    return (ObsTreeType)vItemType.toInt();
}

QString FileVpzObservables::itemName(QTreeWidgetItem * item) const
{
    QList<QVariant> qVList = item->data(0, Qt::UserRole).toList();
    QVariant vItemType = qVList.at(1);
    return vItemType.toString();
}

QTreeWidgetItem* FileVpzObservables::newItem(const ObsTreeType type, const QString name)
{
    QTreeWidgetItem *newItem = new QTreeWidgetItem();

    newItem->setText(0, name);
    QList<QVariant> dataList;
    dataList.append(type);
    dataList.append(name);
    QVariant data(dataList);
    newItem->setData(0, Qt::UserRole, data);

    switch (type) {
    case EObsObs: {
        newItem->setIcon(0, *(new QIcon(":/icon/resources/bricks.png")));
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        break;
    }
    case EObsPort: {
        newItem->setIcon(0, *(new QIcon(":/icon/resources/cog.png")));
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
        break;
    }
    case EObsOut:
    case EObsNone: {
        break;
    }}
    return newItem;
}

void FileVpzObservables::onViewTreeMenu(const QPoint pos)
{
    QPoint globalPos = ui->obsTree->mapToGlobal(pos);

    QAction *lastAction;

    QMenu ctxMenu;
    lastAction = ctxMenu.addAction(tr("Add observable"));
    lastAction->setData(1);
    lastAction = ctxMenu.addAction(tr("Remove observable"));
    lastAction->setData(2);
    lastAction = ctxMenu.addAction(tr("Add port"));
    lastAction->setData(3);
    lastAction = ctxMenu.addAction(tr("Remove port"));
    lastAction->setData(4);
    lastAction = ctxMenu.addAction(tr("Attach views"));
    lastAction->setData(5);
    lastAction = ctxMenu.addAction(tr("Detach views"));
    lastAction->setData(6);

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem) {
        int actCode = selectedItem->data().toInt();
        if (actCode == 1) {
            QString name = mVpm->newObsNameToDoc();
            mVpm->addObservableToDoc(name);
            QTreeWidgetItem *newObsItem = newItem(FileVpzObservables::EObsObs, name);
            ui->obsTree->addTopLevelItem(newObsItem);
            ui->obsTree->editItem(newObsItem, 0);
            ui->obsTree->setCurrentItem(newObsItem);

        } else if (actCode == 2) {
            foreach( QTreeWidgetItem *item, ui->obsTree->selectedItems() ) {
                if (itemType(item) == FileVpzObservables::EObsObs) {
                    mVpm->rmObservableFromDoc(item->text(0));
                    int index = ui->obsTree->indexOfTopLevelItem(item);
                    ui->obsTree->takeTopLevelItem(index);
                    delete item;
                }
            }
        } else if (actCode == 3) {
            QTreeWidgetItem *item = ui->obsTree->currentItem();
            if (itemType(item) == FileVpzObservables::EObsObs) {
                QString name = mVpm->newObsPortNameToDoc(itemName(item));
                mVpm->addObsPortToDoc(itemName(item), name);
                QTreeWidgetItem *newPortItem = newItem(
                        FileVpzObservables::EObsPort, name);
                item->addChild(newPortItem);
            }
        } else if (actCode == 4) {
            foreach(QTreeWidgetItem *item, ui->obsTree->selectedItems()) {
                if (itemType(item) == FileVpzObservables::EObsPort) {
                    QTreeWidgetItem *obs = item->parent();
                    mVpm->rmObsPortToDoc(itemName(item->parent()), item->text(0));
                    obs->removeChild(item);
                    delete item;
                }
            }
        } else if (actCode == 5) {
            foreach(QListWidgetItem *itemView, ui->viewsList->selectedItems()) {
                QString viewName = itemView->text();
                foreach( QTreeWidgetItem *itemPort, ui->obsTree->selectedItems() ) {
                    if (itemType(itemPort) == FileVpzObservables::EObsPort) {
                        QTreeWidgetItem *obs = itemPort->parent();
                        if (not mVpm->existViewFromObsPortDoc(itemName(obs),
                                                              itemName(itemPort), viewName)) {
                            mVpm->attachViewToObsPortDoc(itemName(obs),
                                                         itemName(itemPort), viewName);
                            QTreeWidgetItem *newValueItem = newItem(FileVpzObservables::EObsOut,
                                                                    viewName);
                            itemPort->addChild(newValueItem);

                        }
                    }
                }
            }
        } else if (actCode == 6) {
            foreach( QTreeWidgetItem *item, ui->obsTree->selectedItems() ) {
                if (itemType(item) == FileVpzObservables::EObsOut) {
                    QTreeWidgetItem *port = item->parent();
                    QTreeWidgetItem *obs = port->parent();
                    mVpm->rmViewToObsPortDoc(itemName(obs),
                                             itemName(port), itemName(item));
                    port->removeChild(item);
                    delete item;
                }
            }
        }
    }
}


void
FileVpzObservables::onItemChanged(QTreeWidgetItem *item, int /*column*/)
{
    QList<QVariant> qVList = item->data(0, Qt::UserRole).toList();
    QVariant vItemType = qVList.at(0);
    ObsTreeType curItemType = (ObsTreeType)vItemType.toInt();
    QVariant vItemVal = qVList.at(1);
    QString curItemValue = vItemVal.toString();
    switch (curItemType) {
    case EObsObs:{
        if (curItemValue != item->text(0)) {
            mVpm->renameObservableToDoc(curItemValue, item->text(0));
            qVList[1] = item->text(0);
            QVariant data(qVList);
            item->setData(0, Qt::UserRole, data);
        }
        break;
    } case EObsPort: {
        if (curItemValue != item->text(0)) {
            mVpm->renameObsPortToDoc(item->parent()->text(0),
                    curItemValue, item->text(0));
            qVList[1] = item->text(0);
            QVariant data(qVList);
            item->setData(0, Qt::UserRole, data);
        }
        break;
    } default :
        break;
    }
}

void
FileVpzObservables::onViewsUpdated()
{
    reload();
}

void
FileVpzObservables::onUndoRedoVpm(QDomNode /*oldVpz*/, QDomNode /*newVpz*/,
        QDomNode /*oldVpm*/, QDomNode /*newVpm*/)
{
    reload();
}

FileVpzObservables::~FileVpzObservables()
{
    delete ui;
}

void FileVpzObservables::setVpm(vleVpm* vpm)
{
    mVpm = vpm;
    reload();
    QObject::connect(mVpm,   SIGNAL(viewsUpdated()),
                     this, SLOT(onViewsUpdated()));
    QObject::connect(mVpm,
            SIGNAL(undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)),
            this,
            SLOT(onUndoRedoVpm(QDomNode, QDomNode, QDomNode, QDomNode)));
}

void FileVpzObservables::reload()
{
    reloadViews();
    reloadObservables();
}

void FileVpzObservables::reloadViews()
{
    if (!mVpm) {
        throw vle::utils::InternalError(
            " gvle: error in FileVpzExpView::reloadViews");
    }
    std::vector<std::string> outputNames;
    mVpm->viewOutputNames(outputNames);
    ui->viewsList->clear();
    std::vector<std::string>::iterator itb = outputNames.begin();
    std::vector<std::string>::iterator ite = outputNames.end();
    for ( ; itb != ite; itb++) {
        ui->viewsList->addItem(QString(itb->c_str()));
    }
}

void FileVpzObservables::reloadObservables()
{
    if (!mVpm) {
        throw vle::utils::InternalError(
            " gvle: error in FileVpzExpView::reloadObservables");
    }

    ui->obsTree->clear();
    ui->obsTree->setColumnCount(1);

    std::vector<std::string> observableNames;
    mVpm->viewObservableNames(observableNames);
    QList<QTreeWidgetItem *> obsItems;
    std::vector<std::string>::iterator itb = observableNames.begin();
    std::vector<std::string>::iterator ite = observableNames.end();
    for ( ; itb != ite; itb++) {
        QTreeWidgetItem* newObsItem = newItem(FileVpzObservables::EObsObs,
                itb->c_str());
        reloadPorts(itb->c_str(), newObsItem);
        obsItems.append(newObsItem);
    }
    ui->obsTree->insertTopLevelItems(0, obsItems);
    ui->obsTree->expandAll();
}

void FileVpzObservables::reloadPorts(const QString& obsName,
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

    QDomNodeList portList = mVpm->obsPortsListFromDoc(obsName);
    for (int j = 0; j < portList.length(); j++) {
        QDomNode port = portList.at(j);
        QTreeWidgetItem *newPortItem = newItem(
                FileVpzObservables::EObsPort,
                mVpm->vdo()->attributeValue(port, "name"));
        obsItem->addChild(newPortItem);
        QList<QDomNode> OutList = mVpm->childNodesWithoutText(port,"attachedview");
        for (int k = 0; k < OutList.size(); k++) {
            QDomNode& attachedView = OutList[k];
            QTreeWidgetItem *newValueItem = newItem(
                    FileVpzObservables::EObsOut,
                    mVpm->vdo()->attributeValue(attachedView, "name"));

            newPortItem->addChild(newValueItem);
        }
    }
}

}} //namespaces
