#ifndef ICESCENE_H
#define ICESCENE_H

#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QList>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Link.h"

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

    bool getIsGameOver();

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
    Weapon *spareWeapon1;
    Weapon *spareWeapon2;
    Weapon *spareWeapon3;
    MedicalItem *spareMedicalItem1;

    // 游戏结束相关变量
    bool isGameOver; // 标记游戏是否已结束
    QString gameResultText; // 存储游戏结果文本，用于传递给GameOverScene
    bool checkGameEndCondition(); // 判断游戏是否结束的函数
    void handleGameEnd(); // 处理游戏结束逻辑的函数

    // 游戏循环相关
    QTimer *gameTimer;              // 游戏主循环定时器
    QElapsedTimer elapsedTimer;     // 用于计算帧间隔
    qint64 lastFrameTime;           // 上一帧的时间

    static const int TARGET_FPS = 60;  // 目标帧率
    static const int FRAME_TIME = 1000 / TARGET_FPS;  // 每帧时间（毫秒）

    QList<Platform> platforms;      // 平台列表

    void updateLaserSights(); // 更新激光瞄准线（狙击枪专用）

    // 碰撞检测函数
    void initializePlatforms();    // 初始化平台数据
    void handleBoundaryCollision(Character* character,QPointF& newPos);  // 处理边界碰撞
    void handleGroundCollision(Character* character, QPointF& newPos); // 处理地面碰撞
    void handleAllCollisions(Character* character, QPointF& newPos);       // 处理所有碰撞
    bool checkObstacleCollision(Character* character, const QPointF& testPos); // 检查障碍物碰撞

    bool checkBodyObstacleCollision(Character* character, const QPointF& testPos); // 仅检查身体碰撞
    bool checkHeadHorizontalCollision(Character* character, const QPointF& testPos); // 检查头部水平碰撞
    bool isHeadStuckInObstacle(Character* character, const QPointF& testPos);      // 检测头部是否被卡住
    QPointF calculateSlideDirection(Character* character, const QPointF& currentPos); // 计算滑动方向
    bool isPositionSafe(Character* character, const QPointF& testPos);             // 检查位置是否安全
    bool landedOnPlatform;         // 是否刚落在平台上

    // 用于调试可视化
    void showDebugVisualization();     // 初始化调试可视化
    void hideDebugVisualization();     // 隐藏调试可视化
    void updateDebugVisualization();   // 更新调试可视化
    void addCharacterDebugVisualization(Character* character, QColor primaryColor, QColor fillColor, const QString& name, qreal labelOffsetX); // 添加角色调试可视化
    void addGlobalDebugInfo(); // 添加全局调试项（如FPS计数器）
    QList<QGraphicsItem*> debugItems;

    // 分离静态和动态调试项
    QList<QGraphicsItem*> staticDebugItems;  // 静态调试项（场景边界、平台等）
    QList<QGraphicsItem*> dynamicDebugItems; // 动态调试项（角色框、信息等）
    bool debugVisible = true;  // 调试视图是否可见
};

#endif // ICESCENE_H
