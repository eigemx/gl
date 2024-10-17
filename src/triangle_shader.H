#pragma once

// On GLSL vector data types:
//  The vector datatype allows for some interesting and flexible component selection
//  called swizzling. Swizzling allows us to use syntax like this:

/*
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
*/

// You can use any combination of up to 4 letters to create a new vector (of the same
// type) as long as the original vector has those components; it is not allowed to
// access the .z component of a vec2 for example. We can also pass vectors as arguments
// to different vector constructor calls, reducing the number of arguments required:

/*
vec2 vect = vec2(0.5, 0.7);
vec4 result = vec4(vect, 0.0, 0.0);
vec4 otherResult = vec4(result.xyz, 1.0);
*/

// Vectors are thus a flexible datatype that we can use for all kinds of input and
// output. Throughout the book you'll see plenty of examples of how we can creatively
// manage vectors.

// Uniforms:
// Uniforms are another way to pass data from our application on the CPU to the shaders
// on the GPU. Uniforms are however slightly different compared to vertex attributes.
// First of all, uniforms are global. Global, meaning that a uniform variable is unique
// per shader program object, and can be accessed from any shader at any stage in the
// shader program. Second, whatever you set the uniform value to, uniforms will keep
// their values until they're either reset or updated.
// we can declare a uniform as:
// uniform vec4 ourColor;
// and then set it to a value through our C/C++ code:
// int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
// glUseProgram(shaderProgram);
// glUniform4f(vertexColorLocation, 1.0f, 0.5f, 0.2f, 1.0f);
// We supply the shader program and the name of the uniform (that we want to retrieve
// the location from) to the query function. If glGetUniformLocation returns -1, it
// could not find the location. Lastly we can set the uniform value using the
// glUniform4f function. Note that finding the uniform location does not require you to
// use the shader program first, but updating a uniform does require you to first use
// the program (by calling glUseProgram), because it sets the uniform on the currently
// active shader program.
namespace shaders {
const char* vertex_shader_src =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 vertexColor;\n"
    "void main()\n"
    "{\n"
    //"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   vertexColor = aColor;\n"
    "}\0";

const char* fragment_shader_src =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 vertexColor;\n"
    "void main()\n"
    "{\n"
    //"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "   FragColor = vec4(vertexColor, 1.0);\n"
    "}\n\0";
} // namespace shaders
