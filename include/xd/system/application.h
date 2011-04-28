#ifndef H_XD_SYSTEM_APPLICATION
#define H_XD_SYSTEM_APPLICATION

#include <list>

namespace xd
{
	class window;

	class application
	{
	public:
		application();
		virtual ~application();

		void update();

	private:
		std::list<window*> m_windows;

		void add_window(window* win);
		void del_window(window* win);

		friend class window;
	};
}

#endif
