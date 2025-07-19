#ifndef DEBUGDOTITEM_H
#define DEBUGDOTITEM_H

#include <QGraphicsItem>
#include <QPainter>

class DebugDotItem : public QGraphicsItem
{
public:
    explicit DebugDotItem(QGraphicsItem *parent = nullptr);

    // QGraphicsItem 必须实现的纯虚函数
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    // 设置圆点的位置和颜色
    void setDot(const QPointF& pos, qreal radius, const QColor& color);

private:
    QPointF m_dotPos;
    qreal m_radius;
    QColor m_color;
};

#endif // DEBUGDOTITEM_H

