#include "Level1.h"
#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 20

#define ENEMY_COUNT 4   

unsigned int level1_data[] =
{ 
 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
 3, 0, 3, 0, 0, 3, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void Level1::Initialize(int livesLeft) {

    //state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 2.0f, 4, 1);
    // Initialize Game Objects
    // Initialize Player    
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->lives = livesLeft;
    state.player->position = glm::vec3(2, -3, 0);
    state.player->movement = glm::vec3(0);
    state.player->speed = 3.5f;
    state.player->height = 1.0f;
    state.player->width = 1.0f;
    state.player->textureID = Util::LoadTexture("art.png");

   
    state.enemies = new Entity[ENEMY_COUNT];
    state.enemies[0].entityType = ENEMY;
    GLuint enemyTextureID = Util::LoadTexture("flying bug.png");
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].position = glm::vec3(2.0f, 3.0f, 0);
    state.enemies[0].speed = 0.3;
    state.enemies[0].aiType = FOLLOWER;
    state.enemies[0].aiState = FOLLOW;
    state.enemies[0].isActive = true;

    state.enemies[1].entityType = ENEMY;
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].position = glm::vec3(3.0f, -7.0f, 0);
    state.enemies[1].spawn = glm::vec3(3.0f, -7.0f, 0);
    state.enemies[1].speed = 0.3;
    state.enemies[1].aiType = GUARD;
    state.enemies[1].aiState = IDLE;
    state.enemies[1].isActive = true;

    state.enemies[2].entityType = ENEMY;
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(6.0f, -6.0f, 0);
    state.enemies[2].spawn = glm::vec3(6.0f, -6.0f, 0);
    state.enemies[2].speed = 0.8;
    state.enemies[2].aiType = VERTICAL_PATROLLER;
    state.enemies[2].aiState = PATROL;
    state.enemies[2].isActive = true;

    state.enemies[3].entityType = ENEMY;
    state.enemies[3].textureID = enemyTextureID;
    state.enemies[3].position = glm::vec3(6.0f, -6.0f, 0);
    state.enemies[3].spawn = glm::vec3(4.0f, -5.0f, 0);
    state.enemies[3].speed = 0.8;
    state.enemies[3].aiType = HORIZONTAL_PATROLLER;
    state.enemies[3].aiState = PATROL;
    state.enemies[3].isActive = true;
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, ENEMY_COUNT, state.map);
    }
}
void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);
    for (int i = 0; i < ENEMY_COUNT; i++) state.enemies[i].Render(program);
    state.player->Render(program);
}
