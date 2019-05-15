#include "ram.h"
#include <SFML/Graphics.hpp>

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 160
#define APP_NAME "GameBoy Emulator"

int main()
{
    // create the window
    sf::RenderWindow window(sf::VideoMode(SCREEN_HEIGHT, SCREEN_WIDTH), APP_NAME);

    RAM ram_inst = RAM();
    ram_inst.initialise();

    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        // window.draw(...);

        // end the current frame
        window.display();
    }

    return 0;
}