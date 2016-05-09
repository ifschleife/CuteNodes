#pragma once

#include <QGraphicsPathItem>


class CuteConnection : public QGraphicsPathItem
{
public:
    explicit CuteConnection(QGraphicsItem* sourceItem);
    ~CuteConnection() override;

    enum { Type = UserType + 5 };
    int type() const override { return Type; }

    QGraphicsItem* getDestinationItem() const;
    void setDestinationItem(QGraphicsItem* item);
    QGraphicsItem* getSourceItem() const;

    void hideDeletionPreview();
    void showDeletionPreview();

    void setAsValid();
    void updateDestinationItemPosition(const QPointF& destItemPos);

private:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void snapEndPointsToItems();
    void calculateSpline();
    void addArrowToSpline(QPainterPath& spline) const;

private:
    std::pair<QGraphicsItem*, QGraphicsItem*> _connectedItems;

    // temps
    QPointF _destItemPos;
    QPointF _sourceItemPos;
};
