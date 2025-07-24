#ifndef GAMEOVERFIELD_H
#define GAMEOVERFIELD_H


#include "Map.h"

class GameOverfield: public Map
{
public:
    explicit GameOverfield(QGraphicsItem *parent= nullptr);
    void setPixmap(const QString &pixmapPath) override;
    QString getPixmapPath() const;

private:
    const QString pixmapPath = ":/Items/Maps/Scenicpicture/GameOverfield.png"; // 游戏结束场景的背景图片路径
};


#endif // GAMEOVERFIELD_H
