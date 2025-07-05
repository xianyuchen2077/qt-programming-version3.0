#include <QDateTime>
#include "Scene.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent), timer(new QTimer(this)) {
    connect(timer, &QTimer::timeout, this, &Scene::update);
}

void Scene::update() {
    auto currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (lastTime == -1) { // first frame
        deltaTime = 0;
    } else {
        deltaTime = currentTime - lastTime;
    }
    lastTime = currentTime;

    processInput();
    processMovement();
    processPicking();
}

// 启动定时器，设置游戏循环的帧率
void Scene::startLoop()
{
    timer->start(1000 / 90); // 90FPS
}

void Scene::stopLoop()
{
    timer->stop();
}

void Scene::processInput()
{
    // 处理输入逻辑
    // 这里可以添加键盘、鼠标等输入事件的处理逻辑
    // 例如，监听键盘事件以控制角色移动或执行其他操作
}

void Scene::processMovement()
{
    // 处理角色或物体的移动逻辑
    // 例如，根据按键状态更新角色位置
    // 这里可以使用 deltaTime 来调整移动速度
}

void Scene::processPicking()
{
    // 处理拾取逻辑
    // 例如，检测角色是否接近可拾取物体，并执行拾取操作
    // 这里可以使用角色位置和物体位置的距离来判断是否可以拾取物体
}

