#include "game_sound_handler.h"  
        
void GameSoundHandler::playShotSound(float distance, WeaponName weapon) {
    switch (weapon) {
        case AK47: {
            mixer.PlayChannel(-1, akSound, 0);
            break;
        }
        case M3: {
            mixer.PlayChannel(-1, m3Sound, 0);
            break;
        }
        case AWP: {
            mixer.PlayChannel(-1, awpSound, 0);
            break;
        }
        case GLOCK: {
            mixer.PlayChannel(-1, glockSound, 0);
            break;
        }
        case KNIFE: {
            mixer.PlayChannel(-1, knifeSound, 0);
            break;
        }
    }
}

void GameSoundHandler::playBombSound(float distance) {

};
        
void GameSoundHandler::playDeathSound(float distance) {

};

void GameSoundHandler::playMusic() {

};