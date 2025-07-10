#ifndef QT_PROGRAMMING_2024_LINK_H
#define QT_PROGRAMMING_2024_LINK_H


#include "Character.h"

class Link : public Character
{
public:
    explicit Link(QGraphicsItem *parent = nullptr);

    void setCrouchPixmap(); // 下蹲动作
    void setStandPixmap(); // 站立动作

    void processInput() override; // 处理输入

private:

};


#endif //QT_PROGRAMMING_2024_LINK_H
