#include<SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include<optional>



const int Width = 1280;
const int Height = 720;

int CollisionwithBorder( const sf::CircleShape &shape) {
	if (shape.getPosition().x + shape.getRadius() * 2 > Width || shape.getPosition().x < 0) {
		return 1;
	}
	else if (shape.getPosition().y + 2 * shape.getRadius() > Height || shape.getPosition().y < 0) {
		return 2;
	}
	return 0;
}

int CollisionRectangle(const sf::RectangleShape& rect) {
	if (rect.getPosition().x + rect.getSize().x > Width || rect.getPosition().x < 0) {
		return 1;
	}
	else if (rect.getPosition().y + rect.getSize().y > Height || rect.getPosition().y < 0) {
		return 2;
	}
	return 0;
}

void UpdatePosition(sf::Shape& shape, sf::Vector2f& moveSpeed) {
	shape.setPosition({ shape.getPosition().x + moveSpeed.x, shape.getPosition().y + moveSpeed.y });
}

int main() {
	sf::RenderWindow window(sf::VideoMode({ Width, Height }), "SFML Window Test");
	window.setFramerateLimit(60);
	
	
	//Shapes

	//Circle
	std::vector<sf::CircleShape> circles;
	circles.push_back(sf::CircleShape(50.f));
	circles.push_back(sf::CircleShape(100.f));
	circles.push_back(sf::CircleShape(175.f));
	circles[0].setPosition({ 300.f, 200.f });
	circles[1].setPosition({ 500.f, 400.f });
	circles[2].setPosition({ 700.f, 100.f });
	circles[0].setFillColor(sf::Color::Green);
	circles[1].setFillColor(sf::Color::Blue);
	circles[2].setFillColor(sf::Color::Magenta);
		
	std::vector < sf::RectangleShape > rectangles;
	rectangles.push_back(sf::RectangleShape({ 200.f, 100.f }));
	rectangles.push_back(sf::RectangleShape({ 150.f, 200.f }));
	rectangles.push_back(sf::RectangleShape({ 300.f, 50.f }));
	rectangles[0].setPosition({ 100.f, 500.f });
	rectangles[1].setPosition({ 400.f, 100.f });
	rectangles[2].setPosition({ 800.f, 300.f });
	rectangles[0].setFillColor(sf::Color::Yellow);
	rectangles[1].setFillColor(sf::Color::Cyan);
	rectangles[2].setFillColor(sf::Color::Red);

	std::vector<sf::Vector2f> circleMoveSpeeds;
	circleMoveSpeeds.push_back({ 1.0f, 1.0f });
	circleMoveSpeeds.push_back({ 1.5f, 0.5f });
	circleMoveSpeeds.push_back({ 0.5f, 1.5f });

	std::vector<sf::Vector2f> rectangleMoveSpeeds;
	rectangleMoveSpeeds.push_back({ 1.0f, 1.0f });
	rectangleMoveSpeeds.push_back({ 1.5f, 0.5f });
	rectangleMoveSpeeds.push_back({ 0.5f, 1.5f });


	//Text
	sf::Font myFont;

	if (!myFont.openFromFile("Fonts/Coolvetica Rg Cram.otf")) {
		std::cerr << "Cant Load my Font from File";
		exit(-1);
	}
	std::vector<sf::Text> texts;
	texts.push_back(sf::Text(myFont, "Circle 1", 35));
	texts.push_back(sf::Text(myFont, "Circle 2", 35));
	texts.push_back(sf::Text(myFont, "Circle 3", 35));
	texts.push_back(sf::Text(myFont, "Rectangle 1", 35));
	texts.push_back(sf::Text(myFont, "Rectangle 2", 35));
	texts.push_back(sf::Text(myFont, "Rectangle 3", 35));


	// Main Loop
	while (window.isOpen()) {
		
		while (const std::optional<sf::Event> event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}


			


		}

		window.clear();

		for (int i = 0; i < circles.size(); i++) {
			circles[i].setPosition({ circles[i].getPosition().x + circleMoveSpeeds[i].x, circles[i].getPosition().y + circleMoveSpeeds[i].y });
			int collisionResult = CollisionwithBorder(circles[i]);
			if (collisionResult == 1) {
				circleMoveSpeeds[i].x *= -1;
			}
			else if (collisionResult == 2) {
				circleMoveSpeeds[i].y *= -1;
			}
			texts[i].setPosition({ circles[i].getPosition().x + circles[i].getGeometricCenter().x - (texts[i].findCharacterPos(texts[i].getString().getSize() / 2) - texts[i].getGlobalBounds().position).x  
			, circles[i].getPosition().y + circles[i].getGeometricCenter().y -texts[i].getCharacterSize()/2 });

			window.draw(circles[i]);

			window.draw(texts[i]);
		}

		for (int i = 0; i < rectangles.size(); i++) {
			rectangles[i].setPosition({ rectangles[i].getPosition().x + rectangleMoveSpeeds[i].x, rectangles[i].getPosition().y + rectangleMoveSpeeds[i].y });
			int collisionResult = CollisionRectangle(rectangles[i]);
			if (collisionResult == 1) {
				rectangleMoveSpeeds[i].x *= -1;
			}
			else if (collisionResult == 2) {
				rectangleMoveSpeeds[i].y *= -1;
			}
			texts[i + circles.size()].setPosition({ rectangles[i].getPosition().x + rectangles[i].getSize().x / 2 - (texts[i + circles.size()].findCharacterPos(texts[i + circles.size()].getString().getSize() / 2) - texts[i + circles.size()].getGlobalBounds().position).x,
				rectangles[i].getPosition().y +rectangles[i].getSize().y/2 - texts[i + circles.size()].getCharacterSize() / 2});

			window.draw(rectangles[i]);

			window.draw(texts[i + circles.size()]);
		}


		window.display();
	}

	return 0;
}
