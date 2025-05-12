#ifndef VIEW_H
#define VIEW_H
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

class View {
public:
    View();        // Constructor

    virtual void init();   
    virtual void update(Uint32 deltaTime); // Método de actualización
    void end();    // Método de finalización / limpieza
};

#endif // VIEW_H