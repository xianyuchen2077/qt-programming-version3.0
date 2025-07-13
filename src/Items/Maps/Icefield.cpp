#include "Icefield.h"
#include <random>

Icefield::Icefield(QGraphicsItem *parent): Map(parent,""),maptype(0)
{
    // 设置地图背景
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

    // 设置地图空间结构贴图
    // 矩形障碍物1：icile_1
    QPixmap icicle_1_map(":/Items/Maps/Icefield/icicle_1.png");
    if (!icicle_1_map.isNull())
    {
        // 创建 QGraphicsPixmapItem，并将其父项设置为 Icefield (this)
        icicle_1 = new QGraphicsPixmapItem(icicle_1_map, this);

        // 设置贴图相对于 Icefield 局部坐标系的位置,相对于 Icefield 的 (0,0) 点
        icicle_1->setScale(0.7);      // 设置缩放
        icicle_1->setPos(10, 425);   // 设置位置

        // 设置 Z 值，确保它在地图背景之上，但在角色之下
        icicle_1->setZValue(0);

        // 添加矩形障碍物到障碍物列表
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, icicle_1->mapToScene(icicle_1->boundingRect()).boundingRect()));
    }

    // 斜坡障碍物：icile_2
    QPixmap icicle_2_map(":/Items/Maps/Icefield/icicle_2.png");
    if (!icicle_2_map.isNull())
    {
        // 创建 QGraphicsPixmapItem，并将其父项设置为 Icefield (this)
        icicle_2 = new QGraphicsPixmapItem(icicle_2_map, this);

        // 设置贴图相对于 Icefield 局部坐标系的位置,相对于 Icefield 的 (0,0) 点
        icicle_2->setScale(1);      // 设置缩放
        icicle_2->setPos(1050, 425);   // 设置位置

        // 设置 Z 值，确保它在地图背景之上，但在角色之下
        icicle_2->setZValue(0);
    }

    // 空中大平台：ice_platform
    QPixmap ice_platform_map(":/Items/Maps/Icefield/ice_platform.png");
    if (!ice_platform_map.isNull())
    {
        // 创建 QGraphicsPixmapItem，并将其父项设置为 Icefield (this)
        ice_platform = new QGraphicsPixmapItem(ice_platform_map, this);

        // 设置贴图相对于 Icefield 局部坐标系的位置,相对于 Icefield 的 (0,0) 点
        ice_platform->setScale(0.7);      // 设置缩放
        ice_platform->setPos(300, 270);   // 设置位置

        // 设置 Z 值，确保它在地图背景之上，但在角色之下
        ice_platform->setZValue(0);

        QRectF platformRectLocal = ice_platform->boundingRect(); // 平台自己的局部边界
        platformRectLocal.setWidth(platformRectLocal.width() * ice_platform->scale()); // 考虑缩放后的宽度
        platformRectLocal.setHeight(platformRectLocal.height() * ice_platform->scale()); // 考虑缩放后的高度
        platformRectLocal.moveTo(ice_platform->pos().x(), ice_platform->pos().y()); // 移动到平台在父项中的位置
        // 添加为矩形障碍物
        m_obstacles.append(Obstacle(ObstacleType::Rectangle, platformRectLocal));
    }

    updateGroundGeometry(); // 初始化地面几何
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

    updateGroundGeometry();

    if (character->isOnGround())
    {
        qreal frictionCoefficient = 0.15; // 白色冰原的摩擦力

        if (getMapType() == 1) // 紫色冰原
        {
            frictionCoefficient = 0.02; // 紫色冰原很滑
        }

        QPointF currentVelocity = character->getVelocity();

        // 检查角色是否在主动移动
        bool isActivelyMoving = character->isLeftDown() || character->isRightDown();

        if (!isActivelyMoving)
        {
            // 改进的摩擦力计算，确保方向正确
            qreal currentSpeedX = currentVelocity.x();

            if (qAbs(currentSpeedX) > 0.001) // 如果有水平速度
            {
                // 计算摩擦力方向（与运动方向相反）
                qreal frictionForce = -currentSpeedX * frictionCoefficient;
                qreal newSpeedX = currentSpeedX + frictionForce;

                // 防止摩擦力导致速度反向（过度减速）
                if ((currentSpeedX > 0 && newSpeedX < 0) ||
                    (currentSpeedX < 0 && newSpeedX > 0))
                {
                    newSpeedX = 0; // 直接停止
                }

                character->setVelocity(QPointF(newSpeedX, currentVelocity.y()));
            }
        }
        // 如果正在主动移动，不应用摩擦力
    }
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
