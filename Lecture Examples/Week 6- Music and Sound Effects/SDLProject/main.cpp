#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool notStarted = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, modelMatrix, ballMatrix, redMatrix, blueMatrix, keyInstructionsMatrix;

// Red and Blue (left and right) Paddles
glm::vec3 red_position, blue_position = glm::vec3(0, 0, 0);
glm::vec3 red_movement, blue_movement = glm::vec3(0, 0, 0);
float paddle_speed = 6.0f;

glm::vec3 paddle_scale = glm::vec3(0.30f, 1.2f, 1.0f);
float paddle_height = paddle_scale.y;
float paddle_width = paddle_scale.x;

// Ball
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);
float ball_speed = 5.0f;

glm::vec3 ball_scale = glm::vec3(0.30f, 0.30f, 1.0f);
float ball_height =  ball_scale.y;
float ball_width = ball_scale.x;

//Key instructions image
glm::vec3 keyInstructions_scale = glm::vec3(7.5f, 7.5f, 1.0f);
float keyInstructions_scale_increment = 7.5f;

GLuint ballTextureID;
GLuint redTextureID;
GLuint blueTextureID;
GLuint keyInstructionsTextureID;

Mix_Music* music;
Mix_Chunk* bounce;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Andrew Liang's Project 2- Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("dooblydoo.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 6);

    bounce = Mix_LoadWAV("bounce.wav");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ballTextureID = LoadTexture("pong ball.png");
    redTextureID = LoadTexture("red paddle.png");
    blueTextureID = LoadTexture("blue paddle.png");
    keyInstructionsTextureID = LoadTexture("pong keys.png");
    
    keyInstructionsMatrix = glm::scale(modelMatrix, keyInstructions_scale);
}

bool wallAboveCollision(float y, float height) {
    return !((y + (height / 2.0f)) <= 3.75f);
}

bool wallBelowCollision(float y, float height) {
    return !((y - (height / 2.0f)) >= -3.75f);
}


int paddleBallCollision() {
    //blue paddle
    float blue_xdist = fabs(ball_position.x - blue_position.x - 4.8f) - ((ball_width + 0.1f + paddle_width) / 2.0f);
    float blue_ydist = fabs(ball_position.y - blue_position.y) - ((ball_height + 0.1f + paddle_height) / 2.0f);
    //red paddle
    float red_xdist = fabs(ball_position.x - red_position.x + 4.8f) - ((ball_width + 0.1f + paddle_width) / 2.0f);
    float red_ydist = fabs(ball_position.y - red_position.y) - ((ball_height + 0.1f + paddle_height) / 2.0f);
    if ((blue_xdist < 0 && blue_ydist < 0)) return 2;
    else if (red_xdist < 0 && red_ydist < 0) return 1;
    return -1;
}

bool ballOutOfBounds() {
    return (ball_position.x < -4.81f || ball_position.x > 4.81f);
}

void ProcessInput() {

    red_movement= blue_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_SPACE] && notStarted) {
        if (notStarted) {
            ball_movement = glm::vec3(1, 0.5, 0);
            notStarted = !notStarted;
        }
    }
    if (keys[SDL_SCANCODE_UP] && !wallAboveCollision(blue_position.y, paddle_height)) {
        blue_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && !wallBelowCollision(blue_position.y, paddle_height)) {
        blue_movement.y = -1.0f;
    }

    if (keys[SDL_SCANCODE_W] && !wallAboveCollision(red_position.y, paddle_height)) {
        red_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S] && !wallBelowCollision(red_position.y, paddle_height)) {
        red_movement.y = -1.0f;
    }

    if (glm::length(ball_movement) > 1.0f) ball_movement = glm::normalize(ball_movement);
}

float lastTicks = 0.0f;

void Update() {
    if (ballOutOfBounds()) {
        red_movement = blue_movement = ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    if (keyInstructions_scale.x > 0 && !notStarted) {
        keyInstructions_scale = glm::vec3(keyInstructions_scale.x - keyInstructions_scale_increment * deltaTime,
            keyInstructions_scale.y - keyInstructions_scale_increment * deltaTime,
            keyInstructions_scale.z - keyInstructions_scale_increment * deltaTime );
        keyInstructionsMatrix = glm::scale(modelMatrix, keyInstructions_scale);
    }

    red_position += red_movement * paddle_speed * deltaTime;
    redMatrix = glm::translate(modelMatrix, glm::vec3(-4.8f, 0.0f, 0.0f));
    redMatrix = glm::translate(redMatrix, red_position);
    redMatrix = glm::scale(redMatrix, paddle_scale);

    blue_position += blue_movement * paddle_speed * deltaTime;
    blueMatrix = glm::translate(modelMatrix, glm::vec3(4.8f, 0.0f, 0.0f));
    blueMatrix = glm::translate(blueMatrix, blue_position);
    blueMatrix = glm::scale(blueMatrix, paddle_scale);

    if (glm::length(ball_movement) > 1.0f) ball_movement = glm::normalize(ball_movement);

    if (wallAboveCollision(ball_position.y, ball_height)) {
        Mix_PlayChannel(-1, bounce, 0);
        ball_movement.y = -1.0f * fabs(ball_movement.y);
    }
    else if (wallBelowCollision(ball_position.y, ball_height)) {
        Mix_PlayChannel(-1, bounce, 0);
        ball_movement.y = fabs(ball_movement.y);
    }
    else if (paddleBallCollision() > 0) {
        Mix_PlayChannel(-1, bounce, 0);
        if (paddleBallCollision() == 1) {
            if (ball_position.x - ball_width / 2.0f < red_position.x + paddle_width / 2.0f - 4.82f) {
                ball_movement.y *= -1.0f;
            }
            if (ball_position.x < -4.80f) ball_movement.x = -1.0f * fabs(ball_movement.x);
            else ball_movement.x = fabs(ball_movement.x);
        }
        else if (paddleBallCollision() == 2) {
            if (ball_position.x + ball_width / 2.0f > blue_position.x - paddle_width / 2.0f + 4.82f) {
               ball_movement.y *= -1.0f;
            }
            if (ball_position.x > 4.80f) ball_movement.x = fabs(ball_movement.x);
            else ball_movement.x = -1.0f * fabs(ball_movement.x);
        }
    }
    
    if (glm::length(ball_movement) > 1.0f) ball_movement = glm::normalize(ball_movement);
    ball_position += ball_movement * ball_speed * deltaTime;
    ballMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    ballMatrix = glm::translate(ballMatrix, ball_position);
    ballMatrix = glm::scale(ballMatrix, ball_scale);
}

void drawBall() {
    program.SetModelMatrix(ballMatrix);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawRed() {
    program.SetModelMatrix(redMatrix);
    glBindTexture(GL_TEXTURE_2D, redTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void drawBlue() {
    program.SetModelMatrix(blueMatrix);
    glBindTexture(GL_TEXTURE_2D, blueTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void drawKeyInstructions() {
    program.SetModelMatrix(keyInstructionsMatrix);
    glBindTexture(GL_TEXTURE_2D, keyInstructionsTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Render() {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    drawKeyInstructions();
    drawBall();
    drawRed();
    drawBlue();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}