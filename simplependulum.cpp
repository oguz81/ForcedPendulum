#include <stdio.h>
#include <iostream>
#include "shader.h"
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
GLFWwindow* window;

#define PI 3.141592
#define h 0.01
//denemeler
#define k 0.3 
#define THETA_0 0.5       //initial angle
#define omega_0 0     //initial angular velocity

#define A 2.4
#define b 0
#define m 1
#define R 1
#define grav 1


float f(float time, float tht,float omega){
        return omega;
}
float g(float time, float tht, float omega){
        return -(grav/R)*sin(PI*tht)-((b/(m*R*R))*omega)+((A/(m*R*R))*cos(PI*k*time));
}


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

    window = glfwCreateWindow( 630, 600, "PENDULUM", NULL, NULL);
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


    Shader myshader("pendulum.vs" , "pendulum.fs");
    
    unsigned int shaderProgram = myshader.programID();
    float vertices[2160];
    float vertices2[] = {
        -0.01f, 0.0f,
         0.01f, 0.0f,
         0.01f, 0.8f,
        -0.01f, 0.8f,
        -0.01, 0.0f
    };

    int corner_one, corner_two, corner_three;//corners of triangles. GL_TRIANGLES starts to draw counterclockwise.
    corner_one = -6;
    corner_two = -4;
    corner_three = -2;

    float radius = 0.1f;

    for(int angle = 1; angle <= 360; angle ++){
        corner_one = corner_one + 6;
        corner_two = corner_two + 6;
        corner_three = corner_three + 6;

        vertices[corner_one] = 0.0f;
        vertices[corner_one + 1] = 0.0f;

        vertices[corner_two] = radius * cos((angle -1) * 3.1416 / 180);
        vertices[corner_two + 1] = radius * sin((angle -1) * 3.1416 / 180);

        vertices[corner_three] = radius * cos((angle) * 3.1416 / 180);
        vertices[corner_three + 1] = radius * sin((angle) * 3.1416 / 180);

    }
    

    unsigned int VBO, VBO2, VAO, VAO2;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
    
    //position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);   

    float theta= THETA_0;
        float omg= omega_0;
        float k1,k2,k3,k4,l1,l2,l3,l4;
        float time = 0;

        f(time,theta,omg);
        g(time,theta,omg);
        float ada;
    do{

        k1= h*f(time,theta,omg);
                l1= h*g(time,theta,omg);
                k2= h*f(time+(0.5*h),theta+(0.5*k1),omg+(0.5*l1));
                l2= h*g(time+(0.5*h),theta+(0.5*k1),omg+(0.5*l1));
                k3= h*f(time+(0.5*h),theta+(0.5*k2),omg+(0.5*l2));
                l3= h*g(time+(0.5*h),theta+(0.5*k2),omg+(0.5*l2));
                k4= h*f(time+h,theta+k3,omg+l3);
                l4= h*g(time+h,theta+k3,omg+l3);

                theta = theta+(k1 + (2*k2) + (2*k3) + k4)/6;
                omg = omg+(l1 + (2*l2) + (2*l3) + l4)/6;
        //if(theta>1) theta= theta-2;
        //if(theta<-1) theta= theta+2;
        ada = theta * 180 / PI;
        time =time+h;
        float timem = glfwGetTime() * 1;
        std::cout<<ada<<std::endl;
        glClear( GL_COLOR_BUFFER_BIT );
        glUseProgram(shaderProgram);
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::rotate(view, glm::radians(ada), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::translate(view, glm::vec3(0.0f, -0.8f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 1080);
        glBindVertexArray(VAO2);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glfwTerminate();

    return 0;
}

