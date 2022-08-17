#pragma once
#include "Scene.h"
class sceneB : public Scene {
public:
    int ENEMY_COUNT = 1;

    ~sceneB();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};