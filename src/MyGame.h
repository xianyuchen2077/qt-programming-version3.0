#ifndef QT_PROGRAMMING_2024_MYGAME_H
#define QT_PROGRAMMING_2024_MYGAME_H

#include <QGraphicsView>
#include <QMainWindow>
#include "GameEnums.h"
#include "Scenes/Scene.h"

class MyGame : public QMainWindow
{
Q_OBJECT

public:
    explicit MyGame(QWidget *parent = nullptr);
    ~MyGame();

    // 切换场景
    void switchScene(SceneID id);

    // 添加 Q_ENUM 宏
     Q_ENUM(SceneID)

private slots:
    // 显示设置页面
    void showSettings();

    // 显示战斗场景 (或返回游戏)
    void showBattleScene();

    // 接收场景请求切换的槽函数
    void handleSceneChangeRequest(SceneID id);

    // 处理从设置场景返回的请求
    void returnFromSettings();

protected:
    // 处理键盘事件
    void keyPressEvent(QKeyEvent *event) override;

private:
    QGraphicsView *view;
    Scene *currentScene;            // 当前场景指针
    Scene *previousActiveScene;     // 保存上一个活动场景的指针，以便返回时使用
    SceneID previousSceneId;        // 保存上一个场景的ID
    SceneID currentSceneId;         // 当前场景的ID
};

#endif //QT_PROGRAMMING_2024_MYGAME_H
