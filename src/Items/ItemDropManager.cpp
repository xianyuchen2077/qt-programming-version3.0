#include "ItemDropManager.h"
#include "../Scenes/IceScene.h"
#include "Maps/Icefield.h"
#include "Weapons/Shabby_Pistol.h"
#include "Weapons/SolidBall.h"
#include "Weapons/Sniper_Rifle.h"
#include "Weapons/Knife.h"
#include "Armors/MentalArmor.h"
#include "Armors/Acient_robe.h"
#include "Armors/Qin_ceremonial_robe.h"
#include "Supplies/Bandage.h"
#include "Supplies/MedicalKit.h"
#include "Supplies/Adrenaline.h"
#include <QDebug>

ItemDropManager::ItemDropManager(IceScene* scene, QObject* parent)
    : QObject(parent), scene(scene)
{
    // 初始化掉落更新定时器
    dropTimer = new QTimer(this);
    connect(dropTimer, &QTimer::timeout, this, &ItemDropManager::onDropTimer);
    dropTimer->start(UPDATE_INTERVAL);

    // 自动掉落物品定时器（每30秒掉一个）
    autoDropTimer = new QTimer(this);
    connect(autoDropTimer, &QTimer::timeout, this, &ItemDropManager::onAutoDropTimer);
    autoDropTimer->start(15000); // 30秒

}

void ItemDropManager::dropItem(Item* item)
{
    if (!item || !scene) return;

    // 如果物品同时继承了Mountable，确保其未装备状态
    if (auto mountable = dynamic_cast<Mountable*>(item))
    {
        mountable->unmount();
    }

    // 确保物品在场景中
    if (!scene->items().contains(item))
    {
        scene->addItem(item);
    }

    // 设置随机掉落起始位置
    QPointF startPos = getRandomDropStartPos();
    item->setPos(startPos);
    item->setZValue(5);

    // 添加到掉落列表
    DroppingItem drop(item);
    drop.lastY = startPos.y();
    droppingItems.append(drop);

    qDebug() << "Item started dropping from:" << startPos;
}

void ItemDropManager::dropRandomItem()
{
    Item* item = createRandomItem();
    if (item)
    {
        dropItem(item);
    }
}

void ItemDropManager::updateDrops(qreal deltaTime)
{
    auto it = droppingItems.begin();
    while (it != droppingItems.end())
    {
        DroppingItem& drop = *it;

        if (!drop.isDropping || !drop.item) {
            it = droppingItems.erase(it);
            continue;
        }

        // 保存当前位置
        QPointF currentPos = drop.item->pos();
        drop.lastY = currentPos.y();

        // 应用重力
        drop.velocity.setY(drop.velocity.ry() + GRAVITY * deltaTime);
        if (drop.velocity.y() > MAX_FALL_SPEED)
        {
            drop.velocity.setY(MAX_FALL_SPEED);
        }

        // 计算新位置
        QPointF newPos = currentPos + drop.velocity * deltaTime;

        // 边界检查
        if (newPos.x() < 0)
            newPos.setX(0);
        if (newPos.x() > scene->sceneRect().width())
            newPos.setX(scene->sceneRect().width());

        // 碰撞检测
        bool landed = false;

        // 1. 检查平台碰撞
        if (checkPlatformCollision(drop))
        {
            // 在checkPlatformCollision中已经处理了着陆或继续掉落的逻辑
            // 如果物品仍在掉落，landed保持false
            landed = !drop.isDropping;
        }
        // 2. 检查障碍物碰撞
        else if (checkObstacleCollision(drop))
        {
            // 在checkObstacleCollision中已经处理了着陆逻辑
            landed = true;
        }

        // 3. 检查地面碰撞
        if (checkGroundCollision(drop) && !landed)
        {
            auto icefield = dynamic_cast<Icefield*>(scene->getMap());
            if (icefield)
            {
                qreal floorHeight = icefield->getFloorHeight();
                landItem(drop, floorHeight);
                landed = true;
            }
        }

        if (!landed)
        {
            // 继续掉落
            drop.item->setPos(newPos);
        }

        // 如果物品已着陆，从列表中移除
        if (landed || !drop.isDropping)
        {
            it = droppingItems.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ItemDropManager::stopDrop(Item* item)
{
    removeFromDropList(item);
}

void ItemDropManager::clearAllDrops()
{
    droppingItems.clear();
}

void ItemDropManager::onDropTimer()
{
    // 使用固定的deltaTime来确保稳定的物理模拟
    updateDrops(UPDATE_INTERVAL);
}

QPointF ItemDropManager::getRandomDropStartPos() const
{
    if (!scene)
    {
        return QPointF(0, 0);
    }

    QRectF sceneRect = scene->sceneRect();
    qreal randomX = QRandomGenerator::global()->bounded(
        static_cast<int>(sceneRect.width() * 0.05),
        static_cast<int>(sceneRect.width() * 0.95)
        );

    while ((randomX > 100 && randomX <200)|| (randomX > 1000 && randomX < 1100))
    {
        randomX = QRandomGenerator::global()->bounded(
            static_cast<int>(sceneRect.width() * 0.05),
            static_cast<int>(sceneRect.width() * 0.95)
            );
    }

    return QPointF(randomX, sceneRect.top()); // 从屏幕顶部上方开始
}

bool ItemDropManager::checkGroundCollision(DroppingItem& drop)
{
    if (!drop.item || !scene)
        return false;

    // 获取地图并转换为Icefield类型
    auto icefield = dynamic_cast<Icefield*>(scene->getMap());
    if (!icefield)
        return false;

    qreal floorHeight = icefield->getFloorHeight();
    QPointF itemPos = drop.item->pos();
    qreal itemBottom = itemPos.y() + drop.item->boundingRect().height()*drop.item->scale();

    return (itemBottom >= floorHeight && drop.velocity.y() > 0 && itemPos.x() > 160 && itemPos.x() < 1060);
}

bool ItemDropManager::checkPlatformCollision(DroppingItem& drop)
{
    if (!drop.item || !scene)
        return false;

    // 获取地图并转换为Icefield类型
    auto icefield = dynamic_cast<Icefield*>(scene->getMap());
    if (!icefield) return false;

    // 获取障碍物列表（你的代码中平台也是作为障碍物处理的）
    const QList<Obstacle>& obstacles = icefield->getObstacles();
    if (obstacles.size() <= 1)
    {
        qDebug() << "No platform found in obstacles";
        return false;
    }
    const Obstacle& platform = obstacles[2];  // 平台的代号

    QPointF itemPos = drop.item->pos();
    QRectF itemRect = drop.item->boundingRect();
    itemRect.moveTopLeft(itemPos);

    // 检查物品是否落在平台上（假设某些障碍物是平台）
    if (itemRect.intersects(platform.bounds))
    {
        // 确保是从上方掉落到平台顶部
        if (drop.lastY + itemRect.height() <= platform.bounds.top() + 5 && drop.velocity.y() > 0 && drop.item->x() > platform.bounds.left() + 10 && drop.item->x() < platform.bounds.right() - 10)
        {

            // 检查是否在地面正上方的平台（根据你的描述，这种平台有50%概率继续掉落）
            qreal floorHeight = icefield->getFloorHeight();
            bool isAboveGround = (platform.bounds.bottom() > floorHeight - 100); // 判断是否接近地面

            if (isAboveGround)
            {
                // 50% 概率继续掉落到地面
                if (QRandomGenerator::global()->bounded(100) < 50)
                {
                    qDebug() << "Item passed through platform, continuing to fall";
                    return false; // 继续掉落
                }
            }

            // 着陆在平台上
            landItem(drop, platform.bounds.top());
            qDebug() << "Item landed on platform at Y:" << platform.bounds.top();
            return true;
        }
    }
    return false;
}

bool ItemDropManager::checkObstacleCollision(DroppingItem& drop)
{
    if (!drop.item || !scene)
        return false;

    // 获取地图并转换为Icefield类型
    auto icefield = dynamic_cast<Icefield*>(scene->getMap());
    if (!icefield)
        return false;

    // 获取障碍物列表
    QList<Obstacle> obstacles = icefield->getObstacles();
    obstacles.removeLast(); // 移除平台障碍物

    QPointF itemPos = drop.item->pos();
    QRectF itemRect = drop.item->boundingRect();
    itemRect.moveTopLeft(itemPos);

    for (const Obstacle& obstacle : obstacles)
    {
        // 检查物品是否与障碍物顶部碰撞
        if (itemRect.intersects(obstacle.bounds))
        {
            // 确保是从上方掉落到障碍物顶部
            if (drop.lastY + itemRect.height() <= obstacle.bounds.top() + 5 && drop.velocity.y() > 0)
            {
                // 着陆在障碍物顶部
                landItem(drop, obstacle.bounds.top());
                return true;
            }
        }
    }
    return false;
}

void ItemDropManager::landItem(DroppingItem& drop, qreal landY)
{
    if (!drop.item) return;

    // 设置最终位置
    QPointF finalPos = drop.item->pos();
    finalPos.setY(landY);
    drop.item->setPos(finalPos);

    // 停止掉落
    drop.isDropping = false;
    drop.velocity = QPointF(0, 0);

    qDebug() << "Item landed at:" << finalPos;
}

Item* ItemDropManager::createRandomItem() const
{
    // 创建随机物品用于演示
    QStringList itemTypes = {"weapon", "armor", "medical"};
    QString itemType = itemTypes[QRandomGenerator::global()->bounded(itemTypes.size())];

    Item* item = nullptr;

    if (itemType == "weapon")
    {
        int weaponType = QRandomGenerator::global()->bounded(4);
        switch (weaponType)
        {
        case 0: item = new Shabby_Pistol(); break;
        case 1: item = new SolidBall(); break;
        case 2: item = new Sniper_Rifle(); break;
        case 3: item = new Knife(); break;
        }
    }
    else if (itemType == "armor")
    {
        int armorType = QRandomGenerator::global()->bounded(3);
        switch (armorType)
        {
        case 0: item = new MentalArmor(); break;
        case 1: item = new AcientRobe(); break;
        case 2: item = new Robe_of_Qin(); break;
        }
    }
    else if (itemType == "medical")
    {
        int medicalItemType = QRandomGenerator::global()->bounded(3);
        switch (medicalItemType)
        {
        case 0: item = new Bandage(); break;
        case 1: item = new MedicalKit(); break;
        case 2: item = new Adrenaline(); break;
        }
    }

    return item;
}

void ItemDropManager::removeFromDropList(Item* item)
{
    auto it = droppingItems.begin();
    while (it != droppingItems.end())
    {
        if (it->item == item)
        {
            it = droppingItems.erase(it);
            break;
        } else {
            ++it;
        }
    }
}

void ItemDropManager::onAutoDropTimer()
{
    dropRandomItem();  // 自动掉落一个随机物品
    qDebug() << "[AutoDrop] Dropped a random item.";
}
