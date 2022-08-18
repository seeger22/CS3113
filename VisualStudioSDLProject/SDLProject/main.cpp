#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 4.5f
#define LEVEL1_RIGHT_EDGE 8.5f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "sceneA.h"
#include "sceneB.h"
#include "sceneC.h"
#include "sceneD.h"
#include "sceneE.h"
#include "sceneF.h"
#include "sceneG.h"
#include "sceneH.h"
#include "sceneI.h"
#include "sceneJ.h"



/**
 CONSTANTS
 640 480
 */
const int WINDOW_WIDTH  = 1280,
          WINDOW_HEIGHT = 960;

const float BG_RED     = 0.502f,
            BG_BLUE    = 0.502f,
            BG_GREEN   = 0.502f,
            BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

/**
 VARIABLES
 */
Scene *current_scene;
sceneA *scene_a;
sceneB* scene_b;
sceneC* scene_c;
sceneD* scene_d;
sceneE* scene_e;
sceneF* scene_f;
sceneG* scene_g;
sceneH* scene_h;
sceneI* scene_i;
sceneJ* scene_j;


SDL_Window* display_window;
bool game_is_running = true;

ShaderProgram program;
glm::mat4 view_matrix, projection_matrix;

float previous_ticks = 0.0f;
float accumulator = 0.0f;

void switch_to_scene(Scene *scene, int decision=0)
{
    current_scene = scene;
     current_scene->initialise();
    if (decision) current_scene->decision = decision;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    display_window = SDL_CreateWindow(":D",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    program.Load(V_SHADER_PATH, F_SHADER_PATH);
    
    view_matrix = glm::mat4(1.0f);
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projection_matrix);
    program.SetViewMatrix(view_matrix);
    
    glUseProgram(program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    scene_a = new sceneA();
    scene_b = new sceneB();
    scene_c = new sceneC();
    scene_d = new sceneD();
    scene_e = new sceneE();
    scene_f = new sceneF();
    scene_g = new sceneG();
    scene_h = new sceneH();
    scene_i = new sceneI();
    scene_j = new sceneJ();

    switch_to_scene(scene_a);
    
    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    current_scene->state.player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_q:
                        // Quit the game with a keystroke
                        game_is_running = false;
                        break;
                        
                    case SDLK_j:
                        // Attack (direction depends on movement)
                        current_scene->state.player->is_attacking = true;
                        current_scene->state.player->is_attacking_index = true;
                        current_scene->state.player->animation_index = 0;
                        current_scene->state.player->animation_indices = current_scene->state.player->attacking[current_scene->state.player->RIGHT];
                        break;
                    case SDLK_RETURN: // ADDITION: better advancement system
                        switch (current_scene->next_scene_id) {
                        case 1:
                            switch_to_scene(scene_b);
                            break;
                        case 4:
                            switch_to_scene(scene_e);
                            break;
                        case 7:
                            switch_to_scene(scene_h, current_scene->decision);
                            break;
                        default:
                            break;
                        }
                    case SDLK_y:
                        if (current_scene->cutscene && current_scene->next_scene_id == 5 && current_scene->dialogue_count==2) {
                            switch_to_scene(scene_i,1);
                            break;
                        }
                        break;
                    case SDLK_SPACE:
                        if (current_scene->cutscene) current_scene->dialogue_count--;
                        break;
                    case SDLK_LSHIFT:
                        if (current_scene->state.player->speed != 5.0f) current_scene->state.player->speed = 5.0f;
                        else current_scene->state.player->speed = 2.0f;
                        break;
                    default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);


    if (key_state[SDL_SCANCODE_A])
    {
        current_scene->state.player->movement.x = -1.0f;
        current_scene->state.player->orientation.x = -1.0f;
        current_scene->state.player->orientation.y = 0.0f;
        // ADDITION: diagnal attacks needs to be fixed - orientation

        current_scene->state.player->animation_indices = current_scene->state.player->walking[current_scene->state.player->LEFT];
        if (current_scene->state.player->is_attacking_index) current_scene->state.player->animation_indices = current_scene->state.player->attacking[current_scene->state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_D])
    {
        current_scene->state.player->movement.x = 1.0f;
        current_scene->state.player->orientation.x = 1.0f;
        current_scene->state.player->orientation.y = 0.0f;

        current_scene->state.player->animation_indices = current_scene->state.player->walking[current_scene->state.player->RIGHT];
        if (current_scene->state.player->is_attacking_index) current_scene->state.player->animation_indices = current_scene->state.player->attacking[current_scene->state.player->RIGHT];
    }

    if (key_state[SDL_SCANCODE_W])
    {
        current_scene->state.player->movement.y = 1.0f;
        current_scene->state.player->orientation.x = 0.0f;
        current_scene->state.player->orientation.y = 1.0f;

        current_scene->state.player->animation_indices = current_scene->state.player->walking[current_scene->state.player->UP];
        if (current_scene->state.player->is_attacking_index) current_scene->state.player->animation_indices = current_scene->state.player->attacking[current_scene->state.player->UP];
    }
    else if (key_state[SDL_SCANCODE_S])
    {
        current_scene->state.player->movement.y = -1.0f;
        current_scene->state.player->orientation.x = 0.0f;
        current_scene->state.player->orientation.y = -1.0f;

        current_scene->state.player->animation_indices = current_scene->state.player->walking[current_scene->state.player->DOWN];
        if (current_scene->state.player->is_attacking_index) current_scene->state.player->animation_indices = current_scene->state.player->attacking[current_scene->state.player->DOWN];
    }
    
    if (glm::length(current_scene->state.player->movement) > 1.0f)
    {
        current_scene->state.player->movement = glm::normalize(current_scene->state.player->movement);
    }
    if (current_scene->cutscene)
    {
        current_scene->state.player->movement = glm::vec3(0.0f);
        current_scene->state.player->is_attacking = false;// if currently in cutscene, don't allow movement
    }
    // if (current_scene->state.player->is_attacking) current_scene->state.player->movement = glm::vec3(0.0f);
    // ADDITION: grounding the player when attacking could be a choice, but needs enemy knockback.
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - previous_ticks;
    previous_ticks = ticks;
    
    delta_time += accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        current_scene->update(FIXED_TIMESTEP);
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    accumulator = delta_time;
    
    
    // Prevent the camera from showing anything outside of the "edge" of the level
    view_matrix = glm::mat4(1.0f);
    
    if (current_scene->state.player->get_position().x > LEVEL1_LEFT_EDGE && current_scene->state.player->get_position().x < LEVEL1_RIGHT_EDGE) {
        view_matrix = glm::translate(view_matrix, glm::vec3(-current_scene->state.player->get_position().x, 3.5, 0));
    } 
    else if (current_scene->state.player->get_position().x <= LEVEL1_LEFT_EDGE) {
        view_matrix = glm::translate(view_matrix, glm::vec3(-4.5, 3.5, 0));
    }
    else if (current_scene->state.player->get_position().x >= LEVEL1_RIGHT_EDGE)
    {
        view_matrix = glm::translate(view_matrix, glm::vec3(-8.5, 3.5, 0));
    }
    // Death
    if (current_scene->state.player->get_health() <= 0) switch_to_scene(scene_j);
}

void render()
{
    program.SetViewMatrix(view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    current_scene->render(&program);

    if (current_scene->completed)
    {
        switch (current_scene->next_scene_id) {
        case 2:
            switch_to_scene(scene_c);
            break;
        case 3:
            switch_to_scene(scene_d);
            break;
        case 5:
            switch_to_scene(scene_f);
            break;
        case 6:
            switch_to_scene(scene_g, current_scene->decision);
            break;
        case 8:
            switch_to_scene(scene_i, current_scene->decision);
            break;
        }
    }
    
    SDL_GL_SwapWindow(display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    delete scene_a;
    delete scene_b;
    delete scene_c;
    delete scene_d;
    delete scene_e;
    delete scene_f;
    delete scene_g;
    delete scene_h;
    delete scene_i;
    delete scene_j;

}

/**
 DRIVER GAME LOOP
 */
int main(int argc, char* argv[])
{
    initialise();
    
    while (game_is_running)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
