#pragma once
#include "Scene.h"

class Level_F : public Scene {
public:
    int ENEMY_COUNT = 99;

    ~Level_F();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};