#ifndef A1_HPP
#define A1_HPP

#include <string>

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "a1hw.hpp"

namespace COL781 {

	namespace SoftwareRendering {

		class Attribs {
			// Don't worry about this, we will fill it in later.
		};

		using VertexShader = glm::vec4(*)(const Attribs &in, Attribs &out);
		using FragmentShader = glm::vec4(*)(const Attribs &in);

		using ShaderProgram = std::pair<VertexShader, FragmentShader>*;

		template <typename T> using Buffer = std::vector<T>*;

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
