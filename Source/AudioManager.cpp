//
//  AudioManager.cpp
//  GEngine
//
//  Created by Clark Kromenaker on 8/23/17.
//
#include "AudioManager.h"

#include <iostream>

#include "fmod_errors.h"

#include "Vector3.h"

#include "Audio.h"

bool AudioManager::Initialize()
{
	// Create the FMOD system.
    FMOD_RESULT result = FMOD::System_Create(&mSystem);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
        return false;
    }
	
	// Retrieve the FMOD version.
    unsigned int version;
    result = mSystem->getVersion(&version);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
        return false;
    }
	
	// Verify that the FMOD library version matches the header version.
    if(version < FMOD_VERSION)
    {
        std::cout << "FMOD lib version " << version << " doesn't match header version " <<  FMOD_VERSION << std::endl;
        return false;
    }
	
	// Initialize the FMOD system.
    void* extradriverdata = 0;
    result = mSystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
        return false;
    }
    
    // We initialized audio successfully!
    return true;
}

void AudioManager::Shutdown()
{
	// Close and release FMOD system.
    FMOD_RESULT result = mSystem->close();
    result = mSystem->release();
}

void AudioManager::Update(float deltaTime)
{
    mSystem->update();
}

void AudioManager::Play(Audio* audio)
{
    Play(audio, 0);
}

void AudioManager::Play(Audio* audio, int fadeInMs)
{
    if(audio == nullptr) { return; }
	
	// Need to pass FMOD the length of our audio.
    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = audio->GetDataBufferLength();
	
	// Create the sound using the audio buffer.
    FMOD::Sound* sound1 = nullptr;
    FMOD_RESULT result = mSystem->createSound((const char*)audio->GetDataBuffer(), FMOD_OPENMEMORY | FMOD_LOOP_OFF, &exinfo, &sound1);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
        return;
    }
	
	// Play the sound, which returns the channel being played on.
	FMOD::Channel* channel = nullptr;
    result = mSystem->playSound(sound1, 0, false, &channel);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
        return;
    }
}

void AudioManager::Play3D(Audio *audio, const Vector3 &position)
{
    if(audio == nullptr) { return; }
    
    FMOD_CREATESOUNDEXINFO exinfo;
    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    exinfo.length = audio->GetDataBufferLength();
	
	FMOD::Sound* sound1 = nullptr;
    FMOD_RESULT result = mSystem->createSound((const char*)audio->GetDataBuffer(), FMOD_OPENMEMORY | FMOD_LOOP_OFF | FMOD_3D, &exinfo, &sound1);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
        return;
    }
    
    // Position at the correct location.
    //channel->set3DAttributes((const FMOD_VECTOR*) &position, (const FMOD_VECTOR*) &Vector3::Zero);
    //channel->set3DMinMaxDistance(0.0f, 100000.0f);
    //channel->setLoopCount(100);
	
	FMOD::Channel* channel = nullptr;
    result = mSystem->playSound(sound1, 0, false, &channel);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
        return;
    }
}

void AudioManager::UpdateListener(const Vector3& position, const Vector3& velocity, const Vector3& forward, const Vector3& up)
{
    FMOD_RESULT result = mSystem->set3DListenerAttributes(0, (const FMOD_VECTOR*)&position, (const FMOD_VECTOR*)&velocity,
                                                         (const FMOD_VECTOR*)&forward, (const FMOD_VECTOR*)&up);
    if(result != FMOD_OK)
    {
        std::cout << FMOD_ErrorString(result) << std::endl;
    }
}
