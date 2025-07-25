#include "Icefield.h"
#include <random>

Icefield::Icefield(QGraphicsItem *parent): Map(parent,""), maptype(0), m_currentMode(MapMode::Icefield)
{
    setupMapResources(); // 设置地图资源
    updateGroundGeometry(); // 初始化地面几何
}

void Icefield::setupIcefieldObstacles()
{
    // 矩形障碍物1：icile_1
    QPixmap icicle_1_map(":/Items/Maps/Icefield/icicle_1.png");
    if (!icicle_1_map.isNull())
    {
        icicle_1 = new QGraphicsPixmapItem(icicle_1_map, this);
        icicle_1->setScale(0.55);
        icicle_1->setPos(0, 502);
        icicle_1->setZValue(0);
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, icicle_1->mapToScene(icicle_1->boundingRect()).boundingRect()));
    }

    // 斜坡障碍物：icile_2
    QPixmap icicle_2_map(":/Items/Maps/Icefield/icicle_2.png");
    if (!icicle_2_map.isNull())
    {
        icicle_2 = new QGraphicsPixmapItem(icicle_2_map, this);
        icicle_2->setScale(1);
        icicle_2->setPos(1100, 517);
        icicle_2->setZValue(0);
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, icicle_2->mapToScene(icicle_2->boundingRect()).boundingRect()));
    }

    // 空中大平台：ice_platform
    QPixmap ice_platform_map(":/Items/Maps/Icefield/ice_platform.png");
    if (!ice_platform_map.isNull())
    {
        ice_platform = new QGraphicsPixmapItem(ice_platform_map, this);
        ice_platform->setScale(0.65);
        ice_platform->setPos(305, 270);
        ice_platform->setZValue(0);

        QRectF platformRectLocal = ice_platform->boundingRect();
        platformRectLocal.setWidth(platformRectLocal.width() * ice_platform->scale());
        platformRectLocal.setHeight(platformRectLocal.height() * ice_platform->scale()-15);
        platformRectLocal.moveTo(ice_platform->pos().x(), ice_platform->pos().y()+20);
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, platformRectLocal));
    }
}

void Icefield::setupGrasslandObstacles()
{
    // 草地障碍物1：岩石
    QPixmap tree_1_map(":/Items/Maps/Icefield/rock_1.png");
    if (!tree_1_map.isNull())
    {
        icicle_1 = new QGraphicsPixmapItem(tree_1_map, this); // 重用变量名
        icicle_1->setScale(0.55);
        icicle_1->setPos(0, 502);
        icicle_1->setZValue(0);
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, icicle_1->mapToScene(icicle_1->boundingRect()).boundingRect()));
    }

    // 草地障碍物2：大石头
    QPixmap rock_map(":/Items/Maps/Icefield/rock_2.png");
    if (!rock_map.isNull())
    {
        icicle_2 = new QGraphicsPixmapItem(rock_map, this);
        icicle_2->setScale(1);
        icicle_2->setPos(1100, 517);
        icicle_2->setZValue(0);
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, icicle_2->mapToScene(icicle_2->boundingRect()).boundingRect()));
    }

    // 草地平台：木制平台
    QPixmap wood_platform_map(":/Items/Maps/Icefield/grass_platform.png");
    if (!wood_platform_map.isNull())
    {
        ice_platform = new QGraphicsPixmapItem(wood_platform_map, this);
        ice_platform->setScale(0.65);
        ice_platform->setPos(305, 270);
        ice_platform->setZValue(0);

        QRectF platformRectLocal = ice_platform->boundingRect();
        platformRectLocal.setWidth(platformRectLocal.width() * ice_platform->scale());
        platformRectLocal.setHeight(platformRectLocal.height() * ice_platform->scale()-15);
        platformRectLocal.moveTo(ice_platform->pos().x(), ice_platform->pos().y()+20);
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, platformRectLocal));
    }
}

void Icefield::setupMapResources()
{
    // 清理现有资源
    clearObstacles();

    if (m_currentMode == MapMode::Icefield)
    {
        // 冰原模式 - 使用原有逻辑
        QString path = selectRandomIcefieldPixmapPath(); // 随机生成冰原地图
        setPixmap(QString(path)); // 设置地图图片路径

        // 如果图片路径是":/Items/Maps/Icefield/Icefield_purple.png"，则maptype设置为1
        if(path == ":/Items/Maps/Icefield/Icefield_purple.png")
        {
            maptype = 1; // 紫色冰原
        }
        else
        {
            maptype = 0; // 白色冰原
        }
        pixmapItem->setZValue(-1); // 设置Z轴值，确保地图背景在最底层

        // 设置冰原障碍物
        setupIcefieldObstacles();
    }
    else if (m_currentMode == MapMode::Grassland)
    {
        // 草地模式 - 设置草地背景
        setPixmap(QString(":/Items/Maps/Icefield/Icefield_green.png"));
        maptype = 2; // 设置为草地类型
        pixmapItem->setZValue(-1);

        // 设置草地障碍物
        setupGrasslandObstacles();
    }
}

void Icefield::clearObstacles()
{
    // 清理现有的障碍物图形项
    if (icicle_1)
    {
        icicle_1->setParentItem(nullptr);
        delete icicle_1;
        icicle_1 = nullptr;
    }
    if (icicle_2)
    {
        icicle_2->setParentItem(nullptr);
        delete icicle_2;
        icicle_2 = nullptr;
    }
    if (ice_platform)
    {
        ice_platform->setParentItem(nullptr);
        delete ice_platform;
        ice_platform = nullptr;
    }

    // 清空障碍物列表
    m_obstacles.clear();
}

void Icefield::switchToGrasslandMode()
{
    m_currentMode = MapMode::Grassland;
    setupMapResources();
    updateGroundGeometry();
}

void Icefield::switchToIcefieldMode()
{
    m_currentMode = MapMode::Icefield;
    setupMapResources();
    updateGroundGeometry();
}

bool Icefield::canCharacterHide(Character* character) const
{
    if (m_currentMode != MapMode::Grassland)
        return false;

    if (!character)
        return false;

    // 检查角色是否在地面上且处于下蹲状态
    return character->isOnGround() && character->isCrouching(); // 需要Character类有isCrouching()方法
}

// 随机选择地图图片路径
QString Icefield::selectRandomIcefieldPixmapPath()
{
    // 随机数生成器
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(0, 4);
    int randomNumber = distribution(generator);
    // 有20%的概率生成紫色冰原
    if (randomNumber == 0)
    {
        return ":/Items/Maps/Icefield/Icefield_purple.png";
    }
    else
    {
        return ":/Items/Maps/Icefield/Icefield_white.png";
    }
}

void Icefield::updateGroundGeometry()
{
    auto sceneRect = sceneBoundingRect();

    // 计算地面区域
    m_groundRect = QRectF(
        sceneRect.left() + sceneRect.width() * 0.05,     // 左边距
        sceneRect.bottom() - sceneRect.height() * 0.13,  // 距底部
        sceneRect.width() * 0.9,                         // 宽度
        sceneRect.height() * 0.13                        // 高度
        );

    // 设置地图边界（比可视区域小5%的安全边界）
    m_boundaryRect = sceneRect.adjusted(
        sceneRect.width() * 0.05,
        sceneRect.height() * 0.05,
        -sceneRect.width() * 0.05,
        -sceneRect.height() * 0.05
        );
}

qreal Icefield::getFloorHeight()
{
    updateGroundGeometry(); // 确保几何信息最新
    return m_groundRect.top(); // 返回地面顶部Y坐标
}

void Icefield::applyEffectToCharacter(Character *character, qint64 deltaTime)
{
    if (!character)
        return;

    const qreal ICEFIELD_BASE_SPEED = 0.3; // moveSpeed的初始值

    // 根据当前的地图模式设置角色的移动速度上限
    if (m_currentMode == MapMode::Grassland) // 当前是草地模式
    {
        // 草地模式下速度是冰原的一半
        character->setMoveSpeed(ICEFIELD_BASE_SPEED);
        qDebug() << "Character speed set to grassland mode:" << character->getMoveSpeed();
    }
    else if (m_currentMode == MapMode::Icefield) // 当前是冰原模式
    {
        // 冰原模式下使用基础速度
        character->setMoveSpeed(ICEFIELD_BASE_SPEED * 2.0);
        qDebug() << "Character speed set to icefield mode:" << character->getMoveSpeed();
    }

    updateGroundGeometry();

    if (character->isOnGround())
    {
        qreal frictionCoefficient;
        qreal speedMultiplier = 1.0;

        if (m_currentMode == MapMode::Grassland)
        {
            // 草地模式：高摩擦力，速度减半，无滑行
            frictionCoefficient = 1.0; // 很高的摩擦力，几乎立即停止
            speedMultiplier = 0.5; // 速度减半
        }
        else
        {
            // 冰原模式：原有逻辑
            frictionCoefficient = 0.15; // 白色冰原的摩擦力
            if (getMapType() == 1) // 紫色冰原
            {
                frictionCoefficient = 0.02; // 紫色冰原很滑
            }
        }

        QPointF currentVelocity = character->getVelocity();

        // 检查角色是否在主动移动
        bool isActivelyMoving = character->isLeftDown() || character->isRightDown();

        if (m_currentMode == MapMode::Grassland)
        {
            // 草地模式：直接应用速度限制和高摩擦
            if (isActivelyMoving)
            {
                // 限制最大速度
                qreal maxSpeed = 0.15; // 草地上的最大速度;
                if (qAbs(currentVelocity.x()) > maxSpeed)
                {
                    qreal sign = (currentVelocity.x() > 0) ? 1 : -1;
                    character->setVelocity(QPointF(maxSpeed * sign, currentVelocity.y()));
                }
            }
            else
            {
                // 草地上不滑行，立即停止
                character->setVelocity(QPointF(0, currentVelocity.y()));
            }
        }
        else
        {
            // 冰原模式摩擦力逻辑
            if (!isActivelyMoving)
            {
                qreal currentSpeedX = currentVelocity.x();

                if (qAbs(currentSpeedX) > 0.001)
                {
                    qreal frictionForce = -currentSpeedX * frictionCoefficient;
                    qreal newSpeedX = currentSpeedX + frictionForce;

                    if ((currentSpeedX > 0 && newSpeedX < 0) ||(currentSpeedX < 0 && newSpeedX > 0))
                    {
                        newSpeedX = 0;
                    }

                    character->setVelocity(QPointF(newSpeedX, currentVelocity.y()));
                }
            }
        }
    }

    // 处理隐身效果
    if (m_currentMode == MapMode::Grassland)
    {
        // ·······················只在地面上可以实现隐身效果················
        // 检查角色是否在真实地面上（而不是平台上）
        bool isOnRealGround = isCharacterOnRealGround(character);
        if (character->isCrouching() && character->isOnGround() && isOnRealGround)
        {
            character->setHidden(true);
            qDebug() << "Character hidden on real ground";
        }
        else
        {
            character->setHidden(false);
            if (!isOnRealGround && character->isCrouching())
            {
                qDebug() << "Character on platform, not hidden";
            }
        }

        // // ·······················在地面上和障碍物以及平台上可以实现隐身效果················
        // if (character->isCrouching() && character->isOnGround())
        // {
        //     character->setHidden(true);
        //     qDebug() << "Character hidden on ground";
        // }
        // else
        // {
        //     character->setHidden(false);
        //     qDebug() << "Character on platform, not hidden";
        // }
    }
    else
    {
        character->setHidden(false);
    }
}

// 人物是否在地上
bool Icefield::isCharacterOnRealGround(Character* character) const
{
    if (!character)
        return false;

    QPointF charPos = character->pos();
    QRectF bodyRect = character->getBodyCollisionRect();
    qreal characterBottomY = charPos.y() + bodyRect.bottom();
    qreal realGroundY = 626.4; // 真实地面高度;

    // 允许5像素的误差范围
    qreal tolerance = 5.0;

    // 如果角色底部接近真实地面高度，则认为在真实地面上
    if (qAbs(characterBottomY - realGroundY) <= tolerance)
    {
        return true;
    }

    return false;
}

// 获取地面矩形
QRectF Icefield::getGroundRect() const
{
    return m_groundRect;
}

// 获取边界矩形
QRectF Icefield::getBoundaryRect() const
{
    return m_boundaryRect;
}

// 获取所有障碍物列表
const QList<Obstacle>& Icefield::getObstacles() const
{
    return m_obstacles;
}
