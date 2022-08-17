// 4
#include "sceneE.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int sceneE_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

sceneE::~sceneE()
{
    delete[] this->state.enemies;
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(this->state.jump_sfx);
    Mix_FreeMusic(this->state.bgm);
}

void sceneE::initialise()
{
    dialogue_count = 8;
    next_scene_id = 5; //scene_f, if decide 1 then game ends, if not beat ostrit and go to right


    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, sceneE_DATA, map_texture_id, 1.0f, 4, 1);

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
    state.player->texture_id = Utility::load_texture("assets/george_0.png");

    // Walking
    state.player->walking[state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    state.player->walking[state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    state.player->walking[state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    state.player->walking[state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    state.player->animation_indices = state.player->walking[state.player->RIGHT];  // start George looking left
    state.player->animation_frames = 4;
    state.player->animation_index = 0;
    state.player->animation_time = 0.0f;
    state.player->animation_cols = 4;
    state.player->animation_rows = 4;
    state.player->set_height(0.8f);
    state.player->set_width(0.8f);

    // Attacks
    state.player->set_attack_strength(100);
    state.player->set_attack_range(0.75f);

    GLuint enemy_texture_id = Utility::load_texture("assets/soph.png");
    state.enemies = new Entity[this->ENEMY_COUNT];
    state.enemies[0].set_entity_type(ENEMY);
    state.enemies[0].set_ai_type(GUARD);
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

    state.bgm = Mix_LoadMUS("assets/dooblydoo.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(4.0f);
}

void sceneE::update(float delta_time)
{
    this->state.player->update(delta_time, state.player, state.enemies, this->ENEMY_COUNT, this->state.map);
    for (int i = 0; i < ENEMY_COUNT; i++) this->state.enemies[i].update(delta_time, state.player, NULL, 0, this->state.map);
    if (this->state.player->get_position().x > 15) completed = true;
}

void sceneE::render(ShaderProgram* program)
{
    this->state.map->render(program);
    this->state.player->render(program);
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (this->state.enemies[i].get_active_state()) state.enemies[i].render(program);
        if (!this->state.enemies[i].hostile && this->state.enemies[i].speaking)
        {
            cutscene = true;
            switch (dialogue_count) {
            case 8:
                Utility::draw_text(program, "witcher...", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 7:
                Utility::draw_text(program, "look at the corpses", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 6:
                Utility::draw_text(program, "this is no normal beast", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 5:
                Utility::draw_text(program, "but fear not", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 4:
                Utility::draw_text(program, "you are saved", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 3:
                Utility::draw_text(program, "i will give you 1500 orens", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 2:
                Utility::draw_text(program, "to leave it be, yes? (y)", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 1:
                Utility::draw_text(program, "then DIE!", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
                break;
            case 0:
                cutscene = false;
                this->state.enemies[i].set_hostile(true);
                this->state.enemies[i].set_ai_state(WALKING);
                break;
            }
        }
    }
}