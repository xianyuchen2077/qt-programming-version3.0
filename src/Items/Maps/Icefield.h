#ifndef ICEFIELD_H
#define ICEFIELD_H


#include "Map.h"

// 定义障碍物类型
enum class ObstacleType
{
    Rectangle,
};

// 障碍物结构体
struct Obstacle
{
    ObstacleType type;
    QRectF bounds; // 障碍物的整体包围盒（场景坐标）

    // 构造函数，方便初始化
    Obstacle(ObstacleType t, const QRectF& b) : type(t), bounds(b) {}
};

// 地图模式枚举
enum class MapMode
{
    Icefield,    // 冰原模式
    Grassland    // 草地模式
};

class Icefield: public Map
{
protected:
    QRectF m_groundRect;  // 添加地面矩形区域
    QRectF m_boundaryRect; // 添加地图边界矩形
    int maptype; // 0表示白色冰原，1表示紫色冰原
    MapMode m_currentMode; // 当前地图模式

    // 图片贴图
    QGraphicsPixmapItem* icicle_1 = nullptr;
    QGraphicsPixmapItem* icicle_2 = nullptr;
    QGraphicsPixmapItem* ice_platform = nullptr;

    // 存储所有障碍物
    QList<Obstacle> m_obstacles;

public:
    explicit Icefield(QGraphicsItem *parent= nullptr);

    qreal getFloorHeight() override;

    void applyEffectToCharacter(Character* character, qint64 deltaTime) override;

    // 静态方法用于选择随机冰原图片路径
    static QString selectRandomIcefieldPixmapPath();

    // 获取地图的边界矩形
    QRectF getBoundaryRect() const;

    // 获取地面矩形
    QRectF getGroundRect() const;

    // 获取地图类型
    int getMapType() const { return maptype; }

    // 设置地图类型
    void setMapType(int type) { maptype = type; }

    // 获取所有障碍物列表
    const QList<Obstacle>& getObstacles() const;

    // 切换到草地模式
    void switchToGrasslandMode();

    // 切换到冰原模式
    void switchToIcefieldMode();

    // 获取当前模式
    MapMode getCurrentMode() const { return m_currentMode; }

    // 检查角色是否可以隐身（在草地模式下蹲在地面上）
    bool canCharacterHide(Character* character) const;

protected:
    // 更新地面几何
    void updateGroundGeometry();

    // 设置地图资源（背景和障碍物）
    void setupMapResources();

    // 清理当前障碍物
    void clearObstacles();

    // 设置冰原障碍物
    void setupIcefieldObstacles();

    // 设置草地障碍物
    void setupGrasslandObstacles();
};


#endif // ICEFIELD_H
