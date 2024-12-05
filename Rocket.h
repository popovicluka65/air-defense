#pragma once
#ifndef ROCKET_H
#define ROCKET_H

#include <string> // Za std::string

class Rocket {
private:
    std::string name;
    float x;
    float y;
    bool isSelected;
    int hasTarget;

public:
    // Konstruktori
    Rocket();
    Rocket(const std::string& name, float x, float y);

    // Setteri
    void setName(const std::string& name);
    void setPosition(float x, float y);
    void setSelected(bool selected); // Setter za isSelected
    void setHasTarget(int hasTarget);

    // Getteri
    std::string getName() const;
    void getPosition(float& xOut, float& yOut) const;
    bool getSelected() const; // Getter za isSelected
    int getHasTarget() const;

    // Metoda za ispis
    void printInfo() const;
};

#endif