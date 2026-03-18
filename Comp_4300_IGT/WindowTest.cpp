#include <SFML/Graphics.hpp>
#include <optional> // Required for the new event system

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Window Test");

    sf::CircleShape shape(100.f);

    shape.setPosition({ 100.f, 100.f });
    shape.setFillColor(sf::Color::Green);
    while (window.isOpen()) {

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}