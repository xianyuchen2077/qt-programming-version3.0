#include "Settingsfield.h"

Settingsfield::Settingsfield(QGraphicsItem *parent) : Map(parent, ":/Items/Maps/Scenicpicture/SettingsScene.png") {}

qreal Settingsfield::getFloorHeight()
{
    auto sceneRect = sceneBoundingRect();
    return (sceneRect.top() + sceneRect.bottom()) * 0.63;
}
