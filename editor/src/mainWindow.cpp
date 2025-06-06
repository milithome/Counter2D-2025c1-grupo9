#include "mainWindow.h"

MainWindow::MainWindow(int argc, char *argv[]):
    app(argc, argv),
    menuWindow(QtWindow(app, "Editor de mapas", SCREEN_WIDTH, SCREEN_HEIGHT)),
    
{
}

MainWindow::~MainWindow() {
    if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
    if (sdlWindow) SDL_DestroyWindow(sdlWindow);
}

void MainWindow::initSDLInWidget() {

    MainWindow::MainWindow(int argc, char *argv[])
    {
    }
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    sdlWindow = SDL_CreateWindowFrom((void*)sdlWidget->winId());
    if (!sdlWindow) {
        qFatal("SDL_CreateWindowFrom falló: %s", SDL_GetError());
    }

    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!sdlRenderer) {
        qFatal("SDL_CreateRenderer falló: %s", SDL_GetError());
    }
}