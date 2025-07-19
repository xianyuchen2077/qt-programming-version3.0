#include <QDebug>
#include "IceScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Icefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/HeadEquipments/Helmet_of_the_Paladin.h"
#include "../Items/LegEquipments/LegEquipment.h"
#include "../Items/Weapons/Shabby_Pistol.h"
#include "../Items/Weapons/SolidBall.h"
#include "../Items/Weapons/Sniper_Rifle.h"
#include "../Items/Supplies/Bandage.h"

// IceScene 构造函数
IceScene::IceScene(QObject *parent) : Scene(parent)
{
    setSceneRect(0, 0, 1280, 720);

    isGameOver=false;

    map = new Icefield();
    map->scaleToFitScene(this);

    player1 = new Link();
    player2 = new Link();
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

    // 初始化武器装备补给
    spareWeapon1 = new Shabby_Pistol();
    spareWeapon2 = new SolidBall();
    spareWeapon3 = new Sniper_Rifle();
    spareArmor = new FlamebreakerArmor();
    spareHeadEquipment = new HelmetOfThePaladin();
    spareMedicalItem1 = new Bandage();

    addItem(map);
    addItem(player1);
    addItem(player2);
    addItem(spareWeapon1);
    addItem(spareWeapon2);
    addItem(spareWeapon3);
    addItem(spareArmor);
    addItem(spareHeadEquipment);
    addItem(spareMedicalItem1);

    // 设置备用武器装备补给的位置和状态
    spareWeapon1->unmount();
    spareWeapon1->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.45, floorHeight);
    spareWeapon1->setZValue(5);
    spareWeapon2->unmount();
    spareWeapon2->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.65, floorHeight);
    spareWeapon2->setZValue(5);
    spareWeapon3->unmount();
    spareWeapon3->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.55, floorHeight);
    spareWeapon3->setZValue(5);
    spareArmor->unmount();
    spareArmor->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.75, floorHeight);
    spareArmor->setZValue(5);
    spareHeadEquipment->unmount();
    spareHeadEquipment->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.25, floorHeight);
    spareHeadEquipment->setZValue(5);
    spareMedicalItem1->setPos(sceneRect().left() + (sceneRect().right() - sceneRect().left()) * 0.35, floorHeight);
    spareMedicalItem1->setZValue(5);

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
    platforms.clear();

    // 添加悬空的冰平台（单向平台，只能从上方站立）
    // 对应ice_platform在Icefield.cpp中的位置: setPos(300, 270), setScale(0.7)
    // 原始ice_platform图片假设为200x100像素
    qreal platformWidth = 200 * 0.7;  // 缩放后的宽度
    qreal platformHeight = 20;         // 平台的实际可站立高度
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

// 地面碰撞检测(过渡函数，现在可以删了）
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

    // 第一步：处理头部被卡住的情况（加快滑动速度）
    bool headStuck = isHeadStuckInObstacle(character, newPos);
    bool wasHeadStuck = isHeadStuckInObstacle(character, oldPos);

    if (headStuck || wasHeadStuck)
    {
        qDebug() << "Head stuck detected, applying slide logic";

        // 计算滑动方向
        QPointF slideDirection = calculateSlideDirection(character, newPos);

        if (slideDirection.x() != 0)
        {
            QPointF slidePos = newPos + QPointF(slideDirection.x() * 0.5, 0);

            if (isPositionSafe(character, slidePos))
            {
                newPos.setX(slidePos.x());
                qDebug() << "Applied fast slide to X:" << slidePos.x();
            }
            else
            {
                slidePos = newPos + QPointF(slideDirection.x() * 0.3, 0);
                if (isPositionSafe(character, slidePos))
                {
                    newPos.setX(slidePos.x());
                    qDebug() << "Applied medium slide to X:" << slidePos.x();
                }
                else
                {
                    slidePos = newPos + QPointF(slideDirection.x() * 0.1, 0);
                    if (isPositionSafe(character, slidePos))
                    {
                        newPos.setX(slidePos.x());
                        qDebug() << "Applied slow slide to X:" << slidePos.x();
                    }
                }
            }
        }

        qreal fallDistance = qAbs(headRect.bottom() - bodyRect.top()) * 0.3;
        newPos.setY(newPos.y() + fallDistance);
        qDebug() << "Applied fall distance:" << fallDistance;
    }

    // 第二步：改进的水平碰撞检测
    QPointF testPosX = QPointF(newPos.x(), oldPos.y());

    bool headHorizontalCollision = checkHeadHorizontalCollision(character, testPosX);
    bool bodyHorizontalCollision = checkBodyObstacleCollision(character, testPosX);

    if (headHorizontalCollision || bodyHorizontalCollision)
    {
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

    // 第三步：改进的垂直碰撞检测
    QPointF testPosY = QPointF(newPos.x(), newPos.y());
    bool verticalCollision = false;
    bool isOnAnyPlatform = false;

    // 地面碰撞检测
    qreal floorHeight = map->getFloorHeight();
    qreal bodyBottom = testPosY.y() + bodyRect.bottom();

    if (bodyBottom >= floorHeight)
    {
        newPos.setY(floorHeight - bodyRect.bottom());
        character->setVelocity_y(0);
        character->setOnGround(true);
        verticalCollision = true;
        isOnAnyPlatform = true;
        qDebug() << "Character body landed on ground";
    }

    // 平台碰撞检测
    if (!verticalCollision)
    {
        const QList<Obstacle>& obstacles = static_cast<Icefield*>(map)->getObstacles();

        for (const Obstacle& obstacle : obstacles)
        {
            // 计算角色身体在新位置的碰撞框
            QRectF testBodyRect = bodyRect;
            testBodyRect.moveTopLeft(testPosY + bodyRect.topLeft());

            // 计算角色头部在新位置的碰撞框
            QRectF testHeadRect = headRect;
            testHeadRect.moveTopLeft(testPosY + headRect.topLeft());

            if (obstacle.bounds.intersects(testBodyRect) || obstacle.bounds.intersects(testHeadRect))
            {
                // 计算角色在旧位置的碰撞框
                QRectF oldBodyRect = bodyRect;
                oldBodyRect.moveTopLeft(oldPos + bodyRect.topLeft());

                QRectF oldHeadRect = headRect;
                oldHeadRect.moveTopLeft(oldPos + headRect.topLeft());

                // 情况1：从上方落在平台上
                if (character->getVelocity_y() > 0) // 正在下落
                {
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

                // 情况2：从下方向上撞击平台
                else if (character->getVelocity_y() < 0) // 正在上升
                {
                    // 检查是否是从下方撞击
                    qreal headOldTop = oldPos.y() + headRect.top();
                    qreal headNewTop = testPosY.y() + headRect.top();

                    // 如果角色头部从障碍物下方撞到了障碍物底部
                    if (headOldTop >= obstacle.bounds.bottom() - 5 && // 之前在障碍物下方
                        headNewTop < obstacle.bounds.bottom()) // 现在撞到了障碍物
                    {
                        // 阻止向上穿透，将角色头部位置设置为障碍物底部下方
                        newPos.setY(obstacle.bounds.bottom() - headRect.top());
                        character->setVelocity_y(0); // 停止向上移动
                        verticalCollision = true;
                        qDebug() << "Character head hit platform from below, stopped at Y:" << newPos.y();
                        break;
                    }
                }

                // 情况3：侧面碰撞或其他情况
                if (!verticalCollision)
                {
                    // 如果不是从上方落下或从下方撞击，则恢复到旧位置
                    newPos.setY(oldPos.y());
                    if (character->getVelocity_y() < 0) // 如果是向上移动被阻挡
                    {
                        character->setVelocity_y(0);
                    }
                    qDebug() << "Side collision with platform detected, position restored";
                    break;
                }
            }
        }
    }

    // 第四步：检查角色是否仍在平台上
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

    // 第五步：边界检测
    handleBoundaryCollision(character, newPos);

    // 第六步：最终安全检查
    if (!isPositionSafe(character, newPos))
    {
        qDebug() << "Final position unsafe, adjusting...";

        // 尝试水平调整
        for (int i = 1; i <= 10; i++)
        {
            QPointF adjustedPos = newPos + QPointF(i * 5, 0);
            if (isPositionSafe(character, adjustedPos))
            {
                newPos = adjustedPos;
                qDebug() << "Adjusted position to:" << newPos;
                break;
            }

            adjustedPos = newPos + QPointF(-i * 5, 0);
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
                QPointF adjustedPos = newPos + QPointF(0, -i * 3);
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

    // 调用场景中所有项目的 advance 方法来更新子弹位置和碰撞
    QList<QGraphicsItem*> sceneItems = items();
    for (QGraphicsItem* item : sceneItems)
    {
        if (Bullet* bullet = dynamic_cast<Bullet*>(item))
        {
            bullet->advance(0); // 更新子弹
        }
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


    // 更新激光瞄准线
    updateLaserSights();

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

// 更新场景逻辑
void IceScene::update()
{
    // 如果游戏已结束，不再执行更新逻辑
    if (isGameOver)
        return;

    // 调用基类的 update，更新其他非移动相关的逻辑
    Scene::update();

    if (checkGameEndCondition())
    {
        handleGameEnd();
    }
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

        // 1. 首先处理跳跃逻辑（在移动之前）
        if (player1->isUpDown() && player1->isOnGround())
        {
            player1->handleJump();
            qDebug() << "Player1 jumped! Velocity_y:" << player1->getVelocity_y();
        }

        // 2. 应用重力（如果不在地面上）
        if (!player1->isOnGround())
        {
            player1->setVelocity_y(player1->getVelocity_y() + gravity * deltaTime);
            if (player1->getVelocity_y() > maxFallSpeed)
            {
                player1->setVelocity_y(maxFallSpeed);
            }
        }

        // 3. 计算新位置（水平+垂直）
        // 水平移动
        newPos.setX(newPos.x() + player1->getVelocity().x() * deltaTime);
        // 垂直移动
        newPos.setY(newPos.y() + player1->getVelocity_y() * deltaTime);

        // 4. 完整障碍物检测和位置调整
        handleAllCollisions(player1, newPos);

        // 5. 应用地图效果（摩擦力等）
        static_cast<Icefield*>(map)->applyEffectToCharacter(player1, deltaTime);
    }

    // 处理 player2 (相同逻辑)
    if (player2 != nullptr)
    {
        QPointF oldPos = player2->pos();
        QPointF newPos = oldPos;

        // 1. 首先处理跳跃逻辑（在移动之前）
        if (player2->isUpDown() && player2->isOnGround())
        {
            player2->handleJump();
            qDebug() << "Player2 jumped! Velocity_y:" << player2->getVelocity_y();
        }

        // 2. 应用重力（如果不在地面上）
        if (!player2->isOnGround())
        {
            player2->setVelocity_y(player2->getVelocity_y() + gravity * deltaTime);
            if (player2->getVelocity_y() > maxFallSpeed)
            {
                player2->setVelocity_y(maxFallSpeed);
            }
        }

        // 3. 计算新位置（水平+垂直）
        // 水平移动
        newPos.setX(newPos.x() + player2->getVelocity().x() * deltaTime);
        // 垂直移动
        newPos.setY(newPos.y() + player2->getVelocity_y() * deltaTime);

        // 4. 完整障碍物检测和位置调整
        handleAllCollisions(player2, newPos);

        // 5. 应用地图效果（摩擦力等）
        static_cast<Icefield*>(map)->applyEffectToCharacter(player2, deltaTime);
    }
}

// 处理角色的拾取
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
                // QPointF itemCenter = itemBounds.center();

                // 计算两个中心点之间的距离
                qreal distance = QLineF(characterCenter, itemCenter).length();

                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearest = mountable;
                }

                // 调试输出
                if (auto weapon = dynamic_cast<Weapon*>(item))
                {
                    qDebug() << "Checking weapon:" << weapon->getWeaponName();
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
    else if (auto medicalItem = dynamic_cast<MedicalItem *>(mountable))
    {
        return character->pickupMedicalItem(medicalItem);
    }
    return nullptr;
}

// 判断游戏是否结束
bool IceScene::checkGameEndCondition()
{
    // 任何一个玩家的生命值 <= 0 则游戏结束
    if (player1->getHealth() <= 0)
    {
        gameResultText = "Player 2 Wins!"; //+ 设置游戏结果文本
        return true;
    }
    if (player2->getHealth() <= 0)
    {
        gameResultText = "Player 1 Wins!"; //+ 设置游戏结果文本
        return true;
    }

    return false;
}

// 处理游戏结束逻辑
void IceScene::handleGameEnd()
{
    if (isGameOver)
        return; // 避免重复处理
    isGameOver = true; // 标记游戏已结束

    qDebug() << "Game Over! Result:" << gameResultText;

    // // 发送信号请求 MyGame 切换场景到 GameOverScene
    // emit requestSceneChange(SceneID::GameOverScene_ID);
}

// 更新（狙击枪）激光瞄准线
void IceScene::updateLaserSights()
{
    // 更新 player1 的激光瞄准线
    if (player1 && player1->getWeapon())
    {
        Sniper_Rifle* sniperRifle = dynamic_cast<Sniper_Rifle*>(player1->getWeapon());
        if (sniperRifle)
        {
            QPointF shooterPos = player1->pos();
            QRectF bodyRect = player1->getBodyCollisionRect();
            QPointF bodyCenter = shooterPos + bodyRect.center();
            QPointF gunPos = bodyCenter + QPointF(player1->isFaceRight() ? 30 : -30, -10);
            QPointF direction = player1->isFaceRight() ? QPointF(1, 0) : QPointF(-1, 0);

            sniperRifle->updateLaserSight(gunPos, direction);
        }
    }

    // 更新 player2 的激光瞄准线
    if (player2 && player2->getWeapon())
    {
        Sniper_Rifle* sniperRifle = dynamic_cast<Sniper_Rifle*>(player2->getWeapon());
        if (sniperRifle)
        {
            QPointF shooterPos = player2->pos();
            QRectF bodyRect = player2->getBodyCollisionRect();
            QPointF bodyCenter = shooterPos + bodyRect.center();
            QPointF gunPos = bodyCenter + QPointF(player2->isFaceRight() ? 30 : -30, -10);
            QPointF direction = player2->isFaceRight() ? QPointF(1, 0) : QPointF(-1, 0);

            sniperRifle->updateLaserSight(gunPos, direction);
        }
    }
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
        if (item && item->zValue() >= 160 && item->zValue() <= 170)
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
        addCharacterDebugVisualization(player1, Qt::cyan, QColor(0, 255, 255, 30), "Player1", 50);
    }

    if (player2) {
        addCharacterDebugVisualization(player2, Qt::yellow, QColor(255, 255, 0, 30), "Player2", -200);
    }

    // 添加全局状态信息
    addGlobalDebugInfo();
}

void IceScene::addCharacterDebugVisualization(Character* character, QColor primaryColor, QColor fillColor, const QString& name, qreal labelOffsetX)
{
    if (!character) return;

    QPointF charPos = character->pos();
    QPointF velocity = character->getVelocity();
    qreal velocityY = character->getVelocity_y();

    // 1. 头部碰撞框
    QRectF headRect = character->getHeadCollisionRect();
    headRect.moveTopLeft(charPos + headRect.topLeft());

    QGraphicsRectItem* headBox = new QGraphicsRectItem(headRect);
    headBox->setPen(QPen(primaryColor, 2));
    headBox->setBrush(QBrush(fillColor));
    headBox->setZValue(160);
    addItem(headBox);
    debugItems.append(headBox);

    // 2. 身体碰撞框
    QRectF bodyRect = character->getBodyCollisionRect();
    bodyRect.moveTopLeft(charPos + bodyRect.topLeft());

    QGraphicsRectItem* bodyBox = new QGraphicsRectItem(bodyRect);
    QColor darkPrimary = primaryColor.darker(120);
    bodyBox->setPen(QPen(darkPrimary, 2));
    bodyBox->setBrush(QBrush(QColor(fillColor.red(), fillColor.green(), fillColor.blue(), 40)));
    bodyBox->setZValue(160);
    addItem(bodyBox);
    debugItems.append(bodyBox);

    // 3. 角色中心点
    QGraphicsEllipseItem* center = new QGraphicsEllipseItem(charPos.x() - 3, charPos.y() - 3, 6, 6);
    center->setBrush(primaryColor);
    center->setZValue(161);
    addItem(center);
    debugItems.append(center);

    // 4. 速度向量可视化
    if (qAbs(velocity.x()) > 0.01 || qAbs(velocityY) > 0.01)
    {
        // 水平速度向量
        if (qAbs(velocity.x()) > 0.01)
        {
            qreal arrowLength = velocity.x() * 50; // 放大50倍显示
            QGraphicsLineItem* velocityArrowX = new QGraphicsLineItem(
                charPos.x(), charPos.y(),
                charPos.x() + arrowLength, charPos.y()
                );
            velocityArrowX->setPen(QPen(Qt::red, 3));
            velocityArrowX->setZValue(162);
            addItem(velocityArrowX);
            debugItems.append(velocityArrowX);

            // 箭头头部
            QGraphicsPolygonItem* arrowHead = new QGraphicsPolygonItem();
            QPolygonF arrowPoly;
            qreal headSize = 8;
            if (arrowLength > 0) {
                arrowPoly << QPointF(charPos.x() + arrowLength, charPos.y())
                << QPointF(charPos.x() + arrowLength - headSize, charPos.y() - headSize/2)
                << QPointF(charPos.x() + arrowLength - headSize, charPos.y() + headSize/2);
            } else {
                arrowPoly << QPointF(charPos.x() + arrowLength, charPos.y())
                << QPointF(charPos.x() + arrowLength + headSize, charPos.y() - headSize/2)
                << QPointF(charPos.x() + arrowLength + headSize, charPos.y() + headSize/2);
            }
            arrowHead->setPolygon(arrowPoly);
            arrowHead->setBrush(Qt::red);
            arrowHead->setZValue(162);
            addItem(arrowHead);
            debugItems.append(arrowHead);
        }

        // 垂直速度向量
        if (qAbs(velocityY) > 0.01)
        {
            qreal arrowLength = velocityY * 50; // 放大50倍显示
            QGraphicsLineItem* velocityArrowY = new QGraphicsLineItem(
                charPos.x() + 15, charPos.y(),
                charPos.x() + 15, charPos.y() + arrowLength
                );
            velocityArrowY->setPen(QPen(Qt::blue, 3));
            velocityArrowY->setZValue(162);
            addItem(velocityArrowY);
            debugItems.append(velocityArrowY);

            // 箭头头部
            QGraphicsPolygonItem* arrowHead = new QGraphicsPolygonItem();
            QPolygonF arrowPoly;
            qreal headSize = 8;
            if (arrowLength > 0) {
                arrowPoly << QPointF(charPos.x() + 15, charPos.y() + arrowLength)
                << QPointF(charPos.x() + 15 - headSize/2, charPos.y() + arrowLength - headSize)
                << QPointF(charPos.x() + 15 + headSize/2, charPos.y() + arrowLength - headSize);
            } else {
                arrowPoly << QPointF(charPos.x() + 15, charPos.y() + arrowLength)
                << QPointF(charPos.x() + 15 - headSize/2, charPos.y() + arrowLength + headSize)
                << QPointF(charPos.x() + 15 + headSize/2, charPos.y() + arrowLength + headSize);
            }
            arrowHead->setPolygon(arrowPoly);
            arrowHead->setBrush(Qt::blue);
            arrowHead->setZValue(162);
            addItem(arrowHead);
            debugItems.append(arrowHead);
        }
    }

    // 5. 状态指示器
    QString statusIndicator = "";
    QColor statusColor = Qt::white;

    if (character->isOnGround()) {
        statusIndicator += "G";  // Ground
        statusColor = Qt::green;
    } else {
        statusIndicator += "A";  // Air
        statusColor = Qt::red;
    }

    if (character->isLeftDown()) statusIndicator += "L";
    if (character->isRightDown()) statusIndicator += "R";
    if (character->isUpDown()) statusIndicator += "U";
    if (character->isDownDown()) statusIndicator += "D";
    if (character->isPicking()) statusIndicator += "P";

    QGraphicsTextItem* statusText = new QGraphicsTextItem(statusIndicator);
    statusText->setPos(charPos.x() - 10, charPos.y() - 150);
    statusText->setDefaultTextColor(statusColor);
    statusText->setZValue(163);
    QFont statusFont = statusText->font();
    statusFont.setPointSize(12);
    statusFont.setBold(true);
    statusText->setFont(statusFont);
    addItem(statusText);
    debugItems.append(statusText);

    // 6. 详细信息面板
    QString detailInfo = QString("%1\nPos: (%2, %3)\nVel: (%4, %5)\nVel_Y: %6\nOnGround: %7\nHealth: %8\nHead: (%9,%10) %11x%12\nBody: (%13,%14) %15x%16")
                             .arg(name)
                             .arg(charPos.x(), 0, 'f', 1)
                             .arg(charPos.y(), 0, 'f', 1)
                             .arg(velocity.x(), 0, 'f', 3)
                             .arg(velocity.y(), 0, 'f', 3)
                             .arg(velocityY, 0, 'f', 3)
                             .arg(character->isOnGround() ? "Yes" : "No")
                             .arg(character->getHealth())
                             .arg(headRect.x(), 0, 'f', 1).arg(headRect.y(), 0, 'f', 1)
                             .arg(headRect.width(), 0, 'f', 1).arg(headRect.height(), 0, 'f', 1)
                             .arg(bodyRect.x(), 0, 'f', 1).arg(bodyRect.y(), 0, 'f', 1)
                             .arg(bodyRect.width(), 0, 'f', 1).arg(bodyRect.height(), 0, 'f', 1);

    QGraphicsTextItem* infoLabel = new QGraphicsTextItem(detailInfo);
    infoLabel->setPos(charPos.x() + labelOffsetX, charPos.y() - 120);
    infoLabel->setDefaultTextColor(primaryColor);
    infoLabel->setZValue(165);
    QFont infoFont = infoLabel->font();
    infoFont.setPointSize(8);
    infoLabel->setFont(infoFont);

    // 添加信息面板背景
    QRectF infoRect = infoLabel->boundingRect();
    QGraphicsRectItem* infoBg = new QGraphicsRectItem(infoRect);
    infoBg->setPos(infoLabel->pos());
    infoBg->setBrush(QBrush(QColor(0, 0, 0, 150)));
    infoBg->setPen(QPen(primaryColor, 1));
    infoBg->setZValue(164);
    addItem(infoBg);
    debugItems.append(infoBg);

    addItem(infoLabel);
    debugItems.append(infoLabel);
}

void IceScene::addGlobalDebugInfo()
{
    // 当前帧率和时间信息
    qint64 currentTime = elapsedTimer.elapsed();
    qreal fps = (lastFrameTime > 0) ? (1000.0 / (currentTime - lastFrameTime + 1)) : 0;

    QString globalInfo = QString("=== Game State ===\nFPS: %1\nFrame Time: %2ms\nMap Type: %3\nObstacles: %4\nFloor Height: %5\n\n=== Controls ===\nPlayer1: WASD + J(shoot)\nPlayer2: Arrows + 0(shoot)\nH: Toggle debug\nS/Down: Pick items")
                             .arg(fps, 0, 'f', 1)
                             .arg(currentTime - lastFrameTime)
                             .arg(static_cast<Icefield*>(map)->getMapType() == 0 ? "White Ice" : "Purple Ice")
                             .arg(static_cast<Icefield*>(map)->getObstacles().size())
                             .arg(map->getFloorHeight(), 0, 'f', 1);

    QGraphicsTextItem* globalLabel = new QGraphicsTextItem(globalInfo);
    globalLabel->setPos(10, 250);
    globalLabel->setDefaultTextColor(Qt::white);
    globalLabel->setZValue(170);
    QFont globalFont = globalLabel->font();
    globalFont.setPointSize(9);
    globalFont.setBold(true);
    globalLabel->setFont(globalFont);

    // 添加全局信息背景
    QRectF globalRect = globalLabel->boundingRect();
    QGraphicsRectItem* globalBg = new QGraphicsRectItem(globalRect);
    globalBg->setPos(globalLabel->pos());
    globalBg->setBrush(QBrush(QColor(0, 0, 0, 180)));
    globalBg->setPen(QPen(Qt::white, 2));
    globalBg->setZValue(169);
    addItem(globalBg);
    debugItems.append(globalBg);

    addItem(globalLabel);
    debugItems.append(globalLabel);

    // 添加速度向量说明
    QString velocityLegend = "Velocity Vectors:\nRed Arrow = Horizontal\nBlue Arrow = Vertical\nStatus: G=Ground A=Air\nL/R/U/D=Keys P=Picking";

    QGraphicsTextItem* legendLabel = new QGraphicsTextItem(velocityLegend);
    legendLabel->setPos(sceneRect().width() - 250, 250);
    legendLabel->setDefaultTextColor(Qt::lightGray);
    legendLabel->setZValue(170);
    QFont legendFont = legendLabel->font();
    legendFont.setPointSize(8);
    legendLabel->setFont(legendFont);

    QRectF legendRect = legendLabel->boundingRect();
    QGraphicsRectItem* legendBg = new QGraphicsRectItem(legendRect);
    legendBg->setPos(legendLabel->pos());
    legendBg->setBrush(QBrush(QColor(0, 0, 0, 150)));
    legendBg->setPen(QPen(Qt::lightGray, 1));
    legendBg->setZValue(169);
    addItem(legendBg);
    debugItems.append(legendBg);

    addItem(legendLabel);
    debugItems.append(legendLabel);
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
