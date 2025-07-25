#ifndef QT_PROGRAMMING_2024_LINK_H
#define QT_PROGRAMMING_2024_LINK_H

#include "Character.h"
#include <QElapsedTimer>
#include <QTimer>

class Link : public Character
{
    Q_OBJECT
public:
    explicit Link(QGraphicsItem *parent = nullptr);

    void setCrouchPixmap();  // 下蹲动作
    void setStandPixmap();   // 站立动作
    void setWalkPixmap();    // 行走动作
    void setJumpPixmap();    // 跳跃动作

    void turnFaceLeft();     // 转向左侧
    void turnFaceRight();    // 转向右侧

    void processInput() override; // 处理输入

    void processWalkAnimation(qint64 deltaTime); // 处理行走动画
    void processDyingAnimation(qint64 deltaTime); // 处理死亡动画
    void updateAnimation(qint64 deltaTime); // 更新动画（独立于输入处理）
    void checkDeathStatus(); // 检查死亡状态
    void startDeathSequence(); // 开始死亡序列

private:
    qint64 m_walkAnimationElapsedTime = 0;      // 记录行走动画已经过去的时间
    int m_currentWalkFrame = 0;                 // 当前行走的帧索引（0 或 1）
    const qint64 m_walkAnimationInterval = 100; // 每张图片切换的时间间隔（毫秒）

    int m_currentDyingFrame = 0;                // 当前死亡帧索引
    QStringList m_dyingFrames;                  // 死亡帧图片路径
    QTimer* m_dyingAnimationTimer = nullptr;    // 死亡动画用的定时器
    void handleDyingFrameUpdate();              // 处理死亡动画帧更新
    void processDeathAnimation();               // 处理死亡动画

    // 时间追踪相关
    QElapsedTimer m_lastFrameTime; // 用于计算实际时间间隔
    qint64 m_lastUpdateTime = 0;   // 上次更新的时间
};

#endif //QT_PROGRAMMING_2024_LINK_H
