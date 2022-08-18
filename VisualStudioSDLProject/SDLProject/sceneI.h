#pragma once
#include "Scene.h"

class sceneI : public Scene {
public:
    int ENEMY_COUNT = 0;

    ~sceneI();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
