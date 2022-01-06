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

#define PI 3.141592  //Holy Pi!
#define h 0.01       //step length for Runge-Kutta
#define k 0.67       //driving force frequency (radian)
#define THETA_0 1.57 //initial angle(radian)
#define omega_0 0    //initial angular velocity

#define A 1.4        //driving force amplitude
#define b 0.4        //damping constant
#define m 1          //mass of pendulum
#define R 1          //length of rod
#define grav 1       //gravitational constant


float f(float time, float tht,float omega){
        return omega;
}
float g(float time, float tht, float omega){
        return -(grav/R)*sin(tht)-((b/(m*R*R))*omega)+((A/(m*R*R))*cos(k*time));
}


void drawCircle(float array[]){
    int corner_one, corner_two, corner_three;//corners of triangles. GL_TRIANGLES starts to draw counterclockwise.
    corner_one = -6;
    corner_two = -4;
    corner_three = -2;

    float radius = 0.1f;

    for(int angle = 1; angle <= 360; angle ++){
        corner_one = corner_one + 6;
        corner_two = corner_two + 6;
        corner_three = corner_three + 6;

        array[corner_one] = 0.0f;
        array[corner_one + 1] = 0.0f;

        array[corner_two] = radius * cos((angle -1) * 3.1416 / 180);
        array[corner_two + 1] = radius * sin((angle -1) * 3.1416 / 180);

        array[corner_three] = radius * cos((angle) * 3.1416 / 180);
        array[corner_three + 1] = radius * sin((angle) * 3.1416 / 180);

    }    
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
    Shader forcearrow("forcearrow.vs", "forcearrow.fs");
    
    unsigned int shaderProgram = myshader.programID();
    unsigned int arrow = forcearrow.programID();

    float vertices[2160];
    float vertices2[] = { //vertices2 gives us the rod of the pendulum.
        -0.01f, 0.0f,
         0.01f, 0.0f,
         0.01f, 0.8f,
        -0.01f, 0.8f,
        -0.01, 0.0f
    };
    float arrows[] = {
        -0.5f, 0.0f,
        -0.1f, 0.0f,
        -0.5f, 0.0f,
        -0.3f, 0.1f,
        -0.5f, 0.0f,
        -0.3f, -0.1f
    };

    drawCircle(vertices); //draws the pendulum ball.
    

    unsigned int VBO, VBO2, VAO, VAO2, VBOArrow, VAOArrow;
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

    glGenVertexArrays(1, &VAOArrow);
    glGenBuffers(1, &VBOArrow);
    glBindVertexArray(VAOArrow);
    glBindBuffer(GL_ARRAY_BUFFER, VBOArrow);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrows), arrows, GL_STATIC_DRAW);
    
    //position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);   

    float theta= THETA_0;
    float omg= omega_0;
    float time = 0;

    float k1,k2,k3,k4,l1,l2,l3,l4;
    float driving_force;
    
    //Initialize f and g functions.
    f(time,theta,omg);
    g(time,theta,omg);
    float current_angle;
    do{
        
        driving_force = A * cos(k * time);
        current_angle = theta * 180 / PI;
        std::cout<<current_angle<<std::endl;
        glClear( GL_COLOR_BUFFER_BIT );
        glUseProgram(shaderProgram);
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::rotate(view, glm::radians(current_angle), glm::vec3(0.0f, 0.0f, 1.0f));
        view = glm::translate(view, glm::vec3(0.0f, -0.8f, 0.0f));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
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
        //Below two lines keep the theta in range of -2PI to 2PI.
        if(theta>2*PI) theta= theta-(2*PI);
        if(theta<-2*PI) theta= theta+(2*PI);
        
        time =time+h;
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 1080);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);

        //Create the driving force arrow. It shows magnitude and direction side of the force.
        glUseProgram(arrow);
        glm::mat4 modelArrow= glm::mat4(1.0f);
        glm::mat4 projectionArrow = glm::mat4(1.0f);
        glm::mat4 viewArrow = glm::mat4(1.0f);
        viewArrow = glm::translate(viewArrow, glm::vec3(0.0f, -0.8f, 0.0f));
        viewArrow = glm::scale(viewArrow, glm::vec3(-driving_force * 0.5, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(arrow, "projection"), 1, GL_FALSE, glm::value_ptr(projectionArrow));
        glUniformMatrix4fv(glGetUniformLocation(arrow, "view"), 1, GL_FALSE, glm::value_ptr(viewArrow));
        glUniformMatrix4fv(glGetUniformLocation(arrow, "model"), 1, GL_FALSE, glm::value_ptr(modelArrow));
        glBindVertexArray(VAOArrow);
        glDrawArrays(GL_LINES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glfwTerminate();

    return 0;
}

