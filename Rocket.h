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
    int target;
    bool fly;

public:
    // Konstruktori
    Rocket();
    Rocket(const std::string& name, float x, float y);

    // Setteri
    void setName(const std::string& name);
    void setPosition(float x, float y);
    void setSelected(bool selected); // Setter za isSelected
    void setTarget(int target);
    void setFly(bool fly);

    // Getteri
    std::string getName() const;
    void getPosition(float& xOut, float& yOut) const;
    bool getSelected() const; // Getter za isSelected
    int getTarget() const;
    bool getFly() const;

    // Metoda za ispis
    void printInfo() const;
};

#endif