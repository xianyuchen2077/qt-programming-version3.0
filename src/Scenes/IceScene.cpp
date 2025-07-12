#include <QDebug>
#include "IceScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Icefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/HeadEquipments/Helmet_of_the_Paladin.h"

IceScene::IceScene(QObject *parent) : Scene(parent)
{
    // This is useful if you want the scene to have the exact same dimensions as the view
    // 地图的大小1280*720
    setSceneRect(0, 0, 1280, 720);

    map = new Icefield(); // 创建地图
    player1 = new Link(); // 创建角色实例
    player2 = new Link();
    spareArmor = new FlamebreakerArmor(); // 创建备用护甲实例
    spareHeadEquipment = new HelmetOfThePaladin(); // 创建备用头盔实例

    addItem(map);
    addItem(player1);
    addItem(player2);
    addItem(spareArmor);
    addItem(spareHeadEquipment);

    map->scaleToFitScene(this); // 缩放地图以适应场景
    player1->setPos(100,map->getFloorHeight()); // 设置第一个角色出生点
    player2->setPos(1180,map->getFloorHeight()); // 设置第二个角色出生点
    player1->setMoveSpeed(player1->getMoveSpeed()*2); // 设置角色移动速度(速度*2）
    player2->setMoveSpeed(player2->getMoveSpeed()*2);
    player1->setGroundY(map->getFloorHeight()); // 在角色类中的地面高度
    player2->setGroundY(map->getFloorHeight());
    spareArmor->unmount(); // 确保备用护甲未安装
    spareArmor->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.75, map->getFloorHeight()); // 设置备用护甲位置
    spareHeadEquipment->unmount(); // 确保备用头盔未安装
    spareHeadEquipment->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.25, map->getFloorHeight()); // 设置备用头盔位置

    // 初始化游戏循环
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &IceScene::gameLoop);

    // 启动计时器
    elapsedTimer.start();
    lastFrameTime = 0;

    // 启动游戏循环，60FPS
    gameTimer->start(FRAME_TIME);
}

// 辅助函数：计算角色在斜坡上的Y坐标
qreal calculateSlopeY(const QPointF& charBottomCenter, const Obstacle& slopeObstacle)
{
    // charBottomCenter 是角色脚底的中心点（场景坐标）
    // slopeObstacle 包含了斜坡的 startPoint, endPoint 和 direction

    // 如果 startPoint 和 endPoint 的 X 坐标相同，说明是垂直线，不是斜坡
    if (qFuzzyCompare(slopeObstacle.startPoint.x(), slopeObstacle.endPoint.x()))
    {
        return -1; // 表示无法计算或不是有效斜坡
    }

    qreal slopeYAtCharX;

    if (slopeObstacle.direction == SlopeDirection::UpRight) { // 从左下到右上
        // 斜坡公式: y = m * (x - x1) + y1
        // m = (y2 - y1) / (x2 - x1)
        qreal m = (slopeObstacle.endPoint.y() - slopeObstacle.startPoint.y()) /
                  (slopeObstacle.endPoint.x() - slopeObstacle.startPoint.x());
        slopeYAtCharX = m * (charBottomCenter.x() - slopeObstacle.startPoint.x()) + slopeObstacle.startPoint.y();

    }
    else // 从右下到左上
    {
        qreal m = (slopeObstacle.endPoint.y() - slopeObstacle.startPoint.y()) /
                  (slopeObstacle.endPoint.x() - slopeObstacle.startPoint.x());
        slopeYAtCharX = m * (charBottomCenter.x() - slopeObstacle.startPoint.x()) + slopeObstacle.startPoint.y();
    }

    return slopeYAtCharX;
}

// 新增：游戏主循环
void IceScene::gameLoop()
{
    // 计算帧间隔
    qint64 currentTime = elapsedTimer.elapsed();
    qint64 deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // 更新游戏状态
    processInput();      // 处理输入
    processMovement();   // 处理移动
    processPicking();    // 处理拾取

    // 更新角色动画（传入实际的时间间隔）
    if (player1 != nullptr)
    {
        player1->updateAnimation(deltaTime);
    }
    if (player2 != nullptr)
    {
        player2->updateAnimation(deltaTime);
    }

    update(); // 更新场景
}

void IceScene::processInput()
{
    Scene::processInput();
    if (player1 != nullptr)
    {
        player1->processInput();
    }
    if (player2 != nullptr)
    {
        player2->processInput();
    }
}

void IceScene::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_A:
        if (player1 != nullptr)
        {
            player1->setLeftDown(true);
            player1->processInput();
        }
        break;
    case Qt::Key_D:
        if (player1 != nullptr)
        {
            player1->setRightDown(true);
        }
        break;
    case Qt::Key_S:
        if (player1 != nullptr)
        {
            player1->setDownDown(true);
            player1->setPickDown(true);
            player1->setVelocity(QPointF(0, 0)); // 停止角色移动
            player1->setCrouchPixmap(); // 设置下蹲图片
        }
        break;
    case Qt::Key_W:
        if (player1 != nullptr)
        {
            player1->setUpDown(true);
        }
        break;
    case Qt::Key_Left:
        if (player2 != nullptr)
        {
            player2->setLeftDown(true);
        }
        break;
    case Qt::Key_Right:
        if (player2 != nullptr)
        {
            player2->setRightDown(true);
        }
        break;
    case Qt::Key_Down:
        if (player2 != nullptr)
        {
            player2->setDownDown(true);
            player2->setPickDown(true);
            player2->setVelocity(QPointF(0, 0)); // 停止角色移动
            player2->setCrouchPixmap(); // 设置下蹲图片
        }
        break;
    case Qt::Key_Up:
        if (player2 != nullptr)
        {
            player2->setUpDown(true);
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
        if (player1 != nullptr)
        {
            player1->setLeftDown(false);
            player1->processInput();
        }
        break;
    case Qt::Key_D:
        if (player1 != nullptr)
        {
            player1->setRightDown(false);
        }
        break;
    case Qt::Key_S:
        if (player1 != nullptr)
        {
            player1->setDownDown(false);
            player1->setPickDown(false);
            player1->setVelocity(QPointF(0, 0)); // 停止角色移动
            player1->setStandPixmap(); // 设置站立图片
        }
        break;
    case Qt::Key_W:
        if (player1 != nullptr)
        {
            player1->setUpDown(false);
        }
        break;
    case Qt::Key_Left:
        if (player2 != nullptr)
        {
            player2->setLeftDown(false);
        }
        break;
    case Qt::Key_Right:
        if (player2 != nullptr)
        {
            player2->setRightDown(false);
        }
        break;
    case Qt::Key_Down:
        if (player2 != nullptr)
        {
            player2->setDownDown(false);
            player2->setPickDown(false);
            player2->setVelocity(QPointF(0, 0)); // 停止角色移动
            player2->setStandPixmap(); // 设置站立图片
        }
        break;
    case Qt::Key_Up:
        if (player2 != nullptr)
        {
            player2->setUpDown(false);
        }
        break;
    default:
        Scene::keyReleaseEvent(event);
    }
}

void IceScene::update()
{
    // 调用基类的 update，更新其他非移动相关的逻辑
    Scene::update();
}

// 处理角色的移动
void IceScene::processMovement()
{
    Scene::processMovement(); // 调用基类的 processMovement
    const qreal gravity = 0.008; // 重力加速度，根据需要调整
    // const qreal gravity = 0.015 // 备选重力加速度
    qreal maxFallSpeed = 3.0; // 最大下落速度为每帧 3 像素

    // 获取地图中的所有障碍物
    const auto& obstacles = static_cast<Icefield*>(map)->getObstacles();

    // 处理 player1
    if (player1 != nullptr)
    {
        // 水平移动
        player1->setX(player1->x() + player1->getVelocity().x() * deltaTime);

        // 跳跃逻辑
        if (player1->isUpDown() && player1->isOnGround())
        {
            // 调用角色的跳跃函数，设置初始垂直速度并标记不在地面上
            player1->handleJump();
        }

        // 应用重力（如果不在地面上）
        if (!player1->isOnGround())
        {
            player1->setVelocity_y(player1->getVelocity_y() + gravity * deltaTime);

            if (player1->getVelocity_y() > maxFallSpeed)
            {
                player1->setVelocity_y(maxFallSpeed);
            }
        }

        // 垂直移动
        player1->setY(player1->y() + player1->getVelocity_y() * deltaTime);

        // 落地判断
        qreal characterBottomY = player1->y();
        if (characterBottomY >= player1->getGroundY())
        {
            player1->setY(player1->getGroundY());       // 设置角色到底部
            player1->setVelocity_y(0);                  // 停止垂直运动
            player1->setOnGround(true);                 // 标记为在地面上
        }
        else
        {
            player1->setOnGround(false);                // 标记为不在地面上
        }
    }

    // 处理 player2
    if (player2 != nullptr)
    {
        // 水平移动
        player2->setX(player2->x() + player2->getVelocity().x() * deltaTime);

        // 跳跃逻辑
        if (player2->isUpDown() && player2->isOnGround())
        {
            player2->handleJump();
        }

        // 应用重力（如果不在地面上）
        if (!player2->isOnGround())
        {
            player2->setVelocity_y(player2->getVelocity_y() + gravity * deltaTime);
        }

        // 垂直移动
        player2->setY(player2->y() + player2->getVelocity_y() * deltaTime);

        // 落地判断
        qreal characterBottomY = player2->y();
        if (characterBottomY >= player2->getGroundY())
        {
            player2->setY(player2->getGroundY());
            player2->setVelocity_y(0);
            player2->setOnGround(true);
        }
        else
        {
            player2->setOnGround(false);
        }
    }
}

void IceScene::processPicking()
{
    Scene::processPicking();
    if (player1->isPicking())
    {
        auto mountable = findNearestUnmountedMountable(player1->pos(), 100.);
        if (mountable != nullptr)
        {
            spareArmor = dynamic_cast<Armor *>(pickupMountable(player1, mountable));
        }
    }
    if (player2->isPicking())
    {
        auto mountable = findNearestUnmountedMountable(player2->pos(), 100.);
        if (mountable != nullptr)
        {
            spareArmor = dynamic_cast<Armor *>(pickupMountable(player2, mountable));
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
    else if (auto headEquipment = dynamic_cast<HeadEquipment *>(mountable))
    {
        return character->pickupHeadEquipment(headEquipment);
    }
    else if (auto legsEquipment = dynamic_cast<LegEquipment *>(mountable))
    {
        return character->pickupLegEquipment(legsEquipment);
    }
    else if (auto weapon = dynamic_cast<Weapon *>(mountable))
    {
        return character->pickupWeapon(weapon);
    }
    return nullptr;
}
