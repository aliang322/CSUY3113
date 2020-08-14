#include "Entity.h"

Entity::Entity()
{
    position = glm::vec3(0);
    movement = glm::vec3(0);
    velocity = glm::vec3(0);
    speed = 0;
    modelMatrix = glm::mat4(1.0f);
}

bool Entity::CheckCollision(Entity* other){
    if (isActive == false || other->isActive == false) return false;
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);
    if (xdist < 0 && ydist < 0) {
        //lastCollision = other->entityType;
        return true;
    }
    return false;
}

void Entity::CheckCollisionsY(Entity* objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity* object = &objects[i];
        if (CheckCollision(object)){
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}


void Entity::CheckCollisionsY(Map* map) {
    // Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    }
    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Entity* objects, int objectCount){
    for (int i = 0; i < objectCount; i++){
        Entity* object = &objects[i];
        if (CheckCollision(object)){
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0){
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0){
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Map* map) {
    // Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);

    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);

    float penetration_x = 0;
    float penetration_y = 0;
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }
    
    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.x > 0) {
        position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
    
}

void Entity::CheckIfPlayerKilled(Entity* enemies, int enemyCount) {
    for (int i = 0; i < enemyCount; i++) {
        Entity* enemy = &enemies[i];
        if (CheckCollision(enemy)) lives -= 1;
    }
}

void Entity::AI(Entity* player) {
    switch (aiType) {
    case FOLLOWER:
        AIFollower(player);
        break;
    case GUARD:
        AIGuard(player);
        break;
    case HORIZONTAL_PATROLLER:
        AIHorizontalPatroller(player);
    case VERTICAL_PATROLLER:
        AIVerticalPatroller(player);
    }
}

void Entity::AIFollower(Entity* player) {
    switch (aiState) {
    case FOLLOW:
        if (player->position.x < position.x) movement.x = -1;
        else if (player->position.x > position.x) movement.x = 1;

        if (player->position.y < position.y) movement.y = -1;
        else if (player->position.y > position.y) movement.y = 1;
        break;
    }
}

void Entity::AIGuard(Entity* player) {
    switch (aiState) {
    case IDLE:
        if (glm::distance(position, player->position) < 2.5f) aiState = FOLLOW;
        break;
    case FOLLOW:
        if (glm::distance(position, player->position) < 2.5f) {
            if (player->position.x < position.x) movement.x = -1;
            else if (player->position.x > position.x) movement.x = 1;

            if (player->position.y < position.y) movement.y = -1;
            else if (player->position.y > position.y) movement.y = 1;
        }
        else if (glm::distance(position, spawn) < 2.5f) {
            if (spawn.x < position.x) movement.x = -1;
            else if (spawn.x > position.x) movement.x = 1;

            if (spawn.y < position.y) movement.y = -1;
            else if (spawn.y > position.y) movement.y = 1;
        }
        if (glm::distance(position, spawn) == 0.0f) aiState = IDLE;
        break;
    }
}

void Entity::AIVerticalPatroller(Entity* player) {
    switch (aiState) {
    case PATROL:
        if (position == spawn) movement.y = -1;
        else if (position.y <= spawn.y - 1.5f && movement.y <= 0) movement.y = 1;
        else if (position.y >= spawn.y + 1.5f && movement.y >= 0) movement.y = -1;
    }
}

void Entity::AIHorizontalPatroller(Entity* player) {
    switch (aiState) {
    case PATROL:
        if (position == spawn) movement.x = -1;
        else if (position.x <= spawn.x - 1.5f && movement.x <= 0) movement.x = 1;
        else if (position.x >= spawn.x + 1.5f && movement.x >= 0) movement.x = -1;
    }
}
void Entity::Update(float deltaTime, Entity* player, Entity* objects, int objectCount, Map* map)
{
    if (isActive == false) return;
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;

    float original_x = position.x;
    float original_y = position.y;

    if (entityType == ENEMY) AI(player);
    else if (entityType == PLAYER) CheckIfPlayerKilled(objects, objectCount);

    velocity.x = movement.x * speed;
    velocity.y = movement.y * speed;

    position.x += velocity.x * deltaTime; // Move on X

    position.y += velocity.y * deltaTime; // Move on Y

    if (entityType == PLAYER) {
        if (movement.x != 0.0f) CheckCollisionsX(map);
        else  if (movement.y != 0.0f) CheckCollisionsY(map);
        if (collidedBottom || collidedTop) {
            movement.x = 0;
            position.x = original_x;
            isActive = false;
            
        }
        if (collidedRight || collidedLeft) {
            movement.y = 0;
            position.y = original_y;
            isActive = false;
        }
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;

    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;

    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v };

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram* program) {
    if (isActive == false) return;
    program->SetModelMatrix(modelMatrix);

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, textureID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}