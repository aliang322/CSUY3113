#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "SDL_mixer.h"

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "Level1.h"
#include "menu.h"

#include <vector>

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
bool missionComplete = false;
bool missionFail = false;

Scene* currentScene;
Scene* sceneList[4];

Mix_Music* music;
//Mix_Chunk* jump;

void SwitchToScene(Scene* scene, int lives) {
    currentScene = scene;
    currentScene->Initialize(lives);
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                     640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 1280, 960);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("crypto.mp3");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize the levels and start at the first one
    sceneList[0] = new menu();
    sceneList[1] = new Level1();
    SwitchToScene(sceneList[0], 3);
}

void ProcessInput() {

    currentScene->state.player->movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                // Move the player left
                break;

            case SDLK_RIGHT:
                // Move the player right
                break;

            case SDLK_SPACE:
                break;
            case SDLK_RETURN:
                if (currentScene == sceneList[0]) SwitchToScene(sceneList[1], 3);
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        currentScene->state.player->movement.x = -1.0f;
        currentScene->state.player->movement.y = 0.0f;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        currentScene->state.player->movement.x = 1.0f;
        currentScene->state.player->movement.y = 0.0f;
    }
    else if (keys[SDL_SCANCODE_UP]) {
        currentScene->state.player->movement.y = 1.0f;
        currentScene->state.player->movement.x = 0.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        currentScene->state.player->movement.y = -1.0f;
        currentScene->state.player->movement.x = 0.0f;
    }
    if (glm::length(currentScene->state.player->movement) > 1.0f)
        currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP) {
        //Update. Notice it's FIXED_TIMESTEP. Not deltaTime.
        currentScene->Update(FIXED_TIMESTEP);  
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;

    viewMatrix = glm::mat4(1.0f);
    if (currentScene->state.player->position.x > -100 && 
        currentScene->state.player->position.x < 100) {
        viewMatrix = glm::translate
                    (viewMatrix, glm::vec3(-currentScene->state.player->position.x - 2.5f, 
                                           -currentScene->state.player->position.y - 1.875f, 0));
    }
}
  
void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    program.SetViewMatrix(viewMatrix);  
    //currentScene->state.enemies[0].Render(&program);
    if(!currentScene->state.won && currentScene->state.player->lives > 0) 
        currentScene->Render(&program);
    if(currentScene->state.player->position.x > 5){
        missionComplete = true;
        viewMatrix = glm::mat4(1.0f);
        program.SetViewMatrix(viewMatrix);
        Util::DrawText(&program, Util::LoadTexture("font.png"), "You Win!",
            0.5f, -0.2f, glm::vec3(2.0f, 3.0f, 0));
    }
    else if (currentScene->state.player->lives <= 0) {
        viewMatrix = glm::mat4(1.0f);
        program.SetViewMatrix(viewMatrix);
        Util::DrawText(&program, Util::LoadTexture("font.png"), "You Lose!",
            0.5f, -0.2f, glm::vec3(-1.0f, 0.5f, 0));
    }
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
        if (currentScene->state.nextScene > 0) SwitchToScene(sceneList
            [currentScene->state.nextScene], currentScene->state.player->lives);
        Render();
    }

    Shutdown();
    return 0;
}