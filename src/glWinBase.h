#pragma once
#include <GL/gl.h>
#include <math.h>
#include <cstring>
#include <iostream>
#include <GLFW/glfw3.h>

class GlWinBase
{
public:
    virtual ~GlWinBase() {}
    virtual void initialize(const std::string& title)
    {
        mpWindow = glfwCreateWindow(1200, 800, title.c_str(), nullptr, nullptr);
        if (!mpWindow) {
            throw std::runtime_error("glfwCreateWindow failed");
        }

        glfwMakeContextCurrent(mpWindow);
        glfwSetWindowUserPointer(mpWindow, this);

        glfwSetMouseButtonCallback(mpWindow, [](GLFWwindow *window, int button, int action, int mods) {
                static_cast<GlWinBase*>(glfwGetWindowUserPointer(window))->mouseEvent(window, button, action, mods); });

        glfwSetCursorPosCallback(mpWindow, [](GLFWwindow *window, double xpos, double ypos) {
                static_cast<GlWinBase*>(glfwGetWindowUserPointer(window))->mouseMoveEvent(window, xpos, ypos); });

        glfwSetKeyCallback(mpWindow, [](GLFWwindow* window, int key, int sc, int action, int mods) {
                static_cast<GlWinBase*>(glfwGetWindowUserPointer(window))->keyEvent(window, key, sc, action, mods); });
    }

    virtual void draw() = 0;

protected:
    virtual void mouseEvent(GLFWwindow *window, int button, int action, [[maybe_unused]] int mods) = 0;
    virtual void mouseMoveEvent([[maybe_unused]] GLFWwindow *window, double xpos, double ypos) = 0;
    virtual void keyEvent([[maybe_unused]] GLFWwindow *window, int key, [[maybe_unused]] int sc, int action, [[maybe_unused]] int mods) = 0;

    GLFWwindow *mpWindow{};
};

