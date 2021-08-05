#include <GLFW/glfw3.h>
#include "glWinImage.h"
#include "utils.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <complex>
#include <math.h>


class MainApp
{
    static constexpr int GRID_SIZE{800};
    static constexpr int PALETTE_SIZE{1000};
public:
    using ImageType = Image<GRID_SIZE>;
    using FrameType = Frame<ImageType>;
    using WinImageType = GlWinImage<ImageType, FrameType>;

    MainApp() : mWinImage(mImage, mFrame)
    {
        if (!glfwInit()) {
            throw std::runtime_error("glfwInit failed");
        }
        mWinImage.initialize("Fractal");

        auto square = [](const double x) { return x*x; };
        for(int i=1; i<PALETTE_SIZE; ++i) {
            constexpr double scale = 1.0/0.05;
            constexpr double ps = static_cast<double>(PALETTE_SIZE);
            constexpr double ps2 = ps*ps;
            const double di = static_cast<double>(i);
            mPalette[i] = Pixel(std::exp(-scale * square(di-ps * 0.3)/ps2),
                          std::exp(-scale * square(di-ps * 0.6)/ps2),
                          std::exp(-scale * square(di-ps * 0.9)/ps2));
            // std::cout << i << " " << mPalette[i].r << "/" << mPalette[i].g << "/" << mPalette[i].b << std::endl;
        }
    }

    ~MainApp()
    {
        glfwTerminate();
    }

    void run()
    {
        while (!mWinImage.isFinished()) {
            mWinImage.draw();

            int idx = 0;
            for(int y=0; y<GRID_SIZE; ++y) {
                for(int x=0; x<GRID_SIZE; ++x) {
                    const XYPair loc = mFrame.imageToWorld(x,y);

                    using z_type = std::complex<double>;
                    z_type z{loc.x, loc.y};
                    z_type m{};
                    int i{0};
                    constexpr int maxIter{100};
                    for(; i<maxIter; ++i)
                    {
                        m = m * m + z;
                        if (abs(m) > 1e10) {
                            [[unlikely]];
                            break;
                        }
                    }
                    if (i==maxIter) {
                        i=0;
                    }

                    int palIdx = PALETTE_SIZE * (i / static_cast<double>(maxIter));
                    // std::cout << palIdx << " " << mPalette[palIdx].r << "/" << mPalette[palIdx].g << "/" << mPalette[palIdx].b << std::endl;
                    mImage.image[idx++] = mPalette[palIdx];
                }
            }
        }
    }

private:
    Pixel mPalette[PALETTE_SIZE];
    ImageType mImage;
    FrameType mFrame;
    WinImageType mWinImage;
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
    MainApp* sf = new MainApp();
    sf->run();
    delete sf;

    return 0;
}
