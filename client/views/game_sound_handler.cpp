#include "game_sound_handler.h"

void GameSoundHandler::playShotSound(float distance, WeaponName weapon) {
    if (!mixer) return;

    uint32_t now = SDL_GetTicks();

    switch (weapon) {
        case AK47:
            if (!akSound || now - akSoundCooldown <= GUNSHOT_SOUND_COOLDOWN) return;
            akSoundCooldown = now;
            adjustVolume(*akSound, distance);
            mixer->PlayChannel(-1, *akSound, 0);
            break;

        case M3:
            if (!m3Sound || now - m3SoundCooldown <= GUNSHOT_SOUND_COOLDOWN) return;
            m3SoundCooldown = now;
            adjustVolume(*m3Sound, distance);
            mixer->PlayChannel(-1, *m3Sound, 0);
            break;

        case AWP:
            if (!awpSound || now - awpSoundCooldown <= GUNSHOT_SOUND_COOLDOWN) return;
            awpSoundCooldown = now;
            adjustVolume(*awpSound, distance);
            mixer->PlayChannel(-1, *awpSound, 0);
            break;

        case GLOCK:
            if (!glockSound || now - glockSoundCooldown <= GUNSHOT_SOUND_COOLDOWN) return;
            glockSoundCooldown = now;
            adjustVolume(*glockSound, distance);
            mixer->PlayChannel(-1, *glockSound, 0);
            break;

        case KNIFE:
            if (!knifeSound) return;
            adjustVolume(*knifeSound, distance);
            mixer->PlayChannel(-1, *knifeSound, 0);
            break;

        default:
            break;
    }
}

void GameSoundHandler::playBombSound(float distance) {
    if (!mixer || !bombSound) return;
    adjustVolume(*bombSound, distance);
    mixer->PlayChannel(-1, *bombSound, 0);
}

void GameSoundHandler::playDeathSound(float distance) {
    if (!mixer || !deathSound) return;
    adjustVolume(*deathSound, distance);
    mixer->PlayChannel(-1, *deathSound, 0);
}

void GameSoundHandler::playMusic() {}

void GameSoundHandler::adjustVolume(Chunk& sound, float distance) {
    int volume = (distance >= MAX_SOUND_DISTANCE)
        ? 0
        : MIX_MAX_VOLUME - (distance / MAX_SOUND_DISTANCE) * MIX_MAX_VOLUME;
    Mix_VolumeChunk(sound.Get(), volume);
}
