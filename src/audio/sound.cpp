#include <xd/audio/sound.hpp>
#include <xd/audio/exceptions.hpp>
#include <SFML/Audio.hpp>

namespace xd { namespace detail {
	
	struct sound_handle
	{
		sf::SoundBuffer buffer;
		sf::Sound sound;
	};

} }

xd::sound::sound(const std::string& filename)
{
	// load audio buffer from file
	auto handle = std::unique_ptr<detail::sound_handle>(new detail::sound_handle);
	if (!handle->buffer.LoadFromFile(filename))
		throw audio_file_load_failed(filename);
	// everything went ok, release the memory to the real handle
	m_handle = handle.release();
	m_handle->sound.SetBuffer(m_handle->buffer);
}

xd::sound::~sound()
{
	delete m_handle;
}

void xd::sound::play()
{
	m_handle->sound.Play();
}

void xd::sound::pause()
{
	m_handle->sound.Pause();
}

void xd::sound::stop()
{
	m_handle->sound.Stop();
}


bool xd::sound::playing()
{
	return m_handle->sound.GetStatus() == sf::SoundSource::Playing;
}

bool xd::sound::paused()
{
	return m_handle->sound.GetStatus() == sf::SoundSource::Paused;
}

bool xd::sound::stopped()
{
	return m_handle->sound.GetStatus() == sf::SoundSource::Stopped;
}

void xd::sound::set_offset(float offset)
{
	m_handle->sound.SetPlayingOffset(sf::Seconds(offset));
}

void xd::sound::set_volume(float volume)
{
	m_handle->sound.SetVolume(volume);
}

void xd::sound::set_pitch(float pitch)
{
	m_handle->sound.SetPitch(pitch);
}

void xd::sound::set_looping(bool looping)
{
	m_handle->sound.SetLoop(looping);
}

void xd::sound::set_loop_point(float time)
{
	throw xd::exception("unimplemented");
}

float xd::sound::get_offset()
{
	return m_handle->sound.GetPlayingOffset().AsSeconds();
}

float xd::sound::get_volume()
{
	return m_handle->sound.GetVolume();
}

float xd::sound::get_pitch()
{
	return m_handle->sound.GetPitch();
}

bool xd::sound::get_looping()
{
	return m_handle->sound.GetLoop();
}

float xd::sound::get_loop_point()
{
	throw xd::exception("unimplemented");
}
