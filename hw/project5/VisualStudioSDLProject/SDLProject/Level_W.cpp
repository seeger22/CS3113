#include "Level_W.h"
#include "Utility.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

unsigned int LEVEL_W_DATA[] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0
};

Level_W::~Level_W()
{
    delete    this->state.player;
    delete    this->state.map;
    Mix_FreeChunk(state.shield_sfx);
    Mix_FreeChunk(state.dash_sfx_1);
    Mix_FreeChunk(state.dash_sfx_2);
    Mix_FreeChunk(state.win_sfx);
    Mix_FreeChunk(state.lose_sfx);
    Mix_FreeChunk(state.jump_sfx);
    Mix_FreeMusic(state.bgm);
}

void Level_W::initialise()
{
    GLuint map_texture_id = Utility::load_texture("assets/customtileset.png");
    this->state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_W_DATA, map_texture_id, 1.0f, 4, 1);
    this->state.next_scene_id = 0;
    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->speed = 2.5f;
    state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    state.player->texture_id = Utility::load_texture("assets/geralt.png");

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

    // Jumping, dashing, etc.
    state.player->jumping_power = 5.0f;
    state.player->dashing_speed = 100.0f;
    state.player->set_threat_count(ENEMY_COUNT);
    GLuint enemy_texture_id = Utility::load_texture("assets/ghoul.png");

    state.enemies = new Entity[this->ENEMY_COUNT];
    state.enemies[0].set_entity_type(ENEMY);
    state.enemies[0].set_ai_type(GUARD);
    state.enemies[0].set_ai_state(IDLE);
    state.enemies[0].texture_id = enemy_texture_id;
    state.enemies[0].set_position(glm::vec3(8.0f, 2.0f, 0.0f));
    state.enemies[0].set_movement(glm::vec3(0.0f));
    state.enemies[0].speed = 1.0f;
    state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    state.bgm = Mix_LoadMUS("assets/kmc.mp3");
    Mix_PlayMusic(state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 15.0f);

    state.jump_sfx = Mix_LoadWAV("assets/grunt.wav");
    state.win_sfx = Mix_LoadWAV("assets/win.wav");
    state.lose_sfx = Mix_LoadWAV("assets/lose.wav");
    state.dash_sfx_1 = Mix_LoadWAV("assets/dash.wav");
    state.dash_sfx_2 = Mix_LoadWAV("assets/dash2.wav");
    state.shield_sfx = Mix_LoadWAV("assets/shield.wav");
}

void Level_W::update(float delta_time)
{

}

void Level_W::render(ShaderProgram* program)
{
    this->state.map->render(program);
    Utility::draw_text(program, "YUUUUUUH", 1.0f, -0.5f, glm::vec3(3.0f, -3.0f, 0.0f));

}