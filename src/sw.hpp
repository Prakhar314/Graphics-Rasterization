#ifndef SW_HPP
#define SW_HPP

#include <glm/glm.hpp>
#include <map>
#include <SDL2/SDL.h>
#include <string>
#include <vector>

namespace COL781 {
	namespace Software {

		class Attribs {
			// A class to contain the attributes of ONE vertex
		public:
			// only float, glm::vec2, glm::vec3, glm::vec4 allowed
			template <typename T> T get(int attribIndex) const;
			template <typename T> void set(int attribIndex, T value);
		private:
			std::vector<glm::vec4> values;
			std::vector<int> dims;
		};

		class Uniforms {
			// A class to contain all the uniform variables
		public:
			// any type allowed
			template <typename T> T get(const std::string &name) const;
			template <typename T> void set(const std::string &name, T value);
			~Uniforms();
		private:
			std::map<std::string,void*> values;
		};

		using VertexShader = glm::vec4(*)(const Uniforms &uniforms, const Attribs &in, Attribs &out);
		using FragmentShader = glm::vec4(*)(const Uniforms &uniforms, const Attribs &in);

		struct ShaderProgram {
			VertexShader vs;
			FragmentShader fs;
			Uniforms uniforms;
		};

		struct Object {
			std::vector<Attribs> attribs;
			std::vector<glm::ivec3> indices;
		};

		// Copied from api.hpp
		class Rasterizer {
			public:

				/** Windows **/

				// Creates a window with the given title, size, and samples per pixel.
				bool initialize(const std::string &title, int width, int height, int spp=1);

				// Returns true if the user has requested to quit the program.
				bool shouldQuit(); 

				/** Shader programs **/

				// Creates a new shader program, i.e. a pair of a vertex shader and a fragment shader.
				ShaderProgram createShaderProgram(const VertexShader &vs, const FragmentShader &fs);

				// Makes the given shader program active. Future draw calls will use its vertex and fragment shaders.
				void useShaderProgram(const ShaderProgram &program);

				// Sets the value of a uniform variable.
				// T is only allowed to be float, int, glm::vec2/3/4, glm::mat2/3/4.
				template <typename T> void setUniform(ShaderProgram &program, const std::string &name, T value);

				// Deletes the given shader program.
				void deleteShaderProgram(ShaderProgram &program);

				/** Objects **/

				// Creates an object, i.e. a collection of vertices and triangles.
				// Vertex attribute arrays store the vertex data.
				// A triangle index array stores the indices of the triangles.
				Object createObject();

				// Sets the data for the i'th vertex attribute.
				// T is only allowed to be float, glm::vec2, glm::vec3, or glm::vec4.
				template <typename T> void setVertexAttribs(Object &object, int attribIndex, int n, const T* data);

				// Sets the indices of the triangles.
				void setTriangleIndices(Object &object, int n, glm::ivec3* indices);

				/** Drawing **/
				

				// Enable depth testing.
				void enableDepthTest();

				// Clear the framebuffer, setting all pixels to the given color.
				void clear(glm::vec4 color);

				// Draws the triangles of the given object.
				void drawObject(const Object &object);

				// Displays the framebuffer on the screen.
				void show(); 

				/** Built-in shaders **/

				// A vertex shader that uses the 0th vertex attribute as the position.
				VertexShader vsIdentity();

				// A vertex shader that uses the 0th vertex attribute as the position and passes on the 1th attribute as the color.
				VertexShader vsColor();

				// A vertex shader that applies the transformation matrix given by the uniform named 'transform'.
				VertexShader vsTransform();

				// A vertex shader that handles both transformation and color attributes.
				VertexShader vsColorTransform();

				// A fragment shader that returns a constant colour given by the uniform named 'color'.
				FragmentShader fsConstant(); 

				// A fragment shader that uses the 0th attribute as the color.
				FragmentShader fsIdentity(); 

			private:
				float line_eq(glm::vec3 v1, glm::vec3 v2, float x);
				float get_dist(glm::vec3 v1, glm::vec3 v2, glm::vec3 p);
				glm::vec4 interpolate_3(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec4 q1, glm::vec4 q2, glm::vec4 q3, glm::vec3 p);
				void drawTriangle(glm::vec4 v4_1, glm::vec4 v4_2, glm::vec4 v4_3, glm::vec4 c1, glm::vec4 c2, glm::vec4 c3, int spa);

				SDL_Surface* framebuffer = NULL;
				SDL_Window* window = NULL;
				SDL_Surface* windowSurface = NULL;
				bool quit = false;
				bool depthTesting = false;
				int supersampling = 1;
				int frameWidth, frameHeight;
				ShaderProgram* currentProgram;
			};

	}
}

#endif
