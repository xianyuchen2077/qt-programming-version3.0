#ifndef SETTINGSFIELD_H
#define SETTINGSFIELD_H


#include "Map.h"

class Settingsfield: public Map
{
public:
    explicit Settingsfield(QGraphicsItem *parent= nullptr);

    qreal getFloorHeight() override;
};


#endif // SETTINGSFIELD_H
