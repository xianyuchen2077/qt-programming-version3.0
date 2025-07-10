#include "Icefield.h"
#include <random>

Icefield::Icefield(QGraphicsItem *parent): Map(parent,""),maptype(0)
{
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

}

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
        sceneRect.bottom() - sceneRect.height() * 0.15,  // 距底部
        sceneRect.width() * 0.9,                         // 宽度
        sceneRect.height() * 0.15                        // 高度
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

    // 确保几何信息最新
    updateGroundGeometry();

    // 仅当角色在地面上时应用效果
    if (character->isOnGround())
    {
        // 应用恒定摩擦力系数
        qreal frictionCoefficient=0.8;

        if (getMapType()==1) // 紫色冰原
        {
           qreal frictionCoefficient = 0.01;
        }

        QPointF currentVelocity = character->getVelocity();

        // 应用摩擦力效果 (x轴减速，y轴不变)
        QPointF newVelocity(
            currentVelocity.x() * (1.0 - frictionCoefficient),
            currentVelocity.y()
            );

        character->setVelocity(newVelocity); // 速度更新
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
