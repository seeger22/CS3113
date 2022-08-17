#pragma once
#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Utility.h"
#include "Entity.h"
#include "Map.h"

struct GameState
{
    Map *map;
    Entity *player;
    Entity *enemies;
    Entity* npc = NULL;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
};

class Scene {
public:
    int next_scene_id;
    int dialogue_count = 99; // ADDITION: need another way to distinguish this. this is bad
    int number_of_enemies = 1;
    bool cutscene = false;
    bool completed = false; // flag to show level has completed, immediately switch to next
    
    int decision; // ADDITION: better way to do this

    GameState state;
    
    virtual void initialise() = 0;
    virtual void update(float delta_time) = 0;
    virtual void render(ShaderProgram *program) = 0;
    
    GameState const get_state() const { return this->state; }
};
