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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <vector>
#include "Entity.h"

#define WALL_SPIKES_COUNT 30
#define FLOATING_SPIKES_COUNT 9

struct GameState {
    Entity* player;
    Entity* platforms;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Andrew Liang's Project 3- Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.35f, 0.15f, 0.95f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(-3.0f, 3.0, 0.0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -0.1962f, 0); //-0.3924
    state.player->speed = 1.5f;
    state.player->textureID = LoadTexture("Space Ship Down.png");

    state.player->height = 1.0f;
    state.player->width = 0.85f;


    state.platforms = new Entity[WALL_SPIKES_COUNT + FLOATING_SPIKES_COUNT];

    GLuint portalTextureID = LoadTexture("Blue Portal.png");
    GLuint spikeUpTextureID = LoadTexture("Neon Spike Up.png");
    GLuint spikeLeftTextureID = LoadTexture("Neon Spike Left.png");
    GLuint spikeRightTextureID = LoadTexture("Neon Spike Right.png");
    GLuint deathBlockTextureID = LoadTexture("Neon Death Block.png");


    for (int i = 0; i < WALL_SPIKES_COUNT /3; i++) {
        state.platforms[i].textureID = spikeUpTextureID;
        if(i != 5) state.platforms[i].position = glm::vec3(-4.5 + i, -3.25f, 0);
    }

    for (int j = WALL_SPIKES_COUNT /3; j < 2* WALL_SPIKES_COUNT /3; j++) {
        state.platforms[j].textureID = spikeLeftTextureID;
        state.platforms[j].position = glm::vec3(4.5f, -2.25f + j - WALL_SPIKES_COUNT/3, 0);
    }

    for (int k = 2* WALL_SPIKES_COUNT /3; k < WALL_SPIKES_COUNT; k++) {
        state.platforms[k].textureID = spikeRightTextureID;
        state.platforms[k].position = glm::vec3(-4.5f, -2.25f + k - 2* WALL_SPIKES_COUNT/3, 0);
    }

    for (int l = WALL_SPIKES_COUNT; l < WALL_SPIKES_COUNT + FLOATING_SPIKES_COUNT/3; l++) {
        state.platforms[l].textureID = deathBlockTextureID;
        state.platforms[l].position = glm::vec3(-3.25f + l - WALL_SPIKES_COUNT, -0.5f, 0);
    }

    for (int m = WALL_SPIKES_COUNT + FLOATING_SPIKES_COUNT/3; m < WALL_SPIKES_COUNT + 2*FLOATING_SPIKES_COUNT/3; m++) {
        state.platforms[m].textureID = deathBlockTextureID;
        state.platforms[m].position = glm::vec3( 1.0f + m - WALL_SPIKES_COUNT - FLOATING_SPIKES_COUNT/3, 3.0f, 0);
    }

    for (int n = WALL_SPIKES_COUNT + 2*FLOATING_SPIKES_COUNT/3; n < WALL_SPIKES_COUNT + FLOATING_SPIKES_COUNT; n++) {
        state.platforms[n].textureID = deathBlockTextureID;
        state.platforms[n].position = glm::vec3(3.0f + n - WALL_SPIKES_COUNT - 2*FLOATING_SPIKES_COUNT/3, -2.0f, 0);
    }

    state.platforms[5].textureID = portalTextureID;
    state.platforms[5].position = glm::vec3(0.50, -3.25f, 0);
        
    for (int z = 0; z < WALL_SPIKES_COUNT + FLOATING_SPIKES_COUNT; z++) {
        state.platforms[z].Update(0, NULL, 0);
    }
}

void ProcessInput() {

    state.player->movement = glm::vec3(0);

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

    if (state.player->isNextToOrIn(&state.platforms[5])) state.player->enteredPortal = true;
    else if (!state.player->collidedBottom && !state.player->collidedLeft && !state.player->collidedRight) {
        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->acceleration.x = -75.0f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->acceleration.x = 75.0f;
        }
    }
    
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

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
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.platforms, WALL_SPIKES_COUNT + FLOATING_SPIKES_COUNT);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
              float size, float spacing, glm::vec3 position){
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;

    std::vector<float> vertices;
    std::vector<float> texCoords;

    for (int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;  

        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            });

        texCoords.insert(texCoords.end(), {
            u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
            });
    }

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < WALL_SPIKES_COUNT + FLOATING_SPIKES_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    state.player->Render(&program);

    if (state.player->enteredPortal == true) {
        DrawText(&program, LoadTexture("font2.png"), "[ Mission Successful ]", 0.5f, -0.2f, glm::vec3(-3.25f, 0.5f, 0));
        DrawText(&program, LoadTexture("font2.png"), ">> You Reached the Portal", 0.4f, -0.2f, glm::vec3(-2.75f, 0.0f, 0));
    }
    else if (state.player->collidedLeft || state.player->collidedRight|| state.player-> collidedBottom) {
       DrawText(&program, LoadTexture("font2.png"), "[ Mission Failed ]", 0.5f, -0.2f, glm::vec3(-2.50f, 0.5f, 0));
       DrawText(&program, LoadTexture("font2.png"), ">> Your Ship was Destroyed", 0.4f, -0.2f, glm::vec3(-2.50f, 0.0f, 0));
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
        Render();
    }

    Shutdown();
    return 0;
}