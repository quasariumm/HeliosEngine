#include "Audio/AudioPlayer.h"
#include "Debugger/Debugger.h"
#include "Graphics/Camera.h"

using namespace Engine::Audio;

// FMOD does not support MinGW
#if !(defined __MINGW64__ || defined __MINGW32__)
#include <ranges>
#include <fmod/core/inc/fmod.hpp>
#include <fmod/studio/inc/fmod_studio.hpp>


AudioPlayer::AudioPlayer(const Camera* camera)
	: m_camera(camera)
{
    DebugLog(Engine::LogSeverity::INFO, L"Audio Engine: FMOD Studio by Firelight Technologies Pty Ltd.");

    // Create the Studio System object
    FMOD_RESULT result = FMOD::Studio::System::create(&m_system);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, L"Failed to create the FMOD Studio System!");
        return;
    }

    // Initialize FMOD Studio, which will also initialize FMOD Core
    result = m_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, L"Failed to initialize the FMOD Studio System!");
        return;
    }

    // Get the Core System pointer from the Studio System object
    result = m_system->getCoreSystem(&m_core_system);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, L"Failed to get the FMOD Studio System after initialization!");
        return;
    }

    m_core_system->createSoundGroup("SFX", &m_soundGroupSFX);
    m_core_system->createSoundGroup("Music", &m_soundGroupMusic);
}

AudioPlayer::~AudioPlayer()
{
    m_soundGroupSFX->release();
    m_soundGroupMusic->release();

    // TODO
    // for (auto sound : sounds) sound.second->release();
    for (auto val : m_banks | std::views::values) val->unload();

    m_system->release();
}

// TODO: Update to use camera in ECS
void AudioPlayer::Update()
{
    m_system->update();

    // remove released events from the hashmap?
    std::vector<int> eventsToRemove;
    for (auto eventInstance : m_events)
    {
        FMOD_STUDIO_PLAYBACK_STATE state = FMOD_STUDIO_PLAYBACK_STOPPED;
        eventInstance.second->getPlaybackState(&state);
        if (state == FMOD_STUDIO_PLAYBACK_STOPPED) eventsToRemove.push_back(eventInstance.first);
    }

    for (auto id : eventsToRemove) m_events.erase(id);

	// Set the spatial attributes
	FMOD_3D_ATTRIBUTES attributes = { { 0 } };
	const glm::vec3 forward = m_camera->GetCameraFrontVec();
	const glm::vec3 up = m_camera->GetCameraUpVec();
	const glm::vec3 position = m_camera->GetCameraPosition();

	// Front
	attributes.forward.x = forward.x;
	attributes.forward.y = forward.y;
	attributes.forward.z = forward.z;
	// Up
	attributes.up.x = up.x;
	attributes.up.y = up.y;
	attributes.up.z = up.z;
	// Position
	attributes.position.x = position.x;
	attributes.position.y = position.y;
	attributes.position.z = position.z;

	m_system->setListenerAttributes(0, &attributes);
}

void AudioPlayer::LoadBank(const std::filesystem::path& file)
{
    // check if the bank already exists
    const int hash = static_cast<int>(std::hash<std::filesystem::path>{}(file));
    if (m_banks.contains(hash)) return;

    // try to load the bank
    FMOD::Studio::Bank* bank = nullptr;
    const auto result = m_system->loadBankFile(file.string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, std::format(L"FMOD bank with filename {} could not be loaded!", file.wstring()));
        return;
    }

    // load all the bank's sample data immediately
    bank->loadSampleData();
    m_system->flushSampleLoading();  // enable this to wait for loading to finish

    // store the bank by its ID
    m_banks[hash] = bank;
}

void AudioPlayer::UnloadBank(const std::filesystem::path& file)
{
    const int hash = static_cast<int>(std::hash<std::filesystem::path>{}(file));
    if (!m_banks.contains(hash))
    {
        DebugLog(LogSeverity::SEVERE, std::format(L"FMOD bank with filename {} could not be unloaded!", file.wstring()));
        return;
    }

    m_banks[hash]->unload();
    m_banks.erase(hash);
}

int AudioPlayer::StartEvent(const std::string& name, const glm::vec3& position)
{
    // get the event description
    FMOD::Studio::EventDescription* evd = nullptr;
    auto result = m_system->getEvent(("event:/" + name).c_str(), &evd);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, std::format(L"FMOD event with name {} does not exist!", STR_TO_WSTR(name)));
        return -1;
    }

    // create an event instance
    FMOD::Studio::EventInstance* evi = nullptr;
    result = evd->createInstance(&evi);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, std::format(L"FMOD event instance with name {} could not be created!", STR_TO_WSTR(name)));
        return -1;
    }

    const int eventID = m_nextEventID;
    m_events[eventID] = evi;
    ++m_nextEventID;

	// Set the spatial parameters if wanted
	if (position.x > -1e30f)
	{
		FMOD_3D_ATTRIBUTES attributes = {{0}};
		attributes.forward.z = 1.0f;
		attributes.up.y = 1.0f;
		attributes.position.x = position.x;
		attributes.position.y = position.y;
		attributes.position.z = position.z;
		if (evi->set3DAttributes(&attributes) != FMOD_OK)
			DebugLog(LogSeverity::SEVERE, L"Event spatial parameters failed to set.");
	}

    // trigger the event
    result = evi->start();

    // mark it for release immediately
    result = evi->release();

    return eventID;
}

void AudioPlayer::SetParameter(const std::string& name, float value, int eventInstanceID)
{
    if (eventInstanceID < 0)
    {
        m_system->setParameterByName(name.c_str(), (float)value);
    }
    else
    {
        const auto it = m_events.find(eventInstanceID);
        if (it == m_events.end())
        {
            DebugLog(LogSeverity::SEVERE, L"FMOD event with ID {} does not exist!", eventInstanceID);
            return;
        }

        it->second->setParameterByName(name.c_str(), (float)value);
    }
}

void AudioPlayer::SetParameter(const std::string& name, const std::string& value, int eventInstanceID)
{
    if (eventInstanceID < 0)
    {
        m_system->setParameterByNameWithLabel(name.c_str(), value.c_str());
    }
    else
    {
        const auto it = m_events.find(eventInstanceID);
        if (it == m_events.end())
        {
            DebugLog(LogSeverity::SEVERE, L"FMOD event with ID {} does not exist!", eventInstanceID);
            return;
        }

        it->second->setParameterByNameWithLabel(name.c_str(), value.c_str());
    }
}

void AudioPlayer::LoadSound(const std::filesystem::path& file, bool isMusic)
{
    // check if the sound already exists
    const int hash = static_cast<int>(std::hash<std::filesystem::path>{}(file));
    if (m_sounds.contains(hash)) return;

    // try to load the sound file
    const FMOD_MODE mode = isMusic ? (FMOD_CREATESTREAM | FMOD_LOOP_NORMAL) : FMOD_DEFAULT;
    FMOD::Sound* sound = nullptr;
    const FMOD_RESULT result = m_core_system->createSound(file.string().c_str(), mode, nullptr, &sound);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, std::format(L"Sound with filename {} could not be loaded!", file.wstring()));
        return;
    }

    // attach the sound to the right group, and store it by its ID
    sound->setSoundGroup(isMusic ? m_soundGroupMusic : m_soundGroupSFX);
    m_sounds[hash] = sound;
}

#undef PlaySound
int AudioPlayer::PlaySound(const std::filesystem::path& file)
{
    // check if the sound exists
    const int hash = static_cast<int>(std::hash<std::filesystem::path>{}(file));
    const auto sound = m_sounds.find(hash);
    if (sound == m_sounds.end())
    {
        DebugLog(LogSeverity::SEVERE, std::format(L"Sound with filename {} has not been loaded!", file.wstring()));
        return -1;
    }

    // play it
    FMOD::Channel* channel = nullptr;
    m_core_system->playSound(sound->second, nullptr, false, &channel);

    // return the index of the channel on which it plays
    int channel_index = 0;
    channel->getIndex(&channel_index);
    return channel_index;
}

void AudioPlayer::SetChannelPaused(const int channelID, const bool paused) const
{
    FMOD::Channel* channel = nullptr;
    FMOD_RESULT result = m_core_system->getChannel(channelID, &channel);
    if (result != FMOD_OK)
    {
        DebugLog(LogSeverity::SEVERE, L"Sound channel with ID {} does not exist!", channelID);
        return;
    }
    channel->setPaused(paused);
};
#else
AudioPlayer::AudioPlayer(const Camera* camera)
	: m_camera(camera)
{
	DebugLog(LogSeverity::WARNING, L"Audio player does not support MinGW builds (yet). Please build the engine using Visual Studio or clang-cl on Windows or any compiler on Linux.");
}

// The rest is just made no-op

AudioPlayer::~AudioPlayer()
{
}


void AudioPlayer::Update()
{
}


void AudioPlayer::LoadBank( const std::filesystem::path& )
{
}


void AudioPlayer::UnloadBank( const std::filesystem::path& )
{
}


int AudioPlayer::StartEvent( const std::string&, const glm::vec3& )
{
	return 0;
}


void AudioPlayer::SetParameter( const std::string&, float, int )
{
}


void AudioPlayer::SetParameter( const std::string&, const std::string&, int )
{
}


void AudioPlayer::LoadSound( const std::filesystem::path&, bool )
{
}


#undef PlaySound
int AudioPlayer::PlaySound( const std::filesystem::path& )
{
	return 0;
}


void AudioPlayer::SetChannelPaused( int, bool ) const
{
}

#endif