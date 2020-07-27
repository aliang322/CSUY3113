#include "MainMenu.h"

#define MENU_WIDTH 14
#define MENU_HEIGHT 8

#define MENU_ENEMY_COUNT 1

unsigned int menu_data[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void MainMenu::Initialize() {

    state.nextScene = -1;

	GLuint mapTextureID = Util::LoadTexture("tileset.png");
	state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 4, 1);
	
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(7, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 4.0f;
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

    state.player->jumpPower = 7.0f;

    state.enemies = new Entity[MENU_ENEMY_COUNT];

    GLuint slimeTextureID = Util::LoadTexture("slime.png");
    state.enemies[0].textureID = slimeTextureID;

    for (int i = 0; i < MENU_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 2;
    }

    state.enemies[0].position = glm::vec3(8.0f, -4.75f, 0);
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].aiState = IDLE;
    state.enemies[0].isActive = false;
}

void MainMenu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, MENU_ENEMY_COUNT, state.map);
}

void MainMenu::Render(ShaderProgram* program) {
	
    GLuint fontTextureID = Util::LoadTexture("pixel_font.png");

    Util::DrawText(program, fontTextureID, "Hunter", 0.7f, 0.1f, glm::vec3(5.0, -2.0, 0));
    Util::DrawText(program, fontTextureID, "Press Enter to Play", 0.3f, 0.1f, glm::vec3(3.2, -3.0, 0));
    Util::DrawText(program, fontTextureID, "Left and Right Arrows Keys to Move", 0.15f, 0.05f, glm::vec3(3.5, -3.85, 0));
    Util::DrawText(program, fontTextureID, "Spacebar to Jump", 0.15f, 0.05f, glm::vec3(5.5, -4.2, 0));

    state.map->Render(program);
	state.player->Render(program);
}