//AUTOR LUKA POPOVIC SV4/2021
//Tema Protiv vazdusna odbrana
#define _CRT_SECURE_NO_WARNINGS
#define CRES 30 
#include <iostream>
#include <fstream>
#include <sstream>
#include<vector>
#include <chrono>
#include <thread>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Helicopter.h"
#include "Rocket.h"
#include <glm/glm.hpp>      
#include <set>
#include <glm/gtc/matrix_transform.hpp> 
#include <map>
#include <cmath> 


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
bool isMouseInsideCircle(float mouseX, float mouseY, float circleX, float circleY, float radius);
void createHelicopters(float r); 
void createRocket();
void initializeRandomSeed();
float generateRandomNumber(float min, float max);
void moveHelicoptersToTarget(float speed, GLFWwindow* window);
void drawRocket(std::map<char, Character> Characters);
void moveRocket(float targetX, float targetY, float speed, GLFWwindow* window, int number, std::map<char, Character> Characters, unsigned int shader);
void drawText(std::map<char, Character> Characters, unsigned int shader,float distance);

unsigned int wWidth = 1000, wHeight = 1000;
float dim1 = 0, dim2 = 0,blueX = 0, blueY = 0, yellowX = 0 ,yellowY = 0;
bool isShowedMap = true;
float blueCircle[(CRES + 2) * 2];
float yellowCircle[(CRES + 2) * 2]; 
float helicopterCircle[5][(CRES + 2) * 2];
bool destroyedHel[5];
//vao i vbo [0] - plavi [1] zuti krug
unsigned int circleVAO[2], circleVBO[2];
bool isBlueCircleSet = false,isYellowCircleSet = false,yellowWait = false; 
Helicopter helicopters[5];
unsigned int helicoptersVAO[5], helicoptersVBO[5];
unsigned int basicShader;
unsigned int rocketShader;
std::set<int> helicopterTarget;
bool clicked = false;
bool selectMode = true;
double lastSpacePressTime = 0.0;
const int TARGET_FPS = 60;
const double FRAME_TIME = 1.0 / TARGET_FPS;

//ali je isti djavo ko helikopter zato je ista klasa kao i rocket
Rocket rocket[10];
unsigned int rocketsVAO[10], rocketsVBO[10];
int indexRocket = 0;
float rocketCircle[10][(CRES + 2) * 2];
bool createdHelicopters = false;
int numberDestroyed = 0,numberTouched = 0, numRocket = 10;
bool spacePressed[10];
unsigned int textVAO, textVBO;
//stavio sam 3 put radi lakse demonstracije
float helicopterSpeed = 0.005f;
float rocketSpeed = 3 * helicopterSpeed;

//float helicopterSpeed = 0.001f;
//float rocketSpeed = 100 * helicopterSpeed;


int main(void)
{

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;

    const char wTitle[] = "Protiv vazdusna odbrana SV4/2021";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);

    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
 
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }
    std::map<char, Character> Characters;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "Fonts/LiberationSans-Regular.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    unsigned int textures[128];
    glGenTextures(128, textures);
    /*for (int i = 0; i < 128; i++) {
        std::cout << "TEXTURE : " << textures[i] << std::endl;
    }*/

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        unsigned int texture = textures[c];
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        std::cout << "Texture ID for character " << c << ": " << texture << std::endl;
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        std::cout << "Glyph Width: " << face->glyph->bitmap.width
            << ", Height: " << face->glyph->bitmap.rows << std::endl;
        Characters.insert(std::pair<char, Character>(c, character));
    }


    unsigned int textureShader = createShader("texture.vert", "texture.frag");
    basicShader = createShader("basic.vert", "basic.frag");
    unsigned int helicopterShader = createShader("hel.vert", "hel.frag");
    int colorLoc = glGetUniformLocation(textureShader, "color");

    unsigned int textShader = createShader("text.vert", "text.frag");
    rocketShader = createShader("rocket.vert", "rocket.frag");

    float vertices[] =
    {   //X    Y      S    T 
        1.0, 1.0,   1.0, 1.0,//prvo tjeme
        1.0, -0.6,  1.0, 0.0, //drugo tjeme
        -1.0, -0.6, 0.0, 0.0, //trece tjeme
        -1.0,1.0,   0.0,  1.0
    };

    unsigned int indices[] = {
        0,1,2,
        2,3,0
    };

    unsigned int stride = (2 + 2) * sizeof(float);

    //VAO ZA TEKSTURU
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // da bude cetvorougao moramo spojiti 2 trougla
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //ZA OSTALE TEKSTURE, promeniti naziv da ne bude 1
    float vertices1[] = {
        // Pozicije      // Koordinate teksture
        0.5f,  0.5f,      1.0f, 1.0f, // Gornji desni
        0.5f, -0.5f,      1.0f, 0.0f, // Donji desni
       -0.5f, -0.5f,      0.0f, 0.0f, // Donji levi
       -0.5f,  0.5f,      0.0f, 1.0f  // Gornji levi
    };

    unsigned int indices1[] = {
        0,1,2,
        2,3,0
    };

    //VAO1 je odbrana je uspesna, VAO2 neuspeh, isto i za vbo
    unsigned int VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    unsigned int VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    // da bude cetvorougao moramo spojiti 2 trougla
    unsigned int EBO1;
    glGenBuffers(1, &EBO1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ////VAO2
    unsigned int VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);
    unsigned int VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    unsigned int EBO2;
    glGenBuffers(1, &EBO2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Tekstura
    unsigned checkerTexture = loadImageToTexture("res/map.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, checkerTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(textureShader);
    float greenOverlay[4] = { 0.0f, 1.0f, 0.0f, 0.5f };
    unsigned int overlayColorLoc = glGetUniformLocation(textureShader, "overlayColor");
    glUniform4fv(overlayColorLoc, 1, greenOverlay);
    glUseProgram(0);


    //ODBRANA JE USPESNA
    unsigned checkerTexture1 = loadImageToTexture("res/odbrana.png"); //Ucitavamo teksturu
    if (checkerTexture1 == 0) {
        std::cout << "Failed to load texture!" << std::endl;
        return -1;
    }
    glBindTexture(GL_TEXTURE_2D, checkerTexture1); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(textureShader);
    glUseProgram(0);


    unsigned checkerTexture2 = loadImageToTexture("res/neuspeh.png"); //Ucitavamo teksturu
    if (checkerTexture2 == 0) {
        std::cout << "Failed to load texture!" << std::endl;
        return -1;
    }
    glBindTexture(GL_TEXTURE_2D, checkerTexture2); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(textureShader);
    glUseProgram(0);
     
    //plavi krug
    glGenVertexArrays(1, &circleVAO[0]);
    glGenBuffers(1, &circleVBO[0]);
    glBindVertexArray(circleVAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blueCircle), blueCircle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    ////za zuti
    glGenVertexArrays(1, &circleVAO[1]);
    glGenBuffers(1, &circleVBO[1]);
    glBindVertexArray(circleVAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(yellowCircle), yellowCircle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
   
    createHelicopters(0.1f);

    for (int i = 0; i < 5; i++) {
        glGenVertexArrays(1, &helicoptersVAO[i]);
        glGenBuffers(1, &helicoptersVBO[i]);
        glBindVertexArray(helicoptersVAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, helicoptersVBO[i]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(helicopterCircle[i]), helicopterCircle[i], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);            
    glUseProgram(textShader);
    int position = glGetUniformLocation(textShader, "position");
    glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //std::chrono::duration<double, std::milli> sleep_duration(1000.0 / 60.0);
    while (!glfwWindowShouldClose(window))
    {

        auto fpsStartTime = std::chrono::high_resolution_clock::now();
 
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            isShowedMap = false;
        }

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            isShowedMap = true;
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(textureShader);
        glBindVertexArray(VAO);

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, checkerTexture);


        glClearColor(0.5, 0.5, 0.5, 1.0);

        if (isShowedMap)
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);


        glUseProgram(basicShader);
        glBindVertexArray(circleVAO[0]);
        int isBlueLoc = glGetUniformLocation(basicShader, "isBlue");
        if (isBlueLoc != -1)
            glUniform1i(isBlueLoc, 1);

        GLint translationLoc = glGetUniformLocation(basicShader, "uTranslation");
        glUniform2f(translationLoc, 0.0f, 0.0f); // Centar na (0, 0)

        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(blueCircle) / (2 * sizeof(float)));

        // Drugi objekat (žuti krug)

        

      
        glBindVertexArray(circleVAO[1]);
        if (isBlueLoc != -1)
            glUniform1i(isBlueLoc, 0);

       
        

        glUniform2f(translationLoc, 0.0f, 0.0f); // Primer translacije za žuti krug
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(yellowCircle) / (2 * sizeof(float)));

        glUniform2f(translationLoc, 0.0f, 0.0f);
        float centerX = 0; 
        float centerY = 0;
        
        if (isYellowCircleSet) {

            glUseProgram(helicopterShader);
            for (int i = 0; i < 5; i++) {
                float x, y;
                helicopters[i].getPosition(x, y);
                float dirX =yellowX - x ;
                float dirY = yellowY- y ;

                float distance = sqrt(dirX * dirX + dirY * dirY);

                if (distance > 0.0f) { 
                    dirX /= distance;
                    dirY /= distance;
                }

                float speed = 0.01f;
                x += dirX * speed;
                y += dirY * speed;

                float pulseSpeed = 10.0f + 8.0f * (1.0f - distance);
                float pulseFactor = 0.5f + 0.5f * sin(8*pulseSpeed);

                float redIntensity = 1; 
                float greenIntensity = 1- pulseFactor; 
                float blueIntensity = 1-pulseFactor;

                GLint translationLoc = glGetUniformLocation(helicopterShader, "uTranslation");
                glUniform2f(translationLoc, x-yellowX, y-yellowY);
                GLint colorLoc = glGetUniformLocation(helicopterShader, "color");
                glUniform3f(colorLoc, redIntensity, greenIntensity, blueIntensity);

                if (distance > 0.05f && !destroyedHel[i]) {
                    glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(helicopterCircle[i]) / (2 * sizeof(float)));
                }
                
            }
            moveHelicoptersToTarget(helicopterSpeed, window);
        }

        if (isBlueCircleSet && isYellowCircleSet) {
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                double currentTime = glfwGetTime(); 
                if (currentTime - lastSpacePressTime >= 0.5) { 
                    lastSpacePressTime = currentTime; 
                    spacePressed[indexRocket] = true;
                    indexRocket++;
                    //ovde bi trebalo ali trenutno je kod misa
                    //numRocket--;
                }
            }
        }

        for (int i = 0; i < 5; i++) {
            if (helicopters[i].getSelected()) {
                if (spacePressed[indexRocket-1]) {
                    if (!destroyedHel[i]) {
                        float x, y;
                        helicopters[i].getPosition(x, y);
                        rocket[indexRocket].setSelected(true);

                        moveRocket(x, y, rocketSpeed, window, i, Characters,textShader);
                        drawRocket(Characters);

                    }
                }
            }
        }


        glUseProgram(textShader);
        glUniform3f(glGetUniformLocation(textShader, "textColor"), 0.0f, 0.0f, 0.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);

        float scale = 0.25f;
        float x = 50.0f;
        float y_helicopter = 100.0f;
        float y_current_helicopters = 80.0f;
        float y_rocket = 60.0f;
        float y_target = 40.0f;     
        float y_name = 20.0f;
        std::string text_helicopter = "BROJ OBORENIH HELIKOPTERA :" + std::to_string(numberDestroyed);
        std::string text_current_helicopters = "BROJ HELIKOPTERA U VAZDUHU :" + std::to_string(5 - numberDestroyed - numberTouched);
        std::string text_rocket = "BROJ PREOSTALIH RAKETA :" + std::to_string(numRocket);
        std::string text_target = "STATUS METE :";

        if (numberTouched == 0) 
            text_target += "NEOSTECENA";        
        else if (numberTouched == 1) 
            text_target += "OSTECENA";        
        else 
            text_target += "UNISTENA";
        
        std::string text_name = "LUKA POPOVIC  SV4/2021 - PVO 2D GRAFIKA";

        for (char c : text_helicopter){
            Character ch = Characters[c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y_helicopter - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // VBO za svaki karakter
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Vertikalno pomeranje za svaki karakter
        }

        x = 50.0f;

        for (char c : text_current_helicopters) {
            Character ch = Characters[c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y_current_helicopters - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // VBO za svaki karakter
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Vertikalno pomeranje za svaki karakter
        }

        x = 50.0f;
        
        for (char c : text_rocket) {
            Character ch = Characters[c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y_rocket - (ch.Size.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // VBO za svaki karakter
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Vertikalno pomeranje za svaki karakter
        }

        x = 50.0f;

        for (char c : text_target) {
            Character ch = Characters[c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y_target - (ch.Size.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            // VBO za svaki karakter
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Vertikalno pomeranje za svaki karakter
        }

        x = 50.0f;

        for (char c : text_name) {
            Character ch = Characters[c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y_name - (ch.Size.y - ch.Bearing.y) * scale;
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;

            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Vertikalno pomeranje za svaki karakter
        }

        if (numberDestroyed == 5) {
            glUseProgram(textureShader);  
            glBindVertexArray(VAO1);               
            glActiveTexture(GL_TEXTURE0);             
            glBindTexture(GL_TEXTURE_2D, checkerTexture1); 
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
   
            std::exit(EXIT_FAILURE);
        }
        else if (numberTouched >= 2) {
            glUseProgram(textureShader);    
            glBindVertexArray(VAO2);       
            glActiveTexture(GL_TEXTURE0);  
            glBindTexture(GL_TEXTURE_2D, checkerTexture2); 
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
  

        auto fpsEndTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = fpsEndTime - fpsStartTime;

        double sleepTime = FRAME_TIME - elapsed.count();
        if (sleepTime > 0) {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
        }
    }

    glDeleteTextures(1, &checkerTexture);
    glDeleteTextures(1, &checkerTexture1);
    glDeleteTextures(1, &checkerTexture2);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &circleVBO[0]);
    glDeleteVertexArrays(1,&circleVAO[0]);
    glDeleteBuffers(1, &circleVBO[1]);
    glDeleteVertexArrays(1, &circleVAO[1]);
    glDeleteProgram(rocketShader);
    glDeleteProgram(helicopterShader);
    glDeleteProgram(textureShader);
    glDeleteProgram(basicShader);
    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}


static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}


static void updateCircleData(float* circle, float r, float dim1, float dim2, int number,bool isHelicopter) {
    circle[0] = dim1;
    circle[1] = dim2;
    for (int i = 0; i <= CRES; i++) {
        float angle = (3.141592 / 180) * (i * 360 / CRES);
        circle[2 + 2 * i] = dim1 + r * cos(angle);
        circle[2 + 2 * i + 1] = dim2 + r * sin(angle);
    }

    if (number == -2) {
        glBindBuffer(GL_ARRAY_BUFFER, circleVBO[0]);  
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(blueCircle), blueCircle);
        glBindBuffer(GL_ARRAY_BUFFER, 0);  
        blueX = dim1;
        blueY = dim2;

    }
    else if (number == -1){
        glBindBuffer(GL_ARRAY_BUFFER, circleVBO[1]); 
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(yellowCircle), yellowCircle);
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        yellowX = dim1;
        yellowY = dim2;
        dim1 = 0;
        dim2 = 0;

  
    }
    else {
        if (isHelicopter) {
            glBindBuffer(GL_ARRAY_BUFFER, helicoptersVBO[number]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(helicopterCircle[number]), helicopterCircle[number]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, rocketsVBO[number]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rocketCircle[number]), rocketCircle[number]);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }   
    }
}

void initializeRandomSeed() {
    srand(static_cast<unsigned>(time(nullptr))); // Inicijalizacija semena
}

float generateRandomNumber(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (max - min));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        dim1 = (float)xpos / width * 2.0f - 1.0f;
        dim2 = 1.0f - (float)ypos / height * 2.0f; 

        if (selectMode) {
            if (!isBlueCircleSet) {
                blueX = dim1;
                blueY = dim2;
                updateCircleData(blueCircle, 0.1f, dim1, dim2, -2,false);
                isBlueCircleSet = true;

                std::cout << blueX << " " << blueY << std::endl;
                createRocket();

                for (int i = 0; i < 10; i++) {
                    glGenVertexArrays(1, &rocketsVAO[i]);
                    glGenBuffers(1, &rocketsVBO[i]);
                    glBindVertexArray(rocketsVAO[i]);
                    glBindBuffer(GL_ARRAY_BUFFER, rocketsVBO[i]);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(rocketCircle[i]), rocketCircle[i], GL_STATIC_DRAW);
                    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(0);
                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
                

            }
            else if (!isYellowCircleSet) {
                updateCircleData(yellowCircle, 0.05f, dim1, dim2, -1,false);
                isYellowCircleSet = true;
                clicked = true;
                selectMode = false;  


            }
        }
        else {
            for (int i = 0; i < 5; i++) {
                float x, y;
                helicopters[i].getPosition(x, y);
                float radius = 0.1f;

                if (isMouseInsideCircle(dim1, dim2, x, y, radius)) {
                    helicopters[i].setSelected(true);
                    numRocket--;
                }
           
            }
        }
    }
}

bool isMouseInsideCircle(float mouseX, float mouseY, float circleX, float circleY, float radius) {
    float dx = mouseX - circleX;
    float dy = mouseY - circleY;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void createHelicopters(float r) {
    initializeRandomSeed();
    for (int i = 0; i < 5; i++) {
        int side = rand() % 4;
        helicopters[i].setName("HELICOPTER NUMBER: " + std::to_string(i+1));
        float x = 1;
        float y = 1;
        
        if (side == 0) {
            x = generateRandomNumber(-1.0f, 1.0f);
            y = 1;
        }
        if (side == 1) {
            x = 1;
            y = generateRandomNumber(-0.6f+r/2, 1.0f);
        }
        if (side == 2) {
            x = generateRandomNumber(-1.0f, 1.0f);
            y = -0.6f+r/2;
        }

        if (side == 3) {
            x = -1;
            y = generateRandomNumber(-0.6f+r/2, 1.0f);
        }
        helicopters[i].setPosition(x, y);
        updateCircleData(helicopterCircle[i],r, x, y, i,true);
    }
}

void moveHelicoptersToTarget(float speed, GLFWwindow* window) {
    for (int i = 0; i < 5; i++) {
        float x, y;
        helicopters[i].getPosition(x, y);

        float dirX = yellowX - x;
        float dirY = yellowY - y;

        float distance = sqrt(dirX * dirX + dirY * dirY);

        if (distance > 0.01f) {
            dirX /= distance;
            dirY /= distance;

            x += dirX * speed;
            y += dirY * speed;
            helicopters[i].setPosition(x, y);
        }
        else {
            numberTouched++;
            helicopters[i].setPosition(99999.0f, 99999.0f);
        }
    }
}
void drawRocket(std::map<char, Character> Characters) {

        //for (int i = 0; i <= indexRocket; i++) {
        if (rocket[indexRocket].getSelected()) {
            //glBindVertexArray(textVAO);
            glBindVertexArray(rocketsVAO[indexRocket]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(rocketCircle[indexRocket]) / (2 * sizeof(float)));
        }

        //}
        glBindVertexArray(0);
    
}

void createRocket() {
    for (int i = 0; i < 10; i++) {
        rocket[i].setPosition(blueX, blueY);
        updateCircleData(rocketCircle[i], 0.03f, blueX, blueY, 0, false);

        glUseProgram(rocketShader);

        GLint translationLoc = glGetUniformLocation(rocketShader, "uTranslation");
        if (translationLoc != -1) {
            glUniform2f(translationLoc, 0.0f, 0.0f);
        }
    }
}

void moveRocket(float targetX, float targetY, float speed, GLFWwindow* windrdow,int number, std::map<char, Character> Characters,unsigned int shader) {
        float x, y;
        rocket[indexRocket].getPosition(x, y);

        float dirX = targetX - x;
        float dirY = targetY - y;

        float distance = sqrt(dirX * dirX + dirY * dirY);

        if (distance > 0.001f) {
            dirX /= distance;
            dirY /= distance;

            x += dirX * speed;
            y += dirY * speed;
            rocket[indexRocket].setPosition(x, y);
        }
        
        if (distance < 0.05f) {

            //75% sanse da se unisti
            int probability = rand() % 4;
            std::cout << "PROBABILITY" << std::to_string(probability) << std::endl;
            if (probability == 0 || probability == 1 || probability == 2) {
                destroyedHel[number] = true;
                numberDestroyed++;
                rocket[indexRocket].setSelected(false);
                rocket[indexRocket].setPosition(blueX, blueY);
                helicopters[number].setPosition(10000.0f, 10000.0f);
                helicopters[number].setSelected(false);
            }
            else {
                destroyedHel[number] = false;
                rocket[indexRocket].setPosition(blueX, blueY);
                helicopters[number].setSelected(false);
                rocket[indexRocket].setSelected(false);
            }
        }

        drawText(Characters, shader, distance);

        glUseProgram(rocketShader);
        GLint translationLoc = glGetUniformLocation(rocketShader, "uTranslation");
        if (translationLoc != -1) {
            glUniform2f(translationLoc, x - blueX, y - blueY);
        }

}

void drawText(std::map<char, Character> Characters,unsigned int shader,float distance){ 
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), 0.0f, 0.0f, 0.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    float scale = 0.2f;
    float x, y;
    rocket[indexRocket].getPosition(x, y);
    float textX= ((x  +  1.0f) / 2.0f) * wWidth ;
    float textY = ((y + 1.0f) / 2.0f) * wHeight ;

    distance = std::round(distance * 100000.0) / 100.0;

    std::stringstream ss;
    ss << distance;  
    std::string distanceText = ss.str() + " m";

    for (char c : distanceText) {
        Character ch = Characters[c];

        float xpos = textX + ch.Bearing.x * scale;
        float ypos = textY - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        textX += (ch.Advance >> 6) * scale;  // Pomeri X za sledeæi karakter
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}