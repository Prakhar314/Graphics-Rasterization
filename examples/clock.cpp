#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <ctime>

namespace R = COL781::Software;
// namespace R = COL781::Hardware;
using namespace glm;

int main() {
	R::Rasterizer r;
    // square screen for circular clock
    if (!r.initialize("Clock", 640, 640, 16))
        return EXIT_FAILURE;
    
    // use transformations, constant color
    R::ShaderProgram program = r.createShaderProgram(
        r.vsTransform(),
        r.fsConstant()
    );

    // unit square
    vec4 vertices[] = {
		vec4( 0.0, 0.0, 0.0, 1.0),
		vec4( 1.0, 0.0, 0.0, 1.0),
		vec4( 0.0, 1.0, 0.0, 1.0),
		vec4( 1.0, 1.0, 0.0, 1.0)
    };
	ivec3 triangles[] = {
		ivec3(0, 1, 2),
		ivec3(1, 2, 3)
	};
	R::Object square = r.createObject();
	r.setVertexAttribs(square, 0, 4, vertices);
	r.setTriangleIndices(square, 2, triangles);

    r.useShaderProgram(program);

    while (!r.shouldQuit()) {
        r.clear(vec4(1.0, 1.0, 1.0, 1.0));

        // green for eveything
        r.setUniform<vec4>(program, "color", vec4(0.0, 0.6, 0.0, 1.0));

        // The transformation matrix.
        mat4 mvp = mat4(1.0f);
        for(int i = 0;i<12;i++){
            mat4 loc = mvp;
            loc = rotate(loc, radians(-30.0f * i), normalize(vec3(0.0f, 0.0f, 1.0f)));  // rotation of mark
            loc = scale(loc, vec3(0.04,0.02,1));                                        // shape of mark
            loc = translate(loc, vec3(15, - 0.5f,0));                                   // move along length, center along width
            r.setUniform(program, "transform", loc);
		    r.drawObject(square);
        }

        // current time
        time_t t_n = time(0);
        tm * local_t  = localtime(&t_n);
        int h = local_t->tm_hour;
        int m = local_t->tm_min;
        int s = local_t->tm_sec;

        mat4 hour = rotate(mvp,radians(-30.0f * h + 90.0f), vec3(0.0f, 0.0f, 1.0f));    // position of hand
        hour = scale(hour, vec3(0.2,0.02,1));                                           // shape of hand
        hour = translate(hour, vec3(0,-0.5f,0));                                        // center along width
        r.setUniform(program, "transform", hour);
        r.drawObject(square);

        mat4 minute = rotate(mvp,radians(-6.0f * m + 90.0f), vec3(0.0f, 0.0f, 1.0f));
        minute = scale(minute, vec3(0.35,0.01,1));
        minute = translate(minute, vec3(0,-0.5f,0));
        r.setUniform(program, "transform", minute);
        r.drawObject(square);

        mat4 second = rotate(mvp,radians(-6.0f * s + 90.0f), vec3(0.0f, 0.0f, 1.0f));
        second = scale(second, vec3(0.5,0.006,1));
        second = translate(second, vec3(0,-0.5f,0));
        r.setUniform(program, "transform", second);
        r.drawObject(square);

        r.show();
    }
    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;
}
