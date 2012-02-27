#include <xd/audio/music.hpp>
#include <xd/audio/exceptions.hpp>
#include <SFML/Audio.hpp>
#include <memory>

namespace xd { namespace detail {
	
	struct music_handle
	{
		sf::Music music;
	};

} }

xd::music::music(const std::string& filename)
{
	// load music from file
	auto handle = std::unique_ptr<detail::music_handle>(new detail::music_handle);
	if (!handle->music.OpenFromFile(filename))
		throw audio_file_load_failed(filename);
	// all ok, release the memory to the real handle
	m_handle = handle.release();
}

xd::music::~music()
{
	delete m_handle;
}

void xd::music::play()
{
	m_handle->music.Play();
}

void xd::music::pause()
{
	m_handle->music.Pause();
}

void xd::music::stop()
{
	m_handle->music.Stop();
}

bool xd::music::playing()
{
	return m_handle->music.GetStatus() == sf::SoundSource::Playing;
}

bool xd::music::paused()
{
	return m_handle->music.GetStatus() == sf::SoundSource::Paused;
}

bool xd::music::stopped()
{
	return m_handle->music.GetStatus() == sf::SoundSource::Stopped;
}

void xd::music::set_offset(float offset)
{
	m_handle->music.SetPlayingOffset(sf::Seconds(offset));
}

void xd::music::set_volume(float volume)
{
	m_handle->music.SetVolume(volume);
}

void xd::music::set_pitch(float pitch)
{
	m_handle->music.SetPitch(pitch);
}

void xd::music::set_looping(bool looping)
{
	m_handle->music.SetLoop(looping);
}

void xd::music::set_loop_point(float time)
{
	throw xd::exception("unimplemented");
}

float xd::music::get_offset()
{
	return m_handle->music.GetPlayingOffset().AsSeconds();
}

float xd::music::get_volume()
{
	return m_handle->music.GetVolume();
}

float xd::music::get_pitch()
{
	return m_handle->music.GetPitch();
}

bool xd::music::get_looping()
{
	return m_handle->music.GetLoop();
}

float xd::music::get_loop_point()
{
	throw xd::exception("unimplemented");
}
