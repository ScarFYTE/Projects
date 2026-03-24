#include<SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include<optional>
#include<memory>
#include<cmath>

const int Width = 1280;
const int Height = 720;

int CollisionCheck(sf::Shape& shape) {
	if (shape.getPosition().x + shape.getGlobalBounds().size.x > Width || shape.getPosition().x < 0) {
		return 1; // Collision on X-axis
	}
	else if (shape.getPosition().y + shape.getGlobalBounds().size.y > Height || shape.getPosition().y < 0) {
		return 2; // Collision on Y-axis
	}
	return 0; // No collision
}
int CollisionwithOtherShape(sf::Shape& shape, std::vector<std::shared_ptr<sf::Shape>>& shapes, std::vector <sf::Vector2f>& moveSpeeds) {
	for (auto& otherShape : shapes) {
		//Skip checking collision with itself
		if (otherShape.get() == &shape) {
			continue;
		}

		if (shape.getGlobalBounds().findIntersection(otherShape->getGlobalBounds())) {
			return 1; // Collision with another shape
		}
	}
	return 0; // No collision
}


void UpdatePosition(sf::Shape& shape, sf::Vector2f& moveSpeed) {
	shape.setPosition({ shape.getPosition().x + moveSpeed.x, shape.getPosition().y + moveSpeed.y });
}

int testing() {
	sf::RenderWindow window(sf::VideoMode({ Width, Height }), "SFML Window Test");
	window.setFramerateLimit(60);
	

	//Create Shared PTR for Shapes
	std::vector<std::shared_ptr<sf::Shape>> shapes;

	shapes.push_back(std::make_shared<sf::CircleShape>(100.f));
	shapes.push_back(std::make_shared<sf::RectangleShape>(sf::Vector2f(200.f, 100.f)));
	shapes.push_back(std::make_shared<sf::CircleShape>(50.f));
	shapes.push_back(std::make_shared<sf::RectangleShape>(sf::Vector2f(150.f, 150.f)));

	shapes[0]->setPosition({ 300.f, 200.f });
	shapes[0]->setFillColor(sf::Color::Green);
	shapes[1]->setPosition({ 500.f, 400.f });
	shapes[1]->setFillColor(sf::Color::Red);
	shapes[2]->setPosition({ 700.f, 100.f });	
	shapes[2]->setFillColor(sf::Color::Blue);
	
	std::vector <sf::Vector2f> moveSpeeds = { {1.0f, 1.0f}, {1.5f, 1.0f}, {2.0f, 1.5f}, {1.0f, 2.0f} };


	// Main Loop
	while (window.isOpen()) {
		
		while (const std::optional<sf::Event> event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			
		}

		window.clear();
		

		for (auto& shape : shapes) {
			shape->setPosition({ shape->getPosition().x + moveSpeeds[&shape - &shapes[0]].x, shape->getPosition().y + moveSpeeds[&shape - &shapes[0]].y });
			int collisionResult = CollisionCheck(*shape);
			if (collisionResult == 1) {
				moveSpeeds[&shape - &shapes[0]].x *= -1; // Reverse X direction
				shape->setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256)); // Change to random color
			}
			else if (collisionResult == 2) {
				moveSpeeds[&shape - &shapes[0]].y *= -1; // Reverse Y direction
				shape->setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256)); // Change to random color
			}
			else if (CollisionwithOtherShape(*shape , shapes,moveSpeeds)) {
				shape->setFillColor(sf::Color(rand() % 256, rand() % 256, rand() % 256)); // Change to random color
			}

			window.draw(*shape);
		}
		
		window.display();
	}

	return 0;
}
