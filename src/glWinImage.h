#pragma once
#include "glWinBase.h"
#include <math.h>
#include <cstring>
#include <iostream>
#include "utils.h"


template<typename GridCellsType, typename FrameType>
class GlWinImage : public GlWinBase
{
    static constexpr uint16_t GRID_SIZE = GridCellsType::GRID_SIZE;
    auto POS(auto x, auto y) { return GridCellsType::POS(x,y); }
public:
    GlWinImage(GridCellsType& gc, FrameType& f) : mGridCells(gc), mFrame(f)
    {}

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

private:
    void drawImage(const int width, const int height)
    {
        glPixelZoom(width/static_cast<float>(GRID_SIZE), -height/static_cast<float>(GRID_SIZE));
        glRasterPos2i(0, height);
        glDrawPixels(GRID_SIZE, GRID_SIZE, GL_RGB, GL_FLOAT, &mGridCells.image[0]);
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
                const double gridX = std::max(0.0, std::min<double>(GRID_SIZE-1, GRID_SIZE * px / (double)width));
                const double gridY = std::max(0.0, std::min<double>(GRID_SIZE-1, GRID_SIZE * py / (double)height));
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
        }
    }


    GridCellsType& mGridCells;
    FrameType& mFrame;

    bool mMouseLeftDown{false};
    XYPair mLastMousePos{};
    bool mQuit{false};
};
