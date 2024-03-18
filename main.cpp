// Autor: ML
// // Opis: RADIO

#define _CRT_SECURE_NO_WARNINGS
#define CRES 40

#include <iostream>
#include <fstream>
#include <sstream>


#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const float M_PI = 3.14159265358979323846;



unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);



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
    unsigned int wWidth = 750;
    unsigned int wHeight = 750;
    const char wTitle[] = "Radio Interface, Miljan Lazic RA212/2020";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }


    //SHADERS
    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int radioShader = createShader("basic.vert", "radio.frag");
    unsigned int membraneShader = createShader("basic.vert", "membrane.frag");
    unsigned int netShader = createShader("basic.vert", "net.frag");
    unsigned int lightShader = createShader("basic.vert", "light.frag");
    unsigned int pButtonShader = createShader("pbutton.vert", "pbutton.frag");
    unsigned int textureShader = createShader("texture.vert", "texture.frag");
    unsigned int antennaShader = createShader("basic.vert", "antenna.frag");
    unsigned int sliderShader = createShader("slider.vert", "slider.frag");
    unsigned int progressBarShader = createShader("progressbar.vert", "progressbar.frag");

    // CIRCLE
    float circle[CRES * 2 + 4]; // +4 for x and y coordinates of the center of the circle
    float r = 0.3; // Radius
    circle[0] = 0.0; // Center X0
    circle[1] = 0.0; // Center Y0

    for (int i = 0; i <= CRES; i++) {
        circle[2 + 2 * i] = r * cos((M_PI / 180) * (i * 360 / CRES)); // Xi
        circle[2 + 2 * i + 1] = r * sin((M_PI / 180) * (i * 360 / CRES)); // Yi
    }

    //NET
    const int numNetDots = 200;
    float netVertices[numNetDots * 2];
    float netRadius = 0.32;

    for (int i = 0; i < numNetDots; ++i) {
        float angle = (2 * M_PI * i) / numNetDots;

        float x = netRadius * cos(angle);
        float y = netRadius * sin(angle);

        netVertices[i * 2] = x;
        netVertices[i * 2 + 1] = y;
    }


    float vertices[] = {
        // Position         // Texture Coordinates
        -1.0,  1.0,         0.0, 1.0,
         1.0,  1.0,         1.0, 1.0,
        -1.0, -1.0,         0.0, 0.0,
         1.0, -1.0,         1.0, 0.0,
    };

    float lightVertices[] =
    {
         -1.0, 1.0,      0.2, 0.7, 0.3,
         1.0, 1.0,       0.2, 0.7, 0.3,
         -1.0, -1.0,     0.2, 0.7, 0.3,
         1.0, -1.0,      0.2, 0.7, 0.3,
    };

    float antennaLength = 2.5f;

    float antennaVertices[] = {
        0.25f, 0.1f,
        0.25f, 0.1f + antennaLength,
        0.27f, 0.1f + antennaLength,
        0.27f, 0.1f
    };

    constexpr int numCirclePoints = 100;
    float circleVertices[2 * (numCirclePoints + 2)];  
    float radius = 0.03f;  

    circleVertices[0] = 0.25f;
    circleVertices[1] = 0.1f;

    for (int i = 0; i <= numCirclePoints; ++i) {
        float angle = static_cast<float>(i) / numCirclePoints * 2.0f * M_PI;
        float x = 0.25f + radius * cos(angle);
        float y = 0.1f + antennaLength + radius * sin(angle);

        circleVertices[2 * (i + 1)] = x;
        circleVertices[2 * (i + 1) + 1] = y;
    }

    float x0;
    float y0;
    float sliderElipse[CRES * 2 + 4]; 
    float r1 = 0.1; 
    sliderElipse[0] = x0 = -0.78; 
    sliderElipse[1] = y0 = -0.7; 
    for (int i = 0; i <= CRES; i++)
    {
        sliderElipse[2 + 2 * i] = r1 * cos((M_PI / 180) * (i * 360 / CRES)) - 0.78; 
        sliderElipse[2 + 2 * i + 1] = r1 * sin((M_PI / 180) * (i * 360 / CRES)) - 0.7;
    }


    unsigned int VAO[15];
    glGenVertexArrays(15, VAO);
    unsigned int VBO[15];
    glGenBuffers(15, VBO);     


    //RADIO BODY
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //MEMBRANE
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); 
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //NET
    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(netVertices), netVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //LIGHT
    glBindVertexArray(VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //POWER BUTTON
    glBindVertexArray(VAO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // TEKSTURA
    glBindVertexArray(VAO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int indexTexture = loadImageToTexture("MLindeks.png");
    glUseProgram(textureShader);
    glBindTexture(GL_TEXTURE_2D, indexTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    // ANTENNA
    glBindVertexArray(VAO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(antennaVertices), antennaVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // ANTENNA circle
    glBindVertexArray(VAO[8]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circle), circle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // PROGRESS BAR
    glBindVertexArray(VAO[9]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    //SLIDER
    glBindVertexArray(VAO[10]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sliderElipse), sliderElipse, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //AM MODE
    glBindVertexArray(VAO[11]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[11]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int AMTexture = loadImageToTexture("AM.png");
    glUseProgram(textureShader);
    glBindTexture(GL_TEXTURE_2D, AMTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    //FM MODE
    glBindVertexArray(VAO[12]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[12]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int FMTexture = loadImageToTexture("FM.png");
    glUseProgram(textureShader);
    glBindTexture(GL_TEXTURE_2D, FMTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);

    //poz
    glBindVertexArray(VAO[13]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[13]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // AM/FM SCALE
    glBindVertexArray(VAO[14]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[14]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    unsigned int ScaleTexture = loadImageToTexture("Scale.png");
    glUseProgram(textureShader);
    glBindTexture(GL_TEXTURE_2D, ScaleTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glBindTexture(GL_TEXTURE_2D, 0);



    //Variables
    unsigned int uActive = glGetUniformLocation(lightShader, "uActive");
    float lightColor[2];
    lightColor[0] = lightColor[1] = 0.0;
    bool isPowerOn = false;
    float powerButtonScale = 1.0;

    bool isSpacePressed = false;
    float vibrationFrequency = 0.0f;  
    bool isNetVibrating = false;
    bool isExpanding = false;
    float vibrationStartTime = 0.0f;

    bool showAntenna = false;

    float translationX = -2.0f;
    const float minX = -2.0f;  
    const float maxX = 0.0f;   

    bool AMmode = true;
    
    float x;
    float y;
    x = 0.0;
    y = 0.0;
    const float minnX = 0.0f;
    const float maxnX = 1.66f;

    unsigned int uPos = glGetUniformLocation(sliderShader, "uPos");

    glClearColor(1.0, 1.0, 1.0, 1.0);
    while (!glfwWindowShouldClose(window)) 
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS ) {
                lightColor[0] = 0.9;        
                powerButtonScale = 0.7;
                isPowerOn = true;
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            lightColor[0] = 0.0;
            powerButtonScale = 1.0;
            isPowerOn = false;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            showAntenna = true;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            showAntenna = false;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && isPowerOn) {
            translationX -= 0.0006f;
            x -= 0.0005f;
            x0 -= 0.0005f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && isPowerOn) {
            translationX += 0.0006f;
            x += 0.0005f;
            x0 += 0.0005f;
        }
        translationX = std::max(minX, std::min(translationX, maxX));
        x = std::max(minnX, std::min(x, maxnX));
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && isPowerOn) {
            AMmode = false;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && isPowerOn) {
            AMmode = true;
        }



        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isPowerOn) {
            isExpanding = true;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isPowerOn) {
            if (!isSpacePressed) {
                isSpacePressed = true;
                isNetVibrating = true;
                vibrationStartTime = glfwGetTime();
            }
        }
        else {
            isSpacePressed = false;
            if (isNetVibrating) {
                isNetVibrating = false;
            }
        }

        if (isNetVibrating) {
            const float minDistance = 0.1f;
            const float maxDistance = 0.1f;
            float time = glfwGetTime() - vibrationStartTime;
            float expansionFactor = isExpanding ? 1.0f + 0.2f * sin(12.0f * time) : 1.0f;

            for (int i = 0; i < numNetDots; ++i) {
                float angle = (2 * M_PI * i) / numNetDots;

                float distance = expansionFactor * (netRadius + minDistance);
                distance = std::min(distance, (netRadius + maxDistance)); 

                float x = distance * cos(angle);
                float y = distance * sin(angle);

                netVertices[i * 2] = x;
                netVertices[i * 2 + 1] = y;
            }


            glBindVertexArray(VAO[2]);
            glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(netVertices), netVertices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            if (isExpanding && expansionFactor > 1.1f) {
                isExpanding = false;
            }

            else if (!isExpanding && expansionFactor <= 1.0f) {
                isNetVibrating = false;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT);


        
        //RADIO BODY
        glViewport(50, 120, wWidth - 100, wHeight -250);
        glUseProgram(radioShader);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //LOUDSPEAKERS
        glViewport(-60, 160, 600, 600);
        glUseProgram(membraneShader);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

        glViewport(-60, 160, 600, 600);
        glUseProgram(netShader);
        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_LINES, 0, sizeof(netVertices) / (2 * sizeof(float)));


        glViewport(220, 160, 600, 600);
        glUseProgram(membraneShader);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));

        glViewport(220, 160, 600, 600);
        glUseProgram(netShader);
        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_LINES, 0, sizeof(netVertices) / (2 * sizeof(float)));

        //LIGHT
        glViewport(655, 580, 35, 35);
        glUseProgram(lightShader);
        glBindVertexArray(VAO[3]);
        glUniform1f(uActive, lightColor[0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // POWER BUTTON
        glViewport(30, 570, 70, 70);
        glUseProgram(pButtonShader);
        glUniform1f(glGetUniformLocation(pButtonShader, "uScale"), powerButtonScale);
        glBindVertexArray(VAO[4]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));
        
        //INDEX
        glViewport(-85, 0, 386, 68);
        glUseProgram(textureShader);
        glBindVertexArray(VAO[6]);
        glActiveTexture(GL_TEXTURE0); 

        glBindTexture(GL_TEXTURE_2D, indexTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //ANTENNA
        // Draw the antenna rectangle
        if (showAntenna) {
            glViewport(545, 510, 170, 200);
            glUseProgram(antennaShader);
            glBindVertexArray(VAO[7]);
            glDrawArrays(GL_LINE_LOOP, 0, 4);

            glViewport(622.5, 680, 60, 60);
            glUseProgram(lightShader);
            glBindVertexArray(VAO[1]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));
        }
        
        //PROGRESS BAR
        glViewport(280, 130, 200, 40);
        glUseProgram(unifiedShader);
        glBindVertexArray(VAO[9]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glViewport(280, 130, 200, 40);
        glUseProgram(progressBarShader);
        glUniform1f(glGetUniformLocation(progressBarShader, "translationX"), translationX);
        glBindVertexArray(VAO[9]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // SLIDER
        glViewport(260, 75, 230, 500);
        glUseProgram(sliderShader);
        glBindVertexArray(VAO[10]);
        glUniform2f(uPos, x, y);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sizeof(circle) / (2 * sizeof(float)));
        

        //poz
        glViewport(330, 195, 80, 40);
        glUseProgram(unifiedShader);
        glBindVertexArray(VAO[13]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //AM MODE 
        glViewport(335, 200, 30, 30);
        glUseProgram(textureShader);
        glBindVertexArray(VAO[13]);
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, AMTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        //FM MODE 
        glViewport(373, 200, 30, 30);
        glUseProgram(textureShader);
        glBindVertexArray(VAO[12]);
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, FMTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        if (AMmode)
        {
            glViewport(373, 200, 30, 30);
            glUseProgram(radioShader);
            glBindVertexArray(VAO[12]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        else
        {
            glViewport(335, 200, 30, 30);
            glUseProgram(radioShader);
            glBindVertexArray(VAO[11]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        // AM/FM SKALA
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(145, 250, 450, 82);
        glUseProgram(textureShader);
        glBindVertexArray(VAO[14]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ScaleTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glDisable(GL_BLEND);


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteTextures(1, &AMTexture);
    glDeleteTextures(1, &FMTexture);
    glDeleteTextures(1, &indexTexture);
    glDeleteTextures(1, &ScaleTexture);

    glDeleteProgram(progressBarShader);
    glDeleteProgram(sliderShader);
    glDeleteProgram(antennaShader);
    glDeleteProgram(textureShader);
    glDeleteProgram(pButtonShader);
    glDeleteProgram(lightShader);
    glDeleteProgram(netShader);
    glDeleteProgram(membraneShader);
    glDeleteProgram(radioShader);
    glDeleteProgram(unifiedShader);

    glDeleteBuffers(15, VBO);
    glDeleteVertexArrays(15, VAO);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glfwTerminate();
    return 0;
}



unsigned int compileShader(GLenum type, const char* source)
{
    //Uzima kod u fajlu na putanji "source", kompajlira ga i vraca sejder tipa "type"
    //Citanje izvornog koda iz fajla
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
     const char* sourceCode = temp.c_str(); //Izvorni kod sejdera koji citamo iz fajla na putanji "source"

    int shader = glCreateShader(type); //Napravimo prazan sejder odredjenog tipa (vertex ili fragment)
    
    int success; //Da li je kompajliranje bilo uspjesno (1 - da)
    char infoLog[512]; //Poruka o gresci (Objasnjava sta je puklo unutar sejdera)
    glShaderSource(shader, 1, &sourceCode, NULL); //Postavi izvorni kod sejdera
    glCompileShader(shader); //Kompajliraj sejder

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success); //Provjeri da li je sejder uspjesno kompajliran
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog); //Pribavi poruku o gresci
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
    //Pravi objedinjeni sejder program koji se sastoji od Vertex sejdera ciji je kod na putanji vsSource

    unsigned int program; //Objedinjeni sejder
    unsigned int vertexShader; //Verteks sejder (za prostorne podatke)
    unsigned int fragmentShader; //Fragment sejder (za boje, teksture itd)

    program = glCreateProgram(); //Napravi prazan objedinjeni sejder program

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource); //Napravi i kompajliraj vertex sejder
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource); //Napravi i kompajliraj fragment sejder

    //Zakaci verteks i fragment sejdere za objedinjeni program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program); //Povezi ih u jedan objedinjeni sejder program
    glValidateProgram(program); //Izvrsi provjeru novopecenog programa

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success); //Slicno kao za sejdere
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    //Posto su kodovi sejdera u objedinjenom sejderu, oni pojedinacni programi nam ne trebaju, pa ih brisemo zarad ustede na memoriji
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
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
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

