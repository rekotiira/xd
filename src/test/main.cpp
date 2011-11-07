#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <xd/common.h>
#include <xd/utf8.h>
#include "test.h"

#include <xd/common/entity.h>
#include <iostream>

struct my_data
{
    my_data() : health(10) {}
    std::string name;
    int health;
};

typedef xd::entity<my_data> my_entity;
typedef xd::logic_component<my_entity> logic_component;
typedef xd::render_component<my_entity> render_component;
typedef xd::component<my_entity> component;

struct test_args
{
    int x;
};

struct life_component : xd::logic_component<my_entity>
{
    void init(my_entity& e)
    {
        std::cout << "life_component::init() - " << e.name << std::endl;
        //e.on("test", std::    bind(&life_component::on_test, this, std::placeholders::_1));
        //std::function<bool (const test_args&)> func = std::bind(&life_component::on_test, this, std::placeholders::_1);
        [](){};
        //e.on("test", func);
    }
    
    void update(my_entity& e)
    {
        std::cout << "life_component::update() - " << e.name << std::endl;
    }
};

int main(int argc, char *argv[])
{
    my_entity e;
    e.name = "test";
    e.add_component(logic_component::ptr(new life_component));
    e.update();
    
    test_args args;
    args.x = 1337;
    e.trigger("test", args);

	/*
	try
	{
		xd::lua::vm vm;
		vm.load_library();
		xd::lua::script_handle script = vm.load_file("test.lua");
		script->run();
	} catch (xd::lua::script_load_failed& e) {
		std::cout << "failed to load script:\n" << e.error_message() << std::endl;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	} catch (...) {
	}
	return 0;
	*/

	/*try {
		test my_app;
		my_app.run();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	} catch(...) {
		std::cerr << "unknown exception" << std::endl;
	}*/
	return 0;
}
