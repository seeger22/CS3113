#pragma once
#include "Map.h"

enum EntityType { PLATFORM, PLAYER, ENEMY };
enum AIType     { WALKER, GUARD, STRIGA           };
enum AIState    { WALKING, IDLE, ATTACKING, BACK_AWAY, WEAK, ENRAGED };

class Entity
{
private:
    bool is_active = true;
    EntityType entity_type;
    AIType ai_type;
    AIState ai_state;
    
    int *animation_right = NULL; // move to the right
    int *animation_left  = NULL; // move to the left
    int *animation_up    = NULL; // move upwards
    int *animation_down  = NULL; // move downwards

    int* attacking_left = NULL;
    int* attacking_right = NULL;
    int* attacking_up = NULL;
    int* attacking_down = NULL;
    
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    
    float width  = 0.8f;
    float height = 0.8f;

    // stats
    int health = 100;
    int attack_strength = 100;
    
public:
    // Static attributes
    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT  = 0,
                     RIGHT = 1,
                     UP    = 2,
                     DOWN  = 3;
    
    // Existing
    GLuint texture_id;
    glm::mat4 model_matrix;
    
    // Translating
    float speed;
    glm::vec3 movement;
    
    // Animating
    int **walking          = new int*[4] { animation_left, animation_right, animation_up, animation_down };
    int** attacking = new int* [4]{ attacking_left, attacking_right, attacking_up, attacking_down };
    int *animation_indices = NULL;
    int animation_frames   = 0;
    int animation_index    = 0;
    float animation_time   = 0.0f;
    int animation_cols     = 0;
    int animation_rows     = 0;
    glm::vec3 orientation;
    
    // Jumping
    bool is_jumping     = false;
    float jumping_power = 0;
    
    // Colliding
    bool collided_top    = false;
    bool collided_bottom = false;
    bool collided_left   = false;
    bool collided_right  = false;

    // Attacking
    bool is_attacking = false;
    bool is_attacking_index = false;
    float attack_range = 0.25f;
    int attack_frame = 0;

    // NPC stuff
    bool hostile = true;
    bool invincible = false;
    bool speaking = false; // ADDITION: need better way to distinguish this

    // Decisions
    int decision = 0;
    /*
    1 - ostrit 1500
    2 - killed
    3 - cured 3000
    */

    // Methods
    Entity();
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram *program, GLuint texture_id, int index);
    void update(float delta_time, Entity *player, Entity *objects, int object_count, Map *map);
    void render(ShaderProgram *program);
    void activate_ai(Entity *player);
    void ai_walker();
    void ai_guard(Entity *player);
    void ai_striga(Entity* player);

    // Damage related
    void take_damage(int damage_amount);
    
    void const check_attack_collision(Entity* collidable_entities, int collidable_entity_count, glm::vec3 hit_point);
    void const check_collision_y(Entity *collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity *collidable_entities, int collidable_entity_count);
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    bool const check_collision(Entity *other) const;
    
    void activate()   { is_active = true;  };
    void deactivate() { is_active = false; };
    
    bool const get_active_state() const { return is_active; };
    EntityType const get_entity_type()  const { return entity_type;  };
    AIType     const get_ai_type()      const { return ai_type;      };
    AIState    const get_ai_state()     const { return ai_state;     };
    glm::vec3  const get_position()     const { return position;     };
    glm::vec3  const get_movement()     const { return movement;     };
    glm::vec3  const get_velocity()     const { return velocity;     };
    glm::vec3  const get_acceleration() const { return acceleration; };
    int        const get_width()        const { return width;        };
    int        const get_height()       const { return height;       };

    int const get_health() const { return health; }
    int const get_attack_strength() const { return attack_strength; };
    
    void const set_entity_type(EntityType new_entity_type)  { entity_type  = new_entity_type;      };
    void const set_ai_type(AIType new_ai_type)              { ai_type      = new_ai_type;          };
    void const set_ai_state(AIState new_state)              { ai_state     = new_state;            };
    void const set_position(glm::vec3 new_position)         { position     = new_position;         };
    void const set_movement(glm::vec3 new_movement)         { movement     = new_movement;         };
    void const set_velocity(glm::vec3 new_velocity)         { velocity     = new_velocity;         };
    void const set_acceleration(glm::vec3 new_acceleration) { acceleration = new_acceleration;     };
    void const set_width(float new_width)                   { width        = new_width;            };
    void const set_height(float new_height)                 { height       = new_height;           };
    void const set_health(float new_health) { health = new_health; };
    void const set_attack_strength(int new_strength) { attack_strength = new_strength; };
    void const set_attack_range(float new_range) { attack_range = new_range; };
    void const set_orientation(glm::vec3 new_orientation) { orientation = new_orientation; };
    void const set_hostile(bool new_hostile) { hostile = new_hostile; };
};
