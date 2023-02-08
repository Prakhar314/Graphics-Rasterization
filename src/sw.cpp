#include "sw.hpp"

#include <iostream>
#include <vector>

namespace COL781 {
	namespace Software {

		// Forward declarations

		template <> float Attribs::get(int index) const;
		template <> glm::vec2 Attribs::get(int index) const;
		template <> glm::vec3 Attribs::get(int index) const;
		template <> glm::vec4 Attribs::get(int index) const;

		template <> void Attribs::set(int index, float value);
		template <> void Attribs::set(int index, glm::vec2 value);
		template <> void Attribs::set(int index, glm::vec3 value);
		template <> void Attribs::set(int index, glm::vec4 value);

		// Built-in shaders

		VertexShader Rasterizer::vsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				return vertex;
			};
		}

		VertexShader Rasterizer::vsTransform() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::mat4 transform = uniforms.get<glm::mat4>("transform");
				return transform * vertex;
			};
		}

		VertexShader Rasterizer::vsColor() {
			return [](const Uniforms &uniforms, const Attribs &in, Attribs &out) {
				glm::vec4 vertex = in.get<glm::vec4>(0);
				glm::vec4 color = in.get<glm::vec4>(1);
				out.set<glm::vec4>(0, color);
				return vertex;
			};
		}

		FragmentShader Rasterizer::fsConstant() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = uniforms.get<glm::vec4>("color");
				return color;
			};
		}

		FragmentShader Rasterizer::fsIdentity() {
			return [](const Uniforms &uniforms, const Attribs &in) {
				glm::vec4 color = in.get<glm::vec4>(0);
				return color;
			};
		}

		// Implementation of Attribs and Uniforms classes

		void checkDimension(int index, int actual, int requested) {
			if (actual != requested) {
				std::cout << "Warning: attribute " << index << " has dimension " << actual << " but accessed as dimension " << requested << std::endl;
			}
		}

		template <> float Attribs::get(int index) const {
			checkDimension(index, dims[index], 1);
			return values[index].x;
		}

		template <> glm::vec2 Attribs::get(int index) const {
			checkDimension(index, dims[index], 2);
			return glm::vec2(values[index].x, values[index].y);
		}

		template <> glm::vec3 Attribs::get(int index) const {
			checkDimension(index, dims[index], 3);
			return glm::vec3(values[index].x, values[index].y, values[index].z);
		}

		template <> glm::vec4 Attribs::get(int index) const {
			checkDimension(index, dims[index], 4);
			return values[index];
		}

		void expand(std::vector<int> &dims, std::vector<glm::vec4> &values, int index) {
			if (dims.size() < index+1)
				dims.resize(index+1);
			if (values.size() < index+1)
				values.resize(index+1);
		}

		template <> void Attribs::set(int index, float value) {
			expand(dims, values, index);
			dims[index] = 1;
			values[index].x = value;
		}

		template <> void Attribs::set(int index, glm::vec2 value) {
			expand(dims, values, index);
			dims[index] = 2;
			values[index].x = value.x;
			values[index].y = value.y;
		}

		template <> void Attribs::set(int index, glm::vec3 value) {
			expand(dims, values, index);
			dims[index] = 3;
			values[index].x = value.x;
			values[index].y = value.y;
			values[index].z = value.z;
		}

		template <> void Attribs::set(int index, glm::vec4 value) {
			expand(dims, values, index);
			dims[index] = 4;
			values[index] = value;
		}

		template <typename T> T Uniforms::get(const std::string &name) const {
			return *(T*)values.at(name);
		}

		template <typename T> void Uniforms::set(const std::string &name, T value) {
			auto it = values.find(name);
			if (it != values.end()) {
				// delete it->second;
			}
			values[name] = (void*)(new T(value));
		}

		Uniforms::~Uniforms(){
			// can't do this, because void*
			
			// for(auto& [name,pointers] : values){
			// 	delete pointers;
			// }
		}

		bool Rasterizer::initialize(const std::string &title, int width, int height, int spp){
			if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
				return false; 
			} else {
				window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
				if (window == NULL) {
					return false; 
				} else {
					windowSurface = SDL_GetWindowSurface(window);
					framebuffer = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
				}
			}
			supersampling = std::max(round(sqrt(spp)),1.0);
			frameHeight=height;
			frameWidth=width;
			return true;
		}

		bool Rasterizer::shouldQuit(){
			return this->quit;
		}

		ShaderProgram Rasterizer::createShaderProgram(const VertexShader &vs, const FragmentShader &fs){
			return ShaderProgram{
				vs,
				fs,
				Uniforms()
			};
		}

		void Rasterizer::useShaderProgram(const ShaderProgram &program){
			this->currentProgram = const_cast<ShaderProgram*>(&program);
		}


		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, float value) {
			program.uniforms.set<float>(name,value);
		}
		
		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, int value) {
			program.uniforms.set<int>(name,value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec2 value) {
			program.uniforms.set<glm::vec2>(name,value);
		}
		
		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec3 value) {
			program.uniforms.set<glm::vec3>(name,value);
		}
		
		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::vec4 value) {
			program.uniforms.set<glm::vec4>(name,value);
			// std::cout << "Set "<< name << std::endl;
			// program.
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat2 value) {
			program.uniforms.set<glm::mat2>(name,value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat3 value) {
			program.uniforms.set<glm::mat3>(name,value);
		}

		template <> void Rasterizer::setUniform(ShaderProgram &program, const std::string &name, glm::mat4 value) {
			program.uniforms.set<glm::mat4>(name,value);
		}

		void Rasterizer::deleteShaderProgram(ShaderProgram &program){
			// TODO: nothing here?
			// destructor will get called in programs.uniforms
		}

		Object Rasterizer::createObject(){
			return Object();
		}

		void fillAttribs(Object &object, int n) {
			while(n>object.attribs.size()){
				object.attribs.push_back(Attribs());
			}	
		}

		template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const float* data) {
			fillAttribs(object, n);
			for (size_t i = 0; i < n; i++)
			{
				object.attribs[i].set<float>(attribIndex, data[i]);
			}		
		}

		template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const glm::vec2* data) {
			fillAttribs(object, n);
			for (size_t i = 0; i < n; i++)
			{
				object.attribs[i].set<glm::vec2>(attribIndex, data[i]);
			}		
		}

		template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const glm::vec3* data) {
			fillAttribs(object, n);
			for (size_t i = 0; i < n; i++)
			{
				object.attribs[i].set<glm::vec3>(attribIndex, data[i]);
			}		
		}

		template <> void Rasterizer::setVertexAttribs(Object &object, int attribIndex, int n, const glm::vec4* data) {
			fillAttribs(object, n);
			for (size_t i = 0; i < n; i++)
			{
				object.attribs[i].set<glm::vec4>(attribIndex, data[i]);
			}		
		}

		void Rasterizer::setTriangleIndices(Object &object, int n, glm::ivec3* indices) {
			object.indices = std::vector<glm::ivec3>(indices,indices+n);
		}

		void Rasterizer::enableDepthTest(){
			this->depthTesting = true;
		}

		void Rasterizer::clear(glm::vec4 color){
			// argument is normalized
			color *= 255;
			SDL_Rect framerect {0,0,frameWidth, frameHeight};
			SDL_PixelFormat *format = framebuffer->format;
			Uint32 bgColor = SDL_MapRGBA(format, color[0], color[1], color[2], color[3]);
			SDL_FillRect(framebuffer, &framerect, bgColor);
		}

		void Rasterizer::drawTriangle(glm::vec4 v4_1, glm::vec4 v4_2, glm::vec4 v4_3, glm::vec4 color){
			// transposed on multiplication with vector
			glm::mat4x3 scale{
				frameWidth/2.0,     0,              0,
				0,              frameHeight/2.0,    0,
				0,                  0,              1,
				frameWidth/2.0, frameHeight/2.0,    0
			};
			
			glm::vec3 v1{scale * v4_1}, v2{scale * v4_2}, v3{scale * v4_3};
			
			Uint32 *pixels = (Uint32*)framebuffer->pixels;
			SDL_PixelFormat *format = framebuffer->format;
			color *= 255;

			float step = 1.0/(supersampling+1);

			// v1, v2, v3 should be in clockwise order
			if(glm::cross(v2-v1,v3-v1)[2]<0){
				glm::vec3 temp{v2};
				v2 = v3;
				v3 = temp;
			}

			for (int i = 0; i < frameWidth; i++) {
				for (int j = 0; j < frameHeight; j++) {
					// use previous color as default
					Uint8 r,g,b, a;
					SDL_GetRGBA(
						pixels[i + frameWidth*(frameHeight - 1 - j)],
						format, &r, &g, &b, &a);
					glm::ivec4 default_color{r,g,b,a};

					glm::ivec4 pixel_color{0,0,0,0};
					for(int s_i = 1; s_i <= supersampling; s_i++){
						for(int s_j = 1; s_j <= supersampling; s_j++){
							glm::vec3 pos{i+s_i*step, j+s_j*step, 0};
							if(
								glm::cross(v2-v1,pos-v1)[2]>0 &&
								glm::cross(v3-v2,pos-v2)[2]>0 && 
								glm::cross(v1-v3,pos-v3)[2]>0
							){
								pixel_color += color;
							}
							else{
								pixel_color += default_color;
							}          
						}                    
					}
					// average over number of samples
					pixel_color /= supersampling*supersampling;
					
					pixels[i + frameWidth*(frameHeight - 1 - j)] = SDL_MapRGBA(format, pixel_color[0], pixel_color[1], pixel_color[2], pixel_color[3]);
				}
			}
		}

		void Rasterizer::drawObject(const Object &object){
			Attribs temp;
			for(glm::ivec3  i :object.indices){
				drawTriangle(
					currentProgram->vs(currentProgram->uniforms,object.attribs[i[0]],temp),
					currentProgram->vs(currentProgram->uniforms,object.attribs[i[1]],temp),
					currentProgram->vs(currentProgram->uniforms,object.attribs[i[2]],temp),
					currentProgram->fs(currentProgram->uniforms,object.attribs[i[2]])
				);
			}
		}

		void Rasterizer::show(){
			SDL_BlitScaled(framebuffer, NULL, windowSurface, NULL);
			SDL_UpdateWindowSurface(window);
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0) {
				if(e.type == SDL_QUIT) {
					quit = true;
				}
			}
		}
	}
}
