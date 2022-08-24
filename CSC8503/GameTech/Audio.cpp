#ifdef _x64
#include "Audio.h"
#include "../CSC8503Common/GameObject.h"

using namespace NCL;
using namespace CSC8503;

FMOD::System* audio::fmod_system;

FMOD::Sound* audio::BGM = nullptr;
FMOD::Channel* audio::BGM_channel = nullptr;

FMOD::Sound* audio::step = nullptr;
std::vector<FMOD::Channel*> audio::step_channels;

FMOD::Sound* audio::jump = nullptr;
std::vector<FMOD::Channel*> audio::jump_channels;

FMOD::Sound* audio::shoot = nullptr;
std::vector<FMOD::Channel*> audio::shoot_channels;

FMOD::Sound* audio::stick = nullptr;
FMOD::Channel* audio::stick_channel = nullptr;

void audio::Init() {
    FMOD_RESULT result;
    unsigned int      version;

    void* extradriverdata = 0;
    Common_Init(&extradriverdata);

    result = FMOD::System_Create(&fmod_system);
    ERRCHECK(result);

    result = fmod_system->init(64, FMOD_INIT_NORMAL, extradriverdata);
    ERRCHECK(result);

    step_channels.resize(2);
    jump_channels.resize(2);
    shoot_channels.resize(2);
}

void audio::UpdateAudio(const Maths::NCLVector3& pos_one, const Maths::NCLVector3& pos_two) {
    FMOD_VECTOR audio_source_pos_one = { pos_one.x, pos_one.y, pos_one.z };
    FMOD_VECTOR audio_source_pos_two = { pos_two.x, pos_two.y, pos_two.z };

    FMOD_VECTOR vel = { 1.0f, 1.0f, 1.0f };

    (*step_channels[0]).set3DAttributes(&audio_source_pos_one, &vel);
    (*step_channels[1]).set3DAttributes(&audio_source_pos_two, &vel);

    (*jump_channels[0]).set3DAttributes(&audio_source_pos_one, &vel);
    (*jump_channels[1]).set3DAttributes(&audio_source_pos_two, &vel);

    (*shoot_channels[0]).set3DAttributes(&audio_source_pos_one, &vel);
    (*shoot_channels[1]).set3DAttributes(&audio_source_pos_two, &vel);

    SetListener(pos_one, pos_two);
}

void audio::SetListener(const NCLVector3& pos_one, const NCLVector3& pos_two) {
    FMOD_VECTOR vel = { 1.0f, 1.0f, 1.0f };
    FMOD_VECTOR audio_listener_pos_one = { pos_one.x, pos_one.y, pos_one.z };
    FMOD_VECTOR audio_listener_pos_two = { pos_two.x, pos_two.y, pos_two.z };

    FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };

    fmod_system->set3DNumListeners(2);
    fmod_system->set3DListenerAttributes(0, &audio_listener_pos_two, &vel, &forward, &up);
    fmod_system->set3DListenerAttributes(1, &audio_listener_pos_two, &vel, &forward, &up);
}

FMOD_RESULT audio::Create_Sound
(const char* file, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* ex_info, FMOD::Sound** sound) {
    return fmod_system->createSound(file, mode, ex_info, sound);
}

FMOD_RESULT audio::Play_Sound
(FMOD::Sound* sound, FMOD::ChannelGroup* channel_group, bool paused, FMOD::Channel** channel){
    return fmod_system->playSound(sound, channel_group, paused, channel);
}

void audio::LoadAudio() {
    FMOD_RESULT result;
    result = audio::Create_Sound(Common_MediaPath("BGM.wav"), FMOD_DEFAULT, 0, &BGM);
    ERRCHECK(result);
    result = audio::Create_Sound(Common_MediaPath("FootStep.wav"), FMOD_LOOP_OFF | FMOD_3D, 0, &step);
    ERRCHECK(result);
    result = audio::Create_Sound(Common_MediaPath("Jump.wav"), FMOD_LOOP_OFF | FMOD_3D, 0, &jump);
    ERRCHECK(result);
    result = audio::Create_Sound(Common_MediaPath("Shoot.wav"), FMOD_LOOP_OFF | FMOD_3D, 0, &shoot);
    ERRCHECK(result);
    result = audio::Create_Sound(Common_MediaPath("Stick.wav"), FMOD_LOOP_OFF | FMOD_3D, 0, &stick);
    ERRCHECK(result);

}

void audio::PlayAudio(FMOD::Sound* sound, FMOD::Channel** channel) {
    FMOD_RESULT result;
    bool paused = false;
    result = (*channel)->getPaused(&paused);
    if (paused) 
        result = (*channel)->setPaused(false);
    else
        result = audio::Play_Sound(sound, nullptr, false, channel);

   /* switch (result) {
    case FMOD_OK:
        if (paused) {
            result = (*channel)->setPaused(false);
        }
        break;
    case FMOD_ERR_INVALID_PARAM:
    case FMOD_ERR_INVALID_HANDLE:
    case FMOD_ERR_CHANNEL_STOLEN:
        result = audio::Play_Sound(sound, nullptr, false, channel);
=        break;
    default:
        break;
    }*/
    ERRCHECK(result);
}

void audio::StopAudio(FMOD::Channel** channel) {
    FMOD_RESULT result;
    result = (*channel)->stop();
    if (result == FMOD_OK) {
        return;
    }
}

void audio::PlayStep(const int& i) {
    FMOD_RESULT result;
    bool paused = false;
    result = (*step_channels[i]).getPaused(&paused);
    switch (result)
    {
    case FMOD_OK:
        if (paused) {
            result = (*step_channels[i]).setPaused(false);
        }
        break;
    case FMOD_ERR_INVALID_PARAM:
    case FMOD_ERR_INVALID_HANDLE:
    case FMOD_ERR_CHANNEL_STOLEN:
        result = audio::Play_Sound(step, nullptr, false, &step_channels[i]);
        break;
    default:
        break;
    }
    //PlayAudio(step, &step_channels[i]);
    (*step_channels[i]).setVolume(3.5f);
}

void audio::StopStep(const int& i) {
    StopAudio(&step_channels[i]);
}

void audio::PlayJump(const int& i) {
    PlayAudio(jump, &jump_channels[i]);
    (*jump_channels[i]).setVolume(3.5f);
}

void audio::StopJump(const int& i) {
    StopAudio(&jump_channels[i]);
}

void audio::PlayShoot(const int& i) {
    PlayAudio(shoot, &shoot_channels[i]);
    (*shoot_channels[i]).setVolume(2.0f);
}

void audio::StopShoot(const int& i) {
    StopAudio(&shoot_channels[i]);
}

void audio::PlayStick(const Maths::NCLVector3& pos) {
    FMOD_VECTOR audio_source_pos = { pos.x, pos.y, pos.z };
    FMOD_VECTOR vel = { 1.0f, 1.0f, 1.0f };

    (*stick_channel).set3DAttributes(&audio_source_pos, &vel);
    (*stick_channel).setVolume(3.5f);

    PlayAudio(stick, &stick_channel);
}
#endif