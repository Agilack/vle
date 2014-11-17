/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QMessageBox>
#include "filevpzexpcond.h"
#include "ui_filevpzexpcond.h"

FileVpzExpCond::FileVpzExpCond(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileVpzExpCond)
{
    mCondEdition = 0;

    ui->setupUi(this);

    QTreeWidget *condTree  = ui->expTree;
    QTreeWidget *paramTree = ui->paramTree;
    // Set custom menu to handle right-click popups
    condTree->setContextMenuPolicy(Qt::CustomContextMenu);
    paramTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->paramValueList->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->paramValueTable->setContextMenuPolicy(Qt::CustomContextMenu);

    //
    QObject::connect(condTree, SIGNAL(itemSelectionChanged ()),
                     this,     SLOT  (onConditionTreeSelected()));
    QObject::connect(condTree, SIGNAL(itemChanged (QTreeWidgetItem *, int)),
                     this,     SLOT  (onConditionChanged(QTreeWidgetItem *, int)));
    QObject::connect(condTree, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,     SLOT  (onConditionTreeMenu(const QPoint&)));
    QObject::connect(paramTree, SIGNAL(itemSelectionChanged ()),
                     this,      SLOT  (onCondParamTreeSelected()));
    QObject::connect(paramTree, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,     SLOT  (onCondParamTreeMenu(const QPoint&)));

    QObject::connect(ui->paramValueList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                     this,               SLOT(onCondParamListDblClick(QListWidgetItem*)));
    QObject::connect(ui->paramValueList, SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,               SLOT(onCondParamListMenu(const QPoint&)));
    QObject::connect(ui->butParamTableChange, SIGNAL(clicked()),
                     this,                    SLOT(onParamTableChange()));
    QObject::connect(ui->paramValueTable,     SIGNAL(cellDoubleClicked(int,int)),
                     this,                    SLOT(onCondParamTableDblClick(int, int)));
    QObject::connect(ui->paramValueTable,     SIGNAL(customContextMenuRequested(const QPoint&)),
                     this,                    SLOT(onCondParamTableMenu(const QPoint&)));
    QObject::connect(ui->butParamTextChange,  SIGNAL(clicked()),
                     this,                    SLOT(onParamTextChange()));
    QObject::connect(ui->butEditSimpleSave,   SIGNAL(clicked()),
                     this,                    SLOT(onCondParamSave()));
    QObject::connect(ui->butEditSimpleCancel, SIGNAL(clicked()),
                     this,                    SLOT(onCondParamCancel()));
}

FileVpzExpCond::~FileVpzExpCond()
{
    delete ui;
}

/**
 * @brief FileVpzExpCond::setVpz
 *        Set the VPZ object to use
 *
 */
void FileVpzExpCond::setVpz(vleVpz *vpz)
{
    mVpz = vpz;

    QObject::connect(mVpz, SIGNAL(sigConditionsChanged()),
                     this, SLOT(onConditionTreeRefresh()));
}

/**
 * @brief FileVpzExpCond::reload
 *        Refresh the UI from VPZ datas
 *
 */
void FileVpzExpCond::reload()
{
    onConditionTreeRefresh();
}

/**
 * @brief FileVpzExpCond::onConditionTreeRefresh (slot)
 *        Update the Conditions tree for the current VPZ
 *
 */
void FileVpzExpCond::onConditionTreeRefresh()
{
    QTreeWidget *condTree = ui->expTree;
    // Reset current content
    condTree->clear();
    condTree->setColumnCount(1);

    QList<QTreeWidgetItem *> condItems;

    QList <vpzExpCond *> *condList = mVpz->getConditions();
    for (int i = 0; i < condList->length(); i++)
    {
        vpzExpCond *cond = condList->at(i);

        QTreeWidgetItem *newCondItem = new QTreeWidgetItem();
        newCondItem->setText(0, cond->getName());
        newCondItem->setData(0, Qt::UserRole+0, FileVpzExpCond::ECondCondition);
        newCondItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)cond));
        newCondItem->setIcon(0, *(new QIcon(":/icon/resources/bricks.png")));

        QList <vpzExpCondPort *> *portList = cond->getPorts();
        for (int j = 0; j < portList->length(); j++)
        {
            vpzExpCondPort *port = portList->at(j);

            QTreeWidgetItem *newPortItem = new QTreeWidgetItem();
            newPortItem->setText(0, port->getName());
            newPortItem->setData(0, Qt::UserRole+0, FileVpzExpCond::ECondParameter);
            newPortItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)port));
            newPortItem->setIcon(0, *(new QIcon(":/icon/resources/cog.png")));
            newCondItem->addChild(newPortItem);

            QList <vpzExpCondValue *> *valueList = port->getValues();
            for ( int k = 0; k < valueList->length(); k++)
            {
                vpzExpCondValue *flValue = valueList->at(k);

                QString sDisp;

                if (flValue->isAtomic())
                    sDisp = QString("%1 (%2)")
                            .arg(flValue->getDisplayValue())
                            .arg(flValue->getDisplayName());
                else
                    sDisp = flValue->getDisplayName();

                QTreeWidgetItem *newValueItem = new QTreeWidgetItem();
                newValueItem->setText(0, sDisp);

                newPortItem->addChild(newValueItem);
            }
        }

        condItems.append(newCondItem);
    }
    condTree->insertTopLevelItems(0, condItems);
}

/**
 * @brief FileVpzExpCond::onConditionTreeMenu (slot)
 *        Called on right-click into experience tree to show contextual menu
 *
 */
void FileVpzExpCond::onConditionTreeMenu(const QPoint pos)
{
    QPoint globalPos = ui->expTree->mapToGlobal(pos);

    // Get a pointer to the currently selected item into modelTree
    ExpCondTreeType curItemType;
    QTreeWidgetItem *item = ui->expTree->currentItem();
    if (item)
    {
        // Get the type of the currently selected item
        QVariant vItemType = item->data(0, Qt::UserRole+0);
        curItemType = (ExpCondTreeType)vItemType.toInt();
    }
    else
        curItemType = ECondNone;

    enum eMenuActions { EMenuCondAdd, EMenuCondRename, EMenuCondRemove,
                        EMenuPortAdd, EMenuPortRename, EMenuPortRemove };

    QAction *lastAction;

    QMenu myMenu;
    lastAction = myMenu.addAction(tr("Add condition"));
    lastAction->setData(EMenuCondAdd);
    lastAction = myMenu.addAction(tr("Rename condition"));
    lastAction->setData(EMenuCondRename);
    if (curItemType != ECondCondition)
        lastAction->setEnabled(false);
    lastAction = myMenu.addAction(tr("Remove condition"));
    lastAction->setData(EMenuCondRemove);
    if (curItemType != ECondCondition)
        lastAction->setEnabled(false);
    myMenu.addSeparator();
    lastAction = myMenu.addAction(tr("Add parameter"));
    lastAction->setData(EMenuPortAdd);
    if (curItemType != ECondCondition)
        lastAction->setEnabled(false);
    lastAction = myMenu.addAction(tr("Rename parameter"));
    lastAction->setData(EMenuPortRename);
    if (curItemType != ECondParameter)
        lastAction->setEnabled(false);
    lastAction = myMenu.addAction(tr("Remove parameter"));
    lastAction->setData(EMenuPortRemove);
    if (curItemType != ECondParameter)
        lastAction->setEnabled(false);

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        int actCode = selectedItem->data().toInt();

        // Case of menu "Add condition"
        if (actCode == EMenuCondAdd)
        {
            // Create a new condition into the current VPZ
            vpzExpCond *newCondition = mVpz->addCondition();
            newCondition->setName(tr("New condition"));

            // Create a Widget to show this condition into tree
            QTreeWidgetItem *newCondItem = new QTreeWidgetItem();
            newCondItem->setText(0, newCondition->getName());
            newCondItem->setData(0, Qt::UserRole+0, FileVpzExpCond::ECondCondition);
            newCondItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)newCondition));
            newCondItem->setIcon(0, *(new QIcon(":/icon/resources/bricks.png")));

            // Update widget flags to allow name update
            newCondItem->setFlags(newCondItem->flags() | Qt::ItemIsEditable);

            ui->expTree->addTopLevelItem(newCondItem);
            ui->expTree->editItem(newCondItem, 0);
            ui->expTree->setCurrentItem(newCondItem);
            mCondEdition = newCondItem;
        }
        // Case of menu "Rename condition"
        else if ((actCode == EMenuCondRename) && (item != 0))
        {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->expTree->editItem(item, 0);
            ui->expTree->setCurrentItem(item);
            mCondEdition = item;
        }
        // Case of menu "Remove Condition"
        if (actCode == EMenuCondRemove)
        {
            // Get the condition associated to the selected widget
            QVariant vCond = item->data(0, Qt::UserRole + 1);
            vpzExpCond *cond = (vpzExpCond *)vCond.value<void *>();
            // ToDo : test if used by a model ...
            mVpz->removeCondition(cond);

            int index = ui->expTree->indexOfTopLevelItem(item);
            ui->expTree->takeTopLevelItem(index);
            delete item;
        }
        // Case of menu "Add parameter"
        else if (actCode == EMenuPortAdd)
        {
            // Get the condition associated to the selected widget
            QVariant vCond = item->data(0, Qt::UserRole+1);
            vpzExpCond *cond = (vpzExpCond *)vCond.value<void *>();
            // Create a new Condition Port ...
            vpzExpCondPort *newPort = new vpzExpCondPort();
            newPort->setName(tr("New port"));
            // ... and insert it into Condition
            cond->addPort(newPort);

            // Create a Widget to show this port into tree
            QTreeWidgetItem *newPortItem = new QTreeWidgetItem();
            newPortItem->setText(0, newPort->getName());
            newPortItem->setData(0, Qt::UserRole+0, FileVpzExpCond::ECondParameter);
            newPortItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)newPort));
            newPortItem->setIcon(0, *(new QIcon(":/icon/resources/cog.png")));
            // Set widget flags to allow name update
            newPortItem->setFlags(newPortItem->flags() | Qt::ItemIsEditable);
            item->addChild(newPortItem);
            ui->expTree->editItem(newPortItem, 0);
            ui->expTree->setCurrentItem(newPortItem);

            mCondEdition = newPortItem;
        }
        // Case of menu "Rename parameter"
        else if ((actCode == EMenuPortRename) && (item != 0))
        {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
            ui->expTree->editItem(item, 0);
            ui->expTree->setCurrentItem(item);
            mCondEdition = item;
        }
        // Case of menu "Remove parameter"
        else if (actCode == EMenuPortRemove)
        {
            // Get the port associated to the selected widget
            QVariant vPort = item->data(0, Qt::UserRole+1);
            vpzExpCondPort *port = (vpzExpCondPort *)vPort.value<void *>();

            // Get the parent Condition
            vpzExpCond *cond = port->getParent();
            if (cond)
            {
                cond->removePort(port);
                QTreeWidgetItem *p = item->parent();
                p->removeChild(item);
            }
        }
    }
}

/**
 * @brief FileVpzExpCond::onConditionChanged (slot)
 *        Called when an expTree entry has been edited/modified
 *
 */
void FileVpzExpCond::onConditionChanged(QTreeWidgetItem *item, int column)
{
    ExpCondTreeType curItemType;

    (void)column;

    QVariant vItemType = item->data(0, Qt::UserRole+0);
    curItemType = (ExpCondTreeType)vItemType.toInt();

    if (curItemType == ECondCondition)
    {
        vpzExpCond *cond = 0;

        QVariant vCondPtr = item->data(0, Qt::UserRole + 1);
        if (vCondPtr.isValid())
        {
            void *ptr = vCondPtr.value<void *>();
            cond = (vpzExpCond *)ptr;
        }
        //
        if (cond && (item->text(0) != cond->getName()))
        {
            cond->setName(item->text(0));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            mCondEdition = 0;
        }
    }
    else if (curItemType == ECondParameter)
    {
        vpzExpCondPort *port = 0;

        // Retrieve the ExpCondPort from widget
        QVariant vPortPtr = item->data(0, Qt::UserRole + 1);
        if (vPortPtr.isValid())
        {
            void *ptr = vPortPtr.value<void *>();
            port = (vpzExpCondPort *)ptr;
        }
        if (port && (item->text(0) != port->getName()))
        {
            port->setName(item->text(0));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            mCondEdition = 0;
        }
    }
}

/**
 * @brief FileVpzExpCond::onConditionTreeSelected (slot)
 *        Called when selection has changed on Conditions tree
 *
 */
void FileVpzExpCond::onConditionTreeSelected()
{
    // Get a pointer to the currently selected item into modelTree
    QTreeWidgetItem *item = ui->expTree->currentItem();

    if (mCondEdition)
    {
        // Remove Edition flag
        mCondEdition->setFlags(mCondEdition->flags() ^ Qt::ItemIsEditable);
        mCondEdition = 0;
    }

    // Get the type of the currently selected item
    QVariant vItemType = item->data(0, Qt::UserRole+0);
    ExpCondTreeType itemType = (ExpCondTreeType)vItemType.toInt();

    if (itemType == ECondCondition)
    {
        ui->paramTree->clear();
        ui->paramTree->setEnabled(false);
        onCondParamTreeSelected();
        return;
    }
    else if (itemType == ECondParameter)
    {
        ui->paramTree->clear();

        // Retrieve the Condition Port from WidgetItem
        QVariant vItemPort = item->data(0, Qt::UserRole+1);
        vpzExpCondPort *port = (vpzExpCondPort *)vItemPort.value<void *>();

        QList<QTreeWidgetItem *> portTreeItems;
        // Get the list of values into this Port
        QList <vpzExpCondValue *> *valueList = port->getValues();
        condUpdateTree(valueList, &portTreeItems);

        ui->paramTree->insertTopLevelItems(0, portTreeItems);
        ui->paramTree->setEnabled(true);
        onCondParamTreeSelected();
    }
}

/**
 * @brief FileVpzExpCond::onCondParamTreeSelected (slot)
 *        Called when selection has changed on Parameters (values) tree
 *
 */
void FileVpzExpCond::onCondParamTreeSelected()
{
    vpzExpCondValue *value = 0;

    // Get a pointer to the currently selected item into paramTree
    QTreeWidgetItem *item = 0;
    if (ui->paramTree->topLevelItemCount())
        item = ui->paramTree->currentItem();

    if (item)
    {
        QVariant vValuePtr = item->data(0, Qt::UserRole+1);
        if (vValuePtr.isValid())
            value = (vpzExpCondValue *)vValuePtr.value<void *>();
    }
    if (value == 0)
    {
        ui->editValuesStack->setCurrentIndex(0);
        return;
    }

    condValueShowDetail(value);
}

/**
 * @brief FileVpzExpCond::onCondParamListDblClick (slot)
 *        Called when a double click event occur onto the param List widget
 *
 */
void FileVpzExpCond::onCondParamListDblClick(QListWidgetItem *item)
{
    vpzExpCondValue *value = 0;

    // Get condValue saved into the item (if set)
    QVariant vValue = item->data(Qt::UserRole+1);
    if (vValue.isValid())
        value = (vpzExpCondValue *)vValue.value<void *>();
    if (value == 0)
            return;

    // Select the value, and display detail using paramTree widget
    condParamTreeSelectByValue(value);
}

/**
 * @brief FileVpzExpCond::onParamTableChange (slot)
 *        Called when a click event occurs from "Update" button on param Table tab
 *
 */
void FileVpzExpCond::onParamTableChange()
{
    vpzExpCondValue *value = 0;

    QVariant vValue = ui->paramValueTable->property("vpzExpCondValue");
    if (vValue.isValid())
    {
        void *ptr = vValue.value<void *>();
        value = (vpzExpCondValue *)ptr;
    }
    if (value == 0)
        return;

    bool isRowsValid, isColsValid;
    int cols = ui->paramValueTableCols->text().toInt(&isColsValid);
    int rows = ui->paramValueTableRows->text().toInt(&isRowsValid);

    if ( !isColsValid || !isRowsValid)
        return;

    if (value->type() == vpzExpCondValue::TypeTable)
    {
        QList <vpzExpCondValue *> *valueList = value->getTable();
        valueList->clear();
        // Set the new table size
        value->setTableSize(rows, cols);
    }
    else if (value->type() == vpzExpCondValue::TypeMatrix)
    {
        QList <vpzExpCondValue *> *valueList = value->getMatrix();
        valueList->clear();
        // Set the new matrix size
        value->setMatrixSize(rows, cols);
    }
    else
        return;


    for(int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            vpzExpCondValue *subVal = new vpzExpCondValue(value);
            subVal->setType(vpzExpCondValue::TypeDouble);
            value->addObject(subVal);

            if (value->type() == vpzExpCondValue::TypeTable)
                subVal->setTableIndex(i, j);
        }
    }

    // Call condition tree to refresh param tree
    onConditionTreeSelected();
    // Re-select this value and show detail
    condParamTreeSelectByValue(value);
}

/**
 * @brief FileVpzExpCond::onCondParamTableDblClick (slot)
 *        Called when a double click event occur onto the param Table widget
 *
 */
void FileVpzExpCond::onCondParamTableDblClick(int row, int col)
{
    vpzExpCondValue *value = 0;

    // Get the item at the selected row/col position
    QTableWidgetItem *item = ui->paramValueTable->item(row, col);

    // Get condValue saved into the item (if set)
    QVariant vValue = item->data(Qt::UserRole+1);
    if (vValue.isValid())
        value = (vpzExpCondValue *)vValue.value<void *>();
    if (value == 0)
            return;

    // Select the value, and display detail using paramTree widget
    condParamTreeSelectByValue(value);
}

/**
 * @brief FileVpzExpCond::onCondParamTableMenu (slot)
 *        Called when the contextual menu of param Table widget is requested (right click)
 *
 */
void FileVpzExpCond::onCondParamTableMenu(const QPoint pos)
{
    vpzExpCondValue *value = 0;
    vpzExpCondValue *container = 0;

    QVariant vContainerValue = ui->paramValueTable->property("vpzExpCondValue");
    if (vContainerValue.isValid())
    {
        void *ptr = vContainerValue.value<void *>();
        container = (vpzExpCondValue *)ptr;
    }
    if (container == 0)
        return;
    // Only "Matrix" tables allow types changes
    if (container->type() != vpzExpCondValue::TypeMatrix)
        return;


    QPoint globalPos = ui->paramValueTable->mapToGlobal(pos);

    // Get the selected item
    QTableWidgetItem *item = ui->paramValueTable->currentItem();
    if (item == 0)
        return;

    // Retrieve the value associated with this item
    QVariant vValue = item->data(Qt::UserRole+1);
    if (vValue.isValid())
        value = (vpzExpCondValue *)vValue.value<void *>();
    if (value == 0)
        return;

    QAction *lastAction;

    QMenu ctxMenu;
    lastAction = ctxMenu.addAction(tr("Change to Boolean"));  lastAction->setData(11);
    lastAction = ctxMenu.addAction(tr("Change to Double"));   lastAction->setData(12);
    lastAction = ctxMenu.addAction(tr("Change to Integer"));  lastAction->setData(13);
    lastAction = ctxMenu.addAction(tr("Change to Null"));     lastAction->setData(14);
    lastAction = ctxMenu.addAction(tr("Change to String"));   lastAction->setData(15);
    ctxMenu.addSeparator();
    lastAction = ctxMenu.addAction(tr("Change to Map"));      lastAction->setData(21);
    lastAction = ctxMenu.addAction(tr("Change to Matrix"));   lastAction->setData(22);
    lastAction = ctxMenu.addAction(tr("Change to Set"));      lastAction->setData(23);
    lastAction = ctxMenu.addAction(tr("Change to Table"));    lastAction->setData(24);
    lastAction = ctxMenu.addAction(tr("Change to Tuple"));    lastAction->setData(25);
    lastAction = ctxMenu.addAction(tr("Change to Xml"));      lastAction->setData(26);

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem)
    {
        int actCode = selectedItem->data().toInt();
        if (actCode == 11)
            value->setType(vpzExpCondValue::TypeBoolean);
        if (actCode == 12)
            value->setType(vpzExpCondValue::TypeDouble);
        if (actCode == 13)
            value->setType(vpzExpCondValue::TypeInteger);
        if (actCode == 14)
            value->setType(vpzExpCondValue::TypeNull);
        if (actCode == 15)
            value->setType(vpzExpCondValue::TypeString);
        if (actCode == 21)
            value->setType(vpzExpCondValue::TypeMap);
        if (actCode == 22)
            value->setType(vpzExpCondValue::TypeMatrix);
        if (actCode == 23)
            value->setType(vpzExpCondValue::TypeSet);
        if (actCode == 24)
            value->setType(vpzExpCondValue::TypeTable);
        if (actCode == 25)
            value->setType(vpzExpCondValue::TypeTuple);
        if (actCode == 26)
            value->setType(vpzExpCondValue::TypeXml);
        // Call condition tree to refresh param tree
        onConditionTreeSelected();
        // Re-select this value and show detail
        condParamTreeSelectByValue(value);
    }
}

/**
 * @brief FileVpzExpCond::onCondParamTreeMenu (slot)
 *        Called on right-click into values tree to show contextual menu
 *
 */
void FileVpzExpCond::onCondParamTreeMenu(const QPoint pos)
{
    QPoint globalPos = ui->paramTree->mapToGlobal(pos);
    //
    QTreeWidgetItem *curPort  = ui->expTree->currentItem();
    QTreeWidgetItem *curValue = ui->paramTree->currentItem();

    vpzExpCondPort *port = 0;
    if (curPort)
    {
        QVariant vItemType = curPort->data(0, Qt::UserRole+0);
        int curItemType = (ExpCondTreeType)vItemType.toInt();
        if (curItemType != ECondParameter)
            return;

        QVariant vItemPort = curPort->data(0, Qt::UserRole+1);
        port = (vpzExpCondPort *)vItemPort.value<void *>();
        if (port == 0)
            return;
    }

    vpzExpCondValue *condValue = 0;
    if (curValue)
    {

        QVariant vItemValue = curValue->data(0, Qt::UserRole+1);
        condValue = (vpzExpCondValue *)vItemValue.value<void *>();
    }

    QAction *lastAction;

    QMenu ctxMenu;
    QMenu ctxMenuAdd(tr("Add"));
    lastAction = ctxMenuAdd.addAction(tr("Boolean"));  lastAction->setData(11);
    lastAction = ctxMenuAdd.addAction(tr("Double"));   lastAction->setData(12);
    lastAction = ctxMenuAdd.addAction(tr("Integer"));  lastAction->setData(13);
    lastAction = ctxMenuAdd.addAction(tr("Null"));     lastAction->setData(14);
    lastAction = ctxMenuAdd.addAction(tr("String"));   lastAction->setData(15);
    ctxMenuAdd.addSeparator();
    lastAction = ctxMenuAdd.addAction(tr("Map"));      lastAction->setData(21);
    lastAction = ctxMenuAdd.addAction(tr("Matrix"));   lastAction->setData(22);
    lastAction = ctxMenuAdd.addAction(tr("Set"));      lastAction->setData(23);
    lastAction = ctxMenuAdd.addAction(tr("Table"));    lastAction->setData(24);
    lastAction = ctxMenuAdd.addAction(tr("Tuple"));    lastAction->setData(25);
    lastAction = ctxMenuAdd.addAction(tr("Xml"));      lastAction->setData(26);
    ctxMenu.addMenu(&ctxMenuAdd);

    lastAction = ctxMenu.addAction(tr("Remove"));
    lastAction->setData(2);
    // Menu entry "remove" only allowed when a line is seleted
    if (curValue == 0)
        lastAction->setEnabled(false);

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem)
    {
        int actCode = selectedItem->data().toInt();
        vpzExpCondValue *newValue = 0;
        if ((actCode == 2) && (condValue != 0))
        {
            mVpz->delConditionValue(port, condValue);
        }
        if (actCode > 10)
        {
            vpzExpCondValue::ValueType newType;
            if (actCode == 11)  newType = vpzExpCondValue::TypeBoolean;
            if (actCode == 12)  newType = vpzExpCondValue::TypeDouble;
            if (actCode == 13)  newType = vpzExpCondValue::TypeInteger;
            if (actCode == 14)  newType = vpzExpCondValue::TypeNull;
            if (actCode == 15)  newType = vpzExpCondValue::TypeString;
            if (actCode == 21)  newType = vpzExpCondValue::TypeMap;
            if (actCode == 22)  newType = vpzExpCondValue::TypeMatrix;
            if (actCode == 23)  newType = vpzExpCondValue::TypeSet;
            if (actCode == 24)  newType = vpzExpCondValue::TypeTable;
            if (actCode == 25)  newType = vpzExpCondValue::TypeTuple;
            if (actCode == 26)  newType = vpzExpCondValue::TypeXml;
            newValue = mVpz->addConditionValue(port, newType);
        }
        // Call condition tree to refresh param tree
        onConditionTreeSelected();

        if (newValue)
        {
            condParamTreeSelectByValue(newValue);
        }
    }
}

/**
 * @brief FileVpzExpCond::onCondParamListMenu (slot)
 *        Called when the contextual menu of param List widget is requested (right click)
 *
 */
void FileVpzExpCond::onCondParamListMenu(const QPoint pos)
{
    vpzExpCondValue *value = 0;
    vpzExpCondValue *container = 0;

    QVariant vContainerValue = ui->paramValueList->property("vpzExpCondValue");
    if (vContainerValue.isValid())
    {
        void *ptr = vContainerValue.value<void *>();
        container = (vpzExpCondValue *)ptr;
    }
    if (container == 0)
        return;

    QPoint globalPos = ui->paramValueList->mapToGlobal(pos);

    // Get the selected item
    QListWidgetItem *item = ui->paramValueList->currentItem();
    if (item)
    {
        // Retrieve the value associated with this item
        QVariant vValue = item->data(Qt::UserRole+1);
        if (vValue.isValid())
            value = (vpzExpCondValue *)vValue.value<void *>();
    }

    QAction *lastAction;

    QMenu ctxMenu;

    QMenu ctxMenuChange(tr("Type"));
    lastAction = ctxMenuChange.addAction(tr("Change to Boolean"));  lastAction->setData(11);
    lastAction = ctxMenuChange.addAction(tr("Change to Double"));   lastAction->setData(12);
    lastAction = ctxMenuChange.addAction(tr("Change to Integer"));  lastAction->setData(13);
    lastAction = ctxMenuChange.addAction(tr("Change to Null"));     lastAction->setData(14);
    lastAction = ctxMenuChange.addAction(tr("Change to String"));   lastAction->setData(15);
    ctxMenuChange.addSeparator();
    lastAction = ctxMenuChange.addAction(tr("Change to Map"));      lastAction->setData(21);
    lastAction = ctxMenuChange.addAction(tr("Change to Matrix"));   lastAction->setData(22);
    lastAction = ctxMenuChange.addAction(tr("Change to Set"));      lastAction->setData(23);
    lastAction = ctxMenuChange.addAction(tr("Change to Table"));    lastAction->setData(24);
    lastAction = ctxMenuChange.addAction(tr("Change to Tuple"));    lastAction->setData(25);
    lastAction = ctxMenuChange.addAction(tr("Change to Xml"));      lastAction->setData(26);

    lastAction = ctxMenu.addAction(tr("Add value"));     lastAction->setData(1);
    lastAction = ctxMenu.addAction(tr("Remove value"));  lastAction->setData(2);
    if (value == 0)
        lastAction->setEnabled(false);

    lastAction = ctxMenu.addMenu(&ctxMenuChange);

    if ((value == 0) || (container->type() == vpzExpCondValue::TypeTuple))
        lastAction->setEnabled(false);

    QAction* selectedItem = ctxMenu.exec(globalPos);
    if (selectedItem)
    {
        vpzExpCondValue *vSelect = 0;
        int actCode = selectedItem->data().toInt();
        if (actCode == 1)
        {
            vpzExpCondValue *newValue = 0;
            newValue = new vpzExpCondValue();
            newValue->setType(vpzExpCondValue::TypeDouble);
            newValue->setDouble(0);
            container->addObject(newValue);
            vSelect = container;
        }
        if ((actCode == 2) && value)
        {
            container->delObject(value);
            vSelect = container;
        }
        if (value && (actCode > 10))
            vSelect = value;
        if (actCode == 11)
            value->setType(vpzExpCondValue::TypeBoolean);
        if (actCode == 12)
            value->setType(vpzExpCondValue::TypeDouble);
        if (actCode == 13)
            value->setType(vpzExpCondValue::TypeInteger);
        if (actCode == 14)
            value->setType(vpzExpCondValue::TypeNull);
        if (actCode == 15)
            value->setType(vpzExpCondValue::TypeString);
        if (actCode == 21)
            value->setType(vpzExpCondValue::TypeMap);
        if (actCode == 22)
            value->setType(vpzExpCondValue::TypeMatrix);
        if (actCode == 23)
            value->setType(vpzExpCondValue::TypeSet);
        if (actCode == 24)
            value->setType(vpzExpCondValue::TypeTable);
        if (actCode == 25)
            value->setType(vpzExpCondValue::TypeTuple);
        if (actCode == 26)
            value->setType(vpzExpCondValue::TypeXml);
        // Call condition tree to refresh param tree
        onConditionTreeSelected();
        // Re-select this value and show detail
        if (vSelect)
            condParamTreeSelectByValue(vSelect);
    }
}

/**
 * @brief FileVpzExpCond::onParamTextChange (slot)
 *        Called when a click event occurs on param Text update button
 *
 */
void FileVpzExpCond::onParamTextChange()
{
    vpzExpCondValue *value = 0;

    QVariant vValue = ui->paramValueText->property("vpzExpCondValue");
    if (vValue.isValid())
    {
        void *ptr = vValue.value<void *>();
        value = (vpzExpCondValue *)ptr;
    }
    if (value == 0)
        return;

    QString text = ui->paramValueText->toPlainText();

    if (value->type() == vpzExpCondValue::TypeXml)
        value->setXml(text);

    // Call condition tree to refresh param tree
    onConditionTreeSelected();
}

void FileVpzExpCond::onCondParamCancel()
{
    onCondParamTreeSelected();
}

void FileVpzExpCond::onCondParamSave()
{
    vpzExpCondValue *value = 0;
    QTreeWidgetItem *item = ui->paramTree->currentItem();

    if (item)
    {
        QVariant vValuePtr = item->data(0, Qt::UserRole+1);
        if (vValuePtr.isValid())
            value = (vpzExpCondValue *)vValuePtr.value<void *>();
    }
    // Save request on an unknown object ! Abort
    if (value == 0)
        return;

    vpzExpCondValue::ValueType itemType = value->type();

    if (itemType == vpzExpCondValue::TypeBoolean)
    {
        if (ui->editSimpleBoolean->currentIndex() == 1)
            value->setBoolean(true);
        else if (ui->editSimpleBoolean->currentIndex() == 2)
            value->setBoolean(false);
        else
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Please select true or false"));
            msgBox.exec();
        }

        // Refresh item into the param tree
        if (value->getBoolean() == true)
            item->setText(1, tr("true"));
        else
            item->setText(1, tr("false"));
    }
    else if (itemType == vpzExpCondValue::TypeDouble)
    {
        bool isOk;
        double d = ui->editSimpleDouble->text().toDouble(&isOk);
        if (isOk)
            value->setDouble(d);
        else
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Bad value for this data type"));
            msgBox.exec();
        }
        // Refresh item into the param tree
        item->setText(1, QString("%1").arg(value->getDouble()));
    }
    else if (itemType == vpzExpCondValue::TypeInteger)
    {
        bool isOk;
        int i = ui->editSimpleInteger->text().toInt(&isOk);
        if (isOk)
            value->setInteger(i);
        else
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Bad value for this data type"));
            msgBox.exec();
        }
        // Refresh item into the param tree
        item->setText(1, QString("%1").arg(value->getInteger()));
    }
    else if (itemType == vpzExpCondValue::TypeString)
    {
        value->setString(ui->editSimpleString->text());
        // Refresh item into the param tree
        item->setText(1, value->getString());
    }
}

void FileVpzExpCond::condParamTreeSelectByValue(vpzExpCondValue *reqValue, QTreeWidgetItem *base)
{
    vpzExpCondValue *value = 0;
    if (base)
    {
        int count = base->childCount();
        for (int i = 0; i < count; i++)
        {
            QTreeWidgetItem *item = base->child(i);
            value = 0;
            QVariant vValuePtr = item->data(0, Qt::UserRole+1);
            if (vValuePtr.isValid())
                value = (vpzExpCondValue *)vValuePtr.value<void *>();
            if (value && (value == reqValue))
            {
                ui->paramTree->setCurrentItem(item);
                break;
            }
            if (item->childCount())
                condParamTreeSelectByValue(reqValue, item);
        }
    }
    else
    {
        int count = ui->paramTree->topLevelItemCount();
        for (int i = 0; i < count; i++)
        {
            QTreeWidgetItem *item = ui->paramTree->topLevelItem(i);
            value = 0;
            QVariant vValuePtr = item->data(0, Qt::UserRole+1);
            if (vValuePtr.isValid())
                value = (vpzExpCondValue *)vValuePtr.value<void *>();
            if (value && (value == reqValue))
            {
                ui->paramTree->setCurrentItem(item);
                break;
            }
            if (item->childCount())
                condParamTreeSelectByValue(reqValue, item);
        }
    }
}

/**
 * @brief FileVpzExpCond::condUpdateTree
 *        Parse a vpzExpCondValue list and make a QTreeWidgetItem list :)
 *
 */
void FileVpzExpCond::condUpdateTree(QList <vpzExpCondValue *> *valueList, QList<QTreeWidgetItem *> *widList)
{
    // Read each value from the source list
    for (int i = 0; i < valueList->length(); i++)
    {
        vpzExpCondValue *value = valueList->at(i);

        QTreeWidgetItem *newItem = new QTreeWidgetItem();
        newItem->setText(0, value->getDisplayName());
        newItem->setData(0, Qt::UserRole+0, value->type());
        newItem->setData(0, Qt::UserRole+1, QVariant::fromValue((void*)value));

        if (value->type() == vpzExpCondValue::TypeBoolean)
        {
            if (value->getBoolean() == true)
                newItem->setText(1, tr("true"));
            else
                newItem->setText(1, tr("false"));
        }
        else if (value->type() == vpzExpCondValue::TypeDouble)
            newItem->setText(1, QString("%1").arg(value->getDouble()));
        else if (value->type() == vpzExpCondValue::TypeInteger)
            newItem->setText(1, QString("%1").arg(value->getInteger()));
        else if (value->type() == vpzExpCondValue::TypeNull)
            newItem->setText(1, QString("null"));
        else if (value->type() == vpzExpCondValue::TypeString)
            newItem->setText(1, value->getString());
        else if (value->type() == vpzExpCondValue::TypeMap)
        {
            QList<QTreeWidgetItem *> mapTreeItems;
            QList <vpzExpCondValue *> *valueList = value->getMap();
            condUpdateTree(valueList, &mapTreeItems);
            newItem->addChildren(mapTreeItems);
        }
        else if (value->type() == vpzExpCondValue::TypeMatrix)
        {
            QList<QTreeWidgetItem *> matrixTreeItems;
            QList <vpzExpCondValue *> *valueList = value->getMatrix();
            condUpdateTree(valueList, &matrixTreeItems);
            newItem->addChildren(matrixTreeItems);
        }
        else if (value->type() == vpzExpCondValue::TypeSet)
        {
            QList<QTreeWidgetItem *> setTreeItems;
            QList <vpzExpCondValue *> *valueList = value->getSet();
            condUpdateTree(valueList, &setTreeItems);
            newItem->addChildren(setTreeItems);
        }
        else if (value->type() == vpzExpCondValue::TypeTable)
        {
            QList<QTreeWidgetItem *> tableTreeItems;
            QList <vpzExpCondValue *> *valueList = value->getTable();
            condUpdateTree(valueList, &tableTreeItems);
            newItem->addChildren(tableTreeItems);
        }
        else if (value->type() == vpzExpCondValue::TypeTuple)
        {
            QList<QTreeWidgetItem *> tupleTreeItems;
            QList <vpzExpCondValue *> *valueList = value->getTuple();
            condUpdateTree(valueList, &tupleTreeItems);
            newItem->addChildren(tupleTreeItems);
        }
        else if (value->type() == vpzExpCondValue::TypeXml)
        {
            newItem->setText(1, value->getXml());
        }
        else
            newItem->setText(1, "[Type not currently supported]");

        widList->append(newItem);
    }
}

void FileVpzExpCond::condValueShowDetail(vpzExpCondValue *value)
{
    ui->editSimpleBoolean->setCurrentIndex(0);
    ui->editSimpleBoolean->setEnabled(false);
    ui->editSimpleDouble->setText("");
    ui->editSimpleDouble->setEnabled(false);
    ui->editSimpleInteger->setText("");
    ui->editSimpleInteger->setEnabled(false);
    ui->editSimpleString->setText("");
    ui->editSimpleString->setEnabled(false);

    // Get the type of the currently selected item
    vpzExpCondValue::ValueType itemType = value->type();

    // Simples "atomic" types
    if (itemType == vpzExpCondValue::TypeBoolean)
    {
        if (value->getBoolean())
            ui->editSimpleBoolean->setCurrentIndex(1);
        else
            ui->editSimpleBoolean->setCurrentIndex(2);
        ui->editSimpleBoolean->setEnabled(true);
        ui->editValuesStack->setCurrentIndex(1);
    }
    else if (itemType == vpzExpCondValue::TypeDouble)
    {
        QString txt = QString("%1").arg(value->getDouble());
        ui->editSimpleDouble->setText(txt);
        ui->editSimpleDouble->setEnabled(true);
        ui->editValuesStack->setCurrentIndex(1);
    }
    else if (itemType == vpzExpCondValue::TypeInteger)
    {
        QString txt = QString("%1").arg(value->getInteger());
        ui->editSimpleInteger->setText(txt);
        ui->editSimpleInteger->setEnabled(true);
        ui->editValuesStack->setCurrentIndex(1);
    }
    else if (itemType == vpzExpCondValue::TypeString)
    {
        ui->editSimpleString->setText(value->getString());
        ui->editSimpleString->setEnabled(true);
        ui->editValuesStack->setCurrentIndex(1);
    }
    //
    else if (itemType == vpzExpCondValue::TypeMap)
    {
        paramTreeUpdateList(value);
        ui->editValuesStack->setCurrentIndex(2);
    }
    else if (itemType == vpzExpCondValue::TypeMatrix)
    {
        paramTreeUpdateTable(value);
        ui->editValuesStack->setCurrentIndex(3);
    }
    else if (itemType == vpzExpCondValue::TypeSet)
    {
        paramTreeUpdateList(value);
        ui->editValuesStack->setCurrentIndex(2);
    }
    else if (itemType == vpzExpCondValue::TypeTable)
    {
        paramTreeUpdateTable(value);
        ui->editValuesStack->setCurrentIndex(3);
    }
    else if (itemType == vpzExpCondValue::TypeTuple)
    {
        paramTreeUpdateList(value);
        ui->editValuesStack->setCurrentIndex(2);
    }
    else if (itemType == vpzExpCondValue::TypeXml)
    {
        paramTreeUpdateText(value);
        ui->editValuesStack->setCurrentIndex(4);
    }
    else
        ui->editValuesStack->setCurrentIndex(0);
}

void FileVpzExpCond::paramTreeUpdateList(vpzExpCondValue *value)
{
    QList <vpzExpCondValue *> *valueList = 0;

    // Remove the current content of the widget list
    ui->paramValueList->clear();

    // Save a pointer to the associated value
    ui->paramValueList->setProperty("vpzExpCondValue", QVariant::fromValue((void*)value));

    // Get the list of sub-values according to container type
    if (value->type() == vpzExpCondValue::TypeMap)
        valueList = value->getMap();
    else if (value->type() == vpzExpCondValue::TypeSet)
        valueList = value->getSet();
    else if (value->type() == vpzExpCondValue::TypeTuple)
        valueList = value->getTuple();

    // If no list found, nothing to do :(
    if (valueList == 0)
        return;

    for (int i = 0; i < valueList->length(); i++)
    {
        vpzExpCondValue *v = valueList->at(i);

        QString vString;
        // Put the key name at the begining of the name
        if (value->type() == vpzExpCondValue::TypeMap)
            vString.append( QString("%1 : ").arg(v->getName()) );
        // Insert the value
        vString.append( v->getDisplayValue() );
        // Insert the value type
        if (value->type() != vpzExpCondValue::TypeTuple)
            vString.append( QString(" (%1)").arg(v->getTypeName()) );

        QListWidgetItem *item = new QListWidgetItem();
        item->setData(Qt::UserRole+1, QVariant::fromValue((void*)v));
        item->setText(vString);
        ui->paramValueList->addItem(item);
    }
}

void FileVpzExpCond::paramTreeUpdateTable(vpzExpCondValue *value)
{
    int row, col;

    if (value->type() == vpzExpCondValue::TypeTable)
        value->getTableSize(&row, &col);
    else if (value->type() == vpzExpCondValue::TypeMatrix)
        value->getMatrixSize(&row, &col);
    else
        return;

    // Save a pointer to the associated value
    ui->paramValueTable->setProperty("vpzExpCondValue", QVariant::fromValue((void*)value));

    // Update Table Widget size
    ui->paramValueTable->setColumnCount(col);
    ui->paramValueTable->setRowCount(row);

    // Update display of cols and rows
    ui->paramValueTableCols->setText( QString("%1").arg(col) );
    ui->paramValueTableRows->setText( QString("%1").arg(row) );

    QList <vpzExpCondValue *> *valueList = 0;
    if (value->type() == vpzExpCondValue::TypeTable)
        valueList = value->getTable();
    else if (value->type() == vpzExpCondValue::TypeMatrix)
        valueList = value->getMatrix();

    if (valueList)
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < col; j++)
            {
                vpzExpCondValue *selValue = valueList->at((i*col) + j);

                QTableWidgetItem *item = new QTableWidgetItem();
                item->setData(Qt::UserRole+1, QVariant::fromValue((void*)selValue));
                if (selValue->isAtomic())
                    item->setText(selValue->getDisplayValue());
                else
                    item->setText(selValue->getDisplayName());
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                ui->paramValueTable->setItem(i, j, item);
            }
        }
    }
}

/**
 * @brief FileVpzExpCond::paramTreeUpdateText
 *        Update the param value tab for Text content (text area editor)
 *
 */
void FileVpzExpCond::paramTreeUpdateText(vpzExpCondValue *value)
{
    QString text;

    if (value->type() == vpzExpCondValue::TypeXml)
        text = value->getXml();

    ui->paramValueText->setProperty("vpzExpCondValue", QVariant::fromValue((void*)value));
    ui->paramValueText->setText(text);
}
