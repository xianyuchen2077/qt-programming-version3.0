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
        pixmapItem->setScale(0.3);
        pixmapItem->setPos(-130, -225);  // 图片相对于角色原点的偏移
    }

    // 重新计算碰撞框，考虑图片的实际显示位置和缩放
    if (pixmapItem != nullptr)
    {
        // 获取原始图片尺寸
        QPixmap originalPixmap = pixmapItem->pixmap();
        qreal originalWidth = originalPixmap.width();
        qreal originalHeight = originalPixmap.height();

        // 计算缩放后的尺寸
        qreal scaledWidth = originalWidth * 0.3;
        qreal scaledHeight = originalHeight * 0.3;

        // 图片在角色坐标系中的位置（考虑setPos的偏移）
        QPointF imagePos = pixmapItem->pos(); // (-130, -225)

        // 碰撞框应该在角色脚部附近
        qreal collisionWidth = scaledWidth * 0.5;
        qreal collisionHeight = scaledHeight * 0.9;

        // 碰撞框应该在图片底部中央
        qreal collisionX = imagePos.x() + (scaledWidth - collisionWidth) / 2.0-5; // 调整X位置，使碰撞框稍微偏左;
        qreal collisionY = imagePos.y() + scaledHeight - collisionHeight;

        // 更新基类的碰撞矩形
        m_collisionRect = QRectF(collisionX, collisionY, collisionWidth, collisionHeight);
    }
    else
    {
        // 如果没有图片，给一个默认的碰撞框
        m_collisionRect = QRectF(-25, -80, 50, 80); // 默认尺寸，在角色原点附近
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

void Link::setJumpPixmap()
{
    if (pixmapItem != nullptr)
    {
        updatePixmap(":/Items/Characters/littlerubbish/Reaper_Man_1/PNG Sequences/Jump Start/0_Reaper_Man_Jump Start_005.png");
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

    if(isDownDown())
    {
        if(isOnGround())
        {
            setVelocity(QPointF(0, 0)); // 如果在地面上且下蹲键按下，停止角色移动
        }
        else
        {
            setVelocity(QPointF(getVelocity().x(), 0)); // 如果不在地面上，下蹲时只水平速度不变
        }
        setCrouchPixmap(); // 如果下蹲键按下，设置下蹲图片
    }
    else
    {
        if(isOnGround())
        {
            if (isLeftDown())
            {
                setVelocity(QPointF(-getMoveSpeed(), getVelocity().y()));
                if (isFaceRight())
                    turnFaceLeft();
            }
            else if (isRightDown())
            {
                setVelocity(QPointF(getMoveSpeed(), getVelocity().y()));
                if (!isFaceRight())
                    turnFaceRight();
            }
            else
            {
                // 当没有按键按下时，不要设置速度，让摩擦力自然处理
                if (isUpDown())
                {
                    // 如果按下跳跃键且在地面上，执行跳跃
                    handleJump();
                    setJumpPixmap();
                }
                else
                {
                    setStandPixmap();
                }
            }
        }
        else if (!isOnGround())
        {
            // 在空中时仍然可以左右移动（空中控制）
            if (isLeftDown())
            {
                setVelocity(QPointF(-getMoveSpeed() * 0.5, getVelocity().y())); // 空中移动速度减半
                if (isFaceRight())
                    turnFaceLeft();
            }
            else if (isRightDown())
            {
                setVelocity(QPointF(getMoveSpeed() * 0.5, getVelocity().y())); // 空中移动速度减半
                if (!isFaceRight())
                    turnFaceRight();
            }

            // 如果不在地面上保持跳跃状态
            setJumpPixmap();
        }
    }
}

// 独立的动画更新方法
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
