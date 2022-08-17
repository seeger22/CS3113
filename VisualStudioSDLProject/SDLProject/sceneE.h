#pragma once
#include "Scene.h"
class sceneE : public Scene {
public:
    int ENEMY_COUNT = 1;

    ~sceneE();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};