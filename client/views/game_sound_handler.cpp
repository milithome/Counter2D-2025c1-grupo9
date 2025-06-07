#include "game_sound_handler.h"  
        
void GameSoundHandler::playShotSound(float distance, WeaponName weapon) {


    switch (weapon) {
        case AK47: {
            uint32_t now = SDL_GetTicks();
            if (now - akSoundCooldown > GUNSHOT_SOUND_COOLDOWN) {
                akSoundCooldown = now;
            } else {
                return;
            }

            adjustVolume(akSound, distance);
            mixer.PlayChannel(-1, akSound, 0);
            break;
        }
        case M3: {
            uint32_t now = SDL_GetTicks();
            if (now - m3SoundCooldown > GUNSHOT_SOUND_COOLDOWN) {
                m3SoundCooldown = now;
            } else {
                return;
            }


            adjustVolume(m3Sound, distance);
            mixer.PlayChannel(-1, m3Sound, 0);
            break;
        }
        case AWP: {
            uint32_t now = SDL_GetTicks();
            if (now - awpSoundCooldown > GUNSHOT_SOUND_COOLDOWN) {
                awpSoundCooldown = now;
            } else {
                return;
            }

            adjustVolume(awpSound, distance);
            mixer.PlayChannel(-1, awpSound, 0);
            break;
        }
        case GLOCK: {
            uint32_t now = SDL_GetTicks();
            if (now - glockSoundCooldown > GUNSHOT_SOUND_COOLDOWN) {
                glockSoundCooldown = now;
            } else {
                return;
            }


            adjustVolume(glockSound, distance);
            mixer.PlayChannel(-1, glockSound, 0);
            break;
        }
        case KNIFE: {
            adjustVolume(knifeSound, distance);
            mixer.PlayChannel(-1, knifeSound, 0);
            break;
        }
        default: {
            break;
        }
    }
}

void GameSoundHandler::playBombSound(float distance) {
    adjustVolume(bombSound, distance);
    mixer.PlayChannel(-1, bombSound, 0);
};
        
void GameSoundHandler::playDeathSound(float distance) {
    adjustVolume(deathSound, distance);
    mixer.PlayChannel(-1, deathSound, 0);
};

void GameSoundHandler::playMusic() {

};

void GameSoundHandler::adjustVolume(Chunk& sound, float distance) {
    int volume;
    if (distance >= MAX_SOUND_DISTANCE) {
        volume = 0;
    } else {
        volume = MIX_MAX_VOLUME - (distance / MAX_SOUND_DISTANCE) * MIX_MAX_VOLUME;
    }
    Mix_VolumeChunk(sound.Get(), volume);
}