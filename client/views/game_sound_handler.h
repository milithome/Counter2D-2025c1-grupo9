#ifndef GAMESOUND_H
#define GAMESOUND_H

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "common/structures.h"

using namespace SDL2pp;

class GameSoundHandler {
    public:
        GameSoundHandler() {
            mixer.SetVolume(-1, MIX_MAX_VOLUME); 
        }   

        void playShotSound(float distance, WeaponName weapon);

        void playBombSound(float distance);
        
        void playDeathSound(float distance);

    private:
        Mixer mixer = Mixer(44100, MIX_DEFAULT_FORMAT, 2, 2048);
        int soundsPlaying = 0;
}

#endif