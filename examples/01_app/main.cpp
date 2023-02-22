#include <bg2e/app/Window.hpp>
#include <bg2e/app/MainLoop.hpp>
#include <bg2e/app/AppController.hpp>

#include <iostream>
#include <iomanip>
#include <thread>

class MyAppController : public bg2e::app::AppController
{
public:
    void init()
    {
        std::cout << "init" << std::endl;
    }
    
    void frame(float delta)
    {
        static uint32_t frames = 0;
        static float elapsed = 0.0f;
        if (elapsed >= 1.0f)
        {
            window().setTitle("Hello World! - " + std::to_string(frames) +  " fps");
            frames = 0;
            elapsed = 0.0f;
        }
        else
        {
            ++frames;
            elapsed += delta;
        }
    }
    
    void display()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    void destroy()
    {
        std::cout << "Destroy" << std::endl;
    }
    
    void keyDown(const bg2e::app::KeyboardEvent& evt)
    {
        std::cout << "Key down: " << evt.keyName() << std::endl;
    }
    
    void keyUp(const bg2e::app::KeyboardEvent& evt)
    {
        std::cout << "Key up: " << evt.keyName() << std::endl;
    }
    
    void mouseMove(const bg2e::app::MouseEvent& evt)
    {
        std::cout << "mouseMove(" << evt.mouseStatus().posX << "," << evt.mouseStatus().posY << ")"
            << " leftButton=" << (evt.mouseStatus().leftButton ? "true" : "false")
            << " middleButton=" << (evt.mouseStatus().middleButton ? "true" : "false")
            << " rightButton=" << (evt.mouseStatus().rightButton ? "true" : "false") << std::endl;
    }
    
    void mouseDrag(const bg2e::app::MouseEvent& evt)
    {
        std::cout << "mouseDrag(" << evt.mouseStatus().posX << "," << evt.mouseStatus().posY << ")"
            << " leftButton=" << (evt.mouseStatus().leftButton ? "true" : "false")
            << " middleButton=" << (evt.mouseStatus().middleButton ? "true" : "false")
            << " rightButton=" << (evt.mouseStatus().rightButton ? "true" : "false") << std::endl;
    }
    
    void mouseDown(const bg2e::app::MouseEvent& evt)
    {
        using namespace bg2e::app;
        std::cout << "mouseDown - ";
        switch (evt.button())
        {
        case MouseButton::ButtonLeft:
            std::cout << "left" << std::endl;
            break;
        case MouseButton::ButtonMiddle:
            std::cout << "middle" << std::endl;
            break;
        case MouseButton::ButtonRight:
            std::cout << "right" << std::endl;
            break;
        case MouseButton::ButtonNone:
            break;
        }
    }
    
    void mouseUp(const bg2e::app::MouseEvent& evt)
    {
        using namespace bg2e::app;
        std::cout << "mouseUp - ";
        switch (evt.button())
        {
        case MouseButton::ButtonLeft:
            std::cout << "left" << std::endl;
            break;
        case MouseButton::ButtonMiddle:
            std::cout << "middle" << std::endl;
            break;
        case MouseButton::ButtonRight:
            std::cout << "right" << std::endl;
            break;
        case MouseButton::ButtonNone:
            break;
        }
    }
    
    void mouseWheel(const bg2e::app::MouseEvent& evt)
    {
        std::cout << "mouseWheel - deltaX=" <<
            evt.mouseStatus().deltaX << ", deltaY=" <<
            evt.mouseStatus().deltaY << std::endl;
    }
};

int main(int argc, char ** argv)
{
    using namespace bg2e::app;
    
    auto wnd = std::make_unique<Window>("Hello World!", 1024, 768);
    
    wnd->setAppController(std::make_unique<MyAppController>());

    MainLoop mainLoop(std::move(wnd));
    return mainLoop.run();
}
