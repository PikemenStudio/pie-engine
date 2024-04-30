//
// Created by anton on 18.4.24.
//

#ifndef ENGINE_SOUND_MODULE_HPP
#define ENGINE_SOUND_MODULE_HPP

#include <gtest/gtest.h>
#include <sound/facade/facade.hpp>

class OpenALTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    ASSERT_NO_THROW(Sound = new SoundFacade);
  }

  static void TearDownTestSuite() {
    delete Sound;
  }

  static inline SoundFacade* Sound;
};

// Test case to list all Vulkan devices
TEST_F(OpenALTest, TestSoundLoadingPlaying) {
  ASSERT_NO_THROW({
    SoundStructs::SoundID SoundID;
    SoundID = Sound->ImplInstance.loadSound("../../tests/resources/sound_test_mono.wav");
    ASSERT_TRUE(SoundID != 0);

    SoundStructs::SoundSourceID Source = Sound->ImplInstance.createSoundSource(
        SoundStructs::SoundSourceParams {
            .PosX = 0, .PosY = 0, .PosZ = 0,
            .VelX = 0, .VelY = 0, .VelZ = 0,
            .Looping = false,
            .SndID = SoundID
        });
    ASSERT_TRUE(Source != 0);

    Sound->ImplInstance.playSoundSource(Source);
    ASSERT_TRUE(Sound->ImplInstance.isPlaying(Source));
  });
}

#endif // ENGINE_SOUND_MODULE_HPP
