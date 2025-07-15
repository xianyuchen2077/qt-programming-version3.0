#ifndef QT_PROGRAMMING_2024_LINK_H
#define QT_PROGRAMMING_2024_LINK_H

#include "Character.h"
#include <QElapsedTimer>
#include <QTimer>

class Link : public Character
{
public:
    explicit Link(QGraphicsItem *parent = nullptr);

    void setCrouchPixmap(); // 下蹲动作
    void setStandPixmap(); // 站立动作
    void setWalkPixmap(); // 行走动作
    void setJumpPixmap(); // 跳跃动作

    bool getFaceRight() const { return faceRight; } // 获取角色是否面向右侧
    void setFaceRight(bool right) { faceRight = right; } // 设置角色是否面向右侧
    bool isFaceRight() const { return faceRight; } // 判断角色是否面向右侧
    void turnFaceLeft(); // 转向左侧
    void turnFaceRight(); // 转向右侧

    void processInput() override; // 处理输入

    void processWalkAnimation(qint64 deltaTime); // 处理行走动画
    void processDyingAnimation(qint64 deltaTime); // 处理死亡动画
    void checkDeathStatus(); // 检查死亡状态
    void startDeathSequence(); // 开始死亡序列
    void updateAnimation(qint64 deltaTime); // 更新动画（独立于输入处理）

private:
    qint64 m_walkAnimationElapsedTime = 0;      // 记录行走动画已经过去的时间
    int m_currentWalkFrame = 0;                 // 当前行走的帧索引（0 或 1）
    const qint64 m_walkAnimationInterval = 100; // 每张图片切换的时间间隔（毫秒）
    QTimer* deathTimer = nullptr; // 死亡计时器
    void processDeathAnimation(); // 处理死亡动画
    qint64 m_dyingAnimationElapsedTime = 0;     // 记录死亡动画已经过去的时间
    int m_currentDyingFrame = 0;                // 当前死亡的帧索引（0 到 3）
    const qint64 m_dyingAnimationInterval = 300; // 每张死亡图片切换的时间间隔（毫秒）

    bool faceRight = false; // 角色是否面向右侧

    // 时间追踪相关
    QElapsedTimer m_lastFrameTime; // 用于计算实际时间间隔
    qint64 m_lastUpdateTime = 0;   // 上次更新的时间
};

#endif //QT_PROGRAMMING_2024_LINK_H
