#pragma once

#include <QGraphicsPathItem>


class CuteDock;


class CuteConnection : public QGraphicsPathItem
{
public:
    explicit CuteConnection(CuteDock* source);
    ~CuteConnection() override;

    enum { Type = UserType + 5 };
    int type() const override { return Type; }

    CuteDock* getDestinationItem() const;
    void setDestinationItem(CuteDock* item);
    CuteDock* getSourceItem() const;

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
    std::pair<CuteDock*, CuteDock*> _connectedDocks;

    // temps
    QPointF _destItemPos;
    QPointF _sourceItemPos;
};
