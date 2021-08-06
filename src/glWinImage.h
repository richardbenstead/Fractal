#pragma once
#include <math.h>
#include <cstring>
#include <iostream>
#include "utils.h"
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <complex>

class GlWinImage
{
    static constexpr uint16_t IMAGE_SIZE = 1200;
    using ImageType = Image<IMAGE_SIZE>;
    using FrameType = Frame<ImageType>;
    auto POS(auto x, auto y) { return ImageType::POS(x,y); }

public:
    void draw()
    {
        int width, height;
        glfwGetWindowSize(mpWindow, &width, &height);

        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glOrtho(0, width, 0, height, -1.0, 1.0);
        glViewport(0,0,width,height);

        drawImage(width, height);
        
        glfwSwapBuffers(mpWindow);
        glfwPollEvents();
    }

    bool isFinished()
    {
        return mQuit || glfwWindowShouldClose(mpWindow);
    }

    void initialize(const std::string& title)
    {
        mpWindow = glfwCreateWindow(1200, 800, title.c_str(), nullptr, nullptr);
        if (!mpWindow) {
            throw std::runtime_error("glfwCreateWindow failed");
        }

        glfwMakeContextCurrent(mpWindow);
        glfwSetWindowUserPointer(mpWindow, this);

        glfwSetMouseButtonCallback(mpWindow, [](GLFWwindow *window, int button, int action, int mods) {
                static_cast<GlWinImage*>(glfwGetWindowUserPointer(window))->mouseEvent(window, button, action, mods); });

        glfwSetCursorPosCallback(mpWindow, [](GLFWwindow *window, double xpos, double ypos) {
                static_cast<GlWinImage*>(glfwGetWindowUserPointer(window))->mouseMoveEvent(window, xpos, ypos); });

        glfwSetKeyCallback(mpWindow, [](GLFWwindow* window, int key, int sc, int action, int mods) {
                static_cast<GlWinImage*>(glfwGetWindowUserPointer(window))->keyEvent(window, key, sc, action, mods); });
    }

private:
    inline double getValueM(const XYPair& loc)
    {
        using z_type = std::complex<double>;
        z_type z{loc.x, loc.y};
        z_type m{};
        int i{0};
        constexpr int maxIter{100};
        for(; i<=maxIter; ++i)
        {
            m = m * m + z;
            if (abs(m) > 10) {
                [[unlikely]];
                return i / static_cast<double>(maxIter);
            }
        }
        return 0;
    }

    inline double getValueJ(const XYPair& loc)
    {
        using z_type = std::complex<double>;
        z_type z{-0.5, 0.5};
        z_type m{loc.x, loc.y};
        int i{0};
        constexpr int maxIter{100};
        for(; i<=maxIter; ++i)
        {
            m = m * m + z;
            if (abs(m) > 10) {
                [[unlikely]];
                return i / static_cast<double>(maxIter);
            }
        }
        return 0;
    }
    void drawImage(const int width, const int height)
    {
        int idx = 0;

        if (mType == 0) {
            for(int y=0; y<IMAGE_SIZE; ++y) {
                for(int x=0; x<IMAGE_SIZE; ++x) {
                    const double val = getValueM(mFrame.imageToWorld(x,y));
                    mImage.image[idx++] = mPalette[mPalette.SIZE * val];
                }
            }
        } else {
            for(int y=0; y<IMAGE_SIZE; ++y) {
                for(int x=0; x<IMAGE_SIZE; ++x) {
                    const double val = getValueJ(mFrame.imageToWorld(x,y));
                    mImage.image[idx++] = mPalette[mPalette.SIZE * val];
                }
            }
        }

        glPixelZoom(width/static_cast<float>(IMAGE_SIZE), -height/static_cast<float>(IMAGE_SIZE));
        glRasterPos2i(0, height);
        glDrawPixels(IMAGE_SIZE, IMAGE_SIZE, GL_RGB, GL_FLOAT, &mImage.image[0]);
    }

    void mouseEvent(GLFWwindow *window, int button, int action, [[maybe_unused]] int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            double px, py;
            glfwGetCursorPos(window, &px, &py);
            mLastMousePos = XYPair(px, py);
            mMouseLeftDown = GLFW_PRESS==action;

            int width, height;
            glfwGetWindowSize(mpWindow, &width, &height);
            if (mMouseLeftDown) {
                const double gridX = std::max(0.0, std::min<double>(IMAGE_SIZE-1, IMAGE_SIZE * px / (double)width));
                const double gridY = std::max(0.0, std::min<double>(IMAGE_SIZE-1, IMAGE_SIZE * py / (double)height));
                const XYPair cen = mFrame.imageToWorld(gridX, gridY);
                mFrame.mCentre = cen;
                mFrame.mScale.x *= 0.8;
                mFrame.mScale.y *= 0.8;
            }
        }
    }

    void mouseMoveEvent([[maybe_unused]] GLFWwindow *window, double xpos, double ypos)
    {
        if (mMouseLeftDown) {
            int width, height;
            glfwGetWindowSize(mpWindow, &width, &height);

            XYPair newMousePos(xpos, ypos);
            [[maybe_unused]] XYPair delta = newMousePos - mLastMousePos;
        }
    }

    void keyEvent([[maybe_unused]] GLFWwindow *window, int key, [[maybe_unused]] int sc, int action, [[maybe_unused]] int mods)
    {
        if (GLFW_PRESS == action) {
            if (key == 'Q') mQuit = true;
            if (key == 'R') mFrame.reset();
            if (key == ' ') mType = (mType+1) % 2;
        }
    }

    GLFWwindow *mpWindow{};
    int mType{};
    Palette<> mPalette;
    ImageType mImage;
    FrameType mFrame;

    bool mMouseLeftDown{false};
    XYPair mLastMousePos{};
    bool mQuit{false};
};
