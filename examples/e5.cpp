#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
// Program with perspective correct interpolation of vertex attributes.

// namespace R = COL781::Software;
namespace R = COL781::Hardware;
using namespace glm;
int width, height;
int main() {
	R::Rasterizer r;
    if (!r.initialize("Example 5", 640, 480))
        return EXIT_FAILURE;

    // TODO get the window h w in the class
    width = 640;
    height = 480;

    R::ShaderProgram program = r.createShaderProgram(
        r.vsPerspectiveCorrect(),
        r.fsPerspectiveCorrect()
    );
    vec4 vertices[] = {
        vec4( -0.8,  -0.8, 0.0, 1.0),
        vec4(  0.8,  -0.8, 0.0, 1.0),
		vec4( -0.8,   0.8, 0.0, 1.0),
        vec4(  0.8,   0.8, 0.0, 1.0)
    };
    vec4 colors[] = {
		vec4(0.30, 0.30, 1.00, 1.0),
        vec4(1.0, 1.0, 0.3, 1.0),
		vec4(0.30, 0.30, 1.00, 1.0),
        vec4(1.0, 1.0, 0.3, 1.0)
    };
	ivec3 triangles[] = {
		ivec3(0, 1, 2),
		ivec3(1, 2, 3)
	};
	R::Object shape = r.createObject();
	r.setVertexAttribs(shape, 0, 4, vertices);
	r.setVertexAttribs(shape, 1, 4, colors);
	r.setTriangleIndices(shape, 2, triangles);
    r.enableDepthTest();
    // The transformation matrix.
    mat4 model = mat4(1.0f);
    mat4 view = mat4(1.0f);
    mat4 projection = mat4(1.0f);
    model = rotate(model, radians(-70.0f), vec3(1.0f, 0.0f, 0.0f)); 
    view = mat4(1.0f);
    view = translate(view, vec3(0.0f, 0.0f, -3.0f)); 
    projection = perspective(radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    mat4 pv = projection * view;
    mat4 mvp(1.0f);
    uint deltaTime = 0.0;
    uint lastFrame = SDL_GetTicks64();
    uint currentFrame;
    float speed = 1.0f;
    while (!r.shouldQuit()) {
        currentFrame = SDL_GetTicks64();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        speed = (1.0f / 60.0f) * (float)(deltaTime);
        r.clear(vec4(1.0, 1.0, 1.0, 1.0));
        r.useShaderProgram(program);
        model = rotate(model, radians(speed * 6.0f), normalize(vec3(-0.8f, 0.0f, 0.0f)));
        mvp = pv * model;
        r.setUniform(program, "transform", mvp);
		r.drawObject(shape);
        r.show();
    }
    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;
}
