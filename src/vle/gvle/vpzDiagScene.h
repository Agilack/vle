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

#ifndef gvle_VPZ_DIAG_SCENE_H
#define gvle_VPZ_DIAG_SCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QUndoStack>
#include <QUndoView>

#include "vlevpm.h"
#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#endif

#include <QtDebug>

namespace vle {
namespace gvle {


class VpzDiagScene;

enum {
    VpzPortTypeIn         = QGraphicsItem::UserType + 1,
    VpzPortTypeOut        = QGraphicsItem::UserType + 2,
    VpzSubModelType       = QGraphicsItem::UserType + 3,
    VpzMainModelType      = QGraphicsItem::UserType + 4,
    VpzConnectionLineType = QGraphicsItem::UserType + 5
};
enum SEL_TYPE {
    TOP,
    RIGHT,
    BOTTOM,
    LEFT,
    MIDDLE
};

enum VPZ_DIAG_MENU_ACTION {
    VDMA_copy_models,
    VDMA_paste_models,
    VDMA_Edit_name,
    VDMA_Remove,
    VDMA_Add_input_port,
    VDMA_Add_output_port,
    VDMA_Add_atomic_model,
    VDMA_Add_coupled_model
};

class VpzConnectionLineItem : public QGraphicsItem
{
public:
    VpzConnectionLineItem(QDomNode node, vleVpm* vpz, QLineF l,
            QGraphicsItem* parent, VpzDiagScene* scene);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0);
    QRectF boundingRect() const;
    void update(const QRectF & rect = QRectF());
    int type() const;

    vleVpm* mVpm;
    QDomNode mnode;
};

class VpzPortItem : public QGraphicsObject
{
    Q_OBJECT
public:
    VpzPortItem(QDomNode node, vleVpm* vpz, bool input,
            QGraphicsItem* parent, VpzDiagScene* scene);
    QRectF boundingRect() const;
    void update(const QRectF & rect = QRectF());
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                QWidget *widget = 0);
    QPointF getConnectionPoint();
    QString getPortName();
    QGraphicsTextItem* getTextItem() const;
    QGraphicsPixmapItem* getPixItem() const;
    void setNameEdition(bool val);
    int type() const;
    void hoverEnterEvent(QGraphicsSceneHoverEvent * evt);

public:
    vleVpm* mVpm;
    QDomNode mnode;
    bool mInput;
};

class VpzModelItem : public QGraphicsItem
{
public:
    VpzModelItem(QDomNode node, vleVpm* vpz, QGraphicsItem* parent,
                QGraphicsScene* scene);
    QGraphicsTextItem* getTitle() const;
    QGraphicsRectItem* getRectangle() const;

    QList<VpzPortItem*> getInPorts();
    QList<VpzPortItem*> getOutPorts();

    VpzPortItem* getInPort(const QString& name);
    VpzPortItem* getOutPort(const QString& name);
    void clearPorts();
    void clearTitle();
    QString getModelName();
    double widthInputPorts();
    double widthOutputPorts();
    double heightPort();
    void setNameEdition(bool val);

    vleVpm* mVpm;
    QDomNode mnode;
    double margin;
    double rectWidth;
    double rectHeight;
};

class VpzSubModelItem : public VpzModelItem
{

public:

    VpzSubModelItem(QDomNode node, vleVpm* vpz, QGraphicsItem* parent,
            QGraphicsScene* scene);
    virtual ~VpzSubModelItem();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget = 0);
    void initializeFromDom();
    void update(const QRectF & rect = QRectF());
    void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    SEL_TYPE getSelType(QPointF selPoint);
    QString getModelName();
    int type() const;
    void removeNameEditionMode();
};

class VpzMainModelItem : public VpzModelItem
{

public:
    enum SEL_TYPE {TOP, RIGHT, BOTTOM, LEFT, MIDDLE};

    VpzMainModelItem(QDomNode node, vleVpm* vpz, QGraphicsItem* parent,
            QGraphicsScene* scene);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
            QWidget *widget = 0);
    void initializeFromDom();
    void clearLines();
    void clearSubModels();
    void addConnLines();
    void update(const QRectF & rect = QRectF());

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event);

    QRectF subModelsBoundingRect(bool onlySelected);
    VpzSubModelItem* getSubModel(const QString& subMod);
    QList<VpzSubModelItem*> getSubModels();
    QList<VpzSubModelItem*> getSelectedSubModels();
    QList<VpzConnectionLineItem *> getConnLines();
    void removeNameEditionMode();
    bool isAtomic();
    int type() const;
};


class VpzDiagScene : public QGraphicsScene
{
    Q_OBJECT
public:
    VpzDiagScene();
    /**
     * @brief initialize the scene with a vpm file
     * @param vpm, the vpm file
     * @param className, if empty then the main model is shown
     * @param emitter, the object that send the signal:
     *   undoRedo(QDomNode, QDomNode, QDomNode, QDomNode)
     */
    void init(vleVpm* vpm, const QString& className, QObject* emitter);
    void setFocus(QDomNode selModelNode);
    void clear();
    void update(const QRectF & rect = QRectF());
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void dragEnterEvent(QGraphicsSceneDragDropEvent * event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent * event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event);
    void dropEvent(QGraphicsSceneDragDropEvent * event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent * contextMenuEvent);



    QBrush getBrushAtomicModel();
    QBrush getBrushCoupledModel();
    QPen getPenNotSelectedModel();
    QPen getPenSelectedModel();
    QPen getPenConnection();
    QBrush getBrushBackground();
    QFont getFontPort();
    QFont getFontModel();
    QColor getColorModelNameSelected();
    QColor getColorModelNameNotSelected();

    QString getXQueryOfTheSelectedModel();
    VpzPortItem* getPort(const QString& portName, const QString& subMod);
    QStringList getSelectedModels();
    void unselectAllSubModels();
    void selectSubModel(const QString& nameSub);


    static bool isVpzPort(QGraphicsItem* item);
    static bool isVpzInputPort(QGraphicsItem* item);
    static bool isVpzOutputPort(QGraphicsItem* item);
    static bool isVpzSubModel(QGraphicsItem* item);
    static bool isVpzMainModel(QGraphicsItem* item);
    static bool isVpzConnectionLine(QGraphicsItem* item);
signals:
    void enterCoupledModel(QDomNode node);
    void initializationDone(VpzDiagScene* scene);

public slots:
    void onDragDestroyed(QObject *obj =0);
    void onUndoRedoVpm(QDomNode oldValVpz, QDomNode newValVpz,
            QDomNode oldValVpm, QDomNode newValVpm);

public:
    vleVpm*                mVpm;
    VpzMainModelItem*      mCoupled;
    VpzPortItem*           mPortSel1;
    VpzPortItem*           mPortSel2;
    QGraphicsLineItem*     mConnection;
    QPointF                mDragStartPoint;
    QPointF                mDragCurrPoint;
    SEL_TYPE               mModelSelType;
    bool                   mIsEnteringCoupled;
    QList<QDomNode>        mModelsCopies;
};

}}//namespaces


#endif
