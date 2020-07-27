#include "Level2.h"

#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0,
    3, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

void Level2::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 4, 1);

    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->lives = state.lives;

    state.player->spawn = glm::vec3(1.5, -6.5, 0);
    state.player->position = glm::vec3(1.5, -6.5, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 3.5f;
    state.player->textureID = Util::LoadTexture("george_0.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;

    state.player->height = 0.8f;
    state.player->width = 0.8f;

    state.player->jumpPower = 6.0f;

    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];

    GLuint spiderTextureID = Util::LoadTexture("spider.png");
    state.enemies[0].textureID = spiderTextureID;

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 2;
    }

    state.enemies[0].position = glm::vec3(8.0, -5.0f, 0);
    state.enemies[0].aiType = DIGGER;
    state.enemies[0].aiState = DIGGING;
}

void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    state.lives = state.player->lives;
    if (state.lives <= 0) state.nextScene = 5;
    else if (state.player->position.x >= 12.5 && state.player->position.y >= -2) state.nextScene = 3;
}

void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}