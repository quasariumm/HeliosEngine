#pragma once

#include <string>
#include <unordered_map>
#undef PlaySound

namespace FMOD
{
class System;
class Sound;
class SoundGroup;


namespace Studio
{
	class System;
	class Bank;
	class EventInstance;
} // namespace Studio
} // namespace FMOD

namespace Engine
{
class Camera;
}

namespace Engine::Audio
{

/**
 * A class with a number of functions for audio handling in Bee.
 * The implementation uses FMOD Studio. We recommend to handle your game's audio using FMOD Studio banks and events.
 * For convenience, we have also added functions for simple sound playback using FMOD Core only.
 */
class AudioPlayer
{
public:

	AudioPlayer(const Camera* camera);

	~AudioPlayer();

	/**
	 * @brief Updates the events and spatial attributes.
	 */
	void Update();

	/**
	 * @brief Loads an FMOD Studio bank with the given filename.
	 * @param file The name of the file that contains the bank.
	 */
	void LoadBank( const std::filesystem::path& file );

	/**
	 * @brief Unloads an FMOD Studio bank with the given filename.
	 * @param file The name of the file that contains the bank.
	 */
	void UnloadBank( const std::filesystem::path& file );

	/**
	 * @brief Starts a new instance of an FMOD Studio event with the given name.
	 * @param name The name of the event to start, without the prefix "event:/".
	 * @param position The world position of the event.
	 * @returns The ID of the newly created event instance.
	 *		If the operation failed, this function returns -1 and prints information to the console.
	 */
	int StartEvent( const std::string& name, const glm::vec3& position = glm::vec3(-INFINITY) );

	/**
	 * @brief Sets an FMOD Studio floating-point parameter to the given value.
	 *
	 * @param name The name of the parameter to set.
	 * @param value The new desired value of the parameter.
	 * @param eventInstanceID (Optional) The ID of the event instance to which the parameter is associated.
	 *		Use -1 to set a system-wide parameter that is not tied to a specific event.
	 */
	void SetParameter( const std::string& name, float value, int eventInstanceID = -1 );

	/**
	 * @brief Sets an FMOD Studio string parameter to the given value (label).
	 *
	 * @param name The name of the parameter to set.
	 * @param value The new desired value of the parameter.
	 * @param eventInstanceID (Optional) The ID of the event instance to which the parameter is associated.
	 *		Use -1 to set a system-wide parameter that is not tied to a specific event.
	 */
	void SetParameter( const std::string& name, const std::string& value, int eventInstanceID = -1 );

	/**
	 * @brief Loads a sound file for playback.
	 * @attention This uses FMOD Core only and offers limited control.
	 *		We recommend to use FMOD Studio banks and events for all audio handling.
	 * @param file The name of the sound file to load.
	 * @param isMusic Whether the file should be treated as a music track (instead of a sound effect).
	 *		Music tracks are loaded and buffered differently.
	 */
	void LoadSound( const std::filesystem::path& file, bool isMusic );

	/**
	 * @brief Plays a previously loaded sound file, and returns the channel on which it will play.
	 * @attention This uses FMOD Core only and offers limited control.
	 *		We recommend to use FMOD Studio banks and events for all audio handling.
	 * @param file The name of the sound file to play.</param>
	 * @returns The ID of the channel on which the sound will play.
	 *		If the operation failed, this function returns -1 and prints information to the console.
	 */
	int PlaySound( const std::filesystem::path& file );

	/**
	 * @brief Pauses or unpauses a sound channel.
	 * @attention This uses FMOD Core only and offers limited control.
	 *		We recommend using FMOD Studio banks and events for all audio handling.
	 * @param channelID The ID of the channel to pause or unpause.
	 * @param paused true if the channel should be paused; false if it should resume playing.
	 */
	void SetChannelPaused(int channelID, bool paused) const;

private:

	const Camera* m_camera;

	FMOD::Studio::System* m_system = nullptr;
	FMOD::System* m_core_system = nullptr;

	std::unordered_map<int, FMOD::Sound*> m_sounds;
	FMOD::SoundGroup* m_soundGroupSFX = nullptr;
	FMOD::SoundGroup* m_soundGroupMusic = nullptr;

	std::unordered_map<int, FMOD::Studio::Bank*> m_banks;
	std::unordered_map<int, FMOD::Studio::EventInstance*> m_events;
	int m_nextEventID = 0;
};
} // namespace bee