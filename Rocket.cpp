#include "Rocket.h"
#include <iostream>
#include <string>

Rocket::Rocket() : name(""), x(0.0f), y(0.0f), isSelected(false), hasTarget(-1) {}

Rocket::Rocket(const std::string& name, float x, float y)
    : name(name), x(x), y(y), isSelected(false) {}

void Rocket::setName(const std::string& name) {
    this->name = name;
}

void Rocket::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
}

void Rocket::setSelected(bool selected) {
    this->isSelected = selected;
}

void Rocket::setHasTarget(int hasTarget) {
    this->hasTarget = hasTarget;
}

std::string Rocket::getName() const {
    return name;
}

void Rocket::getPosition(float& xOut, float& yOut) const {
    xOut = x;
    yOut = y;
}

bool Rocket::getSelected() const {
    return isSelected;
}

int Rocket::getHasTarget() const {
    return hasTarget;
}

void Rocket::printInfo() const {
    std::cout << "Helicopter Name: " << name << std::endl;
    std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
    std::cout << "Selected: " << (isSelected ? "Yes" : "No") << std::endl; // Dodata informacija o selekciji
    std::cout << "Has target: " << (hasTarget) << std::endl;
}