#include <stdio.h>
#include <iostream>
#include "shader.h"
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;



int main( void )
{
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow( 800, 600, "PENDULUM", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


    Shader myshader("trianglevertexshader.vs" , "trianglefragmentshader.fs");
    
    unsigned int shaderProgram = myshader.programID();
    float vertices[724];
    vertices[0] = 0.0f; //x coordinate of the circle's center
    vertices[1] = 0.0f; //y coordinate of the circle's center
    
    float radius = 0.5f;
    int index = 0;
    float x, y;

    for(int angle = 0; angle < 361; angle++){
        x = radius * cos(angle * 3.14 / 180);
        y = radius * sin(angle * 3.14 / 180);
        index = index + 2;
        vertices[index] = x;
        vertices[index + 1] = y;
        std::cout<<vertices[index]<<std::endl;
         

    }
    vertices[722] = 0.5f;
    vertices[723] = 0.0f;

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //unsigned int EBO;
    //glGenBuffers(1, &EBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    //position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    
    do{

       // glClearColor(1.0f, 0.3f, 0.2f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT );
        glUseProgram(shaderProgram);

        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 724);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glfwTerminate();

    return 0;
}

