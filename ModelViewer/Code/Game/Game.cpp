#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/Entity.hpp"
#include "Game/Model.hpp"
#include "Engine/Core/ObjLoader.hpp"
#include "Engine/Window/Window.hpp"

Game::Game()
{

}
//..............................
Game::~Game()
{

}
//..............................
void Game::Startup()
{
	m_player = new Player(this);
	m_player->m_position = Vec3(4.f, 0.f, 5.f);
	m_player->m_orientationDegrees = EulerAngles(270.f, 00.f, 0.f);

	m_screenCamera.SetOrthographicView(Vec2(0, 0), Vec2(g_gameConfigBlackboard.GetValue("screenSizeX", 1600.f), g_gameConfigBlackboard.GetValue("screenSizeY", 800.f)));
	m_clock = new Clock(*Clock::s_theSystemClock);

	m_shader = g_theRenderer->CreateShader("Data/Shaders/EverythingShader", VertexType::Vertex_PCUTBN);
	m_diffuse = g_theRenderer->CreateShader("Data/Shaders/Diffuse", VertexType::Vertex_PCUTBN);
	m_diffuseTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Cube_Textured_Diffuse.png");
	m_glossTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Cube_Textured_SpecGlossEmit.png");
	m_normalTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Cube_Textured_Normal.png");

	LoadScenes();

}
//..............................
void Game::Shutdown()
{
	delete m_testModel;
	m_testModel = nullptr;

	delete cubeMiddle;
	cubeMiddle = nullptr;

	delete cube1;
	cube1 = nullptr;

	delete cube2;
	cube2 = nullptr;

	delete cube3;
	cube3 = nullptr;

	delete cube4;
	cube4 = nullptr;

	delete grassCube;
	grassCube = nullptr;

	delete grassSphere;
	grassSphere = nullptr;

	delete brickCube;
	brickCube = nullptr;

	delete brickSphere;
	brickSphere = nullptr;

	delete tutBox;
	tutBox = nullptr;

	delete hadrian;
	hadrian = nullptr;

	m_entityList.clear();
}

void Game::NextScene()
{
	Scenes NextScene = (Scenes)(m_currentScene + 1);
	if (NextScene >= NUMSCENE)
	{
		m_currentScene = (Scenes)0;
	}
	else
	{
		m_currentScene = NextScene;
	}
}

void Game::PreviousScene()
{
	Scenes PreScene = (Scenes)(m_currentScene - 1);
	if (PreScene <= -1)
	{
		m_currentScene = (Scenes)(NUMSCENE - 1);
	}
	else
	{
		m_currentScene = PreScene;
	}
}

void Game::LoadScenes()
{
	m_entityList.clear();

	m_testModel = new Model(this);
	m_entityList.push_back(m_testModel);

	cubeMiddle = new Model(this);
	cubeMiddle->Load("Data/Models/Cube_Textured.xml");
	cubeMiddle->m_position = Vec3(0, 0, 0.5);
	m_entityList.push_back(cubeMiddle);

	cube1 = new Model(this);
	cube1->Load("Data/Models/Cube_Emissive.xml");
	cube1->m_position = Vec3(-1, -1, 0.5);
	m_entityList.push_back(cube1);

	cube2 = new Model(this);
	cube2->Load("Data/Models/Cube_Emissive.xml", Mat44(), Rgba8(255, 0, 0));
	cube2->m_position = Vec3(1, -1, 0.5);
	m_entityList.push_back(cube2);

	cube3 = new Model(this);
	cube3->Load("Data/Models/Cube_Emissive.xml", Mat44(), Rgba8(0, 255, 0));
	cube3->m_position = Vec3(1, 1, 0.5);
	m_entityList.push_back(cube3);

	cube4 = new Model(this);
	cube4->Load("Data/Models/Cube_Emissive.xml", Mat44(), Rgba8(0, 0, 255));
	cube4->m_position = Vec3(-1, 1, 0.5);
	m_entityList.push_back(cube4);

	tutBox = new Model(this);
	tutBox->Load("Data/Models/Tutorial_Box.xml");
	m_entityList.push_back(tutBox);

	hadrian = new Model(this);
	hadrian->Load("Data/Models/Hadrian.xml");
	m_entityList.push_back(hadrian);

	grassCube = new Prop(this);
	grassCube->CreateCube("Data/Materials/Grass.xml");
	grassCube->m_position = Vec3(-2, -2, 0);
	m_entityList.push_back(grassCube);

	grassSphere = new Prop(this);
	grassSphere->CreateSphere("Data/Materials/Grass.xml");
	grassSphere->m_position = Vec3(2, -2, 0);
	m_entityList.push_back(grassSphere);

	brickCube = new Prop(this);
	brickCube->CreateCube("Data/Materials/Brick.xml");
	brickCube->m_position = Vec3(2, 2, 0);
	m_entityList.push_back(brickCube);

	brickSphere = new Prop(this);
	brickSphere->CreateSphere("Data/Materials/Brick.xml");
	brickSphere->m_position = Vec3(-2, 2, 0);
	m_entityList.push_back(brickSphere);
}

//----------------------------------------------------------------------------------------------------------------------------------------
// UPDATE

void Game::Update(float deltaSeconds)
{
	HandleInput(deltaSeconds);

	m_player->Update(deltaSeconds);

	m_sunIntensity = Clamp(m_sunIntensity, 0.f, 1.f);
	m_ambIntensity = 1.f - m_sunIntensity;

	if (m_rotateModel)
	{
		for (size_t i = 0; i < m_entityList.size(); i++)
		{
			m_entityList[i]->m_orientationDegrees.m_yawDegrees += 45.f * deltaSeconds;
		}
	}

	DebugOptions();
}

//----------------------------------------------------------------------------------------------------------------------------------------
// RENDER 

void Game::Render() const
{
	g_theRenderer->ClearScreen(m_blackScreen ? Rgba8(0, 0, 0, 255) : Rgba8(25, 25, 25, 255));
	g_theRenderer->BeginCamera(*m_player->GetCamera());
	g_theRenderer->SetDepthStencilMode(DepthMode::ENABLED);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetLightConstants(m_sunOrientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D(), m_sunIntensity, m_ambIntensity, m_player->GetCamera()->m_position, 0.f, 1.f, 1.f, 1.f, m_lightDebug);


	if (m_currentScene == LoadedModel && m_testModel)
	{
		m_testModel->Render();
	}

	if (m_currentScene == EmissiveCubes)
	{
		cubeMiddle->Render();
		cube1->Render();
		cube2->Render();
		cube3->Render();
		cube4->Render();
	}
	if (m_currentScene == TexutredProps)
	{
		grassCube->Render();
		grassSphere->Render();
		brickCube->Render();
		brickSphere->Render();
	}
	if (m_currentScene == TutorialBox)
	{
		tutBox->Render();
	}
	if (m_currentScene == HadrianTank)
	{
		hadrian->Render();
	}

	if (!m_blackScreen)
	{
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->BindShader(nullptr);
		DrawGrid();
	}

	g_theRenderer->RenderEmissive();
	g_theRenderer->EndCamera(*m_player->GetCamera());

	RenderScreenWorld();
	DebugRenderWorld(*m_player->GetCamera());
	DebugRenderScreen(m_screenCamera);
}
//..............................

void Game::RenderScreenWorld() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::DrawGrid() const
{
	// Drawing Grid

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);

	std::vector<Vertex_PCU> gridVertexes;

	float smallSize = 0.02f;
	float medSize = 0.03f;
	float largeSize = 0.06f;

	for (int x = -50; x <= 50; x++)
	{
		AABB3 cube;
		Rgba8 color;

		if (x % 5 == 0)
		{
			cube = AABB3(-medSize + x, -50.f, -medSize, medSize + x, 50.f, medSize);
			color = Rgba8(0, 180, 0);
		}
		else
		{
			cube = AABB3(-smallSize + x, -50.f, -smallSize, smallSize + x, 50.f, smallSize);
			color = Rgba8::COLOR_GRAY;
		}
		if (x == 0)
		{
			cube = AABB3(-largeSize + x, -50.f, -largeSize, largeSize + x, 50.f, largeSize);
			color = Rgba8::COLOR_GREEN;
		}
		AddVertsForAABB3D(gridVertexes, cube, color);
	}
	for (int y = -50; y <= 50; y++)
	{
		AABB3 cube;
		Rgba8 color;

		if (y % 5 == 0)
		{
			cube = AABB3(-50.f, -medSize + y, -medSize, 50.f, medSize + y, medSize);
			color = Rgba8(180, 0, 0);
		}
		else
		{
			cube = AABB3(-50.f, -smallSize + y, -smallSize, 50.f, smallSize + y, smallSize);
			color = Rgba8::COLOR_GRAY;
		}
		if (y == 0)
		{
			cube = AABB3(-50.f, -largeSize + y, -largeSize, 50.f, largeSize + y, largeSize);
			color = Rgba8::COLOR_RED;
		}
		AddVertsForAABB3D(gridVertexes, cube, color);
	}
	g_theRenderer->SetModelConstants();
	g_theRenderer->DrawVertexArray((int)gridVertexes.size(), gridVertexes.data());
}

void Game::DebugOptions()
{
	Vec3 start = Vec3(0, 0, 5);
	Vec3 end = start + m_sunOrientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D() * 1.5f;
	DebugAddWorldArrow(start, end, 0.075f, 0.f, Rgba8(200, 200, 0), Rgba8::COLOR_YELLOW, DebugRenderMode::XRAY);
	DebugAddWorldWireArrow(start, end, 0.075f, 0.f, Rgba8::COLOR_YELLOW, Rgba8::COLOR_YELLOW, DebugRenderMode::XRAY);

	DebugAddScreenText(Stringf("FPS: %.1f", 1.f / m_clock->GetDeltaSeconds()), Vec2(1420.f, 780.f), 15.f);
	DebugAddScreenText(Stringf("Sun Orientation (Arrows): (%.1f, %.1f, %.1f)", m_sunOrientation.m_yawDegrees, m_sunOrientation.m_pitchDegrees, m_sunOrientation.m_rollDegrees), Vec2(920.f, 760.f), 15.f);
	DebugAddScreenText(Stringf("Scene ([ / ]): %i", (int)m_currentScene), Vec2(1340.f, 740.f), 15.f);
	Vec3 sunDirection = m_sunOrientation.GetAsMatrix_IFwd_JLeft_KUp().GetIBasis3D();
	DebugAddScreenText(Stringf("Sun Direction: (%.1f, %.1f, %.1f)", sunDirection.x, sunDirection.y, sunDirection.z), Vec2(1100.f, 720.f), 15.f);
	DebugAddScreenText(Stringf("Sun Intensity (< / >): %.1f", m_sunIntensity), Vec2(1200.f, 700.f), 15.f);
	DebugAddScreenText(Stringf("Ambient Intensity (< / >): %.1f", m_ambIntensity), Vec2(1140.f, 680.f), 15.f);
	DebugAddScreenText(Stringf("Ambient [1]: %s", (m_lightDebug.RenderAmbient) ? "On" : "Off"), Vec2(1350.f, 660.f), 15.f);
	DebugAddScreenText(Stringf("Diffuse [2]: %s", (m_lightDebug.RenderDiffuse) ? "On" : "Off"), Vec2(1350.f, 640.f), 15.f);
	DebugAddScreenText(Stringf("Specular [3]: %s", (m_lightDebug.RenderSpecular) ? "On" : "Off"), Vec2(1335.f, 620.f), 15.f);
	DebugAddScreenText(Stringf("Emissive [4]: %s", (m_lightDebug.RenderEmissive) ? "On" : "Off"), Vec2(1335.f, 600.f), 15.f);
	DebugAddScreenText(Stringf("Diffuse Map [5]: %s", (m_lightDebug.UseDiffuseMap) ? "On" : "Off"), Vec2(1290.f, 580.f), 15.f);
	DebugAddScreenText(Stringf("Normal Map [6]: %s", (m_lightDebug.UseNormalMap) ? "On" : "Off"), Vec2(1305.f, 560.f), 15.f);
	DebugAddScreenText(Stringf("Specular Map [7]: %s", (m_lightDebug.UseSpecularMap) ? "On" : "Off"), Vec2(1275.f, 540.f), 15.f);
	DebugAddScreenText(Stringf("Glossiness Map [8]: %s", (m_lightDebug.UseGlossinessMap) ? "On" : "Off"), Vec2(1245.f, 520.f), 15.f);
	DebugAddScreenText(Stringf("Emissive Map [9]: %s", (m_lightDebug.UseEmissiveMap) ? "On" : "Off"), Vec2(1275.f, 500.f), 15.f);
}

//----------------------------------------------------------------------------------------------------------------------------------------
// HANDLE INPUT

void Game::HandleInput(float deltaSeconds)
{
	if (g_theInput->WasKeyJustPressed('L'))
	{
		g_theInput->SetCursorMode(false, false);
		g_theInput->ResetCursorMode();
		std::string out_fileName;
		if (Window::GetMainWindowInstance()->GetFileName(out_fileName))
		{
			m_testModel->Load(out_fileName);
		}
		g_theInput->HandleKeyReleased('L');
		m_currentScene = LoadedModel;
	}
	else
	{
		g_theInput->SetCursorMode(true, true);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFTBRACKET))
	{
		PreviousScene();
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_RIGHTBRACKET))
	{
		NextScene();
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_COMMA))
	{
		m_sunIntensity -= 0.1f;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_PERIOD))
	{
		m_sunIntensity += 0.1f;
	}

	if (g_theInput->IsKeyDown(KEYCODE_UPARROW))
	{
		m_sunOrientation.m_pitchDegrees -= 45.f * deltaSeconds;
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW))
	{
		m_sunOrientation.m_pitchDegrees += 45.f * deltaSeconds;
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW))
	{
		m_sunOrientation.m_yawDegrees += 45.f * deltaSeconds;
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW))
	{
		m_sunOrientation.m_yawDegrees -= 45.f * deltaSeconds;
	}
	if (g_theInput->IsKeyDown(44))
	{
		m_sunIntensity -= 0.1f * deltaSeconds;
	}
	if (g_theInput->IsKeyDown(46))
	{
		m_sunIntensity += 0.1f * deltaSeconds;
	}
	if (g_theInput->WasKeyJustPressed(49))
	{
		m_lightDebug.RenderAmbient ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(50))
	{
		m_lightDebug.RenderDiffuse ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(51))
	{
		m_lightDebug.RenderSpecular ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(52))
	{
		m_lightDebug.RenderEmissive ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(53))
	{
		m_lightDebug.UseDiffuseMap ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(54))
	{
		m_lightDebug.UseNormalMap ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(55))
	{
		m_lightDebug.UseSpecularMap ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(56))
	{
		m_lightDebug.UseGlossinessMap ^= 1;
	}
	if (g_theInput->WasKeyJustPressed(57))
	{
		m_lightDebug.UseEmissiveMap ^= 1;
	}
	if (g_theInput->WasKeyJustPressed('R'))
	{
		m_rotateModel ^= 1;
	}
	if (g_theInput->WasKeyJustPressed('N'))
	{
		m_debugTBN ^= 1;
	}
	if (g_theInput->WasKeyJustPressed('B'))
	{
		m_blackScreen ^= 1;
	}
}