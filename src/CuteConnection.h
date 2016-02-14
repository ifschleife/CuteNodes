#pragma once

#include <QGraphicsPathItem>


class CuteConnection : public QGraphicsPathItem
{
public:
    explicit CuteConnection(QGraphicsItem* startItem);
    ~CuteConnection() override;

    enum { Type = UserType + 5 };
    int type() const override { return Type; }

    QGraphicsItem* getStartItem() const;
    QGraphicsItem* getEndItem() const;
    void setEndItem(QGraphicsItem* item);
    void updateEndPoint(const QPointF& endPoint);

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void snapEndPointsToItems();
    void calculateSpline();

private:
    std::pair<QGraphicsItem*, QGraphicsItem*> _connectedItems;
    QPointF _endPoint;
    QPointF _startPoint;
};
