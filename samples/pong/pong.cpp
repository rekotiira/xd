#include <boost/lexical_cast.hpp>
#include <functional>
#include <cstdlib>
#include "pong.hpp"

// the game area size, this is *not* the resolution of the game
const float game_width = 320;
const float game_height = 240;

// game constants, you can tweak gameplay by changing these
const float player_speed = 2.0f;
const float computer_speed = 2.0f;
const float ball_speed = 3.0f;
const float game_margin = 10.0f;
const float wall_thickness = 10.0f;
const float paddle_thickness = 8.0f;
const float paddle_height = 30.0f;
const float ball_size = 7.0f;

struct paddle_renderer :  xd::render_component<pong_entity>
{
	void render(pong_entity& paddle)
	{
		xd::sprite_batch draw_paddle;
		draw_paddle.add(paddle.game.m_texture, paddle.pos.x, paddle.pos.y, 0, xd::vec2(paddle_thickness, paddle_height));
		draw_paddle.draw(paddle.game.m_geometry.mvp());
	}
};

struct ball_renderer : xd::render_component<pong_entity>
{
	void render(pong_entity& ball)
	{
		xd::sprite_batch draw_ball;
		draw_ball.add(ball.game.m_texture, ball.pos.x, ball.pos.y, 0, xd::vec2(ball_size, ball_size));
		draw_ball.draw(ball.game.m_geometry.mvp());
	}
};

struct input_controller : xd::logic_component<pong_entity>
{
	void update(pong_entity& paddle)
	{
		// check if paddle is being moved upwards or downwards
		if (paddle.game.pressed("up"))
			paddle.pos.y -= player_speed ;
		if (paddle.game.pressed("down"))
			paddle.pos.y += player_speed ;
	}
};

// although the components in this particular case could access the ball via
// paddle.game.m_ball, let's show an alternative way; here we inject the
// dependancy via constructor, and store a reference to it in the component
struct ai_controller : xd::logic_component<pong_entity>
{
	// the AI controller will act on ball's position
	ai_controller(pong_entity::handle ball)
		: ball(ball)
	{
	}

	// very dummy AI
	void update(pong_entity& paddle)
	{
		// the amount of units to move, maximum is the computer_speed const
		float diff = (paddle.pos.y+paddle_height/2)-(ball->pos.y+ball_size/2);
		float units = xd::min(computer_speed, xd::abs(diff));

		// move the paddle based on ball's position
		if (diff < 0)
			paddle.pos.y += units;
		else
			paddle.pos.y -= units;
	}

	pong_entity::handle ball;
};

// you could also check if the paddle goes too up or down in input_controller
// and ai_controller; but this way we decouple the collision check from the
// controller method, giving us more flexibility (especially in more complex games)
struct paddle_collision : xd::logic_component<pong_entity>
{
	void update(pong_entity& paddle)
	{
		// check if paddle collided with either of the walls
		// restrict movement in that direction if it is colliding
		if (paddle.pos.y < (game_margin+wall_thickness))
			paddle.pos.y = (game_margin+wall_thickness);
		else if ((paddle.pos.y+paddle_height) > (game_height-game_margin-wall_thickness))
			paddle.pos.y = game_height-game_margin-wall_thickness-paddle_height;

		// check if the paddle is colliding with the ball
		auto& ball = paddle.game.m_ball;
		if (xd::rect(paddle.pos, paddle_thickness, paddle_height).intersects(xd::rect(ball.pos, ball_size, ball_size))) {
			// get the velocity
			auto& velocity = ball.get<glm::vec2>("velocity");
			// we also need to check on which side of the paddle the collision happened on, so we can correctly
			// invert either the x or y velocity of the ball, we simply check whether the collision happened on side
			// or top/bottom of the paddle by checking the x position of ball in previous frame
			if ((velocity.x < 0 && (paddle.pos.x+paddle_thickness) < (ball.pos.x-velocity.x*ball_speed)) ||
				(velocity.x > 0 && paddle.pos.x > (ball.pos.x+ball_size-velocity.x*ball_speed)))
			{
				// the collision happened horizontally; position the ball so that it's not inside the paddle
				ball.pos.x = paddle.pos.x + (velocity.x < 0 ? paddle_thickness : -ball_size);
				// invert the x velocity to bounce the ball back
				velocity.x = -velocity.x;
				// finally to make a things bit interesting, instead of just bouncing the ball back, apply
				// a little bit of variency on it depending on where on the paddle it hit
				float hitting_point = (ball.pos.y+ball_size) - paddle.pos.y;
				// because the ball doesn't have to entirely touch the paddle, it means the collision area
				// is approximately paddle_height+ball_size, let's clamp the value between 0.0f - 1.0f
				hitting_point = hitting_point / (paddle_height+ball_size);
				// hitting_point 0 means that it hit the top of the paddle, and 1 that it hit the bottom,
				// if the ball is moving downwards, the top part should dampen the angle and the bottom should
				// should make it go at sharper angle; this works as intended, but if the ball is moving upwards
				// we need to reverse the hitting_point to do the reverse
				if (velocity.y < 0)
					hitting_point = 1.0f - hitting_point;
				// adjust the y-velocity based on the hitting point, this will effectively change the angle of the ball
				velocity.y *= (hitting_point + 0.5f) * 1.2f;
				// clamp the value to between min and max velocity
				velocity.y = xd::clamp(xd::abs(velocity.y), 0.35f, 0.65f) * (velocity.y < 0 ? -1 : 1);
				// normalize velocity
				velocity = xd::normalize(velocity);
			}
			else
			{
				// the collision happened vertically, position the ball so that it's not inside the paddle
				ball.pos.y = paddle.pos.y + (velocity.y < 0 ? paddle_height : -ball_size);
				// invert the y velocity to bounce the ball back
				velocity.y = -velocity.y;
			}
		}
	}
};

// to show the alternative way, let's check for ball collision in the move component itself
struct move_ball: xd::logic_component<pong_entity>
{
	// init function is ran when component is registered to entity
	void init(pong_entity& ball)
	{
		// components can store additional data not in entity_base
		// we normalize the velocity to make sure the length of the vector is 1
		ball.get<glm::vec2>("velocity") = xd::normalize(xd::vec2(1, 1));
	}

	void update(pong_entity& ball)
	{
		// get the velocity from the entity
		auto& velocity = ball.get<glm::vec2>("velocity");

		// move the ball
		ball.pos.x += ball_speed*velocity.x;
		ball.pos.y += ball_speed*velocity.y;

		// if ball is colliding with wall
		if (ball.pos.y < (game_margin+wall_thickness)) {
			ball.pos.y = game_margin+wall_thickness;
			velocity.y = -velocity.y;
		}
		else if ((ball.pos.y+ball_size) > (game_height-game_margin-wall_thickness)) {
			ball.pos.y = game_height-game_margin-wall_thickness-ball_size;
			velocity.y = -velocity.y;
		}
	}
};

pong::pong()
	: xd::window("Pong", 640, 480)
	, m_texture("texture.png")
	, m_font("font.otf")
	, m_player(*this)
	, m_computer(*this)
	, m_ball(*this)
	, m_player_score(0)
	, m_computer_score(0)
{
	// seed random
	std::srand(static_cast<std::size_t>(std::time(0)));

	// setup viewport to be the size of the window
	glViewport(0, 0, width(), height());

	// enable texturing and alpha testing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	// make player controllible by keyboard and computer by AI, also deal with collisions
	auto collision_detector = xd::create<paddle_collision>();
	m_player.add_component(xd::create<input_controller>());
	m_player.add_component(collision_detector);
	m_computer.add_component(xd::create<ai_controller>(m_ball));
	m_computer.add_component(collision_detector);

	// finally the next component will move the ball
	m_ball.add_component(xd::create<move_ball>());

	// create renderers, multiple entities can share a single component
	// which is why we only create one paddle renderer
	auto render_paddle = xd::create<paddle_renderer>();
	auto render_ball = xd::create<ball_renderer>();

	// register renderers to entities
	m_player.add_component(render_paddle);
	m_computer.add_component(render_paddle);
	m_ball.add_component(render_ball);

	// reset game
	reset();

	// bind physical keys virtual keys
	// this makes it easier to remap keys, and the code that checks for user input
	// won't have to care about what physical keys the player has mapped his keys to
	// we can also bind multiple physical keys to one virtual key, for example let's
	// bind both Up/Down and W/A for moving the player paddle
	bind_key(xd::KEY_UP, "up");
	bind_key(xd::KEY_W, "up");
	bind_key(xd::KEY_DOWN, "down");
	bind_key(xd::KEY_S, "down");
	bind_key(xd::KEY_ESC, "quit");

	// register out update handler; this is where we'll update the game logic
	// target for 60 FPS (1000 milliseconds / 60)
	register_tick_handler(std::bind(&pong::frame_update, this), 1000/120);
}

pong::~pong()
{
}

void pong::run()
{
	// run the game loop
	while (true)
	{
		// update events
		update();

		// check if window was closed
		if (closed() || triggered("quit"))
			break;

		if (triggered(xd::KEY_N))
			reset();

		// render the game
		render();
	}
}

void pong::frame_update()
{
	// update paddles and ball
	m_player.update();
	m_computer.update();
	m_ball.update();

	// check if we should give score
	if ((m_ball.pos.x+ball_size) < 0) {
		m_computer_score++;
		reset();
	}
	if (m_ball.pos.x > game_width) {
		m_player_score++;
		reset();
	}
}

void pong::render()
{
	// clear the window
	clear();

	// setup projection for the game
	// note that even if the window resolution changes it'll properly scale
	// to the whole visible area of the window, but allows us to use these values
	// as game coordinates, making it easy to write resolution-independent code
	m_geometry.projection().load(xd::ortho<float>(0, game_width, game_height, 0, -1, 1));
	
	// load the identity matrix
	m_geometry.model_view().identity();

	// draw the walls on top and bottom with sprite batch
	xd::sprite_batch walls;
	// parameters: texture, x, y, rotation, scale
	walls.add(m_texture, game_margin, game_margin, 0, xd::vec2(game_width-game_margin*2, wall_thickness));
	// this demonstrates the origin feature, you can "anchor"
	// the sprite to a different point by specifying the origin where to draw the sprite from
	// the default is (0,0) which is top-left; here we specify (0,1) which means bottom-left
	// if we didn't do this, we'd have to specify 240-10 as parameter to y instead of just 240
	// parameters: texture, x, y, rotation, scale, color, origin
	walls.add(m_texture, game_margin, game_height-game_margin, 0, xd::vec2(game_width-game_margin*2, wall_thickness), glm::vec4(1,1,1,1), glm::vec2(0,1));
	walls.draw(m_geometry.mvp());

	// now render the paddles and the ball, these will simply use the registered
	// render components of the entities
	m_player.render();
	m_computer.render();
	m_ball.render();

	// render the scores
	xd::simple_text_renderer render_text(game_width, game_height);
	xd::font_style style(xd::vec4(1,1,1,1), 16);
	render_text.render(m_font, style, game_width/2-50-10*(m_player_score/10+1), game_height-(game_margin+wall_thickness+25),
		boost::lexical_cast<std::string>(m_player_score));
	render_text.render(m_font, style, game_width/2+50-10*(m_computer_score/10+1), game_height-(game_margin+wall_thickness+25),
		boost::lexical_cast<std::string>(m_computer_score));

	// show what we've drawn
	swap();
}

void pong::reset()
{
	// set paddle positions
	m_player.pos = xd::vec2(game_margin, game_height/2-paddle_height/2);
	m_computer.pos = xd::vec2(game_width-paddle_thickness-game_margin, game_height/2-paddle_height/2);

	// set ball starting position
	m_ball.pos.x = game_margin + wall_thickness;
	m_ball.pos.y = game_margin + wall_thickness;
	m_ball.pos.y += static_cast<float>(std::rand())/RAND_MAX*(game_height-game_margin*2-wall_thickness*2);

	// randomize y-velocity
	auto& velocity = m_ball.get<glm::vec2>("velocity");
	velocity.x = 1.0f;
	velocity.y = static_cast<float>(std::rand())/RAND_MAX-0.5f;
	if (velocity.y < 0)
		velocity.y -= 0.5f;
	else
		velocity.y += 0.5f;
	velocity = xd::normalize(velocity);
}
