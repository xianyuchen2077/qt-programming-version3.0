#ifndef SETTINGSSCENE_H
#define SETTINGSSCENE_H

#include "Scene.h"
#include "../Items/Maps/Settingsfield.h"
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QLabel>


class SettingsScene : public Scene
{
    Q_OBJECT

public:
    explicit SettingsScene(QObject *parent = nullptr);

signals:
    void backToGame(); // 用于返回游戏场景的信号

private slots:
    void onBackButtonClicked(); // "返回游戏"按钮的槽函数

private:
    // 你可以在这里添加其他设置选项的QWidgets，例如音量滑块、控制设置等
    QGraphicsTextItem *titleText;
    QPushButton *backButton;
    QLabel *descriptionLabel;
    Settingsfield *backgroundMap;
};

#endif // SETTINGSSCENE_H
