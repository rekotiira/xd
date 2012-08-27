#include <xd/factory.hpp>
#include <xd/graphics/font.hpp>
#include <xd/graphics/exceptions.hpp>
#include <xd/vendor/utf8.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_SIZES_H
#include <memory>
#include <map>
#include <memory>

namespace xd { namespace detail { namespace font {

	FT_Library *library = nullptr;

	struct vertex
	{
		glm::vec2 pos;
		glm::vec2 tex;
	};

	struct vertex_traits : public xd::vertex_traits<vertex>
	{
		vertex_traits()
		{
			// bind vertex attribs
			bind_vertex_attribute(VERTEX_POSITION, &vertex::pos);
			bind_vertex_attribute(VERTEX_TEXTURE, &vertex::tex);
		}
	};

	typedef vertex_batch<vertex_traits> vertex_batch_t;
	typedef std::shared_ptr<vertex_batch_t> vertex_batch_ptr_t;

	struct glyph
	{
        glyph() {}
        glyph(const glyph& other)
            : glyph_index(other.glyph_index)
            , texture_id(other.texture_id)
            , quad_ptr(other.quad_ptr)
            , advance(other.advance)
            , offset(other.offset)
        {}

		FT_UInt glyph_index;
		GLuint texture_id;
		vertex_batch_ptr_t quad_ptr;
		glm::vec2 advance, offset;
	};

	struct face
	{
		FT_Face handle;
		std::map<int, FT_Size> sizes;
	};

} } }

xd::font::font(const std::string& filename)
    : m_filename(filename)
	, m_mvp_uniform("mvpMatrix")
	, m_position_uniform("vPosition")
	, m_color_uniform("vColor")
	, m_texture_uniform("colorMap")
{
	int error;
	if (!detail::font::library)
	{
		detail::font::library = new FT_Library;
		error = FT_Init_FreeType(detail::font::library);
		if (error)
			throw font_load_failed(filename);
	}

	// construct a new font face; make sure it gets deleted if exception is thrown
	m_face = std::unique_ptr<detail::font::face>(new detail::font::face);

	// load the font
	error = FT_New_Face(*detail::font::library, filename.c_str(), 0, &m_face->handle);
	if (error)
		throw font_load_failed(filename);

	/*try {
		error = FT_Set_Pixel_Sizes(m_face->handle, 0, size);
		if (error)
			throw font_load_failed(filename);

		// pre-load 7-bit ASCII glyphs
		for (int i = 0; i < 128; i++) {
			load_glyph(i);
		}
	} catch (...) {
		FT_Done_Face(m_face->handle);
		throw;
	}*/
}

xd::font::~font()
{
	// free all textures
	for (auto i = m_glyph_map.begin(); i != m_glyph_map.end(); ++i) {
		glDeleteTextures(1, &i->second->texture_id);
	}
	// free font sizes
	for (auto i = m_face->sizes.begin(); i != m_face->sizes.end(); ++i) {
		FT_Done_Size(i->second);
	}
	// free the font handle
	FT_Done_Face(m_face->handle);
}

void xd::font::link_font(const std::string& type, const std::string& filename)
{
	auto linked_font = xd::create<font>(filename);
	m_linked_fonts[type] = linked_font;
}

void xd::font::link_font(const std::string& type, font::ptr font)
{
	m_linked_fonts[type] = font;
}

void xd::font::unlink_font(const std::string& type)
{
	m_linked_fonts.erase(type);
}

const xd::detail::font::glyph& xd::font::load_glyph(utf8::uint32_t char_index, int size)
{
	// check if glyph is already loaded
	glyph_map_t::iterator i = m_glyph_map.find(char_index);
	if (i != m_glyph_map.end())
		return *i->second;

	FT_UInt glyph_index = FT_Get_Char_Index(m_face->handle, char_index);
	int error = FT_Load_Glyph(m_face->handle, glyph_index, FT_LOAD_DEFAULT);
	if (error)
		throw glyph_load_failed(m_filename, char_index);

	error = FT_Render_Glyph(m_face->handle->glyph, FT_RENDER_MODE_NORMAL);
	if (error)
		throw glyph_load_failed(m_filename, char_index);

	// create glyph
    m_glyph_map[char_index] = std::unique_ptr<detail::font::glyph>(new detail::font::glyph);
	detail::font::glyph& glyph = *m_glyph_map[char_index];
	glyph.glyph_index = glyph_index;
	glyph.advance.x = static_cast<float>(m_face->handle->glyph->advance.x >> 6);
	glyph.advance.y = static_cast<float>(m_face->handle->glyph->advance.y >> 6);

	// get the handle to the bitmap
	FT_Bitmap bitmap = m_face->handle->glyph->bitmap;
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
	detail::font::vertex data[4];
	data[0].pos = glm::vec2(0, 0);
	data[1].pos = glm::vec2(0, bitmap.rows);
	data[2].pos = glm::vec2(bitmap.width, bitmap.rows);
	data[3].pos = glm::vec2(bitmap.width, 0);
	data[0].tex = glm::vec2(0, 1);
	data[1].tex = glm::vec2(0, 0);
	data[2].tex = glm::vec2(1, 0);
	data[3].tex = glm::vec2(1, 1);

	// create a batch
	glyph.quad_ptr = detail::font::vertex_batch_ptr_t(new detail::font::vertex_batch_t(GL_QUADS));
	glyph.quad_ptr->load(data, 4);
	glyph.offset.x = static_cast<float>(m_face->handle->glyph->bitmap_left);
	glyph.offset.y = static_cast<float>(m_face->handle->glyph->bitmap_top - bitmap.rows);

	return glyph;
}

void xd::font::render(const std::string& text, const font_style& style,
	xd::shader_program::ptr shader, const glm::mat4& mvp, glm::vec2 *pos)
{
	// check if we're rendering using this font or a linked font
	if (style.m_type && style.m_type->length() != 0) {
		font_map_t::iterator i = m_linked_fonts.find(*style.m_type);
		if (i == m_linked_fonts.end())
			throw invalid_font_type(*style.m_type);
		font_style linked_style = style;
		linked_style.m_type = boost::none;
		i->second->render(text, linked_style, shader, mvp, pos);
		return;
	}

	// check if the font size is already loaded
	auto it = m_face->sizes.find(style.m_size);
	if (it == m_face->sizes.end()) {
		// create a new size
		FT_Size size;
		if (FT_New_Size(m_face->handle, &size) != 0)
			throw font_load_failed(m_filename);
		// free the size in catch block if something goes wrong
		try {
			if (FT_Activate_Size(size) != 0)
				throw font_load_failed(m_filename);
			// set the pixel size
			if (FT_Set_Pixel_Sizes(m_face->handle, 0, style.m_size) != 0)
				throw font_load_failed(m_filename);
			// pre-load 7-bit ASCII glyphs
			for (int i = 0; i < 128; i++) {
				load_glyph(i, style.m_size);
			}
		} catch (...) {
			// free the size and re-throw
			FT_Done_Size(size);
			throw;
		}
		// insert the newly loaded size in the map
		m_face->sizes.insert(std::make_pair(style.m_size, size));
	} else {
		// activate the size
		FT_Activate_Size(it->second);
	}

	// bind to first texture unit
	glActiveTexture(GL_TEXTURE0);

	// setup the shader
	shader->use();
	shader->bind_uniform(m_mvp_uniform, mvp);
	shader->bind_uniform(m_color_uniform, style.m_color);
	shader->bind_uniform(m_texture_uniform, 0);

	// is kerning supported
	FT_Bool kerning = FT_HAS_KERNING(m_face->handle);

	// render each glyph in the string
	glm::vec2 text_pos;
	if (pos)
		text_pos = *pos;

	FT_UInt prev_glyph_index = 0;
	auto i = text.begin();
	while (i != text.end()) {
		// get the unicode code point
		utf8::uint32_t char_index = utf8::next(i, text.end());

		// get the cached glyph, or cache if it is not yet cached
		const detail::font::glyph& glyph = load_glyph(char_index, style.m_size);

		// bind the texture
		glBindTexture(GL_TEXTURE_2D, glyph.texture_id);

		// check for kerning
		if (kerning && glyph.glyph_index && prev_glyph_index) {
			FT_Vector kerning_delta;
			FT_Get_Kerning(m_face->handle, prev_glyph_index, glyph.glyph_index, FT_KERNING_DEFAULT, &kerning_delta);
			text_pos.x += kerning_delta.x >> 6;
		}

		// calculate exact offset
		glm::vec2 glyph_pos = text_pos;
		glyph_pos.x += glyph.offset.x;
		glyph_pos.y += glyph.offset.y;

		// add optional letter spacing
		glyph_pos.x += style.m_letter_spacing/2;

		// if shadow is enabled, draw the shadow first
		if (style.m_shadow) {
			// calculate shadow position
			glm::vec2 shadow_pos = glyph_pos;
			shadow_pos.x += style.m_shadow->x;
			shadow_pos.y += style.m_shadow->y;

			// calculate shadow color
			glm::vec4 shadow_color = style.m_shadow->color;
			shadow_color.a *= style.m_color.a;

			// bind uniforms
			shader->bind_uniform(m_color_uniform, shadow_color);
			shader->bind_uniform(m_position_uniform, shadow_pos);

			// draw shadow
			glyph.quad_ptr->render();

			// restore the text color
			shader->bind_uniform(m_color_uniform, style.m_color);
		}
		
		// if outline is enabled, draw outline
		if (style.m_outline) {
			// calculate outline color
			glm::vec4 outline_color = style.m_outline->color;
			outline_color.a *= style.m_color.a;

			// bind color
			shader->bind_uniform(m_color_uniform, outline_color);

			// draw font multiple times times to draw outline (EXPENSIVE!)
			for (int x = -style.m_outline->width; x <= style.m_outline->width; x++) {
				for (int y = -style.m_outline->width; y <= style.m_outline->width; y++) {
					if (x == 0 && y == 0)
						continue;
					shader->bind_uniform(m_position_uniform, glyph_pos + glm::vec2(x, y));
					glyph.quad_ptr->render();
				}
			}

			// restore the text color
			shader->bind_uniform(m_color_uniform, style.m_color);
		}

		// bind uniforms
		shader->bind_uniform(m_position_uniform, glyph_pos);

		// draw the glyph
		glyph.quad_ptr->render();
		
		// advance the position
		text_pos += glyph.advance;
		text_pos.x += style.m_letter_spacing;

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
