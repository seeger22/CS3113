// 3
// ADDITION: add logo. so far its just char
#include "sceneD.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

// Font


unsigned int sceneD_DATA[] =
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

sceneD::~sceneD()
{
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeMusic(this->state.bgm);
}

void sceneD::initialise()
{
    cutscene = true;
    next_scene_id = 4; //scene_b, enter


    GLuint map_texture_id = Utility::load_texture("assets/tileset.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, sceneD_DATA, map_texture_id, 1.0f, 4, 1);

    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(5.0f, -3.75f, 0.0f));
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
    // ADDITION: reorganize each level so that these are not set unneccessarily

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    state.bgm = Mix_LoadMUS("assets/tgate.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(4.0f);
}

void sceneD::update(float delta_time)
{
    this->state.player->update(delta_time, state.player, state.enemies, this->ENEMY_COUNT, this->state.map);
}

void sceneD::render(ShaderProgram* program)
{
    this->state.map->render(program);
    this->state.player->render(program);
    Utility::draw_text(program, "press enter", 0.75f, -0.45f, glm::vec3(3.75f, -5.0f, 0.0f));
}