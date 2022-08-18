#pragma once
#include "Scene.h"

class sceneJ : public Scene {
public:
    int ENEMY_COUNT = 0;

    ~sceneJ();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};