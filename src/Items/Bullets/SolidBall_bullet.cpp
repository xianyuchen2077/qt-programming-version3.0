#include "SolidBall_bullet.h"
#include "../Characters/Character.h"
#include "../Maps/Icefield.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QList>
#include <QtMath>

const qreal SolidBall_Bullet::GRAVITY = 0.012;
const qreal SolidBall_Bullet::BOUNCE_FACTOR = 0.7;

SolidBall_Bullet::SolidBall_Bullet(QGraphicsItem *parent, const QPointF& startPos, const QPointF& direction, int damage, Character* shooter)
    : Bullet(parent, BULLET_BASIC_PIXMAP_PATH, startPos, direction, damage),
    ballDamage(damage),
    shooterCharacter(shooter),
    isFlying(true),
    isLanded(false),
    isDestroyed(false),
    frameCount(0),
    landedFrameCount(0)
{
    setScale(1.0);      // 设置子弹缩放比例
    setPos(startPos);   // 设置初始位置
    setZValue(5);       // 确保子弹显示在最上层
    qDebug() << "SolidBall_Bullet created with shooter:" << shooter << "damage:" << damage;

    // 计算45度角的初始速度
    qreal speed = 8.0; // 初始速度大小
    qreal angle = M_PI / 4; // 45度角

    // 根据角色面向方向调整水平方向
    qreal horizontalDirection = shooterCharacter->isFaceRight() ? 1 : -1; // 根据角色朝向确定水平方向

    velocity = QPointF(horizontalDirection * speed * qCos(angle),
                       -speed * qSin(angle)); // 向上为负

    qDebug() << "SolidBall created at:" << startPos << "with velocity:" << velocity;
}

void SolidBall_Bullet::advance(int phase)
{
    if (phase == 0 && !isDestroyed)
    {
        frameCount++;

        if (isFlying)
        {
            // 更新位置
            QPointF newPos = pos() + velocity;
            setPos(newPos);

            // 应用重力
            velocity.setY(velocity.y() + GRAVITY);

            // 处理碰撞
            handleCollisions();
        }
        else if (isLanded)
        {
            // 处理落地后的计时
            landedFrameCount++;
            if (landedFrameCount >= DESTROY_DELAY_FRAMES)
            {
                destroyBall();
            }
        }

        // 检查是否飞出场景太远
        if (scene())
        {
            QRectF sceneRect = scene()->sceneRect();
            if (pos().y() > sceneRect.height() + 200) // 飞出底部太远
            {
                destroyBall();
            }
        }
    }
}

void SolidBall_Bullet::handleCollisions()
{
    if (!isFlying) return;

    // 检查角色碰撞
    QList<QGraphicsItem*> collidingItems = this->collidingItems();
    for (QGraphicsItem* item : collidingItems)
    {
        Character* character = dynamic_cast<Character*>(item);
        if (character)
        {
            // 在安全时间内，不能击中发射者
            if (character == shooterCharacter && frameCount < SAFETY_FRAMES)
            {
                continue; // 跳过发射者
            }

            handleCharacterCollision(character);
            return;
        }
    }

    // 检查墙体碰撞
    handleWallCollision();

    // 检查地面碰撞
    handleGroundCollision();
}

void SolidBall_Bullet::handleWallCollision()
{
    if (!scene()) return;

    // 获取场景中的障碍物
    QList<QGraphicsItem*> allItems = scene()->items();
    Icefield* icefield = nullptr;

    for (QGraphicsItem* item : allItems)
    {
        icefield = dynamic_cast<Icefield*>(item);
        if (icefield) break;
    }

    if (!icefield) return;

    const QList<Obstacle>& obstacles = icefield->getObstacles();
    QRectF ballRect = this->sceneBoundingRect();

    for (const Obstacle& obstacle : obstacles)
    {
        if (obstacle.bounds.intersects(ballRect))
        {
            // 计算重叠区域以确定碰撞方向
            QRectF intersection = obstacle.bounds.intersected(ballRect);

            // 水平碰撞（障碍物左侧或右侧）
            if (intersection.width() < intersection.height())
            {
                velocity.setX(-velocity.x()); // 反转X轴速度，实现水平反弹
                // 调整位置以避免粘连/重叠
                if (ballRect.center().x() < obstacle.bounds.center().x()) // 球撞到障碍物左侧
                {
                    setX(obstacle.bounds.left() - ballRect.width() / 2);
                }
                else // 球撞到障碍物右侧
                {
                    setX(obstacle.bounds.right() + ballRect.width() / 2);
                }
            }
            // 垂直碰撞（障碍物顶部或底部）
            else
            {
                // 调整位置以避免粘连/重叠
                if (ballRect.center().y() < obstacle.bounds.center().y()) // 球撞到障碍物顶部
                {
                    setY(obstacle.bounds.top() - ballRect.height() / 2);
                }
                else // 球撞到障碍物底部
                {
                    velocity.setY(-velocity.y()); // 反转Y轴速度，实现垂直反弹
                    setY(obstacle.bounds.bottom() + ballRect.height() / 2);
                }
            }

            qDebug() << "Ball bounced off obstacle, new velocity:" << velocity;
            return;
        }
    }
}

void SolidBall_Bullet::handleGroundCollision()
{
    if (!scene()) return;

    // 获取地面高度
    QList<QGraphicsItem*> allItems = scene()->items();
    Icefield* icefield = nullptr;

    for (QGraphicsItem* item : allItems)
    {
        icefield = dynamic_cast<Icefield*>(item);
        if (icefield) break;
    }

    if (!icefield) return;

    qreal floorHeight = icefield->getFloorHeight();
    QRectF ballRect = this->sceneBoundingRect();

    // 检查是否撞到地面
    if (ballRect.bottom() >= floorHeight && velocity.y() > 0)
    {
        // 落地
        isFlying = false;
        isLanded = true;
        velocity = QPointF(0, 0);

        // 调整位置到地面上
        setPos(pos().x(), floorHeight - boundingRect().height());

        onLanded();
        qDebug() << "Ball landed on ground";
        return;
    }

    // 检查是否落在障碍物上
    const QList<Obstacle>& obstacles = icefield->getObstacles();
    for (const Obstacle& obstacle : obstacles)
    {
        // 计算重叠区域以确定碰撞方向
        QRectF intersection = obstacle.bounds.intersected(ballRect);

        // 检查球是否在障碍物顶部
        if (velocity.y() > 0 &&
            ballRect.bottom() >= obstacle.bounds.top() &&
            ballRect.bottom() <= obstacle.bounds.top() + 15 &&
            ballRect.left() < obstacle.bounds.right() &&
            ballRect.right() > obstacle.bounds.left() &&
            intersection.width() >= intersection.height() &&
            ballRect.center().y() < obstacle.bounds.center().y()) // 球向下移动且撞到障碍物顶部
        {
            // 落在障碍物上
            isFlying = false;
            isLanded = true;
            velocity = QPointF(0, 0);

            setPos(pos().x(), obstacle.bounds.top() - boundingRect().height());

            onLanded();
            qDebug() << "Ball landed on obstacle";
            return;
        }
    }
}

void SolidBall_Bullet::handleCharacterCollision(Character* character)
{
    if (!character) return;

    // 对角色造成伤害
    character->takeDamage(ballDamage);

    qDebug() << "Ball hit character! Damage:" << ballDamage;

    // 销毁球
    destroyBall();
}

void SolidBall_Bullet::onLanded()
{
    // 落地后开始计时
    landedFrameCount = 0;

    // 更改外观表示已落地
    setScale(1.5);

    // 消失前可以做一个频闪效果
    if (pixmapItem)
    {
        pixmapItem->setOpacity(0.7);
    }

    qDebug() << "Ball landed, will be destroyed in" << DESTROY_DELAY_FRAMES << "frames";
}

void SolidBall_Bullet::onDestroy()
{
    // 子类可以重写这个方法来添加特殊效果
}

void SolidBall_Bullet::destroyBall()
{
    if (isDestroyed) return;

    isDestroyed = true;
    onDestroy();

    qDebug() << "Ball destroyed";

    if (scene())
    {
        scene()->removeItem(this);
    }
    delete this;
}
