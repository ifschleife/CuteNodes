#pragma once

#include <QGraphicsLineItem>


class CuteConnection : public QGraphicsLineItem
{
public:
    explicit CuteConnection(const QLineF& line, QGraphicsItem* startItem);
    ~CuteConnection() override;

    enum { Type = UserType + 5 };
    int type() const override { return Type; }

    QGraphicsItem* getStartItem() const;
    QGraphicsItem* getEndItem() const;
    void setEndItem(QGraphicsItem* item);

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void snapLineToItems();

private:
    std::pair<QGraphicsItem*, QGraphicsItem*> _connectedItems;
};
