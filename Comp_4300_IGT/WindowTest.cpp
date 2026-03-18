#include <SFML/Graphics.hpp>
#include <optional> // Required for the new event system

int main() {

    const int Width = 1280;
    const int Height = 720;
    sf::RenderWindow window(sf::VideoMode({ Width, Height }), "SFML Window Test");
    window.setFramerateLimit(60);


    sf::CircleShape shape(100.f);

    float moveSpeedx = 1.0f;
    float moveSpeedy = 1.0f;

    shape.setPosition({ 300.f, 200.f });
    shape.setFillColor(sf::Color::Green);
    while (window.isOpen()) {

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        shape.setPosition({ shape.getPosition().x + moveSpeedx, shape.getPosition().y + moveSpeedy });
        if (shape.getPosition().x + shape.getRadius() * 2 > Width || shape.getPosition().x < 0) {
            moveSpeedx *= -1;
            if (shape.getFillColor() == sf::Color::Green) shape.setFillColor(sf::Color::Blue);
            else shape.setFillColor(sf::Color::Green);
        }
        else if (shape.getPosition().y + 2*shape.getRadius() > Height || shape.getPosition().y <0) {
            moveSpeedy *= -1;

            if (shape.getFillColor() == sf::Color::Green) shape.setFillColor(sf::Color::Blue);
            else shape.setFillColor(sf::Color::Green);
        }
        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}