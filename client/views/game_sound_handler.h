#ifndef GAMESOUND_H
#define GAMESOUND_H

#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>
#include "common/structures.h"
#include <memory>

using namespace SDL2pp;

#define MAX_SOUND_DISTANCE 10
#define GUNSHOT_SOUND_COOLDOWN 80

class GameSoundHandler {
public:
    GameSoundHandler(bool pulse_available) : pulse_available(pulse_available) {
        if (pulse_available) {
            mixer = std::make_unique<Mixer>(44100, MIX_DEFAULT_FORMAT, 2, 2048);

            glockSound = std::make_unique<Chunk>(BASE_PATH + "sfx/weapons/glock18.wav");
            awpSound = std::make_unique<Chunk>(BASE_PATH + "sfx/weapons/awp.wav");
            akSound = std::make_unique<Chunk>(BASE_PATH + "sfx/weapons/ak47.wav");
            m3Sound = std::make_unique<Chunk>(BASE_PATH + "sfx/weapons/m3.wav");
            knifeSound = std::make_unique<Chunk>(BASE_PATH + "sfx/weapons/knife_slash.wav");
            knifeHit = std::make_unique<Chunk>(BASE_PATH + "sfx/weapons/knife_hit.wav");
            bombSound = std::make_unique<Chunk>(BASE_PATH + "sfx/weapons/c4_explode.wav");
            deathSound = std::make_unique<Chunk>(BASE_PATH + "sfx/player/die1.wav");

            mixer->SetVolume(-1, MIX_MAX_VOLUME);
        }
    }

    void playShotSound(float distance, WeaponName weapon);
    void playBombSound(float distance);
    void playDeathSound(float distance);
    void playMusic();

    void switchMute() {
            if (mixer->GetVolume(-1) <= 0) {
                unmute();
            } else {
                mute();
            }
    }

    void mute() {
        if (mixer) mixer->SetVolume(-1, 0);
    }

    void unmute() {
        if (mixer) mixer->SetVolume(-1, MIX_MAX_VOLUME);
    }

private:
    bool pulse_available;
    std::unique_ptr<Mixer> mixer;
    const std::string BASE_PATH = "/var/taller/";

    std::unique_ptr<Chunk> glockSound;
    std::unique_ptr<Chunk> awpSound;
    std::unique_ptr<Chunk> akSound;
    std::unique_ptr<Chunk> m3Sound;
    std::unique_ptr<Chunk> knifeSound;
    std::unique_ptr<Chunk> knifeHit;
    std::unique_ptr<Chunk> bombSound;
    std::unique_ptr<Chunk> deathSound;

    int akSoundCooldown = 0;
    int m3SoundCooldown = 0;
    int awpSoundCooldown = 0;
    int glockSoundCooldown = 0;

    void adjustVolume(Chunk& sound, float distance);
};

#endif
