#include "Rocket.h"
#include <iostream>
#include <string>

Rocket::Rocket() : name(""), x(0.0f), y(0.0f), isSelected(false), target(-1), fly(false) {}

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

void Rocket::setTarget(int target) {
    this->target = target;
}

void  Rocket::setFly(bool fly) {
    this->fly = fly;
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

int Rocket::getTarget() const {
    return target;
}

bool Rocket::getFly() const {
    return fly;
}

void Rocket::printInfo() const {
    std::cout << "Helicopter Name: " << name << std::endl;
    std::cout << "Position: (" << x << ", " << y << ")" << std::endl;
    std::cout << "Selected: " << (isSelected ? "Yes" : "No") << std::endl; // Dodata informacija o selekciji
    std::cout << "Target: " << (target) << std::endl;
    std::cout << "Fly: " << (fly) << std::endl;
}