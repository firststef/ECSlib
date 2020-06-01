#pragma once
#include <vector>
#include "Component.h"
#include "Entity.h"

struct GridContainerComponent : IComponent
{
    //Engine-related
    bool needsUpdate = true;

    //System-related
    enum ItemSetMode
    {
        FIXED_GET_FIRST_AVAILABLE,
        FIXED_SET_IN_PLACE,
        DYNAMIC_ERASE_SPACES,
        INFINITE_STACK
    } itemSetMode = FIXED_GET_FIRST_AVAILABLE;

    int                                     numOfColumns = 1;
    int                                     numOfLines = 1;

    int                                     maxNumOfColumns = 1;
    int                                     maxNumOfLines = 1;

    float                                   marginLeft = 0;
    float                                   marginUp = 0;
    float                                   marginRight = 0;
    float                                   marginDown = 0;

    float                                   spaceBetween = 0;

    bool                                    stretchEnabled = false;
    bool                                    reversedPositions = false;

    std::vector<Rectangle>                  positionTable;

    int                                     numberOfContainedElements = 0;
    std::vector<EntityPtr>                  items;

    GridContainerComponent(int columns, int lines, float left, float up, float right, float down, float space,
        bool stretch = false, ItemSetMode itemSetMode = FIXED_GET_FIRST_AVAILABLE, bool reversed = false)
    : itemSetMode(itemSetMode), numOfColumns(columns), numOfLines(lines), maxNumOfColumns(columns), maxNumOfLines(lines),
        marginLeft(left), marginUp(up), marginRight(right), marginDown(down), spaceBetween(space), stretchEnabled(stretch),
        reversedPositions(reversed)
    {
    }
};
