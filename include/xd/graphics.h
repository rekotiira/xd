#ifndef H_XD_GRAPHICS
#define H_XD_GRAPHICS

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <xd/graphics/types.h>
#include <xd/graphics/traits.h>
#include <xd/graphics/exceptions.h>
#include <xd/graphics/matrix_stack.h>
#include <xd/graphics/transform_geometry.h>
#include <xd/graphics/shader_program.h>
#include <xd/graphics/shaders.h>
#include <xd/graphics/vertex_batch.h>
#include <xd/graphics/utility.h>

#endif
