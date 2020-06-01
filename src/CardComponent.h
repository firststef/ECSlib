#pragma once
#include "Component.h"
#include "../GameServer/Card.h"

struct CardComponent : IComponent
{
    Card card;

    CardComponent(Card card) : card(card) {}
};
