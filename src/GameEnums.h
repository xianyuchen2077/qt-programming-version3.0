#ifndef GAMEENUMS_H
#define GAMEENUMS_H

#include <QMetaType>

// 使用枚举类来定义场景ID
enum class SceneID
{
    BattleScene_ID = 0,
    IceScene_ID = 1,
    GameOverScene_ID = 666,
    SettingsScene_ID = 999,
    PreviousScene_ID = 1000 // 假设用于返回上一个场景，需要MyGame内部维护
};

#endif // GAMEENUMS_H
