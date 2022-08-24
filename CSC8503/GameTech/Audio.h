#pragma once
#ifdef _x64
#include <fmod.hpp>
#include "common.h"
#include "../../Common/NCLVector3.h"
#include <vector>

#define MEDIA "../media/"

namespace NCL {
	namespace CSC8503 {
		class audio {
		public:
			static void Init();
			static void UpdateAudio(const Maths::NCLVector3& pos_one, const Maths::NCLVector3& pos_two);
			static void LoadAudio();

			static void PlayBGM() { PlayAudio(BGM, &BGM_channel); (*BGM_channel).setVolume(0.5); }

			static void PauseBGM(bool isPause) { BGM_channel->setPaused(isPause); }

			static void PlayStep(const int &player);
			static void StopStep(const int &player);

			static void PlayJump(const int& player);
			static void StopJump(const int& player);

			static void PlayShoot(const int& player);
			static void StopShoot(const int& player);

			static void PlayStick(const Maths::NCLVector3& pos);

			static void SetListener(const Maths::NCLVector3& pos_one, const Maths::NCLVector3& pos_two);
		private:
			static FMOD::System* fmod_system;

			static FMOD_RESULT Create_Sound
			(const char* file, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* ex_info, FMOD::Sound** sound);
			static FMOD_RESULT Play_Sound
			(FMOD::Sound* sound, FMOD::ChannelGroup* channel_group, bool paused, FMOD::Channel** channel);

			//Audio
			static FMOD::Sound* BGM;
			static FMOD::Channel* BGM_channel;

			static FMOD::Sound* stick;
			static FMOD::Channel* stick_channel;

			static FMOD::Sound* step;
			static std::vector<FMOD::Channel*> step_channels;

			static FMOD::Sound* jump;
			static std::vector<FMOD::Channel*> jump_channels;

			static FMOD::Sound* shoot;
			static std::vector<FMOD::Channel*> shoot_channels;
			//
			static void PlayAudio(FMOD::Sound* sound, FMOD::Channel** channel);
			static void StopAudio(FMOD::Channel** channel);
		};
	}
}
#endif