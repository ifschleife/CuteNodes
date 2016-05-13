#include "NodeScene.h"

#include "CuteConnection.h"
#include "CuteDock.h"
#include "CuteNode.h"

#include <math.h>
#include <QAction>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QMenu>
#include <QPainter>


namespace
{
    constexpr qreal ZValueDefault   = -1.0;
    constexpr qreal ZValueShowOnTop = 1.0;
}


NodeScene::NodeScene()
    : QGraphicsScene()
{

}

NodeScene::~NodeScene()
{

}


void NodeScene::read(const QJsonObject& json)
{
    const auto sceneCfg = json["scene"].toObject();
    const auto width = sceneCfg["width"].toDouble();
    const auto height = sceneCfg["height"].toDouble();
    setSceneRect(0.0, 0.0, width, height);

    const auto nodes = json["nodes"].toArray();
    for (const auto& nodeCfg: nodes)
    {
        auto node = new CuteNode;
        node->read(nodeCfg.toObject());
        addItem(node);
    }

    readConnections(json["connections"].toArray());
}

void NodeScene::readConnections(const QJsonArray& connections)
{
    auto allDocks = getItemsOfType<CuteDock>();
    const auto barrier = std::partition(begin(allDocks), end(allDocks), [](const auto& dock)
    {
        return dock->isInputDock();
    });
    const auto sourceDocks = std::vector<CuteDock*>({begin(allDocks), barrier});
    const auto destDocks   = std::vector<CuteDock*>({barrier, end(allDocks)});

    for (const auto& cfg: connections)
    {
        const auto sourceID = QUuid(cfg.toObject()["source"].toString());
        const auto sourceDock = std::find_if(begin(destDocks), end(destDocks), [&sourceID](const auto& dock)
        {
            return dock->getUuid() == sourceID;
        });

        const auto destID = QUuid(cfg.toObject()["dest"].toString());
        const auto destDock = std::find_if(begin(sourceDocks), end(sourceDocks), [&destID](const auto& dock)
        {
            return dock->getUuid() == destID;
        });
        if (end(destDocks) == sourceDock || end(sourceDocks) == destDock)
            continue;

        // If the scene file is faulty and has more than one connection starting from the same dock
        // two connections would be created which will lead to errors later on. This is why we need to
        // make sure that the source dock does not have a connection yet.
        if (nullptr == (*sourceDock)->getConnection())
        {
            auto connection = new CuteConnection{*sourceDock};
            if (!connection)
                return;

            connection->setDestinationItem(*destDock);
            connection->setAsValid();
            addItem(connection);
        }
    }
}

void NodeScene::write(QJsonObject& json) const
{
    auto sceneObject = json["scene"].toObject();
    sceneObject["width"] = width();
    sceneObject["height"] = height();

    json["scene"] = sceneObject;

    QJsonArray nodeArray;

    const auto& nodes = getItemsOfType<CuteNode>();
    for (const auto& node: nodes)
    {
        QJsonObject nodeJson;
        node->write(nodeJson);
        nodeArray.append(nodeJson);
    }
    json["nodes"] = nodeArray;

    writeConnections(json);
}

void NodeScene::writeConnections(QJsonObject& json) const
{
    QJsonArray connectionArray;

    const auto& connections = getItemsOfType<CuteConnection>();
    for (const auto& connection: connections)
    {
        QJsonObject connectionJson;
        const auto source = connection->getSourceItem();
        const auto dest   = connection->getDestinationItem();
        if (!source || !dest)
            continue;

        connectionJson["source"] = source->getUuid().toString();
        connectionJson["dest"]   = dest->getUuid().toString();
        connectionArray.append(connectionJson);
    }

    json["connections"] = connectionArray;
}

void NodeScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    const QRectF backgroundRect = sceneRect() & rect;
    const qreal left = int(backgroundRect.left()) - (int(backgroundRect.left()) % _gridSize.width());
    const qreal top = int(backgroundRect.top()) - (int(backgroundRect.top()) % _gridSize.height());

    QVarLengthArray<QLineF, 100> lines;

    for (qreal x = left; x < backgroundRect.right(); x += _gridSize.width())
    {
        lines.append(QLineF(x, backgroundRect.top(), x, backgroundRect.bottom()));
    }

    for (qreal y = top; y < backgroundRect.bottom(); y += _gridSize.height())
    {
        lines.append(QLineF(backgroundRect.left(), y, backgroundRect.right(), y));
    }

    painter->setPen(Qt::lightGray);
    painter->drawLines(lines.data(), lines.size());

    painter->setPen({Qt::darkGray, 3});
    painter->drawRect(sceneRect());
}

void NodeScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    // only show menu when clicking inside the scene
    if (!sceneRect().contains(event->scenePos()))
        return;

    QMenu menu;

    const auto clickedItem = itemAt(event->scenePos(), QTransform());
    const int clickedItemType = clickedItem ? clickedItem->type() : 0;

    const auto nofSelectedItems = selectedItems().size();
    if (nofSelectedItems > 1)
    {
        addMenuEntriesForMultiSelection(menu);
    }
    else if (clickedItemType == CuteNode::Type || clickedItemType == CuteConnection::Type)
    {
        addMenuEntriesForSingleSelection(event->scenePos(), menu);
    }
    else
    {
        addMenuEntriesForEmptySelection(event->scenePos(), menu);
    }

    menu.exec(event->screenPos());
}

void NodeScene::addMenuEntriesForSingleSelection(const QPointF& scenePos, QMenu& menu)
{
    const auto deleteItemAction{new QAction{tr("Delete"), &menu}};
    connect(deleteItemAction, &QAction::triggered, [this, &scenePos]()
    {
        auto item = itemAt(scenePos, QTransform());
        if (item)
            delete item;
    });

    menu.addAction(deleteItemAction);
}

void NodeScene::addMenuEntriesForEmptySelection(const QPointF& scenePos, QMenu& menu)
{
    const auto newNodeAction{new QAction{tr("Add Node"), &menu}};
    connect(newNodeAction, &QAction::triggered, [&]()
    {
        auto node = new CuteNode;
        node->setPos(scenePos);
        addItem(node);
    });

    menu.addAction(newNodeAction);
}

void NodeScene::addMenuEntriesForMultiSelection(QMenu& menu)
{
    const auto deleteSelectedItemsAction{new QAction{tr("Delete Selected Items"), &menu}};
    connect(deleteSelectedItemsAction, &QAction::triggered, [this]()
    {
        // first we have to delete all nodes
        auto nodes = getSelectedItems<CuteNode>();
        qDeleteAll(nodes);
        // and then all connections, mixing the order could cause double deletes
        auto connections = getSelectedItems<CuteConnection>();
        qDeleteAll(connections);
    });

    menu.addAction(deleteSelectedItemsAction);
}

void NodeScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        if (_draggedNodes.size() > 0)
        {
            handleNodeDragging(event->scenePos());
        }
        else if (_drawnConnection)
        {
            handleConnectionDrawing(event->scenePos());
        }

        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void NodeScene::handleNodeDragging(const QPointF& mousePos)
{
    for (const auto& draggedNode: _draggedNodes)
    {
        QPointF newNodePos = mousePos - draggedNode.second;
        if (_gridSnapping)
        {
            newNodePos.setX((round(newNodePos.x() / _gridSize.width())) * _gridSize.width());
            newNodePos.setY((round(newNodePos.y() / _gridSize.height())) * _gridSize.height());
        }

        if (draggedNodePositionInSceneRect(draggedNode.first, newNodePos))
        {
            if (_nodeOverlap || draggedNodePositionIsValid(draggedNode.first, newNodePos))
            {
                draggedNode.first->setPos(newNodePos);
            }
        }
    }
}

bool NodeScene::draggedNodePositionInSceneRect(const QGraphicsItem* node, const QPointF& nodePos) const
{
    // this is the bounding rect the item will have when it has been moved
    QRectF newBoundingRect = node->sceneBoundingRect();
    newBoundingRect.moveTo(nodePos);

    return sceneRect().contains(newBoundingRect);
}

bool NodeScene::draggedNodePositionIsValid(const QGraphicsItem* node, const QPointF& nodePos) const
{
    // this is the bounding rect the item will have when it has been moved
    QRectF newBoundingRect = node->sceneBoundingRect();
    newBoundingRect.moveTo(nodePos);

    // check if there are other items in this new area
    const QList<QGraphicsItem*> potentialColliders = items(newBoundingRect, Qt::IntersectsItemBoundingRect);

    bool collision = std::any_of(potentialColliders.begin(), potentialColliders.end(), [&node](const auto& item)
    {
        return item->type() == CuteNode::Type && item != node;
    });

    return !collision;
}

void NodeScene::handleConnectionDrawing(const QPointF& mousePos) const
{
    _drawnConnection->updateDestinationItemPosition(mousePos);

    const auto prevDestItem = _drawnConnection->getDestinationItem();
    bool showingPreview = false;

    const auto item = getTopLevelItemAtPos(mousePos, CuteDock::Type);
    const auto dock = qgraphicsitem_cast<CuteDock*>(item);

    if (dock && dock->canAcceptConnectionAtPos(mousePos))
    {
        // if dock already has a connection, that connection will be removed upon mouse release
        auto existingConnection = dock->getConnection();
        if (existingConnection)
        {
            existingConnection->showDeletionPreview();
        }

        // only show preview once
        if (prevDestItem != dock)
        {
            dock->showConnectionPreview();
            _drawnConnection->setDestinationItem(dock);
        }
        showingPreview = true;
    }

    // only hide previously shown preview when there was one
    if (prevDestItem && !showingPreview)
    {
        const auto prevDock = qgraphicsitem_cast<CuteDock*>(prevDestItem);
        prevDock->hideConnectionPreview();

        // also hide the preview for a connection deletion, if previous dock had a connection
        auto existingConnection = prevDock->getConnection();
        if (existingConnection)
            existingConnection->hideDeletionPreview();

        _drawnConnection->setDestinationItem(nullptr);
    }
}

void NodeScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        // needs to be called first so selection is up to date
        QGraphicsScene::mousePressEvent(event);

        QGraphicsItem* clickedItem = itemAt(event->scenePos(), QTransform());
        const int itemType = clickedItem ? clickedItem->type() : 0;

        if (itemType == CuteNode::Type)
        {
            startDraggingSelectedNodes(event->scenePos());
        }
        else if (itemType == CuteDock::Type)
        {
            auto dock = qgraphicsitem_cast<CuteDock*>(clickedItem);
            if (dock->isOutputDock())
            {
                _drawnConnection = new CuteConnection{dock};
                // this ensures that the connection's spline is valid
                _drawnConnection->updateDestinationItemPosition(event->scenePos());
                addItem(_drawnConnection);
            }
            else if (dock->isInputDock())
            {
                auto connection = dock->getConnection();
                if (connection)
                {
                    dock->setConnection(nullptr); // remove connection from dock

                    // set connection to invalid (drawing) state
                    _drawnConnection = connection;
                    _drawnConnection->setDestinationItem(nullptr);
                }
            }
        }
    }
    else if (event->buttons() == Qt::RightButton)
    {
        // this is necessary so the selection is not cleared before the context menu can be shown
        event->accept();
    }
}

void NodeScene::startDraggingSelectedNodes(const QPointF& dragStartPos)
{
    const auto selectedNodes = getSelectedItems<CuteNode>();
    _draggedNodes.reserve(selectedNodes.size());
    for (const auto& node: selectedNodes)
    {
        // make sure dragged node is drawn on top of other nodes
        node->setZValue(ZValueShowOnTop);
        // store mouse pointer offset next to dragged node
        _draggedNodes.emplace_back(std::make_pair(node, dragStartPos - node->pos()));
        // this removes glitches when moving an item after panning/scrolling
        invalidate(node->boundingRect());
    }
}

void NodeScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // for some reason buttons() will show Qt::NoButton, so we call button() instead
    if (event->button() == Qt::LeftButton)
    {
        if (_drawnConnection)
        {
            auto item = getTopLevelItemAtPos(event->scenePos(), CuteDock::Type);
            auto dock = qgraphicsitem_cast<CuteDock*>(item);

            if (dock && dock->isInputDock())
            {
                auto existingConnection = dock->getConnection();
                if (existingConnection)
                {
                    delete existingConnection;
                }

                _drawnConnection->setDestinationItem(dock);
                _drawnConnection->setZValue(-2.0); // hide behind nodes
                _drawnConnection->setAsValid();
            }
            else
            {
                // remove connection when it does not end in another dock
                delete _drawnConnection;
            }

            _drawnConnection = nullptr;
        }

        // reset z value to default
        for (const auto& node: _draggedNodes)
        {
            node.first->setZValue(ZValueDefault);
        }
        _draggedNodes.clear();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

QGraphicsItem* NodeScene::getTopLevelItemAtPos(const QPointF& scenePos, int itemType) const
{
    QList<QGraphicsItem*> itemsAtPos = items(scenePos, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder);
    const auto iter = std::find_if(itemsAtPos.begin(), itemsAtPos.end(), [&itemType](const auto& item)
    {
        return item->type() == itemType;
    });

    return iter != itemsAtPos.end() ? *iter : nullptr;
}

template<typename ItemType>
std::vector<ItemType*> NodeScene::getItemsOfType() const
{
    std::vector<ItemType*> foundItems;
    for (auto& item: items())
    {
        const auto itemOfDesiredType = qgraphicsitem_cast<ItemType*>(item);
        if (itemOfDesiredType)
            foundItems.push_back(itemOfDesiredType);
    }
    return foundItems;
}

template<typename ItemType>
std::vector<QGraphicsItem*> NodeScene::getSelectedItems() const
{
    QList<QGraphicsItem*> items = selectedItems();

    auto lastItem = std::partition(items.begin(), items.end(), [](const auto& item)
    {
        return qgraphicsitem_cast<ItemType*>(item) != nullptr;
    });
    return {items.begin(), lastItem};
}
