#ifndef ITEMDROPMANAGER_H
#define ITEMDROPMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QRandomGenerator>
#include "Item.h"
#include "Mountable.h"

class IceScene;
class Character;

// 前向声明：可装备物品（同时继承Item和Mountable的物品）
class DropableItem;

// 掉落物品信息结构
struct DroppingItem
{
    Item* item;             // 掉落的物品（使用Item*而不是Mountable*）
    QPointF velocity;       // 掉落速度（主要是Y方向）
    bool isDropping;        // 是否正在掉落
    qreal lastY;            // 上一帧的Y坐标，用于检测碰撞

    DroppingItem(Item* i) : item(i), velocity(0, 0), isDropping(true), lastY(0) {}
};

// ItemDropManager只继承QObject，不继承Mountable
class ItemDropManager : public QObject
{
    Q_OBJECT

public:
    explicit ItemDropManager(IceScene* scene, QObject* parent = nullptr);

    // 开始掉落一个物品（接受Item*参数）
    void dropItem(Item* item);

    // 开始掉落一个可装备物品的重载版本
    template<typename T>
    void dropMountableItem(T* item)
    {
        static_assert(std::is_base_of_v<Item, T> && std::is_base_of_v<Mountable, T>,
                      "T must inherit from both Item and Mountable");

        // 确保物品未装备
        item->unmount();

        // 转换为Item*进行掉落处理
        dropItem(static_cast<Item*>(item));
    }

    // 在随机位置生成并掉落物品
    void dropRandomItem();

    // 更新所有掉落中的物品
    void updateDrops(qreal deltaTime);

    // 停止某个物品的掉落
    void stopDrop(Item* item);

    // 清除所有掉落
    void clearAllDrops();

    // 获取当前掉落物品数量
    int getDropCount() const { return droppingItems.size(); }

private slots:
    void onDropTimer();         // 掉落更新定时器的槽函数
    void onAutoDropTimer();     // 自动掉落物品的槽函数

private:
    IceScene* scene;
    QTimer* dropTimer; // 掉落更新定时器
    QTimer* autoDropTimer; // 自动掉落定时器
    QList<DroppingItem> droppingItems;

    // 物理参数
    static constexpr qreal GRAVITY = 0.008;      // 重力加速度
    static constexpr qreal MAX_FALL_SPEED = 4.0; // 最大掉落速度
    static constexpr qreal BOUNCE_FACTOR = 0.3;  // 反弹系数
    static constexpr qreal PLATFORM_FALL_CHANCE = 0.5; // 从平台掉落的概率
    static constexpr int UPDATE_INTERVAL = 16;   // 更新间隔（毫秒）

    // 辅助函数
    QPointF getRandomDropStartPos() const;       // 获取随机掉落起始位置
    bool checkGroundCollision(DroppingItem& drop); // 检查地面碰撞
    bool checkPlatformCollision(DroppingItem& drop); // 检查平台碰撞
    bool checkObstacleCollision(DroppingItem& drop); // 检查障碍物碰撞
    void landItem(DroppingItem& drop, qreal landY); // 物品着陆
    Item* createRandomItem() const;              // 创建随机物品（返回Item*）

    // 从掉落列表中移除物品
    void removeFromDropList(Item* item);
};

#endif // ITEMDROPMANAGER_H
