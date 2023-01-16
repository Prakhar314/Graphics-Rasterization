#ifndef A1HW_HPP
#define A1HW_HPP

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace COL781 {

	namespace HardwareRendering {

		using VertexShader = GLuint;
		using FragmentShader = GLuint;

		using ShaderProgram = GLuint;

		template <typename T> using Buffer = GLuint;

		// Windows

		bool initialize(const std::string &title, int width, int height);
		bool isWindowOpen();

		// Shader programs

		ShaderProgram createShaderProgram(const VertexShader &vs, const FragmentShader &fs);
		void useShaderProgram(const ShaderProgram program);
		void deleteShaderProgram(ShaderProgram program);

		// Buffers
		template <typename T> Buffer<T> bufferVertexData(int n, T* data);
		template <typename T> Buffer<T> bufferElements(int n, T* data);

		// Uniforms and attributes
		template <typename T> void setUniform(const std::string &name, T value);
		template <typename T> void setVertexAttribs(const std::string &name, const Buffer<T> buffer);

		// Drawing
		void clear(glm::vec4 color);
		template <typename T> void drawElements(int n, const Buffer<T> buffer);
		void show();

		// Built-in shaders
		VertexShader vsPassthrough();
		FragmentShader fsConstant();
	}

}

#endif
