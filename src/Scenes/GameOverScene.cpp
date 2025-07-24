#include "GameOverScene.h"
#include <QFont>
#include <QBrush>
#include <QColor>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <qgraphicseffect.h>

GameOverScene::GameOverScene(QObject *parent, const QString& result)
    : Scene(parent)
{
    setSceneRect(0, 0, 1280, 720); // 先设定场景大小

    // 创建背景地图
    backgroundMap = new GameOverfield();
    addItem(backgroundMap);
    backgroundMap->setZValue(-1); // 确保背景图在最底层

    // 检查背景图片是否加载成功
    QPixmap bgPixmap(backgroundMap->getPixmapPath());
    if (bgPixmap.isNull()) {
        qDebug() << "Warning: Background image failed to load, using fallback color";
        setBackgroundBrush(QBrush(QColor(50, 50, 100))); // 深蓝色背景作为备选
    } else {
        qDebug() << "Background image loaded successfully";
        // 缩放背景图片以适应场景
        backgroundMap->setScale(qMin(1280.0 / bgPixmap.width(), 720.0 / bgPixmap.height()));
    }

    // 创建半透明遮罩层，让文字更清晰
    QGraphicsRectItem* overlay = new QGraphicsRectItem(0, 0, 1280, 720);
    overlay->setBrush(QBrush(QColor(0, 0, 0, 120))); // 半透明黑色遮罩
    overlay->setPen(Qt::NoPen);
    overlay->setZValue(0);
    addItem(overlay);

    // 创建并添加显示结果的文本项
    // 标题
    titleText = new QGraphicsTextItem("Game Over !");
    QFont titleFont("Arial", 80, QFont::Bold); // 使用更通用的字体
    titleText->setFont(titleFont);
    titleText->setDefaultTextColor(Qt::white);

    // 添加文字描边效果
    auto* titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(10);
    titleShadow->setColor(Qt::black);
    titleShadow->setOffset(3, 3);
    titleText->setGraphicsEffect(titleShadow);

    qreal titleWidth = titleText->boundingRect().width();
    titleText->setPos((1280 - titleWidth) / 2, 100); // 居中放置
    titleText->setZValue(10);
    addItem(titleText);

    // 游戏结果文本
    QGraphicsTextItem* resultText = new QGraphicsTextItem(result);
    QFont resultFont("Arial", 50, QFont::Bold);
    resultText->setDefaultTextColor(Qt::yellow);
    resultText->setFont(resultFont);

    // 添加结果文字描边效果
    auto* resultShadow = new QGraphicsDropShadowEffect();
    resultShadow->setBlurRadius(8);
    resultShadow->setColor(Qt::black);
    resultShadow->setOffset(2, 2);
    resultText->setGraphicsEffect(resultShadow);

    qreal resultWidth = resultText->boundingRect().width();
    resultText->setPos((1280 - resultWidth) / 2, 250);
    resultText->setZValue(10);
    addItem(resultText);

    // 创建按钮样式
    QString buttonStyle =
        "QPushButton {"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "                                      stop: 0 #4CAF50, stop: 1 #45a049);"
        "    border: 3px solid #2E7D32;"
        "    border-radius: 15px;"
        "    color: white;"
        "    font: bold 18px Arial;"
        "    padding: 10px 20px;"
        "    min-width: 150px;"
        "    min-height: 50px;"
        "}"
        "QPushButton:hover {"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "                                      stop: 0 #5CBF60, stop: 1 #4CAF50);"
        "}"
        "QPushButton:pressed {"
        "    background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "                                      stop: 0 #45a049, stop: 1 #3d8b40);"
        "}";

    // 创建返回按钮
    backButton = new QPushButton("返回主菜单");
    backButton->setStyleSheet(buttonStyle);
    QGraphicsProxyWidget *proxyBack = addWidget(backButton);
    proxyBack->setPos(400, 450);
    proxyBack->setZValue(10);

    // 创建重新开始按钮
    restartButton = new QPushButton("再来一局");
    restartButton->setStyleSheet(buttonStyle.replace("#4CAF50", "#2196F3")
                                     .replace("#45a049", "#1976D2")
                                     .replace("#2E7D32", "#1565C0")
                                     .replace("#5CBF60", "#42A5F5")
                                     .replace("#3d8b40", "#1565C0"));
    QGraphicsProxyWidget *proxyRestart = addWidget(restartButton);
    proxyRestart->setPos(680, 450);
    proxyRestart->setZValue(10);

    // 添加提示文字
    QGraphicsTextItem* hintText = new QGraphicsTextItem("按任意键或点击按钮继续");
    QFont hintFont("Arial", 16);
    hintText->setFont(hintFont);
    hintText->setDefaultTextColor(Qt::lightGray);
    qreal hintWidth = hintText->boundingRect().width();
    hintText->setPos((1280 - hintWidth) / 2, 600);
    hintText->setZValue(10);
    addItem(hintText);

    // 绑定按钮点击信号
    connect(backButton, &QPushButton::clicked, this, [this]() {
        emit requestSceneChange(SceneID::IceScene_ID); // 假设有主菜单场景
    });

    connect(restartButton, &QPushButton::clicked, this, [this]() {
        emit requestSceneChange(SceneID::IceScene_ID); // 重新开始游戏
    });

    qDebug() << "GameOverScene created with result:" << result;
}

void GameOverScene::update()
{
    // 游戏结束界面通常不需要持续更新，除非有动画效果等
}

void GameOverScene::keyPressEvent(QKeyEvent *event)
{
    // 监听任意按键，触发返回主菜单
    emit requestSceneChange(SceneID::IceScene_ID);
    qDebug() << "Key pressed in GameOverScene, requesting scene change.";
    Scene::keyPressEvent(event); // 调用基类的keyPressEvent，确保事件被处理
}

void GameOverScene::keyReleaseEvent(QKeyEvent *event)
{
    Scene::keyReleaseEvent(event);
}
