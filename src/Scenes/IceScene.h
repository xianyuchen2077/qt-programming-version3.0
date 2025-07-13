#ifndef ICESCENE_H
#define ICESCENE_H

#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Maps/Icefield.h"
#include "../Items/Characters/Link.h"
#include <QList>

struct Platform
{
    QRectF rect;        // 平台的矩形区域
    bool isOneWay;      // 是否是单向平台（只能从上方站立）

    Platform(qreal x, qreal y, qreal width, qreal height, bool oneWay = true)
        : rect(x, y, width, height), isOneWay(oneWay) {}
};

class IceScene : public Scene
{
    Q_OBJECT

public:
    explicit IceScene(QObject *parent);

    void processInput() override;
    void processMovement() override;
    void processPicking() override;

protected slots:
    void update() override;
    void gameLoop(); // 游戏主循环

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());
    static Mountable * pickupMountable(Character *character, Mountable *mountable);

    Map *map;
    Link *player1;
    Link *player2;
    Armor *spareArmor;
    HeadEquipment *spareHeadEquipment;

    // 新增：游戏循环相关
    QTimer *gameTimer;              // 游戏主循环定时器
    QElapsedTimer elapsedTimer;     // 用于计算帧间隔
    qint64 lastFrameTime;           // 上一帧的时间

    static const int TARGET_FPS = 60;  // 目标帧率
    static const int FRAME_TIME = 1000 / TARGET_FPS;  // 每帧时间（毫秒）

    QList<Platform> platforms;      // 平台列表

    // 碰撞检测函数
    void initializePlatforms();    // 初始化平台数据
    void handleCollisions(Character* character, qint64 deltaTime);  // 处理碰撞
    void handleBoundaryCollision(Character* character);  // 处理边界碰撞
    bool landedOnPlatform;         // 是否刚落在平台上

// 用于调试
private:
    // 用于调试可视化
    void showDebugVisualization();     // 初始化调试可视化
    void hideDebugVisualization();     // 隐藏调试可视化
    void updateDebugVisualization();   // 更新调试可视化（新增）
    QList<QGraphicsItem*> debugItems;

    // 分离静态和动态调试项
    QList<QGraphicsItem*> staticDebugItems;  // 静态调试项（场景边界、平台等）
    QList<QGraphicsItem*> dynamicDebugItems; // 动态调试项（角色框、信息等）
    bool debugVisible = true;  // 调试视图是否可见
};

#endif // ICESCENE_H
