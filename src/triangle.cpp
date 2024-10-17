//
// DISCLAIMER: Most of the comments in this file are taken from learnopengl.com
//
// OpenGL is only really a standard/specification it is up to the driver manufacturer
// to implement the specification to a driver that the specific graphics card supports.
// Since there are many different versions of OpenGL drivers, the location of most of
// its functions is not known at compile-time and needs to be queried at run-time. It
// is then the task of the developer to retrieve the location of the functions he/she
// needs and store them in function pointers for later use. For which we use GLAD
// library.
#include <glad/glad.h>

// GLFW is a library, written in C, specifically targeted at OpenGL. GLFW gives us the
// bare necessities required for rendering goodies to the screen. It allows us to
// create an OpenGL context, define window parameters, and handle user input, which is
// plenty enough for our purposes.
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include "triangle_shader.H"

auto constexpr WINDOW_WIDTH = 800;
auto constexpr WINDOW_HEIGHT = 600;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void escape_key_pressed_callback(GLFWwindow* window);

auto main(void) -> int {
    // initialized GLFW
    spdlog::info("Initializing GLFW");
    glfwInit();

    // We'd tell GLFW that 3.3 is the OpenGL version we want to use. This way GLFW can
    // make the proper arrangements when creating the OpenGL context. This ensures that
    // when a user does not have the proper OpenGL version GLFW fails to run. We set
    // the major and minor version both to 3. We also tell GLFW we want to explicitly
    // use the core-profile. Telling GLFW we want to use the core-profile means we'll
    // get access to a smaller subset of OpenGL features without backwards-compatible
    // features we no longer need.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window =
        glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gl", nullptr, nullptr);

    if (window == nullptr) {
        spdlog::error("Failed to create a GLFW window. Exiting.");
        glfwTerminate();
        return -1;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    // we want to initialize GLAD before we call any OpenGL function
    if (!static_cast<bool>(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))) {
        spdlog::error("Failed to initialize GLAD");
        return -1;
    }

    // The first two parameters of glViewport set the location of the lower left corner
    // of the window. The third and fourth parameter set the width and height of the
    // rendering window in pixels, which we set equal to GLFW's window size.
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Prepare our vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    // The glShaderSource function takes the shader object to compile to as its first
    // parameter. The second parameter specifies how many strings we're passing as
    // source code, which is only one. The third parameter is the actual source code of
    // the vertex shader and we can leave the 4th parameter to null.
    spdlog::info("Compiling vertex shader");
    glShaderSource(vertex_shader, 1, &shaders::vertex_shader_src, nullptr);
    glCompileShader(vertex_shader);

    // check if shader compilation was successful
    int success;
    char info_log[512];

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!static_cast<bool>(success)) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        spdlog::error("Vertex shader compilation failed: {}", info_log);
    }

    // The fragment shader is the second and final shader we're going to create for
    // rendering a triangle. The fragment shader is all about calculating the color
    // output of your pixels. To keep things simple the fragment shader will always
    // output a constant color.
    // The process for compiling a fragment shader is similar to the vertex shader,
    // although this time we use the GL_FRAGMENT_SHADER constant as the shader type
    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &shaders::fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!static_cast<bool>(success)) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        spdlog::error("Fragment shader compilation failed: {}", info_log);
    }
    // Both the shaders are now compiled and the only thing left to do is link both
    // shader objects into a shader program that we can use for rendering.
    unsigned int shader_program;
    shader_program = glCreateProgram();

    // Now we can attach the vertex and fragment shaders to the shader program.
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!static_cast<bool>(success)) {
        glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
        spdlog::error("Shader program compilation failed: {}", info_log);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Because we want to render a single triangle we want to specify a total of three
    // vertices with each vertex having a 3D position. We define them in normalized
    // device coordinates (the visible region of OpenGL) in a float array.
    // clang-format off
    float vertices[] = {
       -0.5f, -0.5f, 0.0f,  // Bottom left
        0.5f, -0.5f, 0.0f,  // Bottom right
        0.0f,  0.5f, 0.0f   // Top
    };

    unsigned int indices[] = {
        0, 1, 2  // First triangle
    };
    // clang-format on

    // With the vertex data defined we'd like to send it as input to the first process
    // of the graphics pipeline: the vertex shader. This is done by creating memory on
    // the GPU where we store the vertex data, configure how OpenGL should interpret
    // the memory and specify how to send the data to the graphics card. The vertex
    // shader then processes as much vertices as we tell it to from its memory.
    unsigned int VAO; // Vertex array object
    unsigned int VBO; // Vertex buffer object
    unsigned int EBO; // Element buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    // From that point on any buffer calls we make (on the GL_ARRAY_BUFFER target) will
    // be used to configure the currently bound buffer, which is VBO. Then we can make
    // a call to the glBufferData function that copies the previously defined vertex
    // data into the buffer's memory: The fourth parameter specifies how we want the
    // graphics card to manage the given data. This can take 3 forms:
    // GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few
    // times.
    // GL_STATIC_DRAW: the data is set only once and used many times.
    // GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    spdlog::info("Copying triangle vertex data to vertix buffer");
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Similar to the VBO we bind the EBO and copy the indices into the buffer with
    // glBufferData. Also, just like the VBO we want to place those calls between a
    // bind and an unbind call, although this time we specify GL_ELEMENT_ARRAY_BUFFER
    // as the buffer type.
    spdlog::info("Copying triangle element indices to element buffer");
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Right now we sent the input vertex data to the GPU and instructed the GPU how
    // it should process the vertex data within a vertex and fragment shader. We're
    // almost there, but not quite yet. OpenGL does not yet know how it should
    // interpret the vertex data in memory and how it should connect the vertex data
    // to the vertex shader's attributes. We'll be nice and tell OpenGL how to do
    // that.
    // The function glVertexAttribPointer has quite a few parameters so let's
    // carefully walk through them:
    // 1) The first parameter specifies which vertex attribute we want to configure.
    // Remember that we specified the location of the position vertex attribute in the
    // vertex shader with `layout (location = 0)`
    // 2) The second parameter specifies the size of the vertex attribute. The vertex
    // attribute is a vec3 so it is composed of 3 values.
    // 3) The third parameter specifies the type of the data which is GL_FLOAT (a vec*
    // in GLSL consists of floating point values).
    // 4) the fourth parameter specifies if we want the data to be normalized. If we're
    // inputting integer data types (int, byte) and we've set this to GL_TRUE, the
    // integer data is normalized to 0 (or -1 for signed data) and 1 when converted to
    // float. This is not relevant for us so we'll leave this at GL_FALSE.
    // 5) The fifth parameter is known as the stride and tells us the space between
    // consecutive vertex attributes. Since the next set of position data is located
    // exactly 3 times the size of a float away we specify that value as the stride.
    // Note that since we know that the array is tightly packed (there is no space
    // between the next vertex attribute value) we could've also specified the stride
    // as 0 to let OpenGL determine the stride (this only works when values are tightly
    // packed). Whenever we have more vertex attributes we have to carefully define the
    // spacing between each vertex attribute but we'll get to see more examples of that
    // later on.
    // 6) The last parameter is of type void* and thus requires that weird cast. This
    // is the offset of where the position data begins in the buffer. Since the
    // position data is at the start of the data array this value is just 0.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // I am not sure why this is needed. Will be commented out for now.
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindVertexArray(0);

    while (!static_cast<bool>(glfwWindowShouldClose(window))) {
        // If escape key is pressed, the windows should be closed.
        escape_key_pressed_callback(window);

        // We can clear the screen's color buffer using glClear where we pass in buffer
        // bits to specify which buffer we would like to clear. The possible bits we
        // can set are GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and
        // GL_STENCIL_BUFFER_BIT. the glClearColor function is a state-setting function
        // and glClear is a state-using function in that it uses the current state to
        // retrieve the clearing color from.
        glClearColor(0.11f, 0.11f, 0.11f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        // Every shader and rendering call after glUseProgram will now use this program
        // object (and thus the shaders).
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        // The glfwSwapBuffers will swap the color buffer (a large 2D buffer that
        // contains color values for each pixel in GLFW's window) that is used to
        // render to during this render iteration and show it as output to the screen.
        glfwSwapBuffers(window);

        // The glfwPollEvents function checks if any events are triggered (like
        // keyboard input or mouse movement events), updates the window state, and
        // calls the corresponding functions (which we can register via callback
        // methods).
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    spdlog::info("Resizing framebuffer to {}x{}", width, height);
    glViewport(0, 0, width, height);
}

void escape_key_pressed_callback(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        spdlog::info("Escape key pressed");
        glfwSetWindowShouldClose(window, static_cast<int>(true));
    }
}
