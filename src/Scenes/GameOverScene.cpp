#include "GameOverScene.h"
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QDebug>

GameOverScene::GameOverScene(QObject *parent, const QString& result)
    : Scene(parent)
{
    setSceneRect(0, 0, 1280, 720); // 设定场景大小与主游戏场景一致

    // 设置背景颜色
    setBackgroundBrush(QBrush(Qt::darkGray));

    // 创建并添加显示结果的文本项

    // 标题
    titleText = new QGraphicsTextItem("Game Over !");
    QFont titleFont("宋体", 100); // 设置标题字体和大小
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::black); // 设置标题颜色
    qreal xPos = (width() - titleText->boundingRect().width()) / 2;
    titleText->setPos(xPos, 150); // 居中放置
    addItem(titleText);

    // 可以添加提示，比如“按任意键返回主菜单”
    QGraphicsTextItem* resultText = new QGraphicsTextItem(result);
    QFont resultFont("Consolas", 70);
    resultText->setDefaultTextColor(Qt::white);
    resultText->setFont(resultFont);
    qreal resultWidth = resultText->boundingRect().width();
    qreal resultHeight = resultText->boundingRect().height();
    resultText->setPos((width() - resultWidth) / 2, (height() - resultHeight) / 2 - 50); // 稍微向上偏移
    addItem(resultText);

    qDebug() << "GameOverScene created.";
}

void GameOverScene::update()
{
    // 游戏结束界面通常不需要持续更新，除非有动画效果等
}

void GameOverScene::keyPressEvent(QKeyEvent *event)
{
    // 监听任意按键，触发返回主菜单（或某个起始场景）
    // 这里的 SceneID::BattleScene_ID 可以替换成您的主菜单或第一个游戏场景ID
    emit requestSceneChange(SceneID::BattleScene_ID);
    qDebug() << "Key pressed in GameOverScene, requesting scene change to BattleScene.";
    Scene::keyPressEvent(event); // 调用基类的keyPressEvent，确保事件被处理
}

void GameOverScene::keyReleaseEvent(QKeyEvent *event)
{
    Scene::keyReleaseEvent(event);
}
