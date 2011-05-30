#include <xd/graphics/font.h>
#include <xd/graphics/exceptions.h>
#include <xd/utf8.h>
#include <memory>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

namespace xd { namespace detail {

	FT_Library *font_library = NULL;

	struct font_vertex
	{
		glm::vec2 pos;
		glm::vec2 tex;
	};

	struct font_vertex_traits : public vertex_traits
	{
		font_vertex_traits();
	};

	typedef vertex_batch<font_vertex_traits> font_vertex_batch_t;
	typedef boost::shared_ptr<font_vertex_batch_t> font_vertex_batch_ptr_t;

	struct font_glyph
	{
		FT_UInt glyph_index;
		GLuint texture_id;
		font_vertex_batch_ptr_t quad_ptr;
		glm::vec2 advance, offset;
	};

	struct font_face
	{
		FT_Face face;
	};
} }

xd::detail::font_vertex_traits::font_vertex_traits()
{
	// we have XY position
	vertex_attr_traits pos;
	pos.type = GL_FLOAT;
	pos.size = 2;
	pos.offset = 0;
	pos.normalized = GL_FALSE;

	// and texture coordinates
	vertex_attr_traits tex;
	tex.type = GL_FLOAT;
	tex.size = 2;
	tex.offset = sizeof(glm::vec2);
	tex.normalized = GL_FALSE;

	// add the traits to the vertex
	add_attr_traits(vertex_attr_pos, pos);
	add_attr_traits(vertex_attr_tex, tex);

	// set the size of the vertex
	vertex_size = sizeof(font_vertex);
}

xd::font::font(const std::string& filename, int size)
	: m_face(0)
	, m_filename(filename)
	, m_size(size)
	, m_mvp_uniform("mvpMatrix")
	, m_position_uniform("vPosition")
	, m_color_uniform("vColor")
	, m_texture_uniform("colorMap")
{
	int error;
	if (!detail::font_library)
	{
		detail::font_library = new FT_Library;
		error = FT_Init_FreeType(detail::font_library);
		if (error)
			throw font_load_failed(filename);
	}

	// construct a new font face; make sure it gets deleted if exception is thrown
	m_face = new detail::font_face;
	std::auto_ptr<detail::font_face> face_ptr(m_face);

	// load the font
	error = FT_New_Face(*detail::font_library, filename.c_str(), 0, &m_face->face);
	if (error)
		throw font_load_failed(filename);

	try {
		error = FT_Set_Pixel_Sizes(m_face->face, 0, size);
		if (error)
			throw font_load_failed(filename);

		// pre-load 7-bit ASCII glyphs
		for (int i = 0; i < 128; i++) {
			load_glyph(i);
		}
	} catch (...) {
		FT_Done_Face(m_face->face);
		throw;
	}

	// all fine, release the auto ptr
	face_ptr.release();
}

xd::font::~font()
{
	for (glyph_map_t::iterator i = m_glyph_map.begin(); i != m_glyph_map.end(); ++i) {
		glDeleteTextures(1, &i->second.texture_id);
	}
	FT_Done_Face(m_face->face);
	delete m_face;
}

void xd::font::link_font(const std::string& type, const std::string& filename)
{
	font_ptr linked_font = font_ptr(new font(filename, m_size));
	m_linked_fonts[type] = linked_font;
}

void xd::font::link_font(const std::string& type, font_ptr font)
{
	m_linked_fonts[type] = font;
}

void xd::font::unlink_font(const std::string& type)
{
	m_linked_fonts.erase(type);
}

const xd::detail::font_glyph& xd::font::load_glyph(utf8::uint32_t char_index)
{
	// check if glyph is already loaded
	glyph_map_t::iterator i = m_glyph_map.find(char_index);
	if (i != m_glyph_map.end())
		return i->second;

	FT_UInt glyph_index = FT_Get_Char_Index(m_face->face, char_index);
	int error = FT_Load_Glyph(m_face->face, glyph_index, FT_LOAD_DEFAULT);
	if (error)
		throw glyph_load_failed(m_filename, char_index);

	error = FT_Render_Glyph(m_face->face->glyph, FT_RENDER_MODE_NORMAL);
	if (error)
		throw glyph_load_failed(m_filename, char_index);

	// create glyph
	detail::font_glyph& glyph = m_glyph_map[char_index] = detail::font_glyph();
	glyph.glyph_index = glyph_index;
	glyph.advance.x = static_cast<float>(m_face->face->glyph->advance.x >> 6);
	glyph.advance.y = static_cast<float>(m_face->face->glyph->advance.y >> 6);

	// get the handle to the bitmap
	FT_Bitmap bitmap = m_face->face->glyph->bitmap;
	glGenTextures(1, &glyph.texture_id);
	glBindTexture(GL_TEXTURE_2D, glyph.texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.rows,
		0, GL_LUMINANCE, GL_UNSIGNED_BYTE, bitmap.buffer);

	// create quad for it
	detail::font_vertex data[4];
	data[0].pos = glm::vec2(0, 0);
	data[1].pos = glm::vec2(0, bitmap.rows);
	data[2].pos = glm::vec2(bitmap.width, bitmap.rows);
	data[3].pos = glm::vec2(bitmap.width, 0);
	data[0].tex = glm::vec2(0, 1);
	data[1].tex = glm::vec2(0, 0);
	data[2].tex = glm::vec2(1, 0);
	data[3].tex = glm::vec2(1, 1);

	// create a batch
	glyph.quad_ptr = detail::font_vertex_batch_ptr_t(new detail::font_vertex_batch_t(GL_QUADS));
	glyph.quad_ptr->load(data, 4);
	glyph.offset.x = static_cast<float>(m_face->face->glyph->bitmap_left);
	glyph.offset.y = static_cast<float>(m_face->face->glyph->bitmap_top - bitmap.rows);

	return glyph;
}

void xd::font::render(const std::string& text, const font_style& style,
	xd::shader_program& shader, const glm::mat4& mvp, glm::vec2 *pos)
{
	// check if we're rendering using this font or a linked font
	if (style.type && style.type->length() != 0) {
		font_map_t::iterator i = m_linked_fonts.find(*style.type);
		if (i == m_linked_fonts.end())
			throw invalid_font_type(*style.type);
		font_style linked_style = style;
		linked_style.type = boost::none;
		i->second->render(text, linked_style, shader, mvp, pos);
		return;
	}

	// setup the shader
	shader.use();
	shader.bind_uniform(m_mvp_uniform, mvp);
	shader.bind_uniform(m_color_uniform, style.color);
	shader.bind_uniform(m_texture_uniform, 0);

	// is kerning supported
	FT_Bool kerning = FT_HAS_KERNING(m_face->face);

	// render each glyph in the string
	glm::vec2 text_pos;
	if (pos)
		text_pos = *pos;

	FT_UInt prev_glyph_index = 0;
	std::string::const_iterator i = text.begin();
	while (i != text.end()) {
		// get the unicode code point
		utf8::uint32_t char_index = utf8::next(i, text.end());

		// get the cached glyph, or cache if it is not yet cached
		const detail::font_glyph& glyph = load_glyph(char_index);

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, glyph.texture_id);

		// check for kerning
		if (kerning && glyph.glyph_index && prev_glyph_index) {
			FT_Vector kerning_delta;
			FT_Get_Kerning(m_face->face, prev_glyph_index, glyph.glyph_index, FT_KERNING_DEFAULT, &kerning_delta);
			text_pos.x += kerning_delta.x >> 6;
		}

		// calculate exact offset
		glm::vec2 glyph_pos = text_pos;
		glyph_pos.x += glyph.offset.x;
		glyph_pos.y += glyph.offset.y;

		// add optional letter spacing
		glyph_pos.x += style.letter_spacing/2;

		// if shadow is enabled, draw the shadow first
		if (style.shadow) {
			// calculate shadow position
			glm::vec2 shadow_pos = glyph_pos;
			shadow_pos.x += style.shadow->x;
			shadow_pos.y += style.shadow->y;

			// calculate shadow color
			glm::vec4 shadow_color = style.shadow->color;
			shadow_color.a *= style.color.a;

			// bind uniforms
			shader.bind_uniform(m_color_uniform, shadow_color);
			shader.bind_uniform(m_position_uniform, shadow_pos);

			// draw shadow
			glyph.quad_ptr->render();

			// restore the text color
			shader.bind_uniform(m_color_uniform, style.color);
		}
		
		// if outline is enabled, draw outline
		if (style.outline) {
			// calculate outline color
			glm::vec4 outline_color = style.outline->color;
			outline_color.a *= style.color.a;

			// bind color
			shader.bind_uniform(m_color_uniform, outline_color);

			// draw font multiple times times to draw outline (EXPENSIVE!)
			for (int x = -style.outline->width; x <= style.outline->width; x++) {
				for (int y = -style.outline->width; y <= style.outline->width; y++) {
					if (x == 0 && y == 0)
						continue;
					shader.bind_uniform(m_position_uniform, glyph_pos + glm::vec2(x, y));
					glyph.quad_ptr->render();
				}
			}

			// restore the text color
			shader.bind_uniform(m_color_uniform, style.color);
		}

		// bind uniforms
		shader.bind_uniform(m_position_uniform, glyph_pos);

		// draw the glyph
		glyph.quad_ptr->render();
		
		// advance the position
		text_pos += glyph.advance;
		text_pos.x += style.letter_spacing;

		// keep track of previous glyph to do kerning
		prev_glyph_index = glyph.glyph_index;
	}

	// give back the updated coords
	if (pos)
		*pos = text_pos;
}

const std::string& xd::font::get_mvp_uniform()
{
	return m_mvp_uniform;
}

void xd::font::set_mvp_uniform(const std::string& uniform_name)
{
	m_mvp_uniform = uniform_name;
}

const std::string& xd::font::get_pos_uniform()
{
	return m_position_uniform;
}

void xd::font::set_pos_uniform(const std::string& uniform_name)
{
	m_position_uniform = uniform_name;
}

const std::string& xd::font::get_color_uniform()
{
	return m_color_uniform;
}

void xd::font::set_color_uniform(const std::string& uniform_name)
{
	m_color_uniform = uniform_name;
}

const std::string& xd::font::get_texture_uniform()
{
	return m_texture_uniform;
}

void xd::font::set_texture_uniform(const std::string& uniform_name)
{
	m_texture_uniform = uniform_name;
}
