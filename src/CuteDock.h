#pragma once

#include <QBrush>
#include <QGraphicsItem>
#include <QRectF>


class CuteDock : public QGraphicsItem
{
public:
    explicit CuteDock(QGraphicsItem* parent, const QPointF& pos);
    ~CuteDock() override;

    enum { Type = UserType + 2 };

    QRectF boundingRect() const override;
    int type() const override { return Type; }

    QPointF getConnectionMagnet() const;

    void hideConnectionPreview();
    void showConnectionPreview();

private:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QRectF _paintRect{0.0, 0.0, 20.0, 10.0};
    QBrush _brush;
};
