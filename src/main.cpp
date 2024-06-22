// OpenGL is only really a standard/specification it is up to the driver manufacturer to implement
// the specification to a driver that the specific graphics card supports. Since there are many
// different versions of OpenGL drivers, the location of most of its functions is not known at
// compile-time and needs to be queried at run-time. It is then the task of the developer to
// retrieve the location of the functions he/she needs and store them in function pointers for
// later use. For which we use GLAD library.
#include <glad/glad.h>

// GLFW is a library, written in C, specifically targeted at OpenGL. GLFW gives us the bare
// necessities required for rendering goodies to the screen. It allows us to create an OpenGL
// context, define window parameters, and handle user input, which is plenty enough for our
// purposes.
#include <GLFW/glfw3.h>

#include <fmt/core.h>

auto constexpr WINDOW_WIDTH = 800;
auto constexpr WINDOW_HEIGHT = 600;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void escape_key_pressed_callback(GLFWwindow* window);

auto main() -> int {
    // initialized GLFW
    glfwInit();

    // Since the focus of this book is on OpenGL version 3.3 we'd like to tell GLFW that 3.3 is
    // the OpenGL version we want to use. This way GLFW can make the proper arrangements when
    // creating the OpenGL context. This ensures that when a user does not have the proper OpenGL
    // version GLFW fails to run. We set the major and minor version both to 3. We also tell GLFW
    // we want to explicitly use the core-profile. Telling GLFW we want to use the core-profile
    // means we'll get access to a smaller subset of OpenGL features without backwards-compatible
    // features we no longer need.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "gl", nullptr, nullptr);

    if (window == nullptr) {
        fmt::println("Failed to create a GLFW window. Exiting.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // we want to initialize GLAD before we call any OpenGL function
    if (!static_cast<bool>(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))) {
        fmt::println("Failed to initialize GLAD");
        return -1;
    }

    // The first two parameters of glViewport set the location of the lower left corner of the
    // window. The third and fourth parameter set the width and height of the rendering window in
    // pixels, which we set equal to GLFW's window size.
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);


    while (!static_cast<bool>(glfwWindowShouldClose(window))) {
        // If escape key is pressed, the windows should be closed.
        escape_key_pressed_callback(window);

        // We can clear the screen's color buffer using glClear where we pass in buffer bits to
        // specify which buffer we would like to clear. The possible bits we can set are
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT.
        // the glClearColor function is a state-setting function and glClear is a state-using
        // function in that it uses the current state to retrieve the clearing color from.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // The glfwSwapBuffers will swap the color buffer (a large 2D buffer that contains color
        // values for each pixel in GLFW's window) that is used to render to during this render
        // iteration and show it as output to the screen.
        glfwSwapBuffers(window);

        // The glfwPollEvents function checks if any events are triggered (like keyboard input or
        // mouse movement events), updates the window state, and calls the corresponding functions
        // (which we can register via callback methods).
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void escape_key_pressed_callback(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, static_cast<int>(true));
    }
}