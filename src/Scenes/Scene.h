#ifndef QT_PROGRAMMING_2024_SCENE_H
#define QT_PROGRAMMING_2024_SCENE_H

#include "../GameEnums.h"
#include <QGraphicsScene>
#include <QTimer>

class Scene : public QGraphicsScene
{
Q_OBJECT

public:
    explicit Scene(QObject *parent);

    void startLoop();

    void stopLoop();

    virtual void processInput();

    virtual void processMovement();

    virtual void processPicking();

signals: // 新增信号部分
    // 请求 MyGame 切换到指定场景的信号
    void requestSceneChange(SceneID id); // 需要 MyGame::SceneID

protected slots:

    virtual void update();

protected:
    qint64 deltaTime{};

private:
    QTimer *timer;

    // 用于计算帧率deltaTime
    qint64 lastTime{-1};
};


#endif //QT_PROGRAMMING_2024_SCENE_H
