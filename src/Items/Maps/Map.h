#ifndef QT_PROGRAMMING_2024_MAP_H
#define QT_PROGRAMMING_2024_MAP_H

#include <QGraphicsScene>
#include <QPointF>
#include "../Item.h"
#include "../Characters/Character.h"

class Map : public Item {
public:
    explicit Map(QGraphicsItem *parent = nullptr, const QString &pixmapPath = "");

    void scaleToFitScene(QGraphicsScene *scene);

    virtual qreal getFloorHeight(); // Need to rewrite to support multiple platforms

    virtual QPointF getSpawnPos();

    virtual void setPixmap(const QString &pixmapPath); // 设置图片路径

    virtual void applyEffectToCharacter(Character* character, qint64 deltaTime);
};


#endif //QT_PROGRAMMING_2024_MAP_H
