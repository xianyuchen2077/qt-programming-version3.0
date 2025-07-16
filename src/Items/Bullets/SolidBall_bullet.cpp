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
    setScale(0.2);      // 设置子弹缩放比例
    setPos(startPos);   // 设置初始位置
    setZValue(5);       // 确保子弹显示在最上层);
    qDebug() << "SolidBall_Bullet created with shooter:" << shooter << "damage:" << damage;

    // 计算45度角的初始速度
    qreal speed = 4.0; // 初始速度大小
    qreal angle = M_PI / 4; // 45度角

    // 根据角色面向方向调整水平方向
    qreal horizontalDirection = direction.x() > 0 ? 1 : -1;

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
            // 判断碰撞方向并反弹
            QPointF obstacleCenter = obstacle.bounds.center();
            QPointF ballCenter = ballRect.center();
            QPointF diff = ballCenter - obstacleCenter;

            // 简单的反弹逻辑
            if (qAbs(diff.x()) > qAbs(diff.y()))
            {
                // 水平碰撞
                velocity.setX(-velocity.x() * BOUNCE_FACTOR);
                // 调整位置避免重叠
                if (diff.x() > 0)
                {
                    setPos(obstacle.bounds.right() + 5, pos().y());
                }
                else
                {
                    setPos(obstacle.bounds.left() - boundingRect().width() - 5, pos().y());
                }
            }
            else
            {
                // 垂直碰撞
                velocity.setY(-velocity.y() * BOUNCE_FACTOR);
                // 调整位置避免重叠
                if (diff.y() > 0)
                {
                    setPos(pos().x(), obstacle.bounds.bottom() + 5);
                }
                else
                {
                    setPos(pos().x(), obstacle.bounds.top() - boundingRect().height() - 5);
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
        if (velocity.y() > 0 && // 向下移动
            ballRect.bottom() >= obstacle.bounds.top() &&
            ballRect.bottom() <= obstacle.bounds.top() + 15 &&
            ballRect.left() < obstacle.bounds.right() &&
            ballRect.right() > obstacle.bounds.left())
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
    setScale(0.15);
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
