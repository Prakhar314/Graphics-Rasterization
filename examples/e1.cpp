#include "../a1.hpp"

namespace R = COL781::SoftwareRendering;
using namespace glm;

int main() {
    if (!R::initialize("Example 1", 640, 480))
        return EXIT_FAILURE;
    R::ShaderProgram program = R::createShaderProgram(
        R::vsPassthrough(),
        R::fsConstant()
    );
    vec4 vertices[] = {
		vec4(-0.8,  0.0, 0.0, 1.0),
        vec4(-0.4, -0.8, 0.0, 1.0),
        vec4( 0.8,  0.8, 0.0, 1.0),
        vec4(-0.4, -0.4, 0.0, 1.0)
    };
	ivec3 elements[] = {
		ivec3(0, 1, 3),
		ivec3(1, 2, 3)
	};
    R::Buffer<vec4> bVertices = R::bufferVertexData<vec4>(3, vertices);
    R::Buffer<ivec3> bElements = R::bufferElements<ivec3>(2, elements);
    while (R::isWindowOpen()) {
        R::clear(vec4(1.0, 1.0, 1.0, 1.0));
        R::useShaderProgram(program);
        R::setUniform<vec4>("color", vec4(0.0, 0.6, 0.0, 1.0));
        R::setVertexAttribs<vec4>("vertex", bVertices);
        R::drawElements<ivec3>(2, bElements);
        R::show();
    }
    R::deleteShaderProgram(program);
    return EXIT_SUCCESS;
}
