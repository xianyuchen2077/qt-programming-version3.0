#include "DebugDotItem.h"

DebugDotItem::DebugDotItem(QGraphicsItem *parent)
    : QGraphicsItem(parent), m_radius(5.0), m_color(Qt::red) // 默认值
{
    // 设置一个非常高的Z值，确保它在最上层
    setZValue(999); // 通常比游戏元素的Z值高得多
}

QRectF DebugDotItem::boundingRect() const
{
    // 返回包含小圆点的边界矩形
    // 需要考虑圆点的半径，并稍微扩大一点以避免裁剪问题
    return QRectF(m_dotPos.x() - m_radius, m_dotPos.y() - m_radius,
                  m_radius * 2, m_radius * 2);
}

void DebugDotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(m_color);
    painter->setPen(Qt::NoPen); // 不绘制边框
    painter->drawEllipse(m_dotPos, m_radius, m_radius); // 绘制圆点
}

void DebugDotItem::setDot(const QPointF& pos, qreal radius, const QColor& color)
{
    prepareGeometryChange(); // 通知视图项的几何形状即将改变，以便重新计算边界矩形
    m_dotPos = pos;
    m_radius = radius;
    m_color = color;
    update(); // 触发重新绘制
}
