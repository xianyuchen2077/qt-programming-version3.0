#include "Map.h"

Map::Map(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath){}

void Map::scaleToFitScene(QGraphicsScene *scene)
{
    // Calculate scale factors to scale the item to fit the entire scene
    QRectF sceneRect = scene->sceneRect(); // 场地尺寸
    QRectF itemRect = boundingRect(); // 原始地图边界

    qreal scaleX = sceneRect.width() / itemRect.width(); // 计算X方向的缩放比例
    qreal scaleY = sceneRect.height() / itemRect.height(); // 计算Y方向的缩放比例

    // Choose the smaller scale factor to maintain aspect ratio
    qreal scaleFactor = qMin(scaleX, scaleY); // 取较小比例（保持宽高比）

    // Apply the scale to the item
    setTransform(QTransform::fromScale(scaleFactor, scaleFactor), true);

    // Center the item in the scene (optional)
    // 计算缩放后的地图尺寸与场景尺寸的差值，取半值作为偏移量实现居中
    setPos((sceneRect.width() - boundingRect().width() * scaleFactor) / 2,
           (sceneRect.height() - boundingRect().height() * scaleFactor) / 2);
}

// 获取地图的出生位置
QPointF Map::getSpawnPos()
{
    auto boundingRect = sceneBoundingRect();
    auto midX = (boundingRect.left() + boundingRect.right()) * 0.5;
    return {midX, getFloorHeight()}; // 依赖getFloorHeight()获取地面高度
}

// 计算地面高度的函数，基类成员还要重写
qreal Map::getFloorHeight()
{
    auto sceneRect = sceneBoundingRect();
    return sceneRect.top() + (sceneRect.top() - sceneRect.bottom()) * 0.5;
}

void Map::applyEffectToCharacter(Character *character, qint64 deltaTime)
{
    // Default implementation does nothing, can be overridden in derived classes
    Q_UNUSED(character);
    Q_UNUSED(deltaTime);
}

void Map::setPixmap(const QString &pixmapPath)
{
    if (pixmapItem != nullptr)
    {
        pixmapItem->setPixmap(QPixmap(pixmapPath));
    } else {
        // 如果 pixmapItem 为空，则创建一个新的 QGraphicsPixmapItem
        pixmapItem = new QGraphicsPixmapItem(QPixmap(pixmapPath), this);
    }
}
