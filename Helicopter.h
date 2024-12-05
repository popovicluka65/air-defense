#pragma once
#ifndef HELICOPTER_H
#define HELICOPTER_H

#include <string> // Za std::string

class Helicopter {
private:
    std::string name;
    float x;
    float y;
    //neka za helikopter predstavlja da li je select a za rocket da li isShow treba
    bool isSelected; 
    bool hasRocket;

public:
    Helicopter();
    Helicopter(const std::string& name, float x, float y);

    // Setteri
    void setName(const std::string& name);
    void setPosition(float x, float y);
    void setSelected(bool selected); // Setter za isSelected
    void setHasRocket(bool hasRocket); // Setter za isSelected

    // Getteri
    std::string getName() const;
    void getPosition(float& xOut, float& yOut) const;
    bool getSelected() const; // Getter za isSelected
    bool getHasRocket() const; // Getter za isSelected

    // Metoda za ispis
    void printInfo() const;
};

#endif
