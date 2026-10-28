// This will manage to create the callback of world left click, 
// which will create an object editor object. 
// At first the object editor will show boundaries of the selected object. 
#pragma once

#include "ObjectManager.h"

class ObjectEditor
{
public:
    static ObjectEditor& getInstance();

    /**
     * \brief Update the selected object based on the given position.
     * \param position The position to update the selected object with.
     */
    static void updateSelectedObject(point2D position);

private:
    ObjectEditor();

    void updateBoundaries();

    static ObjectEditor* instance;
    ObjectPtr selectedObject;
    ObjectPtr boundariesObject;
    
};
