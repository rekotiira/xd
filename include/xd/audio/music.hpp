#ifndef H_XD_AUDIO_MUSIC
#define H_XD_AUDIO_MUSIC

#include <xd/config.hpp>
#include <xd/ref_counted.hpp>
#include <boost/noncopyable.hpp>
#include <boost/intrusive_ptr.hpp>
#include <string>

#pragma warning(disable: 4275)

namespace xd
{
	namespace detail
	{
		struct music_handle;
	}

	class XD_API music : public xd::ref_counted, public boost::noncopyable
	{
	public:
		typedef boost::intrusive_ptr<music> ptr;

		music(const std::string& filename);
		virtual ~music();

		void play();
		void pause();
		void stop();

		bool playing();
		bool paused();
		bool stopped();

		void set_offset(float offset);
		void set_volume(float volume);
		void set_pitch(float pitch);
		void set_looping(bool looping);
		void set_loop_point(float time);

		float get_offset();
		float get_volume();
		float get_pitch();
		bool get_looping();
		float get_loop_point();

	private:
		detail::music_handle *m_handle;
	};
}

#endif
