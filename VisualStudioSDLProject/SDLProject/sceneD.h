#pragma once
#include "Scene.h"
class sceneD : public Scene {
public:
    int ENEMY_COUNT = 0;

    ~sceneD();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};