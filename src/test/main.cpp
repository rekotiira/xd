#include <iostream>
#include <fstream>
#include <string>
#include "test.h"

#include <xd/entity.hpp>
#include <iostream>

#include <xd/lua.hpp>
#include <luabind/tag_function.hpp>

class my_data
{
public:
	my_data(test& app) : m_app(app) {}
	test& get_app() { return m_app; }
private:
	test& m_app;
};

typedef xd::entity<my_data> my_entity;

struct test_args
{
    int x;
};

class life_component : public xd::logic_component<my_entity>
{
	void init(my_entity& e)
	{
		// register event with a lambda filter
		e.on("test", &life_component::on_test, this);
	}

	void update(my_entity& e)
	{
		std::cout << "life_component::update() - " << e.get<std::string>("name") << std::endl;
	}

	bool on_test(const test_args& args)
	{
		std::cout << "foo: " << args.x << std::endl;
		return true;
	}
};

class wait_task : public xd::lua::scheduler_task
{
public:
	wait_task(int secs) : m_secs(secs), m_start(std::time(0)) {}
	bool is_complete() { return ((std::time(0) - m_start) >= m_secs); }
private:
	int m_secs;
	std::time_t m_start;
};

struct wait
{
	xd::lua::scheduler& scheduler;
	wait(xd::lua::scheduler& scheduler) : scheduler(scheduler) {}
	void operator()(int secs) const
	{
		std::time_t start = std::time(0);
		/*return scheduler.yield([secs, start](xd::lua::scheduler_task_result& result) {
			return ((std::time(0) - start) >= secs);
		});*/
		return scheduler.yield<wait_task>(secs);
	}
};

/*
void wait(xd::lua::scheduler& scheduler, int secs)
{
	std::time_t start = std::time(0);
	return scheduler.yield([secs, start]() {
		return ((std::time(0) - start) >= secs);
	});
}
*/

/*
void input(std::string message)
{
	bool complete = false;
	std::string line;
	game.wait_for_input([&](std::string input) {
		complete = true;
		line = input;
	});
	return scheduler.yield([&](xd::lua::scheduler_task_result& result) -> bool {
		if (complete)
			result.set_value(input);
		return complete;
	});
}
*/

#include <xd/factory.hpp>

class foo
{
public:
	typedef std::shared_ptr<foo> ptr;
private:
	foo() {}
	friend class xd::factory;
};

int main(int argc, char *argv[])
{
	auto x = xd::create<foo>();

	try
	{
		xd::lua::virtual_machine vm;
		vm.load_library();

		xd::lua::scheduler scheduler(vm);
		scheduler.register_function<void (int)>("wait", wait(scheduler));

		scheduler.start(vm.load_file("test.lua"));
		while (scheduler.pending_tasks() > 0) {
			scheduler.run();
		}
	} catch (xd::lua::script_load_failed& e) {
		std::cout << e.what() << std::endl;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	} catch (...) {
	}
	return 0;

	try {
		test my_app;

		my_entity e(my_app);
		e.get<std::string>("name") = "foo";
		e.add_component(new life_component);
		e.update();
    
		test_args args;
		args.x = 1337;
		e.trigger("test", args);

		my_app.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "unknown exception" << std::endl;
	}
	return 0;
}
