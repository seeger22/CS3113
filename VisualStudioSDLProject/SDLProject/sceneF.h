#pragma once
#include "Scene.h"
class sceneF : public Scene {
public:
    int ENEMY_COUNT = 1;

    ~sceneF();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};