#pragma once

#include <QGraphicsLineItem>


class CuteConnection : public QGraphicsLineItem
{
public:
    explicit CuteConnection(const QLineF& line, QGraphicsItem* startItem);
    ~CuteConnection() override;

    enum { Type = UserType + 3 };
    int type() const override { return Type; }

    QGraphicsItem* getStartItem() const;
    void setEndItem(QGraphicsItem* item);

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool connectionIsValid() const { return _connectedItems.first && _connectedItems.second; }

private:
    std::pair<QGraphicsItem*, QGraphicsItem*> _connectedItems;
};
