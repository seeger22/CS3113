#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
//extras
#include <stdlib.h> // for srand
#include <time.h> // for time

enum Coordinate
{
    x_coordinate,
    y_coordinate
};

#define LOG(argument) std::cout << argument << '\n'

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;

const int NUMBER_OF_TEXTURES = 1; // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;   // this value MUST be zero

const char PLAYER_SPRITE_FILEPATH_BOAT[] = "boat.png";
const char PLAYER_SPRITE_FILEPATH_KNIFE[] = "knife.png";
const char PLAYER_SPRITE_FILEPATH_BALL[] = "soph.png";
// minimalist picture selection

SDL_Window* display_window;
bool game_is_running = true;

ShaderProgram program;
glm::mat4 view_matrix, model_matrix_paddle1, model_matrix_paddle2, model_matrix_ball, projection_matrix, trans_matrix;

float paddle1_y = 0.0f;
float paddle2_y = 0.0f;

const float MINIMUM_COLLISION_DISTANCE = 0.5f;

float ball_x = 0.0f,
ball_y = 0.0f;
float player_speed = 2.5f;
float ball_speed = 1.0f;
bool start; // flag to start/restart game
float previous_ticks = 0.0f;

GLuint player_texture_id_boat;
GLuint player_texture_id_knife;
GLuint player_texture_id_ball;
SDL_Joystick* player_one_controller;

// overall position
glm::vec3 player_position = glm::vec3(-4.5f, 0.0f, 0.0f);
glm::vec3 player_two_position = glm::vec3(4.5f, 0.0f, 0.0f);
glm::vec3 ball_position = glm::vec3(0.0f, 0.0f, 0.0f);

// movement tracker
glm::vec3 player_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 player_two_movement = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);

float get_screen_to_ortho(float coordinate, Coordinate axis)
{
    switch (axis) {
    case x_coordinate:
        return ((coordinate / WINDOW_WIDTH) * 10.0f) - (10.0f / 2.0f);
    case y_coordinate:
        return (((WINDOW_HEIGHT - coordinate) / WINDOW_HEIGHT) * 7.5f) - (7.5f / 2.0f);
    default:
        return 0.0f;
    }
}

bool check_collision(glm::vec3& position_a, glm::vec3& position_b)
{
    return sqrt(pow(position_b[0] - position_a[0], 2) + pow(position_b[1] - position_a[1], 2)) < MINIMUM_COLLISION_DISTANCE;

    //return sqrt(pow(position_b[0] - position_a[0], 2) + pow(position_b[1] - position_b[1], 2)) < MINIMUM_COLLISION_DISTANCE;
}

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
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    // Initialize random seed
    srand(time(NULL));

    // Initialise video and joystick subsystems
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    // Open the first controller found. Returns null on error
    player_one_controller = SDL_JoystickOpen(0);

    display_window = SDL_CreateWindow("PONG",
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

    model_matrix_paddle1 = glm::mat4(1.0f);
    model_matrix_paddle2 = glm::mat4(1.0f);
    model_matrix_ball = glm::mat4(1.0f);

    view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.

    program.SetProjectionMatrix(projection_matrix);
    program.SetViewMatrix(view_matrix);
    // Notice we haven't set our model matrix yet!

    glUseProgram(program.programID);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    player_texture_id_boat = load_texture(PLAYER_SPRITE_FILEPATH_BOAT);
    player_texture_id_knife = load_texture(PLAYER_SPRITE_FILEPATH_KNIFE);
    player_texture_id_ball = load_texture(PLAYER_SPRITE_FILEPATH_BALL);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    player_movement = glm::vec3(0.0f);
    player_two_movement = glm::vec3(0.0f);

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
        case SDL_WINDOWEVENT_CLOSE:
        case SDL_QUIT:
            game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                game_is_running = false;
                break;
            case SDLK_SPACE:
                start = true;
            default:
                break;
            }
        default:
            break;
        }
    }

    const Uint8* key_states = SDL_GetKeyboardState(NULL); // array of key states [0, 0, 1, 0, 0, ...]

    if (key_states[SDL_SCANCODE_W])
    {
        player_movement.y = 1.0f;
    }
    else if (key_states[SDL_SCANCODE_S])
    {
        player_movement.y = -1.0f;
    }

    if (key_states[SDL_SCANCODE_UP])
    {
        player_two_movement.y = 1.0f;
    }
    else if (key_states[SDL_SCANCODE_DOWN])
    {
        player_two_movement.y = -1.0f;
    }

    // Don't know if needed
    if (glm::length(player_movement) > 1.0f)
    {
        player_movement = glm::normalize(player_movement);
    }
    if (glm::length(player_two_movement) > 1.0f)
    {
        player_two_movement = glm::normalize(player_two_movement);
    }
}

void update()
{
    // BALL MOVEMENT
    if ((ball_position.x <= -5.0f) || (ball_position.x >= 5.0f)) {
        game_is_running = false; // right now win/lose is just closing the game
    }
    // Collision with paddles
    if (check_collision(ball_position, player_position)){
        ball_movement.x = 1.0f;
        ball_movement.y = ((rand() % 100) < 50) ? 1.0f : -1.0f;
    }
    else if (check_collision(ball_position, player_two_position)){
        ball_movement.x = -1.0f;
        ball_movement.y = ((rand() % 100) < 50) ? 1.0f : -1.0f;
    }

    // Collision with top or bottom
    if (ball_position.y >= 3.75f) {
        ball_movement.y = -1.0f;
    }
    else if (ball_position.y <= -3.75f) {
        ball_movement.y = 1.0f;
    }

    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - previous_ticks; // the delta time is the difference from the last frame
    previous_ticks = ticks;

    // ensures that player one does not go offscreen
    if (player_position.y > 3.25f) {
        player_movement = glm::vec3(0.0f);
        player_position.y = 3.25f;
    }
    else if (player_position.y < -3.25f) {
        player_movement = glm::vec3(0.0f);
        player_position.y = -3.25f;
    }
    // ensures that player two does not go offscreen
    if (player_two_position.y > 3.25f) {
        player_two_movement = glm::vec3(0.0f);
        player_two_position.y = 3.25f;
    }
    else if (player_two_position.y < -3.25f) {
        player_two_movement = glm::vec3(0.0f);
        player_two_position.y = -3.25f;
    }

    // Add             direction       * elapsed time * units per second
    player_position += player_movement * delta_time * player_speed;
    player_two_position += player_two_movement * delta_time * player_speed;

    if (start) {
        ball_position = glm::vec3(0.0f);
        ball_movement.x = ((rand() % 100) < 50) ? 1.0f : -1.0f;
        ball_movement.y = ((rand() % 100) < 50) ? 1.0f : -1.0f;
        start = false;
    }
    else {
        ball_position += ball_movement * delta_time * ball_speed;
    }

    model_matrix_paddle1 = glm::mat4(1.0f);
    model_matrix_paddle2 = glm::mat4(1.0f);
    model_matrix_ball = glm::mat4(1.0f);

    model_matrix_paddle1 = glm::translate(model_matrix_paddle1, player_position);
    model_matrix_paddle2 = glm::translate(model_matrix_paddle2, player_two_position);
    model_matrix_ball = glm::translate(model_matrix_ball, ball_position);
}

void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    program.SetModelMatrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Vertices
    float vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,  // triangle 1
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f   // triangle 2
    };

    // Textures
    float texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
    };

    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);

    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(program.texCoordAttribute);

    // Bind texture
    draw_object(model_matrix_paddle1, player_texture_id_boat);
    draw_object(model_matrix_paddle2, player_texture_id_knife);
    draw_object(model_matrix_ball, player_texture_id_ball);

    // We disable two attribute arrays now
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(display_window);
}

void shutdown()
{
    SDL_JoystickClose(player_one_controller);
    SDL_Quit();
}

/**
 Start here�we can see the general structure of a game loop without worrying too much about the details yet.
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