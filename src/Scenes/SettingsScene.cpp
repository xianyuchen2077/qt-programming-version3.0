#include <QFont>
#include <QBrush>
#include <QColor>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include "SettingsScene.h"

SettingsScene::SettingsScene(QObject *parent) : Scene(parent)
{
    setSceneRect(0, 0, 1280, 720); // 设置场景大小与游戏场景一致

    // 设置背景颜色
    // setBackgroundBrush(QBrush(QColor(50, 50, 50))); // 深灰色背景

    // 使用Settingsfield的图片作为背景
    backgroundMap = new Settingsfield(); // 创建 Settingsfield 对象
    addItem(backgroundMap);               // 将其添加到场景中
    backgroundMap->scaleToFitScene(this); // 缩放地图以适应场景
    backgroundMap->setZValue(-1);         // 将背景放到最底层

    // 标题
    titleText = new QGraphicsTextItem("设置");
    QFont titleFont("宋体", 70); // 设置标题字体和大小
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::black); // 设置标题颜色
    qreal xPos = (width() - titleText->boundingRect().width()) / 2;
    titleText->setPos(xPos, 50); // 居中放置
    addItem(titleText);

    // 返回按钮
    backButton = new QPushButton("返回游戏");
    backButton->setFixedSize(200, 50); // 设置按钮大小
    backButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-size: 20px; border: none; }"
                              "QPushButton:hover { background-color: #45a049; }"); // 设置按钮样式

    // 将 QPushButton 包装成 QGraphicsProxyWidget 以添加到 QGraphicsScene
    QGraphicsProxyWidget *backButtonProxy = new QGraphicsProxyWidget();
    backButtonProxy->setWidget(backButton);
    qreal backButtonX = (width() - backButtonProxy->boundingRect().width()) / 2;
    backButtonProxy->setPos(backButtonX, 600); // 放置在底部
    addItem(backButtonProxy);

    // 连接按钮的点击信号到槽函数
    connect(backButton, &QPushButton::clicked, this, &SettingsScene::onBackButtonClicked);

    // 在这里添加更多按钮或设置控件

    // 示例按钮 1: 音量设置
    QPushButton *volumeButton = new QPushButton("音量设置");
    volumeButton->setFixedSize(200, 50);
    volumeButton->setStyleSheet("QPushButton { background-color: #008CBA; color: white; font-size: 20px; border: none; }"
                                "QPushButton:hover { background-color: #007bb5; }");
    QGraphicsProxyWidget *volumeButtonProxy = new QGraphicsProxyWidget();
    volumeButtonProxy->setWidget(volumeButton);
    qreal volumeButtonX = (width() - volumeButtonProxy->boundingRect().width()) / 2;
    volumeButtonProxy->setPos(volumeButtonX, 200); // 放置在标题下方
    addItem(volumeButtonProxy);
    // connect(volumeButton, &QPushButton::clicked, this, &SettingsScene::onVolumeButtonClicked); // 连接到你的槽函数

    // 示例按钮 2: 控制设置
    QPushButton *controlsButton = new QPushButton("控制设置");
    controlsButton->setFixedSize(200, 50);
    controlsButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-size: 20px; border: none; }"
                                  "QPushButton:hover { background-color: #da190b; }");
    QGraphicsProxyWidget *controlsButtonProxy = new QGraphicsProxyWidget();
    controlsButtonProxy->setWidget(controlsButton);
    qreal controlsButtonX = (width() - controlsButtonProxy->boundingRect().width()) / 2;
    controlsButtonProxy->setPos(controlsButtonX, 280); // 放置在音量按钮下方
    addItem(controlsButtonProxy);
    // connect(controlsButton, &QPushButton::clicked, this, &SettingsScene::onControlsButtonClicked); // 连接到你的槽函数

    // ====================================================================
}

void SettingsScene::onBackButtonClicked()
{
    emit backToGame(); // 发出信号通知 MyGame 返回游戏场景
}
