#ifndef GAMESOUND_H
#define GAMESOUND_H

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "common/structures.h"

using namespace SDL2pp;


#define MAX_SOUND_DISTANCE 20
#define GUNSHOT_SOUND_COOLDOWN 80

class GameSoundHandler {
    public:

        GameSoundHandler() {
            mixer.SetVolume(-1, MIX_MAX_VOLUME); 
        }   

        void playShotSound(float distance, WeaponName weapon);

        void playBombSound(float distance);
        
        void playDeathSound(float distance);

        void playMusic();

        void switchMute() {
            if (mixer.GetVolume(-1) <= 0) {
                unmute();
            } else {
                mute();
            }
        }
        void mute() {
            mixer.SetVolume(-1, 0); 
        }

        void unmute() {
            mixer.SetVolume(-1, MIX_MAX_VOLUME); 
        }

    private:
        Mixer mixer = Mixer(44100, MIX_DEFAULT_FORMAT, 2, 2048);


        Chunk glockSound = Chunk("../assets/sfx/weapons/glock18.wav");
        Chunk awpSound = Chunk("../assets/sfx/weapons/awp.wav");
        Chunk akSound = Chunk("../assets/sfx/weapons/ak47.wav");
        Chunk m3Sound = Chunk("../assets/sfx/weapons/m3.wav");
        Chunk knifeSound = Chunk("../assets/sfx/weapons/knife_slash.wav");
        Chunk knifeHit = Chunk("../assets/sfx/weapons/knife_hit.wav");

        Chunk bombSound = Chunk("../assets/sfx/weapons/c4_explode.wav");
        Chunk deathSound = Chunk("../assets/sfx/player/die1.wav");

        int akSoundCooldown = 0;
        int m3SoundCooldown = 0;
        int awpSoundCooldown = 0;
        int glockSoundCooldown = 0;

        void adjustVolume(Chunk& sound, float distance);
};

#endif