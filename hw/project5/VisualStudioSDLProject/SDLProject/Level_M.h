#pragma once
#include "Scene.h"

class Level_M : public Scene {
public:
    int ENEMY_COUNT = 99;

    ~Level_M();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};