#ifndef ICEFIELD_H
#define ICEFIELD_H


#include "Map.h"

// 定义障碍物类型
enum class ObstacleType
{
    Rectangle,
    Slope
};

// 定义斜坡方向
enum class SlopeDirection
{
    UpRight,   // 从左下到右上 ( / )
    UpLeft     // 从右下到左上 ( \ )
};

// 障碍物结构体
struct Obstacle
{
    ObstacleType type;
    QRectF bounds; // 障碍物的整体包围盒（场景坐标）

    // 对于斜坡：
    QPointF startPoint; // 斜坡的低点（例如，左下角或右下角）
    QPointF endPoint;   // 斜坡的高点（例如，右上角或左上角）
    SlopeDirection direction; // 斜坡方向

    // 构造函数，方便初始化
    Obstacle(ObstacleType t, const QRectF& b) : type(t), bounds(b) {}
    Obstacle(ObstacleType t, const QRectF& b, const QPointF& start, const QPointF& end, SlopeDirection dir)
        : type(t), bounds(b), startPoint(start), endPoint(end), direction(dir) {}
};

class Icefield: public Map
{
protected:
    QRectF m_groundRect;  // 添加地面矩形区域
    QRectF m_boundaryRect; // 添加地图边界矩形
    int maptype; // 0表示白色冰原，1表示紫色冰原

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

protected:
    // 更新地面几何
    void updateGroundGeometry();
};


#endif // ICEFIELD_H
