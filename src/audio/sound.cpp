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
	if (!handle->buffer.loadFromFile(filename))
		throw audio_file_load_failed(filename);
	// everything went ok, release the memory to the real handle
	m_handle = handle.release();
	m_handle->sound.setBuffer(m_handle->buffer);
}

xd::sound::~sound()
{
	delete m_handle;
}

void xd::sound::play()
{
	m_handle->sound.play();
}

void xd::sound::pause()
{
	m_handle->sound.pause();
}

void xd::sound::stop()
{
	m_handle->sound.stop();
}


bool xd::sound::playing() const
{
	return m_handle->sound.getStatus() == sf::SoundSource::Playing;
}

bool xd::sound::paused() const
{
	return m_handle->sound.getStatus() == sf::SoundSource::Paused;
}

bool xd::sound::stopped() const
{
	return m_handle->sound.getStatus() == sf::SoundSource::Stopped;
}

void xd::sound::set_offset(float offset)
{
	m_handle->sound.setPlayingOffset(sf::seconds(offset));
}

void xd::sound::set_volume(float volume)
{
	m_handle->sound.setVolume(volume);
}

void xd::sound::set_pitch(float pitch)
{
	m_handle->sound.setPitch(pitch);
}

void xd::sound::set_looping(bool looping)
{
	m_handle->sound.setLoop(looping);
}

void xd::sound::set_loop_point(float time)
{
	throw xd::exception("unimplemented");
}

float xd::sound::get_offset() const
{
	return m_handle->sound.getPlayingOffset().asSeconds();
}

float xd::sound::get_volume() const
{
	return m_handle->sound.getVolume();
}

float xd::sound::get_pitch() const
{
	return m_handle->sound.getPitch();
}

bool xd::sound::get_looping() const
{
	return m_handle->sound.getLoop();
}

float xd::sound::get_loop_point() const
{
	throw xd::exception("unimplemented");
}
