#ifndef H_PONG
#define H_PONG

#include <xd/system.hpp>
#include <xd/graphics.hpp>
#include <xd/entity.hpp>

// the entity data needs forward declaration of pong class
class pong;

// the data that each entity share
// pong has only two entities: paddle and ball
struct entity_data
{
	entity_data(pong& game)
		: game(game)
	{}

	// reference to the game
	pong& game;
	// position
	xd::vec2 pos;
};

// our entity class
typedef xd::entity<entity_data> pong_entity;

// the game class
class pong : public xd::window
{
public:
	pong();
	~pong();

	void run();
	void frame_update();
	void render();
	void reset();

private:
	// some resources required by our game
	xd::texture m_texture;
	xd::font m_font;
	xd::text_shader m_text_shader;
	xd::transform_geometry m_geometry;

	// the game entities
	pong_entity m_player;
	pong_entity m_computer;
	pong_entity m_ball;

	// scores
	int m_player_score;
	int m_computer_score;

	// give components access to the internals
	friend struct paddle_renderer;
	friend struct ball_renderer;
	friend struct input_controller;
	friend struct paddle_collision;
	friend struct move_ball;
};

#endif
