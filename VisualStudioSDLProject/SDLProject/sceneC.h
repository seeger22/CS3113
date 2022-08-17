#pragma once
#include "Scene.h"
class sceneC : public Scene {
public:
    int ENEMY_COUNT = 1;

    ~sceneC();

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};