#include "lab_m1/tema1/tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/tema1/tema1_transform2D.h"
#include "lab_m1/tema1/2dObject.h"
#define BODY 75
#define ENEMYBODY 50
#define ENEMYRADIUS 25
#define PLAYERBOX 80
#define PLAYERSAFESPACE 200
#define SPEED 350
#define EYE 25
#define OBSTACLESNO 6
#define OBSX 400
#define OBSY 200
#define PRX 20
#define PRY 40
#define PRSPEED 450
#define MAXDISTANCE 500

using namespace std;
using namespace m1;
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

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	logicSpace.x = 0;
	logicSpace.y = 0;
	logicSpace.width = 1000;
	logicSpace.height = 1000;

	glm::vec3 corner = glm::vec3(0, 0, 0);

	translateX = logicSpace.width / 2;
	translateY = logicSpace.height / 2;
	timePassedEnemies = 0.f;
	timePassedProjectile = 0.f;

	Mesh* body = object2D::create_square("body", corner, BODY, glm::vec3(1, 0.43, 0.78), true);
	AddMeshToList(body);

	Mesh* eye = object2D::create_square("eye", corner, EYE, glm::vec3(0.35, 0.35, 0.87), true);
	AddMeshToList(eye);

	Mesh* enemyBody = object2D::create_square("enemyBody", corner, ENEMYBODY, glm::vec3(0.89, 0.47, 0.20), true);
	AddMeshToList(enemyBody);

	Mesh* enemy = object2D::create_circle(ENEMYRADIUS, ENEMYRADIUS, "enemy", glm::vec3(250, 250, 0.10), true);
	AddMeshToList(enemy);

	Mesh* background = object2D::create_rectangle("background", corner, 1500, 2500, glm::vec3(1, 0, 1));
	AddMeshToList(background);

	Mesh* obstacle = object2D::create_rectangle("obstacle", corner, OBSY, OBSX, glm::vec3(0.196078, 0.8, 0.6), true);
	AddMeshToList(obstacle);

	Mesh* projectile = object2D::create_rectangle("projectile", corner, PRX, PRY, glm::vec3(0.90, 0.91, 0.98), true);
	AddMeshToList(projectile);

	Mesh* healthBonus1 = object2D::create_rectangle("healthBonus1", corner, 15, 30, glm::vec3(0.5, 1, 0.5), true);
	AddMeshToList(healthBonus1);

	Mesh* healthBonus2 = object2D::create_rectangle("healthBonus2", corner, 30, 15, glm::vec3(0.5, 1, 0.5), true);
	AddMeshToList(healthBonus2);

	Mesh* scoreBonus1 = object2D::create_rectangle("scoreBonus1", corner, 15, 30, glm::vec3(0.8, 0.196078, 0.6), true);
	AddMeshToList(scoreBonus1);

	Mesh* scoreBonus2 = object2D::create_rectangle("scoreBonus2", corner, 30, 15, glm::vec3(0.8, 0.196078, 0.6), true);
	AddMeshToList(scoreBonus2);

	healthBar.posX = 999;
	healthBar.posY = 899;
	healthBar.scaleX = 1;
	healthBar.scaleY = 1;
	healthBar.sizeX = 300;
	healthBar.sizeY = 80;

	healthBarEmpty.posX = 1000;
	healthBarEmpty.posY = 900;
	healthBarEmpty.scaleX = 1;
	healthBarEmpty.scaleY = 1;
	healthBarEmpty.sizeX = 300;
	healthBarEmpty.sizeY = 80;

	Mesh* healthBarEmpty = object2D::create_rectangle("healthBarEmpty", corner, 80, 300, glm::vec3(1, 0.09, 0.09));
	AddMeshToList(healthBarEmpty);

	Mesh* healthBar = object2D::create_rectangle("healthBar", corner, 80, 300, glm::vec3(0.5, 1, 0.5), true);
	AddMeshToList(healthBar);

	obstacles.resize(OBSTACLESNO);
}

void Tema1::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Tema1::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->GetResolution();
	timePassedEnemies += deltaTimeSeconds;
	timePassedProjectile += deltaTimeSeconds;
	timePassedBonus += deltaTimeSeconds;
	timePassedScore += deltaTimeSeconds;

	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	SetViewportArea(viewSpace, glm::vec3(0), true);

	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

	RenderHealthBar(deltaTimeSeconds);
	RenderHealthBonus(deltaTimeSeconds);
	RenderScoreBonus(deltaTimeSeconds);

	RenderEnemies(deltaTimeSeconds);

	if (wasMousePressed) {
		RenderProjectile(deltaTimeSeconds);
	}

	RenderPlayer(deltaTimeSeconds);

	RenderObstacles(deltaTimeSeconds);

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(0, 0);
	RenderMesh2D(meshes["background"], shaders["VertexColor"], modelMatrix);

	map.posX = 0;
	map.posY = 0;
	map.scaleX = 1;
	map.scaleY = 1;
	map.sizeX = 2500;
	map.sizeY = 1500;
}

void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	bool collision = false;

	if (window->KeyHold(GLFW_KEY_W) && ((map.posY < (player.posY + deltaTime * SPEED + PLAYERBOX))
		&& ((player.posY + deltaTime * SPEED + PLAYERBOX) < (map.posY + map.sizeY)))) {

		for (int i = 0; i < OBSTACLESNO; i++) {
			if (player.posX + player.sizeX >= obstacles[i].posX &&
				obstacles[i].posX + obstacles[i].sizeX >= player.posX) {
				collision = CheckCollision(player, player.posX, translateY + deltaTime * SPEED, obstacles[i]);
				if (collision) {
					break;
				}
			}
		}
		if (!collision) {
			translateY += deltaTime * SPEED;
			logicSpace.y += deltaTime * SPEED;
			healthBar.posY += deltaTime * SPEED;
			healthBarEmpty.posY += deltaTime * SPEED;
		}
	}

	if (window->KeyHold(GLFW_KEY_A) && ((map.posX < player.posX - deltaTime * SPEED - BODY + PLAYERBOX)
		&& (player.posX - deltaTime * SPEED - BODY + PLAYERBOX) < (map.posX + map.sizeX))) {

		for (int i = 0; i < OBSTACLESNO; i++) {
			if (player.posY + player.sizeY >= obstacles[i].posY &&
				obstacles[i].posY + obstacles[i].sizeY >= player.posY) {
				collision = CheckCollision(player, translateX - deltaTime * SPEED, player.posY, obstacles[i]);
				if (collision) {
					break;
				}
			}
		}
		if (!collision) {
			translateX -= deltaTime * SPEED;
			logicSpace.x -= deltaTime * SPEED;
			healthBar.posX -= deltaTime * SPEED;
			healthBarEmpty.posX -= deltaTime * SPEED;
		}
	}

	if (window->KeyHold(GLFW_KEY_S) && ((map.posY < player.posY - deltaTime * SPEED - BODY + PLAYERBOX)
		&& (player.posY - deltaTime * SPEED - BODY + PLAYERBOX < (map.posY + map.sizeY)))) {

		for (int i = 0; i < OBSTACLESNO; i++) {
			if (player.posX + player.sizeX > obstacles[i].posX &&
				obstacles[i].posX + obstacles[i].sizeX > player.posX) {
				collision = CheckCollision(player, player.posX, translateY - deltaTime * SPEED, obstacles[i]);
				if (collision) {
					break;
				}
			}
		}

		if (!collision) {
			translateY -= deltaTime * SPEED;
			logicSpace.y -= deltaTime * SPEED;
			healthBar.posY -= deltaTime * SPEED;
			healthBarEmpty.posY -= deltaTime * SPEED;
		}
	}

	if (window->KeyHold(GLFW_KEY_D) && ((map.posX < player.posX + deltaTime * SPEED + PLAYERBOX)
		&& (player.posX + deltaTime * SPEED + PLAYERBOX) < (map.posX + map.sizeX))) {

		for (int i = 0; i < OBSTACLESNO; i++) {
			if (player.posY + player.sizeY > obstacles[i].posY &&
				obstacles[i].posY + obstacles[i].sizeY > player.posY) {
				collision = CheckCollision(player, translateX + deltaTime * SPEED, player.posY, obstacles[i]);
				cout << collision << endl;
				if (collision) {
					break;
				}
			}
		}
		if (!collision) {
			translateX += deltaTime * SPEED;
			logicSpace.x += deltaTime * SPEED;
			healthBar.posX += deltaTime * SPEED;
			healthBarEmpty.posX += deltaTime * SPEED;
		}
	}

	if (window->KeyHold(GLFW_KEY_Z)) {
		logicSpace.height += deltaTime * SPEED;
		logicSpace.width += deltaTime * SPEED;
	}
}

void Tema1::OnKeyPress(int key, int mods)
{
}

void Tema1::OnKeyRelease(int key, int mods)
{
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	glm::ivec2 mouse = window->GetCursorPosition();
	glm::ivec2 resolution = window->GetResolution();
	player.angle = atan2((mouseX - resolution.x / 2), (mouseY - resolution.y / 2)) + 3 * 3.14 / 2;
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	glm::ivec2 resolution = window->GetResolution();

	if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT) && (timePassedProjectile > 0.5)) {
		wasMousePressed = true;
		Entity proj;
		proj.angle = atan2((mouseX - resolution.x / 2), (mouseY - resolution.y / 2)) + 3 * 3.14 / 2;
		proj.posX = player.posX + player.sizeX / 2;
		proj.posY = player.posY + player.sizeY / 2;
		proj.sizeX = PRX;
		proj.sizeY = PRY;
		proj.render = true;
		proj.distance = 0;

		projectiles.push_back(proj);

		projectilesNo++;
		timePassedProjectile = 0;
	}
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}

void Tema1::RenderObstacles(GLfloat deltaTimeSeconds)
{
	int i = 0;

	//obstacle 1
	scaleX = 0.5;
	scaleY = 2;

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(200, 200);
	modelMatrix *= transform2D::Scale((scaleX), (scaleY));
	RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

	obstacles[i].posX = 200;
	obstacles[i].posY = 200;
	obstacles[i].scaleX = scaleX;
	obstacles[i].scaleY = scaleY;
	obstacles[i].sizeX = OBSX * scaleX;
	obstacles[i].sizeY = OBSY * scaleY;

	//obstacle 2
	i++;
	scaleX = 0.75;
	scaleY = 1;

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(1000, 300);
	modelMatrix *= transform2D::Scale((scaleX), (scaleY));
	RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

	obstacles[i].posX = 1000;
	obstacles[i].posY = 300;
	obstacles[i].scaleX = scaleX;
	obstacles[i].scaleY = scaleY;
	obstacles[i].sizeX = OBSX * scaleX;
	obstacles[i].sizeY = OBSY * scaleY;


	//obstacle 3
	i++;
	scaleX = 0.625;
	scaleY = 2;

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(1950, 300);
	modelMatrix *= transform2D::Scale((scaleX), (scaleY));
	RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

	obstacles[i].posX = 1950;
	obstacles[i].posY = 300;
	obstacles[i].scaleX = scaleX;
	obstacles[i].scaleY = scaleY;
	obstacles[i].sizeX = OBSX * scaleX;
	obstacles[i].sizeY = OBSY * scaleY;

	//obstacle 4

	i++;
	scaleX = 1.25;
	scaleY = 2;
	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(400, 900);
	modelMatrix *= transform2D::Scale((scaleX), (scaleY));
	RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
	obstacles[i].posX = 400;
	obstacles[i].posY = 900;
	obstacles[i].scaleX = scaleX;
	obstacles[i].scaleY = scaleY;
	obstacles[i].sizeX = OBSX * scaleX;
	obstacles[i].sizeY = OBSY * scaleY;

	//obstacle 5
	i++;
	scaleX = 1.75;
	scaleY = 1;
	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(1400, 1000);
	modelMatrix *= transform2D::Scale((scaleX), (scaleY));
	RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
	obstacles[i].posX = 1400;
	obstacles[i].posY = 1000;
	obstacles[i].scaleX = scaleX;
	obstacles[i].scaleY = scaleY;
	obstacles[i].sizeX = OBSX * scaleX;
	obstacles[i].sizeY = OBSY * scaleY;

	//obstacle 6

	i++;
	scaleX = 0.5;
	scaleY = 1.5;

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(1400, 700);
	modelMatrix *= transform2D::Scale((scaleX), (scaleY));
	RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

	obstacles[i].posX = 1400;
	obstacles[i].posY = 700;
	obstacles[i].scaleX = scaleX;
	obstacles[i].scaleY = scaleY;
	obstacles[i].sizeX = OBSX * scaleX;
	obstacles[i].sizeY = OBSY * scaleY;
}

void Tema1::RenderPlayer(GLfloat deltaTimeSeconds)
{
	player.posX = translateX;
	player.posY = translateY;
	player.scaleX = 1;
	player.scaleY = 1;
	player.sizeX = 75;
	player.sizeY = 75;

	modelMatrix = glm::mat3(1);
	modelMatrix = playerMatrix * transform2D::Translate(player.sizeX - 10, player.sizeY - 70);
	RenderMesh2D(meshes["eye"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix = playerMatrix * transform2D::Translate(player.sizeX - 10, player.sizeY - 30);
	RenderMesh2D(meshes["eye"], shaders["VertexColor"], modelMatrix);


	playerMatrix = glm::mat3(1);
	playerMatrix = visMatrix * transform2D::Translate(translateX, translateY);
	playerMatrix *= transform2D::Translate(BODY / 2.f, BODY / 2.f);
	playerMatrix *= transform2D::Rotate(player.angle);
	playerMatrix *= transform2D::Translate(-BODY / 2.f, -BODY / 2.f);

	RenderMesh2D(meshes["body"], shaders["VertexColor"], playerMatrix);
}

void Tema1::RenderEnemies(GLfloat deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();

	CreateEnemy(deltaTimeSeconds);

	for (int i = 0; i < enemiesNo; i++) {
		if (enemies[i].render) {
			float dirX = player.posX - enemies[i].posX;
			float dirY = player.posY - enemies[i].posY;

			float hyp = sqrt(dirX * dirX + dirY * dirY);
			dirX /= hyp;
			dirY /= hyp;

			enemies[i].posX += dirX * enemies[i].speed * deltaTimeSeconds;
			enemies[i].posY += dirY * enemies[i].speed * deltaTimeSeconds;
			enemies[i].angle = atan2((player.posX - resolution.x / 2), (player.posY - resolution.y / 2)) + 3 * 3.14 / 2;

			modelMatrix = glm::mat3(1);
			modelMatrix = visMatrix * transform2D::Translate(enemies[i].posX + ENEMYBODY / 2,
				enemies[i].posY + ENEMYBODY / 2);
			RenderMesh2D(meshes["enemy"], shaders["VertexColor"], modelMatrix);

			enemyMatrix = glm::mat3(1);
			enemyMatrix = visMatrix * transform2D::Translate(enemies[i].posX,
				enemies[i].posY);
			enemyMatrix *= transform2D::Translate(ENEMYBODY / 2.f, ENEMYBODY / 2.f);
			enemyMatrix *= transform2D::Rotate(enemies[i].angle);
			enemyMatrix *= transform2D::Translate(-ENEMYBODY / 2.f, -ENEMYBODY / 2.f);
			RenderMesh2D(meshes["enemyBody"], shaders["VertexColor"], enemyMatrix);

			if (CheckCollisionP(player, enemies[i])) {
				enemies[i].render = false;
				healthBar.scaleX -= 0.1;
				healthBar.sizeX *= healthBar.scaleX;
				if (healthBar.scaleX < 0.1) {
					cout << "Game Over!" << endl;
					cout << "Final Score: " << score << endl;
					exit(0);
				}
			}

			if (((map.posY >= enemies[i].posY) || (enemies[i].posY >= (map.posY + map.sizeY))
				|| (map.posX >= enemies[i].posX)
				|| (enemies[i].posX >= (map.posX + map.sizeX)))) {
				enemies[i].render = false;
			}
		}
	}
}

void Tema1::CreateEnemy(GLfloat deltaTimeSeconds) {
	glm::ivec2 resolution = window->GetResolution();

	if (timePassedEnemies > 3) {
		int posX;
		int posY;
		do {
			posX = rand() % 2000;
			posY = rand() % 1000;
		} while ((abs(player.posX - posX) >= PLAYERSAFESPACE) && ((abs(player.posY - posY) >= PLAYERSAFESPACE)));

		int speed = 100 + rand() % 350;

		Entity enm;
		enm.angle = atan2((player.posX - resolution.x / 2), (player.posY - resolution.y / 2)) + 3 * 3.14 / 2;
		enm.posX = posX;
		enm.posY = posY;
		enm.scaleX = 1;
		enm.scaleY = 1;
		enm.sizeX = ENEMYBODY;
		enm.sizeY = ENEMYBODY;
		enm.render = true;
		enm.speed = speed;

		enemies.push_back(enm);

		enemiesNo++;
		timePassedEnemies = 0;
	}
}

void Tema1::RenderProjectile(GLfloat deltaTimeSeconds) {
	for (int i = 0; i < projectilesNo; i++) {
		if (projectiles[i].render) {

			projectileMatrix = glm::mat3(1);
			projectileMatrix = visMatrix * transform2D::Translate(projectiles[i].posX, projectiles[i].posY);
			projectileMatrix *= transform2D::Rotate(projectiles[i].angle);

			RenderMesh2D(meshes["projectile"], shaders["VertexColor"], projectileMatrix);
			projectiles[i].posX += PRSPEED * deltaTimeSeconds * cos(projectiles[i].angle);
			projectiles[i].posY += PRSPEED * deltaTimeSeconds * sin(projectiles[i].angle);
			projectiles[i].distance += PRSPEED * deltaTimeSeconds;

			if (projectiles[i].distance > MAXDISTANCE) {
				projectiles[i].render = false;
			}

			if (((map.posY >= (projectiles[i].posY + PRSPEED * deltaTimeSeconds * sin(projectiles[i].angle)))
				|| ((projectiles[i].posY + PRSPEED * deltaTimeSeconds * sin(projectiles[i].angle)) >= (map.posY + map.sizeY))
				|| ((map.posX >= (projectiles[i].posX + PRSPEED * deltaTimeSeconds * cos(projectiles[i].angle)))
					|| ((projectiles[i].posX + PRSPEED * deltaTimeSeconds * sin(projectiles[i].angle)) >= (map.posX + map.sizeX))))) {
				projectiles[i].render = false;
			}

			for (int j = 0; j < OBSTACLESNO; j++) {
				if (CheckCollision(projectiles[i], projectiles[i].posX + PRSPEED * deltaTimeSeconds * cos(projectiles[i].angle),
					projectiles[i].posY + PRSPEED * deltaTimeSeconds * sin(projectiles[i].angle), obstacles[j])) {
					projectiles[i].render = false;
					break;
				}
			}

			for (int j = 0; j < enemiesNo; j++) {
				if (CheckCollisionP(projectiles[i], enemies[j]) && (enemies[j].render)) {
					projectiles[i].render = false;
					enemies[j].render = false;
					score += 100;
					cout << "New score: " << score << endl;
					break;
				}
			}
		}
	}
}

void Tema1::RenderHealthBar(GLfloat deltaTimeSeconds) {

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(healthBarEmpty.posX, healthBarEmpty.posY);
	RenderMesh2D(meshes["healthBarEmpty"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix = visMatrix * transform2D::Translate(healthBar.posX, healthBar.posY);
	modelMatrix *= transform2D::Scale(healthBar.scaleX, healthBar.scaleY);

	RenderMesh2D(meshes["healthBar"], shaders["VertexColor"], modelMatrix);

}

void Tema1::RenderHealthBonus(GLfloat deltaTimeSeconds) {
	CreateHealthBonus();

	for (int i = 0; i < healthBonusNo; i++) {
		if (healthBonuses[i].render) {

			healthMatrix = glm::mat3(1);
			healthMatrix = visMatrix * transform2D::Translate(healthBonuses[i].posX, healthBonuses[i].posY);
			RenderMesh2D(meshes["healthBonus1"], shaders["VertexColor"], healthMatrix);

			modelMatrix = glm::mat3(1);
			modelMatrix = visMatrix * transform2D::Translate(healthBonuses[i].posX + 7.5, healthBonuses[i].posY - 7.5);
			RenderMesh2D(meshes["healthBonus2"], shaders["VertexColor"], modelMatrix);

			if (CheckCollisionP(player, healthBonuses[i])) {
				healthBonuses[i].render = false;
				if (healthBar.scaleX < 1) {
					healthBar.scaleX += 0.1;
					healthBar.sizeX *= healthBar.scaleX;
				}
			}
		}
	}
}

void m1::Tema1::CreateHealthBonus() {
	if (timePassedBonus > 10) {
		Entity bonus;

		int posX = rand() % 2500;
		int posY = rand() % 1500;

		bonus.posX = posX;
		bonus.posY = posY;
		bonus.sizeX = 30;
		bonus.sizeY = 30;
		bonus.render = true;

		healthBonuses.push_back(bonus);

		healthBonusNo++;
		timePassedBonus = 0;
	}

}

void Tema1::RenderScoreBonus(GLfloat deltaTimeSeconds) {
	CreateScoreBonus();

	for (int i = 0; i < scoreBonusNo; i++) {

		if (scoreBonuses[i].render) {

			healthMatrix = glm::mat3(1);
			healthMatrix = visMatrix * transform2D::Translate(scoreBonuses[i].posX, scoreBonuses[i].posY);
			RenderMesh2D(meshes["scoreBonus1"], shaders["VertexColor"], healthMatrix);

			modelMatrix = glm::mat3(1);
			modelMatrix = visMatrix * transform2D::Translate(scoreBonuses[i].posX + 7.5, scoreBonuses[i].posY - 7.5);
			RenderMesh2D(meshes["scoreBonus2"], shaders["VertexColor"], modelMatrix);

			if (CheckCollisionP(player, scoreBonuses[i])) {
				scoreBonuses[i].render = false;
				score += 300;
				cout << "New score: " << score << endl;
			}
		}
	}
}

void m1::Tema1::CreateScoreBonus() {

	if (timePassedScore > 15) {
		Entity bonusScore;

		int posX = rand() % 2500;
		int posY = rand() % 1500;

		bonusScore.posX = posX;
		bonusScore.posY = posY;
		bonusScore.sizeX = 30;
		bonusScore.sizeY = 30;
		bonusScore.render = true;

		scoreBonuses.push_back(bonusScore);

		scoreBonusNo++;
		timePassedScore = 0;
	}
}

GLboolean m1::Tema1::CheckCollision(const Entity& one, int positionX, int positionY, const Entity& two)
{
	if (positionX < two.posX + two.sizeX &&
		positionX + one.sizeX > two.posX &&
		positionY < two.posY + two.sizeY &&
		one.sizeY + positionY > two.posY) {
		return true;
	}
	return false;
}

GLboolean m1::Tema1::CheckCollisionP(const Entity& one, const Entity& two)
{
	if (one.posX < two.posX + two.sizeX &&
		one.posX + one.sizeX > two.posX &&
		one.posY < two.posY + two.sizeY &&
		one.sizeY + one.posY > two.posY) {

		return true;
	}
	return false;
}

glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

