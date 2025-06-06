#ifndef EDITOR_H
#define EDITOR_H

#include "client/views/qtwindow.h"
#include "client/controllers/menu_controller.h"

#include <iostream>
#include <string>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

class Editor {
public:
    // Constructor
    Editor();

    // Public methods
    void run();

private:
    // Private attributes
    MenuController menuController;

    // Private methods
    
};
#endif // EDITOR_H
