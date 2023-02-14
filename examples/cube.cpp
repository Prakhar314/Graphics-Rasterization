#include "../src/a1.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <algorithm>

// Rubik's cube
// Hover over screen to rotate
// Hover over green button to continue solving

namespace R = COL781::Software;
// namespace R = COL781::Hardware;
using namespace glm;

vec4 const rubiks_colors[]{
    vec4(240, 240, 240, 255) / 255.0f, // white
    vec4(255, 213, 0, 255) / 255.0f,   // yellow
    vec4(0, 155, 72, 255) / 255.0f,    // green
    vec4(0, 70, 173, 255) / 255.0f,    // blue
    vec4(183, 18, 52, 255) / 255.0f,   // red
    vec4(255, 88, 0, 255) / 255.0f,    // orange
};

class Cube
{
    std::vector<R::Object> faces;
    mat4 t;
    ivec3 triangles[2]{
        ivec3(0, 1, 2),
        ivec3(1, 2, 3)};

public:
    Cube(R::Rasterizer &r, const vec4 colors[6], const mat4 &transformation = mat4(1.0f)) : t(transformation)
    {
        for (int i = 0; i < 6; i++)
        {
            /*
            0 000 110       000 010 100 110
            1 000 101       000 001 100 101
            2 000 011       000 001 010 011
            3 111 001       111 110 101 001
            4 111 010       111 101 101 010
            5 111 100       111 101 110 100
            */
            vec4 vertices[4]{
                vec4(i > 2, i > 2, i > 2, 1.0),
                vec4(i > 2, (i > 2) ^ (i % 3 == 0), (i > 2) ^ (i % 3 != 0), 1.0),
                vec4((i > 2) ^ (i % 3 != 2), (i > 2) ^ (i % 3 == 2), i > 2, 1.0),
                vec4((i > 2) ^ (i % 3 != 2), (i > 2) ^ (i % 3 != 1), (i > 2) ^ (i % 3 != 0), 1.0)};
            vec4 c = colors[i];
            vec4 cs[] = {c, c, c, c};
            R::Object shape = r.createObject();
            r.setVertexAttribs(shape, 0, 4, vertices);
            r.setVertexAttribs(shape, 1, 4, cs);
            r.setTriangleIndices(shape, 2, triangles);
            faces.push_back(shape);
        }
    }

    void draw(R::Rasterizer &r, R::ShaderProgram &program, const mat4 &w = mat4(1.0f))
    {
        r.setUniform(program, "transform", w * t);
        for (R::Object &f : faces)
        {
            r.drawObject(f);
        }
    }

    void setMatrix(mat4 m)
    {
        t = m;
    }
    mat4 &getMatrix()
    {
        return t;
    }
};

class RubiksCube
{
    std::vector<Cube> cubes;
    std::vector<std::pair<int, int>> moveList;
    mat4 t;

public:
    RubiksCube(R::Rasterizer &r)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int k = 0; k < 3; k++)
                {
                    vec4 colors[] = {
                        (k == 0) ? rubiks_colors[4] : vec4(0, 0, 0, 1),
                        (j == 0) ? rubiks_colors[2] : vec4(0, 0, 0, 1),
                        (i == 0) ? rubiks_colors[0] : vec4(0, 0, 0, 1),
                        (k == 2) ? rubiks_colors[5] : vec4(0, 0, 0, 1),
                        (j == 2) ? rubiks_colors[3] : vec4(0, 0, 0, 1),
                        (i == 2) ? rubiks_colors[1] : vec4(0, 0, 0, 1),
                    };
                    cubes.push_back(
                        Cube(r, colors, translate(mat4(1.0f), vec3(i * 1.01 - 1.51f, j * 1.01 - 1.51f, k * 1.01 - 1.51f))));
                }
            }
        }

        std::mt19937 rng(42);
        std::uniform_int_distribution<int> genLayer(0, 8);
        std::uniform_int_distribution<int> genDir(-1, 2);

        moveList.reserve(50 * sizeof(int));
        for (int i = 0; i < 25; i++)
        {
            moveList.push_back({0, 0});
        }
        for (int i = 24; i >= 0; i--)
        {
            int l = genLayer(rng);
            while(i<24 && l==moveList[i+1].first){
                l = genLayer(rng);
            }
            int d = genDir(rng);
            rotateLayer(l, -90.0f * d);
            reset(l, d);
            moveList[i] = {l, -d};
        }
    }
    void reset(int layer, int direction)
    {
        direction = (direction + 4) % 4;
        int d_1 = 9;
        int d_2 = 3;
        int d_3 = 1;
        if (layer > 2)
        {
            if (layer < 6)
            {
                d_1 = 3;
                d_2 = 1;
                d_3 = 9;
            }
            else
            {
                d_1 = 1;
                d_2 = 9;
                d_3 = 3;
            }
        }
        int base = (layer % 3) * d_1;
        for (int iter = 0; iter < direction; iter++)
        {
            for (int corner = 0; corner < 2; corner++)
            {
                int pos_x = 0;
                int pos_y = corner;
                Cube last(std::move(cubes[base + pos_x * d_2 + pos_y * d_3]));
                for (int j = 0; j < 4; j++)
                {
                    int next_x = 1 + (pos_y - 1);
                    int next_y = 1 - (pos_x - 1);
                    Cube temp(std::move(cubes[base + next_x * d_2 + next_y * d_3]));
                    cubes[base + next_x * d_2 + next_y * d_3] = std::move(last);
                    last = std::move(temp);
                    pos_x = next_x;
                    pos_y = next_y;
                }
            }
        }
    }
    void rotateLayer(int layer, float angle)
    {
        int d_1 = 9;
        int d_2 = 3;
        int d_3 = 1;
        if (layer > 2)
        {
            if (layer < 6)
            {
                d_1 = 3;
                d_2 = 1;
                d_3 = 9;
            }
            else
            {
                d_1 = 1;
                d_2 = 9;
                d_3 = 3;
            }
        }
        int base = (layer % 3) * d_1;
        mat4 transformation = rotate(mat4(1.0f), radians(angle), vec3(d_1 == 9, d_1 == 3, d_1 == 1));
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                cubes[base + i * d_2 + j * d_3].setMatrix(
                    transformation * cubes[base + i * d_2 + j * d_3].getMatrix());
            }
        }
    }
    void continueRotation(float speed = 180.0f)
    {
        static float progress = 0.0f;
        static uint32 move = 0;
        if (move >= moveList.size())
        {
            return;
        }
        float step = moveList[move].second * -1 * speed / 100.0f;
        rotateLayer(moveList[move].first, step);
        progress += step;
        if (abs(progress) >= abs(moveList[move].second * -90.0f))
        {
            reset(moveList[move].first, moveList[move].second);
            move++;
            progress = 0;
        }
    }
    void draw(R::Rasterizer &r, R::ShaderProgram &program)
    {
        for (Cube &c : cubes)
        {
            c.draw(r, program, t);
        }
    }
    void setMatrix(mat4 m)
    {
        t = m;
    }
};

class Arrows
{
    std::vector<R::Object> arrows;

public:
    Arrows(R::Rasterizer &r, const vec4 &color = vec4(200, 200, 200, 255) / 255.0f)
    {
        vec4 vertices[3]{
            vec4(-0.05, 0.85, -1.0, 1.0f),
            vec4(0.05, 0.85, -1.0, 1.0f),
            vec4(0.0, 0.95, -1.0, 1.0f)};
        vec4 cs[] = {color, color, color};
        ivec3 triangles[2]{ivec3(0, 1, 2)};
        mat4 t = rotate(mat4(1.0f), radians(90.0f), vec3(0, 0, 1));
        for (int i = 0; i < 4; i++)
        {
            R::Object shape = r.createObject();
            r.setVertexAttribs(shape, 0, 3, vertices);
            r.setVertexAttribs(shape, 1, 3, cs);
            r.setTriangleIndices(shape, 1, triangles);
            arrows.push_back(shape);
            for (int j = 0; j < 3; j++)
            {
                vertices[j] = t * vertices[j];
            }
        }
        // green button to resume solving
        {
            vec4 vertices[3]{
                vec4(0.82, -0.75, -1.0, 1.0f),
                vec4(0.82, -0.95, -1.0, 1.0f),
                vec4(0.92, -0.85, -1.0, 1.0f)};
            vec4 color = vec4(0, 255, 0, 255) / 255.0f;
            vec4 cs[] = {color, color, color};
            R::Object shape = r.createObject();
            r.setVertexAttribs(shape, 0, 3, vertices);
            r.setVertexAttribs(shape, 1, 3, cs);
            r.setTriangleIndices(shape, 1, triangles);
            arrows.push_back(shape);
        }
    }
    void draw(R::Rasterizer &r, R::ShaderProgram &program, mat4 t = mat4(1.0f))
    {
        r.setUniform(program, "transform", t);
        for (R::Object &o : arrows)
        {
            r.drawObject(o);
        }
    }
};

int main()
{
    R::Rasterizer r;
    int width = 640, height = 480;
    if (!r.initialize("Cube", width, height, 16))
        return EXIT_FAILURE;

    R::ShaderProgram program = r.createShaderProgram(
        r.vsColorTransform(),
        r.fsIdentity());

    RubiksCube c(r);
    Arrows arrows(r);
    // 3d
    r.enableDepthTest();
    // The transformation matrix.
    mat4 model = rotate(mat4(1.0f), radians(45.0f), normalize(vec3(1.0, 1.0, 0.0)));
    mat4 view = translate(mat4(1.0f), vec3(0.0f, 0.0f, -7.0f));
    mat4 projection = perspective(radians(60.0f), (float)width / (float)height, 0.1f, 80.0f);
    // cube rotation speed
    float speed = 90.0f;

    // bool mouse_down = false;
    std::pair<int, int> mouse_location = {0, 0};

    while (!r.shouldQuit())
    {
        r.clear(vec4(1.0, 1.0, 1.0, 1.0));
        r.useShaderProgram(program);

        // to handle mouse events
        // TODO: Doesn't work in SW
        // SDL_Event e;
        // while (SDL_PollEvent(&e))
        // {
        //     switch (e.type)
        //     {
        //     case SDL_QUIT:
        //         std::cout << " Quit" << std::endl;
        //         return false;
        //     case SDL_MOUSEBUTTONDOWN:
        //         mouse_down = true;
        //         std::cout << " Quit" << std::endl;
        //         break;
        //     case SDL_MOUSEBUTTONUP:
        //         mouse_down = false;
        //         break;
        //     }
        // }
        // if (mouse_down)
        // {
            SDL_GetMouseState(&mouse_location.first, &mouse_location.second);
            if (mouse_location.first > 0.85f * width && mouse_location.second > 0.85f * height)
            {
                c.continueRotation();
            }
            else
            {
                // height inverted
                vec3 mouse_v(mouse_location.first - width / 2, height / 2 - mouse_location.second, 0);
                vec3 axis = normalize(cross(vec3(0, 0, 1), mouse_v));
                // rotation on interaction
                model = rotate(mat4(1.0f), radians(speed / 100), axis) * model;
            }
        // }

        c.setMatrix(projection * view * model);
        c.draw(r, program);
        arrows.draw(r, program);
        r.show();
    }
    r.deleteShaderProgram(program);
    return EXIT_SUCCESS;
}
