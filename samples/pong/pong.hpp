#ifndef H_PONG
#define H_PONG

#include <xd/system.hpp>
#include <xd/graphics.hpp>
#include <xd/entity.hpp>

// the entity data needs forward declaration of pong class
class pong;

// our entity class
struct pong_entity : xd::entity<pong_entity>
{
	pong_entity(pong& game)
		: game(game)
	{}

	// reference to the game
	pong& game;
	// position
	xd::vec2 pos;
};

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
	xd::texture::ptr m_texture;
	xd::font::ptr m_font;
	xd::text_shader::ptr m_text_shader;
	xd::transform_geometry m_geometry;

	// the game entities
	pong_entity::ptr m_player;
	pong_entity::ptr m_computer;
	pong_entity::ptr m_ball;

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
