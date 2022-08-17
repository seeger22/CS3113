#pragma once
#include "Scene.h"
class sceneA : public Scene {
public:
    int ENEMY_COUNT = 0;

    ~sceneA();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};