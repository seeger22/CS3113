#pragma once
#include "Scene.h"
class sceneG : public Scene {
public:
    int ENEMY_COUNT = 0;

    ~sceneG();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};