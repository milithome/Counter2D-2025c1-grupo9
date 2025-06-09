#include <SDL2/SDL.h>
#include "editor.h"
#include "editorMenuWindow.h"

int main(int argc, char* argv[]) {
    
    //Editor editor;    

    QApplication app(argc, argv);

    EditorMenuWindow window;
    window.resize(1000, 700);
    window.show();
    window.setObjectName("MainEditorWindow");
    window.setStyleSheet(
        "QWidget#MainEditorWindow {"
        "  background-image: url(:/assets/gfx/cs2d.png);"
        "  background-repeat: no-repeat;"
        "  background-position: center;"
        "}"
    );

    return app.exec();

    return 0;
}
