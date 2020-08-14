#include "menu.h"
#define MENU_WIDTH 11
#define MENU_HEIGHT 8

unsigned int menu_data[] =
{
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3
};

void menu::Initialize(int livesLeft) {

    state.nextScene = 0;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);
    // Initialize Game Objects
    // Initialize Player    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->lives = livesLeft;
    state.player->position = glm::vec3(2, -3, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 3.5f;
    state.player->height = 0.8f;
    state.player->width = 0.7f;
    state.player->textureID = Util::LoadTexture("art.png");
}

void menu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, 0, state.map);
  
    if (state.player->position.x >= 12) state.nextScene = 1;
}
void menu::Render(ShaderProgram* program) {
    Util::DrawText(program, Util::LoadTexture("font.png"), "Space Bugs",
        0.6f, 0.2f, glm::vec3(1.50f, -1.0f, 0));
    Util::DrawText(program, Util::LoadTexture("font.png"), "3 Lives Left",
        0.4f, 0.1f, glm::vec3(2.75f, -2.0f, 0));
    Util::DrawText(program, Util::LoadTexture("font.png"), 
        "Bugs change color after attacking!",
        0.25f, 0.0f, glm::vec3(1.0f, -4.0f, 0));
    Util::DrawText(program, Util::LoadTexture("font.png"), "Press Enter",
        0.3f, 0.1f, glm::vec3(4.75f, -6.0f, 0));
    state.map->Render(program);
    state.player->Render(program);
   
}
