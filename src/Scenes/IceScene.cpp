#include <QDebug>
#include "IceScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Icefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"

IceScene::IceScene(QObject *parent) : Scene(parent)
{
    // This is useful if you want the scene to have the exact same dimensions as the view
    // 地图的大小1280*720
    setSceneRect(0, 0, 1280, 720);
    map = new Icefield(); // 创建地图
    character = new Link(); // 创建角色实例
    spareArmor = new FlamebreakerArmor(); // 创建备用护甲实例

    addItem(map);
    addItem(character);
    addItem(spareArmor);

    map->scaleToFitScene(this);
    character->setPos(100,630); // 设置角色出生点
    character->setMoveSpeed(character->getMoveSpeed()*1.5); // 设置角色移动速度(速度*1.5）
    spareArmor->unmount();
    spareArmor->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.75, map->getFloorHeight());
}

void IceScene::processInput()
{
    Scene::processInput();
    if (character != nullptr)
    {
        character->processInput();
    }
}

void IceScene::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_A:
        if (character != nullptr)
        {
            character->setLeftDown(true);
        }
        break;
    case Qt::Key_D:
        if (character != nullptr)
        {
            character->setRightDown(true);
        }
        break;
    case Qt::Key_S:
        if (character != nullptr)
        {
            character->setDownDown(true);
            character->setPickDown(true);
        }
        break;
    default:
        Scene::keyPressEvent(event);
    }
}

void IceScene::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_A:
        if (character != nullptr)
        {
            character->setLeftDown(false);
        }
        break;
    case Qt::Key_D:
        if (character != nullptr)
        {
            character->setRightDown(false);
        }
        break;
    case Qt::Key_S:
        if (character != nullptr)
        {
            character->setDownDown(false);
            character->setPickDown(false);
        }
        break;
    default:
        Scene::keyReleaseEvent(event);
    }
}

void IceScene::update()
{
    Scene::update();
}

void IceScene::processMovement()
{
    Scene::processMovement();
    if (character != nullptr)
    {
        character->setPos(character->pos() + character->getVelocity() * (double) deltaTime);
    }
}

void IceScene::processPicking()
{
    Scene::processPicking();
    if (character->isPicking())
    {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100.);
        if (mountable != nullptr)
        {
            spareArmor = dynamic_cast<Armor *>(pickupMountable(character, mountable));
        }
    }
}

Mountable *IceScene::findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold)
{
    Mountable *nearest = nullptr;
    qreal minDistance = distance_threshold;

    for (QGraphicsItem *item: items())
    {
        if (auto mountable = dynamic_cast<Mountable *>(item))
        {
            if (!mountable->isMounted())
            {
                qreal distance = QLineF(pos, item->pos()).length();
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearest = mountable;
                }
            }
        }
    }

    return nearest;
}

Mountable *IceScene::pickupMountable(Character *character, Mountable *mountable)
{
    // Limitation: currently only supports armor
    if (auto armor = dynamic_cast<Armor *>(mountable))
    {
        return character->pickupArmor(armor);
    }
    return nullptr;
}
