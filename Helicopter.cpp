#include "Helicopter.h"
#include <iostream>
#include <string>

Helicopter::Helicopter() : name(""), x(0.0f), y(0.0f), isSelected(false),hasRocket(false) {}

Helicopter::Helicopter(const std::string& name, float x, float y)
    : name(name), x(x), y(y), isSelected(false) {}

void Helicopter::setName(const std::string& name) {
    this->name = name;
}

void Helicopter::setPosition(float x, float y) {
    this->x = x;
    this->y = y;
}

void Helicopter::setSelected(bool selected) {
    this->isSelected = selected;
}

void Helicopter::setHasRocket(bool hasRocket) {
    this->hasRocket = hasRocket;
}

std::string Helicopter::getName() const {
    return name;
}

void Helicopter::getPosition(float& xOut, float& yOut) const {
    xOut = x;
    yOut = y;
}

bool Helicopter::getSelected() const {
    return isSelected;
}

bool Helicopter::getHasRocket() const {
    return hasRocket;
}

void Helicopter::printInfo() const {
    std::cout << "Helicopter Name: " << name << std::endl;
    std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
    std::cout << "Selected: " << (isSelected ? "Yes" : "No") << std::endl; // Dodata informacija o selekciji
}