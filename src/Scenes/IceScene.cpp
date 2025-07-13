#include <QDebug>
#include "IceScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Icefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/HeadEquipments/Helmet_of_the_Paladin.h"

// IceScene 构造函数
IceScene::IceScene(QObject *parent) : Scene(parent)
{
    // 现有代码...
    setSceneRect(0, 0, 1280, 720);

    map = new Icefield();
    player1 = new Link();
    player2 = new Link();
    spareArmor = new FlamebreakerArmor();
    spareHeadEquipment = new HelmetOfThePaladin();

    addItem(map);
    addItem(player1);
    addItem(player2);
    addItem(spareArmor);
    addItem(spareHeadEquipment);

    map->scaleToFitScene(this);

    // 正确设置角色初始位置
    qreal floorHeight = map->getFloorHeight();
    player1->setPos(150, floorHeight);  // 设置在地面上
    player2->setPos(1150, floorHeight); // 设置在地面上
    player2->turnFaceLeft(); // 初始面向左侧

    // 设置角色属性
    player1->setMoveSpeed(player1->getMoveSpeed() * 2);
    player2->setMoveSpeed(player2->getMoveSpeed() * 2);
    player1->setGroundY(floorHeight);
    player2->setGroundY(floorHeight);

    // 确保角色初始时在地面上
    player1->setOnGround(true);
    player2->setOnGround(true);
    player1->setVelocity_y(0);
    player2->setVelocity_y(0);

    // 设置备用装备的位置和状态
    spareArmor->unmount();
    spareArmor->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.75, floorHeight);
    spareHeadEquipment->unmount();
    spareHeadEquipment->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.25, floorHeight);
    spareHeadEquipment->setZValue(5);

    // 初始化游戏循环
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &IceScene::gameLoop);

    // 启动计时器
    elapsedTimer.start();
    lastFrameTime = 0;

    // 启动游戏循环，60FPS
    gameTimer->start(FRAME_TIME);

    // 在构造函数末尾添加（在 gameTimer->start(FRAME_TIME); 之后）：
    // 显示调试可视化
    showDebugVisualization();
    qDebug() << "Debug visualization enabled. Press 'H' to toggle.";
}

// 初始化平台数据
void IceScene::initializePlatforms()
{
    // 添加悬空平台（示例坐标，你需要根据实际地图调整）
    platforms.append(Platform(340, 310, 610, 100, true));  // 悬空平台，单向

    // 添加墙壁/障碍物（双向碰撞）
    // platforms.append(Platform(0, 0, 10, 720, false));     // 左边界墙
    // platforms.append(Platform(1270, 0, 10, 720, false));  // 右边界墙
    // platforms.append(Platform(0, 0, 1280, 10, false));    // 顶部边界
}

// void IceScene::handleCollisions(Character* character, qint64 deltaTime)
// {
//     if (character == nullptr) return;

//     // 获取角色的碰撞盒
//     QRectF characterRect = character->boundingRect();
//     QPointF currentPos = character->pos();
//     characterRect.moveTopLeft(currentPos);

//     // 计算移动后的位置
//     QPointF velocity = character->getVelocity();
//     qreal verticalVelocity = character->getVelocity_y();

//     // 计算位移
//     qreal deltaX = velocity.x() * deltaTime * 0.01; // 调整时间比例
//     qreal deltaY = verticalVelocity * deltaTime * 0.01;

//     QPointF newPos = currentPos;

//     // 水平碰撞检测
//     if (deltaX != 0) {
//         QPointF testPos = currentPos + QPointF(deltaX, 0);
//         QRectF testRect = characterRect;
//         testRect.moveTopLeft(testPos);

//         bool horizontalCollision = false;
//         for (const Platform& platform : platforms) {
//             if (!platform.isOneWay && platform.rect.intersects(testRect)) {
//                 horizontalCollision = true;
//                 break;
//             }
//         }

//         if (!horizontalCollision) {
//             newPos.setX(testPos.x());
//         } else {
//             // 水平碰撞，停止水平移动
//             character->setVelocity(QPointF(0, velocity.y()));
//         }
//     }

//     // 垂直碰撞检测
//     landedOnPlatform = false;  // 重置标志

//     if (deltaY != 0) {
//         QPointF testPos = newPos + QPointF(0, deltaY);
//         QRectF testRect = characterRect;
//         testRect.moveTopLeft(testPos);

//         bool verticalCollision = false;

//         for (const Platform& platform : platforms) {
//             if (platform.rect.intersects(testRect)) {
//                 if (platform.isOneWay) {
//                     // 单向平台：只有从上方落下才能站立
//                     if (verticalVelocity > 0 && // 正在下落
//                         currentPos.y() + characterRect.height() <= platform.rect.top() + 5) { // 角色脚底在平台上方

//                         // 角色站在平台上
//                         newPos.setY(platform.rect.top() - characterRect.height());
//                         character->setVelocity_y(0);
//                         character->setOnGround(true);
//                         landedOnPlatform = true;
//                         verticalCollision = true;
//                         break;
//                     }
//                 } else {
//                     // 双向平台：完全阻挡
//                     if (verticalVelocity > 0) {
//                         // 从上方碰撞（落在平台上）
//                         newPos.setY(platform.rect.top() - characterRect.height());
//                         character->setVelocity_y(0);
//                         character->setOnGround(true);
//                         landedOnPlatform = true;
//                     } else if (verticalVelocity < 0) {
//                         // 从下方碰撞（撞头）
//                         newPos.setY(platform.rect.bottom());
//                         character->setVelocity_y(0);  // 立即停止上升
//                     }
//                     verticalCollision = true;
//                     break;
//                 }
//             }
//         }

//         if (!verticalCollision) {
//             newPos.setY(testPos.y());
//         }
//     }

//     // 检查是否离开了所有平台
//     if (!landedOnPlatform) {
//         QRectF currentRect = characterRect;
//         currentRect.moveTopLeft(newPos);

//         bool onAnyPlatform = false;
//         for (const Platform& platform : platforms) {
//             if (platform.isOneWay) {
//                 // 检查是否仍在单向平台上
//                 if (qAbs(newPos.y() + characterRect.height() - platform.rect.top()) < 5 &&
//                     newPos.x() + characterRect.width() > platform.rect.left() &&
//                     newPos.x() < platform.rect.right()) {
//                     onAnyPlatform = true;
//                     break;
//                 }
//             }
//         }

//         // 检查是否在地面上
//         if (newPos.y() + characterRect.height() >= character->getGroundY()) {
//             onAnyPlatform = true;
//         }

//         if (!onAnyPlatform) {
//             character->setOnGround(false);
//         }
//     }

//     // 更新角色位置
//     character->setPos(newPos);

//     // 处理边界碰撞
//     handleBoundaryCollision(character);
// }

// // 处理边界碰撞
// void IceScene::handleBoundaryCollision(Character* character)
// {
//     if (character == nullptr) return;

//     QRectF characterRect = character->boundingRect();
//     QPointF pos = character->pos();

//     // 左边界
//     if (pos.x() < 0) {
//         pos.setX(0);
//         character->setVelocity(QPointF(0, character->getVelocity().y()));
//     }

//     // 右边界
//     if (pos.x() + characterRect.width() > sceneRect().width()) {
//         pos.setX(sceneRect().width() - characterRect.width());
//         character->setVelocity(QPointF(0, character->getVelocity().y()));
//     }

//     // 顶部边界
//     if (pos.y() < 0) {
//         pos.setY(0);
//         character->setVelocity_y(0);
//     }

//     // 底部边界（防止角色掉出地图）
//     if (pos.y() + characterRect.height() > sceneRect().height()) {
//         pos.setY(sceneRect().height() - characterRect.height());
//         character->setVelocity_y(0);
//         character->setOnGround(true);
//     }

//     character->setPos(pos);
// }

// 游戏主循环
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
    // 注意：不要在这里调用角色的processInput，因为它们会在keyPressEvent中被调用
    Scene::processInput();
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
            player1->processInput();
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
            player2->processInput();
        }
        break;
    case Qt::Key_Right:
        if (player2 != nullptr)
        {
            player2->setRightDown(true);
            player2->processInput();
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


    // 在 keyPressEvent 中添加切换调试视图的按键：
    // 切换调试视图
    case Qt::Key_H:
        for (auto item : debugItems)
        {
            item->setVisible(!item->isVisible());
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
            player1->processInput();
        }
        break;
    case Qt::Key_S:
        if (player1 != nullptr)
        {
            player1->setDownDown(false);
            player1->setPickDown(false);
            player1->processInput();
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
            player2->processInput();
        }
        break;
    case Qt::Key_Right:
        if (player2 != nullptr)
        {
            player2->setRightDown(false);
            player2->processInput();
        }
        break;
    case Qt::Key_Down:
        if (player2 != nullptr)
        {
            player2->setDownDown(false);
            player2->setPickDown(false);
            player2->processInput();
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
    Scene::processMovement();
    const qreal gravity = 0.008;
    qreal maxFallSpeed = 3.0;

    const auto& obstacles = static_cast<Icefield*>(map)->getObstacles();

    // 处理 player1
    if (player1 != nullptr)
    {
        // 水平移动
        player1->setX(player1->x() + player1->getVelocity().x() * deltaTime);

        // 跳跃逻辑
        if (player1->isUpDown() && player1->isOnGround())
        {
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
            player1->setY(player1->getGroundY());
            player1->setVelocity_y(0);
            player1->setOnGround(true);
        }
        else
        {
            player1->setOnGround(false);
        }

        // 应用地图效果（摩擦力等）
        static_cast<Icefield*>(map)->applyEffectToCharacter(player1, deltaTime);
    }

    // 处理 player2（类似的修复）
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

            if (player2->getVelocity_y() > maxFallSpeed)
            {
                player2->setVelocity_y(maxFallSpeed);
            }
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

        // 应用地图效果（摩擦力等）
        static_cast<Icefield*>(map)->applyEffectToCharacter(player2, deltaTime);
    }
}

void IceScene::processPicking()
{
    Scene::processPicking();
    if (player1 != nullptr && player1->isPicking())
    {
        auto mountable = findNearestUnmountedMountable(player1->pos(), 100.);
        if (mountable != nullptr)
        {
            auto pickedUp = pickupMountable(player1, mountable);
            if (auto armor = dynamic_cast<Armor*>(pickedUp))
            {
                spareArmor = armor;
            }
            else if (auto headEquip = dynamic_cast<HeadEquipment*>(pickedUp))
            {
                spareHeadEquipment = headEquip;
            }
        }
    }
    if (player2 != nullptr && player2->isPicking())
    {
        auto mountable = findNearestUnmountedMountable(player2->pos(), 100.);
        if (mountable != nullptr)
        {
            auto pickedUp = pickupMountable(player2, mountable);
            if (auto armor = dynamic_cast<Armor*>(pickedUp))
            {
                spareArmor = armor;
            }
            else if (auto headEquip = dynamic_cast<HeadEquipment*>(pickedUp))
            {
                spareHeadEquipment = headEquip;
            }
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

// 用于调试可视化
// 在 IceScene.cpp 中添加实现：
void IceScene::showDebugVisualization()
{
    // 清除之前的调试项
    for (auto item : debugItems) {
        removeItem(item);
        delete item;
    }
    debugItems.clear();

    // 1. 显示场景边界
    QPen sceneBorderPen(Qt::magenta, 5, Qt::DashLine);
    QGraphicsRectItem* sceneBorder = new QGraphicsRectItem(sceneRect());
    sceneBorder->setPen(sceneBorderPen);
    sceneBorder->setBrush(Qt::NoBrush);
    sceneBorder->setZValue(200);
    addItem(sceneBorder);
    debugItems.append(sceneBorder);

    // 在场景边界上添加标签
    QGraphicsTextItem* sceneLabel = new QGraphicsTextItem("Scene Boundary");
    sceneLabel->setPos(10, 10);
    sceneLabel->setDefaultTextColor(Qt::magenta);
    sceneLabel->setZValue(201);
    addItem(sceneLabel);
    debugItems.append(sceneLabel);

    // 2. 显示平台的碰撞框
    for (int i = 0; i < platforms.size(); ++i) {
        const Platform& platform = platforms[i];

        // 根据平台类型选择颜色
        QColor color = platform.isOneWay ? Qt::blue : Qt::red;
        QPen platformPen(color, 2);
        QBrush platformBrush(color);
        platformBrush.setStyle(Qt::Dense6Pattern);  // 半透明填充

        QGraphicsRectItem* platformRect = new QGraphicsRectItem(platform.rect);
        platformRect->setPen(platformPen);
        platformRect->setBrush(platformBrush);
        platformRect->setOpacity(0.3);  // 半透明
        platformRect->setZValue(150);
        addItem(platformRect);
        debugItems.append(platformRect);

        // 添加平台标签
        QString labelText = QString("Platform %1\n%2\nPos: (%3, %4)\nSize: %5x%6")
                                .arg(i)
                                .arg(platform.isOneWay ? "One-way" : "Solid")
                                .arg(platform.rect.x())
                                .arg(platform.rect.y())
                                .arg(platform.rect.width())
                                .arg(platform.rect.height());

        QGraphicsTextItem* platformLabel = new QGraphicsTextItem(labelText);
        platformLabel->setPos(platform.rect.center());
        platformLabel->setDefaultTextColor(color);
        platformLabel->setZValue(151);
        QFont font = platformLabel->font();
        font.setPointSize(8);
        platformLabel->setFont(font);
        addItem(platformLabel);
        debugItems.append(platformLabel);
    }

    // 3. 显示地面线
    qreal floorHeight = map->getFloorHeight();
    QPen floorPen(Qt::green, 3, Qt::DotLine);
    QGraphicsLineItem* floorLine = new QGraphicsLineItem(0, floorHeight, sceneRect().width(), floorHeight);
    floorLine->setPen(floorPen);
    floorLine->setZValue(100);
    addItem(floorLine);
    debugItems.append(floorLine);

    // 地面标签
    QGraphicsTextItem* floorLabel = new QGraphicsTextItem(QString("Floor Height: %1").arg(floorHeight));
    floorLabel->setPos(sceneRect().width() / 2 - 50, floorHeight - 20);
    floorLabel->setDefaultTextColor(Qt::green);
    floorLabel->setZValue(101);
    addItem(floorLabel);
    debugItems.append(floorLabel);

    // 4. 显示角色1的碰撞框和信息
    if (player1) {
        QRectF p1Rect = player1->boundingRect();
        QPointF p1Pos = player1->pos();

        // 转换到场景坐标
        QRectF p1SceneRect = p1Rect;
        p1SceneRect.moveTopLeft(p1Pos);

        QPen p1Pen(Qt::cyan, 2);
        QGraphicsRectItem* p1Box = new QGraphicsRectItem(p1SceneRect);
        p1Box->setPen(p1Pen);
        p1Box->setBrush(Qt::NoBrush);
        p1Box->setZValue(160);
        addItem(p1Box);
        debugItems.append(p1Box);

        // 角色中心点
        QGraphicsEllipseItem* p1Center = new QGraphicsEllipseItem(p1Pos.x() - 3, p1Pos.y() - 3, 6, 6);
        p1Center->setBrush(Qt::cyan);
        p1Center->setZValue(161);
        addItem(p1Center);
        debugItems.append(p1Center);

        // 角色信息
        QString p1Info = QString("Player1\nPos: (%1, %2)\nVel: (%3, %4)\nOnGround: %5\nMoveSpeed: %6")
                             .arg(p1Pos.x(), 0, 'f', 1)
                             .arg(p1Pos.y(), 0, 'f', 1)
                             .arg(player1->getVelocity().x(), 0, 'f', 2)
                             .arg(player1->getVelocity_y(), 0, 'f', 2)
                             .arg(player1->isOnGround() ? "Yes" : "No")
                             .arg(player1->getMoveSpeed(), 0, 'f', 2);

        QGraphicsTextItem* p1Label = new QGraphicsTextItem(p1Info);
        p1Label->setPos(p1Pos.x() + 50, p1Pos.y() - 100);
        p1Label->setDefaultTextColor(Qt::cyan);
        p1Label->setZValue(162);
        QFont infoFont = p1Label->font();
        infoFont.setPointSize(10);
        p1Label->setFont(infoFont);
        addItem(p1Label);
        debugItems.append(p1Label);
    }

    // 5. 显示角色2的碰撞框和信息
    if (player2) {
        QRectF p2Rect = player2->boundingRect();
        QPointF p2Pos = player2->pos();

        // 转换到场景坐标
        QRectF p2SceneRect = p2Rect;
        p2SceneRect.moveTopLeft(p2Pos);

        QPen p2Pen(Qt::yellow, 2);
        QGraphicsRectItem* p2Box = new QGraphicsRectItem(p2SceneRect);
        p2Box->setPen(p2Pen);
        p2Box->setBrush(Qt::NoBrush);
        p2Box->setZValue(160);
        addItem(p2Box);
        debugItems.append(p2Box);

        // 角色中心点
        QGraphicsEllipseItem* p2Center = new QGraphicsEllipseItem(p2Pos.x() - 3, p2Pos.y() - 3, 6, 6);
        p2Center->setBrush(Qt::yellow);
        p2Center->setZValue(161);
        addItem(p2Center);
        debugItems.append(p2Center);

        // 角色信息
        QString p2Info = QString("Player2\nPos: (%1, %2)\nVel: (%3, %4)\nOnGround: %5\nMoveSpeed: %6")
                             .arg(p2Pos.x(), 0, 'f', 1)
                             .arg(p2Pos.y(), 0, 'f', 1)
                             .arg(player2->getVelocity().x(), 0, 'f', 2)
                             .arg(player2->getVelocity_y(), 0, 'f', 2)
                             .arg(player2->isOnGround() ? "Yes" : "No")
                             .arg(player2->getMoveSpeed(), 0, 'f', 2);

        QGraphicsTextItem* p2Label = new QGraphicsTextItem(p2Info);
        p2Label->setPos(p2Pos.x() - 150, p2Pos.y() - 100);
        p2Label->setDefaultTextColor(Qt::yellow);
        p2Label->setZValue(162);
        QFont infoFont = p2Label->font();
        infoFont.setPointSize(10);
        p2Label->setFont(infoFont);
        addItem(p2Label);
        debugItems.append(p2Label);
    }

    // 6. 显示坐标轴
    QPen axisPen(Qt::darkGray, 1);

    // X轴刻度
    for (int x = 0; x <= sceneRect().width(); x += 100) {
        QGraphicsLineItem* tick = new QGraphicsLineItem(x, sceneRect().height() - 10, x, sceneRect().height());
        tick->setPen(axisPen);
        tick->setZValue(90);
        addItem(tick);
        debugItems.append(tick);

        QGraphicsTextItem* tickLabel = new QGraphicsTextItem(QString::number(x));
        tickLabel->setPos(x - 15, sceneRect().height() - 30);
        tickLabel->setDefaultTextColor(Qt::darkGray);
        tickLabel->setZValue(91);
        QFont tickFont = tickLabel->font();
        tickFont.setPointSize(8);
        tickLabel->setFont(tickFont);
        addItem(tickLabel);
        debugItems.append(tickLabel);
    }

    // Y轴刻度
    for (int y = 0; y <= sceneRect().height(); y += 100) {
        QGraphicsLineItem* tick = new QGraphicsLineItem(0, y, 10, y);
        tick->setPen(axisPen);
        tick->setZValue(90);
        addItem(tick);
        debugItems.append(tick);

        QGraphicsTextItem* tickLabel = new QGraphicsTextItem(QString::number(y));
        tickLabel->setPos(15, y - 10);
        tickLabel->setDefaultTextColor(Qt::darkGray);
        tickLabel->setZValue(91);
        QFont tickFont = tickLabel->font();
        tickFont.setPointSize(8);
        tickLabel->setFont(tickFont);
        addItem(tickLabel);
        debugItems.append(tickLabel);
    }

    // 7. 显示说明
    QString instructions = "Debug Visualization:\n"
                           "- Magenta dashed: Scene boundary\n"
                           "- Green dotted: Floor line\n"
                           "- Red filled: Solid platforms\n"
                           "- Blue filled: One-way platforms\n"
                           "- Cyan box: Player1\n"
                           "- Yellow box: Player2\n"
                           "Press 'H' to hide debug view";

    QGraphicsTextItem* instructionText = new QGraphicsTextItem(instructions);
    instructionText->setPos(10, 50);
    instructionText->setDefaultTextColor(Qt::white);
    instructionText->setZValue(202);
    QFont instructionFont = instructionText->font();
    instructionFont.setPointSize(10);
    instructionFont.setBold(true);
    instructionText->setFont(instructionFont);

    // 添加背景框使文字更清晰
    QRectF textRect = instructionText->boundingRect();
    QGraphicsRectItem* textBg = new QGraphicsRectItem(textRect);
    textBg->setPos(10, 50);
    textBg->setBrush(QBrush(QColor(0, 0, 0, 180)));  // 半透明黑色背景
    textBg->setPen(Qt::NoPen);
    textBg->setZValue(201);
    addItem(textBg);
    debugItems.append(textBg);

    addItem(instructionText);
    debugItems.append(instructionText);
}
