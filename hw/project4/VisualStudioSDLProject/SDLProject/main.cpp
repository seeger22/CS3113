#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define ENEMY_COUNT 3
#define LEVEL1_WIDTH 20
#define LEVEL1_HEIGHT 5

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"

#include <stdlib.h> // for srand
#include <time.h> // for time
#include <string.h> // for converting double to string to display time

/**
 STRUCTS AND ENUMS
 */
struct GameState
{
    Entity *player;
    Entity *enemies;
    
    Map *map;
    
    Mix_Music *bgm;
    Mix_Chunk *jump_sfx;
    Mix_Chunk* win_sfx;
    Mix_Chunk* lose_sfx;
    Mix_Chunk* dash_sfx_1;
    Mix_Chunk* dash_sfx_2;
    Mix_Chunk* shield_sfx;
};

/**
 CONSTANTS
 */
const int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

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
const char SPRITESHEET_FILEPATH[] = "assets/geralt.png";
const char FONT_FILEPATH[] = "assets/font1.png";

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL  = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER   = 0;   // this value MUST be zero

unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 3, 0, 0, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3,
    3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
};

/**
 VARIABLES
 */
GameState state;

SDL_Window* display_window;
bool game_is_running = true; // for global flags
bool game_over = false; // for local render flags
bool game_reset = false;

ShaderProgram program;
glm::mat4 view_matrix, projection_matrix;

float previous_ticks = 0.0f;
float accumulator = 0.0f;
const int FONTBANK_SIZE = 16;

std::clock_t start;
double duration;

/**
 GENERAL FUNCTIONS
 */
GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);
    
    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }
    
    // STEP 2: Generating and binding a texture ID to our image
    GLuint texture_id;
    glGenTextures(NUMBER_OF_TEXTURES, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // STEP 3: Setting our texture filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // STEP 4: Setting our texture wrapping modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // the last argument can change depending on what you are looking for
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // STEP 5: Releasing our file from memory and returning our texture id
    stbi_image_free(image);
    
    return texture_id;
}

void DrawText(ShaderProgram* program, GLuint font_texture_id, std::string text, float screen_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for each character
    // Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their position
        //    relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (screen_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (0.5f * screen_size), -0.5f * screen_size,
            offset + (0.5f * screen_size), 0.5f * screen_size,
            offset + (-0.5f * screen_size), -0.5f * screen_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }
    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->SetModelMatrix(model_matrix);
    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void initialise()
{
    srand(time(NULL)); // random var

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    display_window = SDL_CreateWindow("YUH",
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
    
    view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.
    
    program.SetProjectionMatrix(projection_matrix);
    program.SetViewMatrix(view_matrix);
    
    glUseProgram(program.programID);
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    /**
     Map Stuff
     */
    GLuint map_texture_id = load_texture("assets/customtileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, LEVEL_1_DATA, map_texture_id, 1.0f, 4, 1);
    
    /**
     George's Stuff
     */
    // Existing
    state.player = new Entity();
    state.player->set_entity_type(PLAYER);
    state.player->set_position(glm::vec3(0.0f, 0.0f, 0.0f));
    state.player->set_movement(glm::vec3(0.0f));
    state.player->speed = 2.5f;
    state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    state.player->texture_id = load_texture(SPRITESHEET_FILEPATH);
    state.player->set_threat_count(ENEMY_COUNT);
    
    // Walking
    state.player->walking[state.player->LEFT]  = new int[4] { 1, 5, 9,  13 };
    state.player->walking[state.player->RIGHT] = new int[4] { 3, 7, 11, 15 };
    state.player->walking[state.player->UP]    = new int[4] { 2, 6, 10, 14 };
    state.player->walking[state.player->DOWN]  = new int[4] { 0, 4, 8,  12 };

    state.player->animation_indices = state.player->walking[state.player->RIGHT];  // start George looking left
    state.player->animation_frames = 4;
    state.player->animation_index  = 0;
    state.player->animation_time   = 0.0f;
    state.player->animation_cols   = 4;
    state.player->animation_rows   = 4;
    state.player->set_height(0.8f);
    state.player->set_width(0.8f);
    
    // Jumping
    state.player->jumping_power = 5.0f;

    // Dashing
    state.player->dashing_speed = 100.0f;
    
    
    //Enemies' stuff
    
    // Wyvern
    state.enemies = new Entity[ENEMY_COUNT];
    state.enemies[0].set_entity_type(ENEMY);
    state.enemies[0].set_ai_type(WYVERN);
    state.enemies[0].set_ai_state(IDLE);
    state.enemies[0].texture_id = load_texture("assets/wyvern.png");
    state.enemies[0].set_position(glm::vec3(8.0f, 1.5f, 0.0f));
    state.enemies[0].set_movement(glm::vec3(0.0f));
    state.enemies[0].speed = 0.8f;
    state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f));

    // Wyvern animation
    state.enemies[0].walking[state.enemies[0].LEFT] = new int[4]{ 1, 5, 9,  13 };
    state.enemies[0].walking[state.enemies[0].RIGHT] = new int[4]{ 3, 7, 11, 15 };
    state.enemies[0].walking[state.enemies[0].UP] = new int[4]{ 2, 6, 10, 14 };
    state.enemies[0].walking[state.enemies[0].DOWN] = new int[4]{ 0, 4, 8,  12 };

    state.enemies[0].animation_indices = state.enemies[0].walking[state.enemies[0].UP];  // start UP = idle
    state.enemies[0].animation_frames = 4;
    state.enemies[0].animation_index = 0;
    state.enemies[0].animation_time = 0.0f;
    state.enemies[0].animation_cols = 4;
    state.enemies[0].animation_rows = 4;
    state.enemies[0].set_height(0.8f);
    state.enemies[0].set_width(0.8f);


    // Ghoul
    state.enemies[1].set_entity_type(ENEMY);
    state.enemies[1].set_ai_type(GUARD);
    state.enemies[1].set_ai_state(IDLE);
    state.enemies[1].texture_id = load_texture("assets/ghoul.png");
    state.enemies[1].set_position(glm::vec3(2.5f, 3.0f, 0.0f));
    state.enemies[1].set_movement(glm::vec3(0.0f));
    state.enemies[1].speed = 1.0f;
    state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    // Ekimmara
    state.enemies[2].set_entity_type(ENEMY);
    state.enemies[2].set_ai_type(EKIMMARA);
    state.enemies[2].set_ai_state(IDLE);
    state.enemies[2].texture_id = load_texture("assets/kiki.png");
    state.enemies[2].set_position(glm::vec3(16.0f, 3.0f, 0.0f));
    state.enemies[2].set_movement(glm::vec3(0.0f));
    state.enemies[2].speed = 1.2f;
    state.enemies[2].jumping_power = 3.0f;
    state.enemies[2].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    // Ekimmara animation
    state.enemies[2].walking[state.enemies[2].LEFT] = new int[4]{1, 5, 9,  13};
    state.enemies[2].walking[state.enemies[2].RIGHT] = new int[4]{ 3, 7, 11, 15 };
    state.enemies[2].walking[state.enemies[2].UP] = new int[4]{ 2, 6, 10, 14 };
    state.enemies[2].walking[state.enemies[2].DOWN] = new int[4]{ 0, 4, 8,  12 };

    state.enemies[2].animation_indices = state.enemies[2].walking[state.enemies[2].UP];  // start UP = idle
    state.enemies[2].animation_frames = 4;
    state.enemies[2].animation_index = 0;
    state.enemies[2].animation_time = 0.0f;
    state.enemies[2].animation_cols = 4;
    state.enemies[2].animation_rows = 4;
    state.enemies[2].set_height(0.8f);
    state.enemies[2].set_width(0.8f);
    
    
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
    
    start = std::clock();

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    state.player->set_movement(glm::vec3(0.0f));
    
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
                        
                    case SDLK_w:
                        // Jump
                        if (state.player->collided_bottom)
                        {
                            state.player->is_jumping = true;
                            Mix_PlayChannel(-1, state.jump_sfx, 0);
                        }
                        break;
                    case SDLK_e:
                        // Dash Attack
                        state.player->is_dashing = true;
                        Mix_PlayChannel(-1, ((rand() % 100) < 50) ? state.dash_sfx_1 : state.dash_sfx_2, 0);
                        state.player->animation_indices = state.player->walking[state.player->DOWN];
                        break;

                    case SDLK_r:
                        // reset
                        game_reset = true;
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
        state.player->movement.x = -1.0f;
        state.player->animation_indices = state.player->walking[state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_D])
    {
        state.player->movement.x = 1.0f;
        state.player->animation_indices = state.player->walking[state.player->RIGHT];
    }
    if (key_state[SDL_SCANCODE_SPACE])
    {
        state.player->is_shielding = true;
        Mix_PlayChannel(-1, state.shield_sfx, 0);
        state.player->animation_indices = state.player->walking[state.player->UP];
    }
    
    // This makes sure that the player can't move faster diagonally
    if (glm::length(state.player->movement) > 1.0f)
    {
        state.player->movement = glm::normalize(state.player->movement);
    }
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
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->update(FIXED_TIMESTEP, state.player, state.enemies, ENEMY_COUNT, state.map);
        
        for (int i = 0; i < ENEMY_COUNT; i++) state.enemies[i].update(FIXED_TIMESTEP, state.player, NULL, 0, state.map);
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    accumulator = delta_time;
    
    view_matrix = glm::mat4(1.0f);
    view_matrix = glm::translate(view_matrix, glm::vec3(-state.player->get_position().x, 0.0f, 0.0f));
}

void render()
{
    program.SetViewMatrix(view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    state.player->render(&program);
    state.map->render(&program);
    
    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        if (state.enemies[i].get_active_state()) state.enemies[i].render(&program);
    }
    GLuint font_texture_id = load_texture(FONT_FILEPATH);
    if (!state.player->get_active_state()) {
        DrawText(&program, font_texture_id, "YOU DIED", 1.0f, -0.5f, glm::vec3(state.player->get_position().x - 0.7f, 2.0f, 0.0f));
        if (!game_over) {
            Mix_PauseMusic();
            Mix_PlayChannel(-1, state.lose_sfx, 0);
            game_over = true;
        }
    }
    else if (state.player->get_threat_count() == 0) {
        DrawText(&program, font_texture_id, "YUH", 1.0f, -0.5f, glm::vec3(state.player->get_position().x - 0.7f, 2.0f, 0.0f));
        if (!game_over) {
            duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
            Mix_PauseMusic();
            Mix_PlayChannel(-1, state.win_sfx, 0);
            game_over = true;
        }
        DrawText(&program, font_texture_id, std::to_string(duration), 1.0f, -0.5f, glm::vec3(state.player->get_position().x - 0.7f, 1.4f, 0.0f));
    }
    SDL_GL_SwapWindow(display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    delete [] state.enemies;
    delete    state.player;
    delete    state.map;
    Mix_FreeChunk(state.shield_sfx);
    Mix_FreeChunk(state.dash_sfx_1);
    Mix_FreeChunk(state.dash_sfx_2);
    Mix_FreeChunk(state.win_sfx);
    Mix_FreeChunk(state.lose_sfx);
    Mix_FreeChunk(state.jump_sfx);
    Mix_FreeMusic(state.bgm);
}

/**
 DRIVER GAME LOOP
 */
int main(int argc, char* argv[])
{
    initialise();
    while (game_is_running)
    {
        if (game_reset) {
            game_reset = false;
            // initialise(); // bad
        }
        process_input();
        update();
        render();
    }

    return 0;
}
