// 7
#include "sceneH.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int sceneH_DATA[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

sceneH::~sceneH()
{
    delete[] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void sceneH::initialise()
{
    dialogue_count = 7;
    next_scene_id = 8; //scene_i, end of dialogue


    GLuint map_texture_id = Utility::load_texture("assets/sceneC_tiles.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, sceneH_DATA, map_texture_id, 1.0f, 7, 1);

    // Code from main.cpp's initialise()
    /**
     George's Stuff
     */
     // Existing
    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(2.0f, -3.0f, 0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->set_orientation(glm::vec3(1.0f, 0.0f, 0.0f));
    state.player->speed = 2.5f;
    state.player->set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    state.player->texture_id = Utility::load_texture("assets/geralt_new.png");

    // Walking
    state.player->walking[state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    state.player->walking[state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    state.player->walking[state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    state.player->walking[state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };
    // Attacking
    state.player->attacking[state.player->LEFT] = new int[4]{ 17, 21, 25, 29 };
    state.player->attacking[state.player->RIGHT] = new int[4]{ 19, 23, 27, 31 };
    state.player->attacking[state.player->UP] = new int[4]{ 18, 22, 26, 30 };
    state.player->attacking[state.player->DOWN] = new int[4]{ 20, 24, 28, 32 };

    state.player->animation_indices = state.player->walking[state.player->DOWN];  // start George looking left
    state.player->animation_frames = 4;
    state.player->animation_index = 0;
    state.player->animation_time = 0.0f;
    state.player->animation_cols = 4;
    state.player->animation_rows = 8;
    state.player->set_height(0.8f);
    state.player->set_width(0.8f);

    // Attacks
    state.player->set_attack_strength(100);
    state.player->set_attack_range(0.75f);

    GLuint enemy_texture_id = Utility::load_texture("assets/foltest.png");
    state.enemies = new Entity[this->ENEMY_COUNT];
    state.enemies[0].set_entity_type(ENEMY);
    state.enemies[0].set_ai_type(WALKER);
    state.enemies[0].set_ai_state(IDLE);
    state.enemies[0].texture_id = enemy_texture_id;
    state.enemies[0].set_position(glm::vec3(8.0f, -3.0f, 0.0f));
    state.enemies[0].set_movement(glm::vec3(0.0f));
    state.enemies[0].speed = 1.0f;
    state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));
    state.enemies[0].set_hostile(false);

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
}

void sceneH::update(float delta_time)
{
    this->state.player->update(delta_time, state.player, state.enemies, this->ENEMY_COUNT, this->state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) this->state.enemies[i].update(delta_time, state.player, NULL, 0, this->state.map);
}

void sceneH::render(ShaderProgram* program)
{
    this->state.map->render(program);
    this->state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (this->state.enemies[i].get_active_state()) state.enemies[i].render(program);
        if (!this->state.enemies[i].hostile && this->state.enemies[i].speaking)
        {
            cutscene = true;
            if (decision == 2)
            {
                switch (dialogue_count) {
                case 7:
                    Utility::draw_text(program, "so ... i suppose", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 6:
                    Utility::draw_text(program, "i shall hear your excuse", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 5:
                    Utility::draw_text(program, "what? it was life or death?", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 4:
                    Utility::draw_text(program, "...", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 3:
                    Utility::draw_text(program, "perhaps. but so was her", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 2:
                    Utility::draw_text(program, "no matter. i do not wish", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 1:
                    Utility::draw_text(program, "to see you ever again.", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 0:
                    cutscene = false;
                    completed = true;
                    break;
                }
            }
            else if (decision == 3) 
            {
                switch (dialogue_count) {
                case 7:
                    Utility::draw_text(program, "splendid! orens, as promised", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 6:
                    Utility::draw_text(program, "i know you will come through", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 5:
                    Utility::draw_text(program, "...", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 4:
                    Utility::draw_text(program, "but i do wonder", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 3:
                    Utility::draw_text(program, "is she going to be ok?", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 2:
                    Utility::draw_text(program, "she has not seen the light", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 1:
                    Utility::draw_text(program, "for 14 long years ...", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                    break;
                case 0:
                    cutscene = false;
                    completed = true;
                    break;
                }
            }
        }
    }
}