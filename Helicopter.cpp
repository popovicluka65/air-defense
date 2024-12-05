#include "Helicopter.h"
#include <iostream>
#include <string>

Helicopter::Helicopter() : name(""), x(0.0f), y(0.0f), isSelected(false) {}

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

void Helicopter::printInfo() const {
    std::cout << "Helicopter Name: " << name << std::endl;
    std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
    std::cout << "Selected: " << (isSelected ? "Yes" : "No") << std::endl; // Dodata informacija o selekciji
}