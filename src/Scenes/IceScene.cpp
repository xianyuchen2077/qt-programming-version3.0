#include <QDebug>
#include "IceScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Icefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/HeadEquipments/Helmet_of_the_Paladin.h"
#include "../Items/LegEquipments/LegEquipment.h"
#include "../Items/Weapons/Weapon.h"

// IceScene 构造函数
IceScene::IceScene(QObject *parent) : Scene(parent)
{
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
    player1->setPos(250, floorHeight);  // 设置在地面上
    player2->setPos(1000, floorHeight); // 设置在地面上
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

    // // 初始化平台数据
    // initializePlatforms();
    // landedOnPlatform = false;

    // 在构造函数末尾添加（在 gameTimer->start(FRAME_TIME); 之后）：
    // 显示调试可视化
    showDebugVisualization();
    qDebug() << "Debug visualization enabled. Press 'H' to toggle.";
}

// 初始化平台数据
void IceScene::initializePlatforms()
{
    platforms.clear();

    // 添加悬空的冰平台（单向平台，只能从上方站立）
    // 对应ice_platform在Icefield.cpp中的位置: setPos(300, 270), setScale(0.7)
    // 原始ice_platform图片假设为200x100像素
    qreal platformWidth = 200 * 0.7;  // 缩放后的宽度
    qreal platformHeight = 20;         // 平台的实际可站立高度（不是整个图片高度）
    platforms.append(Platform(300, 270, platformWidth, platformHeight, true));  // 单向平台

    // 添加地面边界（双向，不可穿越）
    qreal floorHeight = map->getFloorHeight();
    platforms.append(Platform(0, floorHeight, sceneRect().width(), 50, false));  // 地面

    // 添加左右边界墙（双向，不可穿越）
    platforms.append(Platform(-10, 0, 10, sceneRect().height(), false));  // 左墙
    platforms.append(Platform(sceneRect().width(), 0, 10, sceneRect().height(), false));  // 右墙

    // 可以根据需要添加更多平台
    // platforms.append(Platform(x, y, width, height, isOneWay));
}

// 处理角色与边界的碰撞
void IceScene::handleBoundaryCollision(Character* character, QPointF& newPos)
{
    if (!character) return;

    // 获取身体碰撞框（用于边界检测）
    QRectF bodyRect = character->getBodyCollisionRect();

    // 计算角色身体实际占用的区域
    QRectF actualBodyRect = bodyRect;
    actualBodyRect.moveTopLeft(newPos + bodyRect.topLeft());

    // 左边界
    if (actualBodyRect.left() < 0)
    {
        newPos.setX(-bodyRect.left());
        character->setVelocity(QPointF(0, character->getVelocity().y()));
        qDebug() << "Hit left boundary";
    }

    // 右边界
    if (actualBodyRect.right() > sceneRect().width())
    {
        newPos.setX(sceneRect().width() - bodyRect.width() - bodyRect.left());
        character->setVelocity(QPointF(0, character->getVelocity().y()));
        qDebug() << "Hit right boundary";
    }

    // 顶部边界
    if (actualBodyRect.top() < 0)
    {
        newPos.setY(-bodyRect.top());
        character->setVelocity_y(0);
        qDebug() << "Hit top boundary";
    }

    // 底部边界（防止角色掉出地图）
    if (actualBodyRect.bottom() > sceneRect().height())
    {
        newPos.setY(sceneRect().height() - bodyRect.height() - bodyRect.top());
        character->setVelocity_y(0);
        character->setOnGround(true);
        qDebug() << "Hit bottom boundary";
    }
}

// 地面碰撞检测
void IceScene::handleGroundCollision(Character* character, QPointF& newPos)
{
    if (!character) return;

    // 获取地面高度
    qreal floorHeight = map->getFloorHeight();

    // 获取角色的碰撞框
    QRectF characterRect = character->boundingRect();

    // 计算角色碰撞框底部在新位置的Y坐标
    qreal characterBottom = newPos.y() + characterRect.bottom();

    // 检查是否碰到地面
    if (characterBottom >= floorHeight)
    {
        // 角色落在地面上
        newPos.setY(floorHeight - characterRect.bottom());
        character->setVelocity_y(0);
        character->setOnGround(true);
        qDebug() << "Character landed on ground at Y:" << newPos.y();
    }
    else
    {
        // 角色在空中
        character->setOnGround(false);
    }

    // 边界检测
    handleBoundaryCollision(character, newPos);

    // 应用新位置
    character->setPos(newPos);
}

// 处理所有碰撞，包括障碍物和地面
void IceScene::handleAllCollisions(Character* character, QPointF& newPos)
{
    if (!character) return;

    QRectF headRect = character->getHeadCollisionRect();
    QRectF bodyRect = character->getBodyCollisionRect();
    QPointF oldPos = character->pos();
    bool wasOnGround = character->isOnGround();

    // ========== 第一步：处理头部被卡住的情况（加快滑动速度） ==========
    bool headStuck = isHeadStuckInObstacle(character, newPos);
    bool wasHeadStuck = isHeadStuckInObstacle(character, oldPos);

    if (headStuck || wasHeadStuck)
    {
        qDebug() << "Head stuck detected, applying slide logic";

        // 计算滑动方向
        QPointF slideDirection = calculateSlideDirection(character, newPos);

        // ========== 修改：加快滑动效果，从0.1改为0.5 ==========
        if (slideDirection.x() != 0)
        {
            QPointF slidePos = newPos + QPointF(slideDirection.x() * 0.5, 0); // 渐进式滑动，加快速度

            // 检查滑动后的位置是否安全
            if (isPositionSafe(character, slidePos))
            {
                newPos.setX(slidePos.x());
                qDebug() << "Applied fast slide to X:" << slidePos.x();
            }
            else
            {
                // 如果0.5倍速度不安全，尝试0.3倍速度
                slidePos = newPos + QPointF(slideDirection.x() * 0.3, 0);
                if (isPositionSafe(character, slidePos))
                {
                    newPos.setX(slidePos.x());
                    qDebug() << "Applied medium slide to X:" << slidePos.x();
                }
                else
                {
                    // 如果0.3倍速度还不安全，使用原来的0.1倍速度
                    slidePos = newPos + QPointF(slideDirection.x() * 0.1, 0);
                    if (isPositionSafe(character, slidePos))
                    {
                        newPos.setX(slidePos.x());
                        qDebug() << "Applied slow slide to X:" << slidePos.x();
                    }
                }
            }
        }

        // 在滑动的同时允许下落
        // 计算合适的下落距离
        qreal fallDistance = qAbs(headRect.bottom() - bodyRect.top()) * 0.3; // 头部和身体差值的30%
        newPos.setY(newPos.y() + fallDistance);

        qDebug() << "Applied fall distance:" << fallDistance;
    }

    // ========== 第二步：改进的水平碰撞检测（先头部，后身体） ==========
    QPointF testPosX = QPointF(newPos.x(), oldPos.y()); // 只改变X坐标

    // ========== 修改：首先检查头部水平碰撞 ==========
    bool headHorizontalCollision = checkHeadHorizontalCollision(character, testPosX);
    bool bodyHorizontalCollision = checkBodyObstacleCollision(character, testPosX);

    if (headHorizontalCollision || bodyHorizontalCollision)
    {
        // 如果头部或身体发生水平碰撞，阻止水平移动
        newPos.setX(oldPos.x());
        character->setVelocity(QPointF(0, character->getVelocity().y()));

        if (headHorizontalCollision && !bodyHorizontalCollision)
        {
            qDebug() << "Head-only horizontal collision detected";
        }
        else if (!headHorizontalCollision && bodyHorizontalCollision)
        {
            qDebug() << "Body-only horizontal collision detected";
        }
        else
        {
            qDebug() << "Both head and body horizontal collision detected";
        }
    }

    // ========== 第三步：垂直碰撞检测（地面和平台） ==========
    QPointF testPosY = QPointF(newPos.x(), newPos.y());
    bool verticalCollision = false;
    bool isOnAnyPlatform = false;

    // 地面碰撞检测 - 仅使用身体碰撞框
    qreal floorHeight = map->getFloorHeight();
    qreal bodyBottom = testPosY.y() + bodyRect.bottom();

    if (bodyBottom >= floorHeight)
    {
        // 角色的身体部分落在地面上
        newPos.setY(floorHeight - bodyRect.bottom());
        character->setVelocity_y(0);
        character->setOnGround(true);
        verticalCollision = true;
        isOnAnyPlatform = true;
        qDebug() << "Character body landed on ground";
    }

    // 平台碰撞检测 - 仅使用身体碰撞框
    if (!verticalCollision && checkBodyObstacleCollision(character, testPosY))
    {
        // 检查是否是从上方落在平台上
        if (character->getVelocity_y() > 0) // 正在下落
        {
            const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

            for (const Obstacle& obstacle : obstacles)
            {
                QRectF testBodyRect = bodyRect;
                testBodyRect.moveTopLeft(testPosY + bodyRect.topLeft());

                if (obstacle.bounds.intersects(testBodyRect))
                {
                    // 检查是否是从上方碰撞 - 使用身体碰撞框的底部
                    qreal bodyOldBottom = oldPos.y() + bodyRect.bottom();
                    if (bodyOldBottom <= obstacle.bounds.top() + 5) // 允许5像素误差
                    {
                        // 角色的身体站在障碍物上
                        newPos.setY(obstacle.bounds.top() - bodyRect.bottom());
                        character->setVelocity_y(0);
                        character->setOnGround(true);
                        verticalCollision = true;
                        isOnAnyPlatform = true;
                        qDebug() << "Character body landed on platform at Y:" << newPos.y();
                        break;
                    }
                }
            }
        }

        if (!verticalCollision)
        {
            // 如果不是落在平台上，则是撞到障碍物，恢复Y坐标
            newPos.setY(oldPos.y());
            if (character->getVelocity_y() < 0) // 向上撞头
            {
                character->setVelocity_y(0);
            }
            qDebug() << "Vertical obstacle collision detected";
        }
    }

    // ========== 第四步：检查角色是否仍在平台上 ==========
    if (!isOnAnyPlatform)
    {
        QRectF newBodyRect = bodyRect;
        newBodyRect.moveTopLeft(newPos + bodyRect.topLeft());

        bool stillOnPlatform = false;
        qreal newBodyBottom = newPos.y() + bodyRect.bottom();

        // 检查是否在地面上
        if (newBodyBottom >= floorHeight - 2)
        {
            stillOnPlatform = true;
        }

        // 检查是否在任何障碍物平台上
        if (!stillOnPlatform)
        {
            const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();
            for (const Obstacle& obstacle : obstacles)
            {
                if (qAbs(newBodyBottom - obstacle.bounds.top()) < 5)
                {
                    qreal bodyLeft = newPos.x() + bodyRect.left();
                    qreal bodyRight = newPos.x() + bodyRect.right();

                    if (bodyRight > obstacle.bounds.left() && bodyLeft < obstacle.bounds.right())
                    {
                        stillOnPlatform = true;
                        qDebug() << "Character body still on platform" << obstacle.bounds;
                        break;
                    }
                }
            }
        }

        if (!stillOnPlatform)
        {
            character->setOnGround(false);
            qDebug() << "Character body left all platforms, now falling";
        }
    }

    // ========== 第五步：边界检测 ==========
    handleBoundaryCollision(character, newPos);

    // ========== 第六步：最终安全检查（加快调整速度） ==========
    // 确保最终位置不会导致身体与障碍物重叠
    if (!isPositionSafe(character, newPos))
    {
        qDebug() << "Final position unsafe, adjusting...";

        // ========== 修改：加快微调步长，从2像素改为5像素 ==========
        for (int i = 1; i <= 10; i++)
        {
            QPointF adjustedPos = newPos + QPointF(i * 5, 0); // 加快调整步长
            if (isPositionSafe(character, adjustedPos))
            {
                newPos = adjustedPos;
                qDebug() << "Adjusted position to:" << newPos;
                break;
            }

            adjustedPos = newPos + QPointF(-i * 5, 0); // 加快调整步长
            if (isPositionSafe(character, adjustedPos))
            {
                newPos = adjustedPos;
                qDebug() << "Adjusted position to:" << newPos;
                break;
            }
        }

        // 如果水平调整不成功，尝试垂直调整
        if (!isPositionSafe(character, newPos))
        {
            qDebug() << "Horizontal adjustment failed, trying vertical adjustment...";
            for (int i = 1; i <= 5; i++)
            {
                QPointF adjustedPos = newPos + QPointF(0, -i * 3); // 向上调整
                if (isPositionSafe(character, adjustedPos))
                {
                    newPos = adjustedPos;
                    qDebug() << "Vertically adjusted position to:" << newPos;
                    break;
                }
            }
        }
    }

    // 应用新位置
    character->setPos(newPos);
}

// 检查角色与障碍物的碰撞
bool IceScene::checkObstacleCollision(Character* character, const QPointF& testPos)
{
    if (!character) return false;

    // 获取角色的所有碰撞矩形
    QList<QRectF> collisionRects = character->getAllCollisionRects();

    // 计算每个碰撞矩形在测试位置的实际位置
    QList<QRectF> testRects;
    for (const QRectF& rect : collisionRects)
    {
        QRectF testRect = rect;
        testRect.moveTopLeft(testPos + rect.topLeft());
        testRects.append(testRect);
    }

    // 获取障碍物列表
    const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

    for (const Obstacle& obstacle : obstacles)
    {
        // 检查任何一个碰撞矩形是否与障碍物相交
        for (const QRectF& testRect : testRects)
        {
            if (obstacle.bounds.intersects(testRect))
            {
                qDebug() << "Collision detected with obstacle at:" << obstacle.bounds
                         << "Character rect:" << testRect;
                return true;
            }
        }
    }

    return false;
}

// 检测头部是否被障碍物卡住
bool IceScene::isHeadStuckInObstacle(Character* character, const QPointF& testPos)
{
    if (!character) return false;

    QRectF headRect = character->getHeadCollisionRect();
    QRectF testHeadRect = headRect;
    testHeadRect.moveTopLeft(testPos + headRect.topLeft());

    const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

    for (const Obstacle& obstacle : obstacles)
    {
        if (obstacle.bounds.intersects(testHeadRect))
        {
            return true;
        }
    }

    return false;
}

// 计算滑动方向和距离
QPointF IceScene::calculateSlideDirection(Character* character, const QPointF& currentPos)
{
    if (!character) return QPointF(0, 0);

    QRectF headRect = character->getHeadCollisionRect();
    QRectF testHeadRect = headRect;
    testHeadRect.moveTopLeft(currentPos + headRect.topLeft());

    const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

    QPointF slideDirection(0, 0);

    for (const Obstacle& obstacle : obstacles)
    {
        if (obstacle.bounds.intersects(testHeadRect))
        {
            // 计算头部中心点
            QPointF headCenter = testHeadRect.center();
            QPointF obstacleCenter = obstacle.bounds.center();

            // 计算从障碍物中心到头部中心的向量
            QPointF awayFromObstacle = headCenter - obstacleCenter;

            // 只考虑水平方向的滑动，垂直方向交给重力处理
            if (awayFromObstacle.x() != 0)
            {
                slideDirection.setX(awayFromObstacle.x() > 0 ? 1 : -1);
            }

            // 计算滑动距离 - 头部和身体差值的一半左右
            QRectF bodyRect = character->getBodyCollisionRect();
            qreal slideDistance = qAbs(headRect.center().x() - bodyRect.center().x()) * 0.5;

            slideDirection.setX(slideDirection.x() * slideDistance);

            qDebug() << "Head stuck in obstacle, slide direction:" << slideDirection;
            break;
        }
    }

    return slideDirection;
}

// 检查位置是否安全（不会导致身体与障碍物重叠）
bool IceScene::isPositionSafe(Character* character, const QPointF& testPos)
{
    if (!character) return false;

    QRectF bodyRect = character->getBodyCollisionRect();
    QRectF testBodyRect = bodyRect;
    testBodyRect.moveTopLeft(testPos + bodyRect.topLeft());

    const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

    for (const Obstacle& obstacle : obstacles)
    {
        if (obstacle.bounds.intersects(testBodyRect))
        {
            return false;
        }
    }

    return true;
}

// 仅检查身体碰撞的函数
bool IceScene::checkBodyObstacleCollision(Character* character, const QPointF& testPos)
{
    if (!character) return false;

    QRectF bodyRect = character->getBodyCollisionRect();
    QRectF testBodyRect = bodyRect;
    testBodyRect.moveTopLeft(testPos + bodyRect.topLeft());

    const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

    for (const Obstacle& obstacle : obstacles)
    {
        if (obstacle.bounds.intersects(testBodyRect))
        {
            qDebug() << "Body collision detected with obstacle at:" << obstacle.bounds
                     << "Body rect:" << testBodyRect;
            return true;
        }
    }

    return false;
}

// 检查头部水平碰撞的函数
bool IceScene::checkHeadHorizontalCollision(Character* character, const QPointF& testPos)
{
    if (!character) return false;

    QRectF headRect = character->getHeadCollisionRect();
    QRectF testHeadRect = headRect;
    testHeadRect.moveTopLeft(testPos + headRect.topLeft());

    const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

    for (const Obstacle& obstacle : obstacles)
    {
        if (obstacle.bounds.intersects(testHeadRect))
        {
            qDebug() << "Head horizontal collision detected with obstacle at:" << obstacle.bounds
                     << "Head rect:" << testHeadRect;
            return true;
        }
    }

    return false;
}

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

    // 每帧更新动态调试信息
    if (debugVisible)
    {
        updateDebugVisualization();
    }

    update(); // 更新场景
}

void IceScene::processInput()
{
    // 调用基类的 processInput，处理其他输入逻辑
    Scene::processInput();

    // 确保角色处理输入
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
            player1->setPickDown(true);  // 设置拾取状态
        }
        break;
    case Qt::Key_W:
        if (player1 != nullptr)
        {
            player1->setUpDown(true);
        }
        break;
    case Qt::Key_J:
        if (player1 != nullptr && player1->canShoot())
        {
            QPointF shootDirection = player1->isFaceRight() ? QPointF(1, 0) : QPointF(-1, 0);
            player1->shoot(shootDirection);
            qDebug() << "Player1 shoots!";
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
            player2->setPickDown(true);  // 设置拾取状态
        }
        break;
    case Qt::Key_Up:
        if (player2 != nullptr)
        {
            player2->setUpDown(true);
        }
        break;
    case Qt::Key_0:
        if (player2 != nullptr && player2->canShoot())
        {
            QPointF shootDirection = player2->isFaceRight() ? QPointF(1, 0) : QPointF(-1, 0);
            player2->shoot(shootDirection);
            qDebug() << "Player2 shoots!";
        }
        break;
    case Qt::Key_H:
        debugVisible = !debugVisible;
        if (debugVisible) {
            showDebugVisualization();
        } else {
            hideDebugVisualization();
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
            player1->setPickDown(false);  // 清除拾取状态
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
            player2->setPickDown(false);  // 清除拾取状态
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

    // 处理 player1
    if (player1 != nullptr)
    {
        QPointF oldPos = player1->pos();
        QPointF newPos = oldPos;

        // 水平移动
        newPos.setX(newPos.x() + player1->getVelocity().x() * deltaTime);

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
        newPos.setY(newPos.y() + player1->getVelocity_y() * deltaTime);

        // 完整障碍物检测
        handleAllCollisions(player1, newPos);

        // 应用地图效果（摩擦力等）
        static_cast<Icefield*>(map)->applyEffectToCharacter(player1, deltaTime);
    }

    // 处理 player2 (相同逻辑)
    if (player2 != nullptr)
    {
        QPointF oldPos = player2->pos();
        QPointF newPos = oldPos;

        // 水平移动
        newPos.setX(newPos.x() + player2->getVelocity().x() * deltaTime);

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
        newPos.setY(newPos.y() + player2->getVelocity_y() * deltaTime);

        // 完整障碍物检测
        handleAllCollisions(player2, newPos);

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
                // 使用角色的实际中心点而不是pos()来计算距离
                // 角色的pos()是脚底位置，需要计算实际中心
                QPointF characterCenter = pos;

                // 物品的中心位置
                QRectF itemBounds = item->boundingRect();
                QPointF itemCenter = item->pos() + QPointF(itemBounds.width() / 2, itemBounds.height() / 2);

                // 计算两个中心点之间的距离
                qreal distance = QLineF(characterCenter, itemCenter).length();

                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearest = mountable;
                }
            }
        }
    }

    if (nearest)
    {
        qDebug() << "Nearest item found at distance:" << minDistance;
    }
    else
    {
        qDebug() << "No suitable items found within threshold:" << distance_threshold;
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

void IceScene::showDebugVisualization()
{
    // 如果调试项已经存在，只需要显示它们
    if (!debugItems.isEmpty())
    {
        for (auto item : std::as_const(debugItems))
        {
            if (item)
            {
                item->setVisible(true);
            }
        }
        return;
    }

    // 第一次创建调试项
    debugItems.clear();

    // 1. 显示场景边界
    QPen sceneBorderPen(Qt::magenta, 5, Qt::DashLine);
    QGraphicsRectItem* sceneBorder = new QGraphicsRectItem(sceneRect());
    sceneBorder->setPen(sceneBorderPen);
    sceneBorder->setBrush(Qt::NoBrush);
    sceneBorder->setZValue(200);
    addItem(sceneBorder);
    debugItems.append(sceneBorder);

    // 场景边界标签
    QGraphicsTextItem* sceneLabel = new QGraphicsTextItem("Scene Boundary");
    sceneLabel->setPos(10, 10);
    sceneLabel->setDefaultTextColor(Qt::magenta);
    sceneLabel->setZValue(201);
    addItem(sceneLabel);
    debugItems.append(sceneLabel);

    // 2. 显示地面线
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

    // 3. 显示障碍物边界
    QList<Obstacle> sceneObstacles = static_cast<Icefield*>(map)->getObstacles();
    qDebug() << "Found" << sceneObstacles.size() << "obstacles to visualize";

    for (int i = 0; i < sceneObstacles.size(); ++i)
    {
        const Obstacle& obstacle = sceneObstacles[i];

        // 为不同类型的障碍物使用不同颜色
        QPen obstaclePen;
        if (obstacle.type == ObstacleType::Rectangle)
        {
            obstaclePen = QPen(Qt::red, 3, Qt::SolidLine);
        }
        else
        {
            obstaclePen = QPen(QColorConstants::Svg::orange, 3, Qt::SolidLine);
        }

        // 创建障碍物边界矩形
        QGraphicsRectItem* obstacleRect = new QGraphicsRectItem(obstacle.bounds);
        obstacleRect->setPen(obstaclePen);
        obstacleRect->setBrush(QBrush(QColor(255, 0, 0, 50))); // 半透明红色填充
        obstacleRect->setZValue(150); // 在场景元素之上，但在角色调试信息之下
        addItem(obstacleRect);
        debugItems.append(obstacleRect);

        // 添加障碍物标签
        QGraphicsTextItem* obstacleLabel = new QGraphicsTextItem(QString("Obstacle %1\n(%2,%3)\n%4x%5")
                                                                     .arg(i)
                                                                     .arg(obstacle.bounds.x(), 0, 'f', 1)
                                                                     .arg(obstacle.bounds.y(), 0, 'f', 1)
                                                                     .arg(obstacle.bounds.width(), 0, 'f', 1)
                                                                     .arg(obstacle.bounds.height(), 0, 'f', 1));
        obstacleLabel->setPos(obstacle.bounds.topLeft() + QPointF(5, 5));
        obstacleLabel->setDefaultTextColor(Qt::red);
        obstacleLabel->setZValue(151);
        QFont obstacleFont = obstacleLabel->font();
        obstacleFont.setPointSize(8);
        obstacleFont.setBold(true);
        obstacleLabel->setFont(obstacleFont);
        addItem(obstacleLabel);
        debugItems.append(obstacleLabel);

        qDebug() << "Visualized obstacle" << i << "at" << obstacle.bounds;
    }

    // 4. 显示文字说明
    QString instructions = "Debug Visualization:\n"
                           "- Magenta dashed: Scene boundary\n"
                           "- Green dotted: Floor line\n"
                           "- Red rectangles: Obstacles\n"
                           "- Cyan box: Player1 (WASD move, J shoot)\n"
                           "- Yellow box: Player2 (Arrow keys move, 0 shoot)\n"
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
    textBg->setBrush(QBrush(QColor(0, 0, 0, 180)));
    textBg->setPen(Qt::NoPen);
    textBg->setZValue(201);
    addItem(textBg);
    debugItems.append(textBg);

    addItem(instructionText);
    debugItems.append(instructionText);
}

void IceScene::updateDebugVisualization()
{
    // 移除旧的动态调试项（角色框和信息）
    auto it = debugItems.begin();
    while (it != debugItems.end())
    {
        auto item = *it;
        // 检查是否是角色相关的调试项（通过Z值判断）
        if (item && item->zValue() >= 160 && item->zValue() <= 162)
        {
            removeItem(item);
            delete item;
            it = debugItems.erase(it);
        } else {
            ++it;
        }
    }

    // 添加新的动态调试项
    if (player1) {
        QPointF p1Pos = player1->pos();

        // 头部碰撞框
        QRectF p1HeadRect = player1->getHeadCollisionRect();
        p1HeadRect.moveTopLeft(p1Pos + p1HeadRect.topLeft());

        QGraphicsRectItem* p1HeadBox = new QGraphicsRectItem(p1HeadRect);
        p1HeadBox->setPen(QPen(Qt::cyan, 2));
        p1HeadBox->setBrush(QBrush(QColor(0, 255, 255, 30))); // 半透明青色
        p1HeadBox->setZValue(160);
        addItem(p1HeadBox);
        debugItems.append(p1HeadBox);

        // 身体碰撞框
        QRectF p1BodyRect = player1->getBodyCollisionRect();
        p1BodyRect.moveTopLeft(p1Pos + p1BodyRect.topLeft());

        QGraphicsRectItem* p1BodyBox = new QGraphicsRectItem(p1BodyRect);
        p1BodyBox->setPen(QPen(Qt::darkCyan, 2));
        p1BodyBox->setBrush(QBrush(QColor(0, 200, 200, 30))); // 半透明深青色
        p1BodyBox->setZValue(160);
        addItem(p1BodyBox);
        debugItems.append(p1BodyBox);

        // 角色中心点
        QGraphicsEllipseItem* p1Center = new QGraphicsEllipseItem(p1Pos.x() - 3, p1Pos.y() - 3, 6, 6);
        p1Center->setBrush(Qt::cyan);
        p1Center->setZValue(161);
        addItem(p1Center);
        debugItems.append(p1Center);

        // 角色信息
        QString p1Info = QString("Player1\nPos: (%1, %2)\nVel: (%3, %4)\nOnGround: %5\nHead: (%6,%7)\nBody: (%8,%9)")
                             .arg(p1Pos.x(), 0, 'f', 1)
                             .arg(p1Pos.y(), 0, 'f', 1)
                             .arg(player1->getVelocity().x(), 0, 'f', 2)
                             .arg(player1->getVelocity_y(), 0, 'f', 2)
                             .arg(player1->isOnGround() ? "Yes" : "No")
                             .arg(p1HeadRect.x(), 0, 'f', 1).arg(p1HeadRect.y(), 0, 'f', 1)
                             .arg(p1BodyRect.x(), 0, 'f', 1).arg(p1BodyRect.y(), 0, 'f', 1);

        QGraphicsTextItem* p1Label = new QGraphicsTextItem(p1Info);
        p1Label->setPos(p1Pos.x() + 50, p1Pos.y() - 120);
        p1Label->setDefaultTextColor(Qt::cyan);
        p1Label->setZValue(162);
        QFont infoFont = p1Label->font();
        infoFont.setPointSize(8);
        p1Label->setFont(infoFont);
        addItem(p1Label);
        debugItems.append(p1Label);
    }

    // 对player2执行相同操作
    if (player2) {
        QPointF p2Pos = player2->pos();

        // 头部碰撞框
        QRectF p2HeadRect = player2->getHeadCollisionRect();
        p2HeadRect.moveTopLeft(p2Pos + p2HeadRect.topLeft());

        QGraphicsRectItem* p2HeadBox = new QGraphicsRectItem(p2HeadRect);
        p2HeadBox->setPen(QPen(Qt::yellow, 2));
        p2HeadBox->setBrush(QBrush(QColor(255, 255, 0, 30))); // 半透明黄色
        p2HeadBox->setZValue(160);
        addItem(p2HeadBox);
        debugItems.append(p2HeadBox);

        // 身体碰撞框
        QRectF p2BodyRect = player2->getBodyCollisionRect();
        p2BodyRect.moveTopLeft(p2Pos + p2BodyRect.topLeft());

        QGraphicsRectItem* p2BodyBox = new QGraphicsRectItem(p2BodyRect);
        p2BodyBox->setPen(QPen(QColor(200, 200, 0), 2));
        p2BodyBox->setBrush(QBrush(QColor(200, 200, 0, 30))); // 半透明深黄色
        p2BodyBox->setZValue(160);
        addItem(p2BodyBox);
        debugItems.append(p2BodyBox);

        QGraphicsEllipseItem* p2Center = new QGraphicsEllipseItem(p2Pos.x() - 3, p2Pos.y() - 3, 6, 6);
        p2Center->setBrush(Qt::yellow);
        p2Center->setZValue(161);
        addItem(p2Center);
        debugItems.append(p2Center);

        QString p2Info = QString("Player2\nPos: (%1, %2)\nVel: (%3, %4)\nOnGround: %5\nHead: (%6,%7)\nBody: (%8,%9)")
                             .arg(p2Pos.x(), 0, 'f', 1)
                             .arg(p2Pos.y(), 0, 'f', 1)
                             .arg(player2->getVelocity().x(), 0, 'f', 2)
                             .arg(player2->getVelocity_y(), 0, 'f', 2)
                             .arg(player2->isOnGround() ? "Yes" : "No")
                             .arg(p2HeadRect.x(), 0, 'f', 1).arg(p2HeadRect.y(), 0, 'f', 1)
                             .arg(p2BodyRect.x(), 0, 'f', 1).arg(p2BodyRect.y(), 0, 'f', 1);

        QGraphicsTextItem* p2Label = new QGraphicsTextItem(p2Info);
        p2Label->setPos(p2Pos.x() - 200, p2Pos.y() - 120);
        p2Label->setDefaultTextColor(Qt::yellow);
        p2Label->setZValue(162);
        QFont infoFont = p2Label->font();
        infoFont.setPointSize(8);
        p2Label->setFont(infoFont);
        addItem(p2Label);
        debugItems.append(p2Label);
    }
}

void IceScene::hideDebugVisualization()
{
    // 安全地隐藏所有调试项
    for (auto item : std::as_const(debugItems))
    {
        if (item)
        {
            // 检查item是否有效且在场景中
            item->setVisible(false);
        }
    }
}
