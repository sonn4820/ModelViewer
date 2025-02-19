#pragma once
#include "Game/GameCommon.hpp"

class Entity;
class Player;
class Prop;
class Model;

enum Scenes
{
	EmissiveCubes,
	TexutredProps,
	TutorialBox,
	HadrianTank,
	NUMSCENE,
	LoadedModel
};
class Game
{
public:
	Game();
	~Game();

	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void Shutdown();

	Camera m_screenCamera;
	bool m_rotateModel = false;
	bool m_debugTBN = false;
	bool m_blackScreen = false;

public:
	LightingDebug m_lightDebug;

	Player* m_player = nullptr;
	Clock* m_clock = nullptr;
	std::vector<Entity*> m_entityList;

	Shader* m_shader = nullptr;
	Shader* m_diffuse = nullptr;
	Texture* m_diffuseTexture = nullptr;
	Texture* m_glossTexture = nullptr;
	Texture* m_normalTexture = nullptr;
	EulerAngles m_sunOrientation = EulerAngles(120, 45, 0);
	float m_sunIntensity = 1.f;
	float m_ambIntensity = 0.f;

	Model* m_testModel = nullptr;

	Scenes m_currentScene = EmissiveCubes;

	void NextScene();
	void PreviousScene();
	void LoadScenes();

	Model* cubeMiddle = nullptr;
	Model* cube1 = nullptr;
	Model* cube2 = nullptr;
	Model* cube3 = nullptr;
	Model* cube4 = nullptr;

	Prop* grassCube = nullptr;
	Prop* grassSphere = nullptr;
	Prop* brickCube = nullptr;
	Prop* brickSphere = nullptr;

	Model* tutBox = nullptr;
	Model* hadrian = nullptr;
private:;

	void DebugOptions();
	void HandleInput(float deltaSeconds);

	// RENDER
	void RenderScreenWorld() const;
	void DrawGrid() const;
};