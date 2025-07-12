#include <memory>
#include "Link.h"
#include <QPixmap>
#include <QDebug>
#include <QTransform>
#include <QElapsedTimer>
#include "../HeadEquipments/CapOfTheHero.h"
#include "../Armors/OldShirt.h"
#include "../LegEquipments/WellWornTrousers.h"
#include "../Weapons/Shabby_Pistol.h"

Link::Link(QGraphicsItem *parent): Character(parent, ":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Walking/0_Reaper_Man_Walking_006.png")
{
    if (pixmapItem != nullptr)
    {
        pixmapItem->setScale(0.3);          // 只对人物图片进行缩放
        pixmapItem->setPos(-130, -225);      // 人物图片相对与父类 Character 局部原点的位置偏移
    }

    headEquipment = new CapOfTheHero(this);
    legEquipment = new WellWornTrousers(this);
    armor = new OldShirt(this);
    weapon = new Shabby_Pistol(this);
    headEquipment->mountToParent();
    legEquipment->mountToParent();
    armor->mountToParent();
    weapon->mountToParent();

    // 初始化面向方向
    faceRight = true; // 默认面向右侧

    // 设置初始状态为站立（这应该是最后设置的）
    setStandPixmap();

    // 初始化时间追踪
    m_lastFrameTime = QElapsedTimer();
    m_lastFrameTime.start();
}

void Link::setCrouchPixmap()
{
    if (pixmapItem != nullptr)
    {
        updatePixmap(":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Crouch/0_Reaper_Man_Dying_000.png");
        pixmapItem->setScale(0.3);
        pixmapItem->setPos(-130, -225); // 确保图片位置正确
    }
}

void Link::setStandPixmap()
{
    if (pixmapItem != nullptr)
    {
        updatePixmap(":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Walking/0_Reaper_Man_Walking_006.png");
        pixmapItem->setScale(0.3);
        pixmapItem->setPos(-130, -225); // 确保图片位置正确
    }
}

// 设置角色面向左侧
void Link::turnFaceLeft()
{
    if (isFaceRight())
    {
        setFaceRight(false);
        if (pixmapItem != nullptr)
        {
            // 直接设置变换，而不是在当前变换基础上再变换
            setTransform(QTransform().scale(-1, 1));
        }
    }
}

// 设置角色面向右侧
void Link::turnFaceRight()
{
    if (!isFaceRight())
    {
        setFaceRight(true);
        if (pixmapItem != nullptr)
        {
            // 重置变换为正常状态
            setTransform(QTransform().scale(1, 1));
        }
    }
}

void Link::processWalkAnimation(qint64 deltaTime)
{
    m_walkAnimationElapsedTime += deltaTime;

    if (m_walkAnimationElapsedTime >= m_walkAnimationInterval)
    {
        m_walkAnimationElapsedTime = 0; // 重置计时器

        // 切换到下一帧
        m_currentWalkFrame = (m_currentWalkFrame + 1) % 2; // 在 0 和 1 之间切换

        if (pixmapItem != nullptr)
        {
            if (m_currentWalkFrame == 0)
            {
                // 第一张行走图片
                updatePixmap(":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Walking/0_Reaper_Man_Walking_006.png");
            }
            else
            {
                // 第二张行走图片
                updatePixmap(":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Walking/0_Reaper_Man_Walking_000.png");
            }
            pixmapItem->setScale(0.3);
            pixmapItem->setPos(-130, -225);
        }
    }
}

void Link::processInput()
{
    Character::processInput();

    if (isLeftDown())
    {
        setVelocity(QPointF(-getMoveSpeed(), getVelocity().y()));
        if (isFaceRight())
            turnFaceLeft(); // 如果当前朝向右，则转向左
    }
    else if (isRightDown())
    {
        setVelocity(QPointF(getMoveSpeed(), getVelocity().y()));
        if (!isFaceRight())
            turnFaceRight(); // 如果当前朝向左，则转向右
    }
    else
    {
        // 如果没有左右移动键按下，水平速度设为0
        setVelocity(QPointF(0, getVelocity().y()));
    }
}

// 新增：独立的动画更新方法
void Link::updateAnimation(qint64 deltaTime)
{
    bool isMoving = (isLeftDown() || isRightDown()) && isOnGround();

    if (isMoving)
    {
        processWalkAnimation(deltaTime);
    }
    else if (isOnGround())
    {
        // 重置动画状态
        m_walkAnimationElapsedTime = 0;
        m_currentWalkFrame = 0;

        if (isDownDown())
        {
            setCrouchPixmap();
        }
        else
        {
            setStandPixmap();
        }
    }
}
