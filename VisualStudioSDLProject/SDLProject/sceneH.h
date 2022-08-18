#pragma once
#include "Scene.h"
class sceneH : public Scene {
public:
    int ENEMY_COUNT = 1;

    ~sceneH();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};