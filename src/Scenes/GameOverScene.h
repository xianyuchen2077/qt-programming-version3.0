#ifndef GAMEOVERSCENE_H
#define GAMEOVERSCENE_H

#include "Scene.h"
#include "../Items/Maps/GameOverfield.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QLabel>

class GameOverScene : public Scene
{
    Q_OBJECT
public:
    explicit GameOverScene(QObject *parent = nullptr, const QString& result = "Winner is Player1 ！");

protected:

    void update() override; // 可能不需要复杂的更新逻辑，但保留

    // 重写按键
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    QGraphicsTextItem* titleText; // 显示游戏结果的文本
    QPushButton *backButton; // 返回初始界面按钮
    QPushButton *restartButton; // 再来一局游戏按钮
    QLabel *descriptionLabel; // 描述标签
    GameOverfield *backgroundMap; // 背景地图
};

#endif // GAMEOVERSCENE_H
