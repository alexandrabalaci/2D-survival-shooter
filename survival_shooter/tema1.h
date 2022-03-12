#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
     public:

         struct ViewportSpace
         {
             ViewportSpace() : x(0), y(0), width(1), height(1) {}
             ViewportSpace(int x, int y, int width, int height)
                 : x(x), y(y), width(width), height(height) {}
             int x;
             int y;
             int width;
             int height;
         };

         struct LogicSpace
         {
             LogicSpace() : x(0), y(0), width(1), height(1) {}
             LogicSpace(float x, float y, float width, float height)
                 : x(x), y(y), width(width), height(height) {}
             float x;
             float y;
             float width;
             float height;
         };

         struct Entity
         {
             GLfloat scaleX;
             GLfloat scaleY;
             GLfloat posY;
             GLfloat posX;
             GLfloat sizeY;
             GLfloat sizeX;
             GLfloat angle;
             GLfloat distance;
             GLboolean render;
             GLfloat speed;

         };

    public:
         Tema1();
        ~Tema1();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void RenderObstacles(GLfloat deltaTimeSeconds);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);
        void RenderPlayer(GLfloat deltaTimeSeconds);
        GLboolean CheckCollision(const Entity& one, int positionX, int positionY, const Entity& two);
        GLboolean CheckCollisionP(const Entity& one, const Entity& two);
        void RenderEnemies(GLfloat deltaTimeSeconds);
        void RenderProjectile(GLfloat deltaTimeSeconds);
        void CreateEnemy(GLfloat deltaTimeSeconds);
        void RenderHealthBar(GLfloat deltaTimeSeconds);
        void RenderHealthBonus(GLfloat deltaTimeSeconds);
        void RenderScoreBonus(GLfloat deltaTimeSeconds);
        void CreateHealthBonus();
        void CreateScoreBonus();



     protected:
        glm::mat3 modelMatrix, visMatrix, playerMatrix, enemyMatrix, projectileMatrix, healthMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angle;
        int healthBonusNo = 0;
        int scoreBonusNo = 0;
        int enemiesNo = 0;
        int projectilesNo = 0;

        Entity player;
        Entity map;
        Entity healthBar;
        Entity healthBarEmpty;
        Entity healthBonus;
        Entity scorehBonus;


        GLfloat timePassedEnemies;
        GLfloat timePassedBonus = 0;
        GLfloat timePassedScore = 0;
        GLfloat timePassedProjectile;
        GLboolean wasMousePressed = false;
        GLfloat score = 0;

        std::vector<Entity> obstacles;
        std::vector<Entity> enemies;
        std::vector<Entity> projectiles;
        std::vector<Entity> healthBonuses;
        std::vector<Entity> scoreBonuses;

        ViewportSpace viewSpace;
        LogicSpace logicSpace;

    };
}   // namespace m1
