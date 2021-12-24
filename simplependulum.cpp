#include <stdio.h>
#include <iostream>
#include "shader.h"
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

//denemeler

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
    float vertices[3180];
    int corner_one, corner_two, corner_three;
    corner_one = 0;
    corner_two = 2;
    corner_three = 4;

    float radius = 0.5f;

    for(int angle = 1; angle <= 360; angle ++){
        vertices[corner_one] = 0.0f;
        vertices[corner_one + 1] = 0.0f;

        vertices[corner_two] = cos((angle -1) * 3.1416 / 180);
        vertices[corner_two + 1] = sin((angle -1) * 3.1416 / 180);

        vertices[corner_three] = cos((angle) * 3.1416 / 180);
        vertices[corner_three + 1] = sin((angle) * 3.1416 / 180);

        corner_one = corner_one + 6;
        corner_two = corner_two + 6;
        corner_one = corner_one + 6;

    }

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
        glDrawArrays(GL_TRIANGLES, 0, 21);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glfwTerminate();

    return 0;
}

