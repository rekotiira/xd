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
	if (!handle->music.openFromFile(filename))
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
	m_handle->music.play();
}

void xd::music::pause()
{
	m_handle->music.pause();
}

void xd::music::stop()
{
	m_handle->music.stop();
}

bool xd::music::playing() const
{
	return m_handle->music.getStatus() == sf::SoundSource::Playing;
}

bool xd::music::paused() const
{
	return m_handle->music.getStatus() == sf::SoundSource::Paused;
}

bool xd::music::stopped() const
{
	return m_handle->music.getStatus() == sf::SoundSource::Stopped;
}

void xd::music::set_offset(float offset)
{
	m_handle->music.setPlayingOffset(sf::seconds(offset));
}

void xd::music::set_volume(float volume)
{
	m_handle->music.setVolume(volume);
}

void xd::music::set_pitch(float pitch)
{
	m_handle->music.setPitch(pitch);
}

void xd::music::set_looping(bool looping)
{
	m_handle->music.setLoop(looping);
}

void xd::music::set_loop_point(float time)
{
	throw xd::exception("unimplemented");
}

float xd::music::get_offset() const
{
	return m_handle->music.getPlayingOffset().asSeconds();
}

float xd::music::get_volume() const
{
	return m_handle->music.getVolume();
}

float xd::music::get_pitch() const
{
	return m_handle->music.getPitch();
}

bool xd::music::get_looping() const
{
	return m_handle->music.getLoop();
}

float xd::music::get_loop_point() const
{
	throw xd::exception("unimplemented");
}
