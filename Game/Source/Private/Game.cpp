#include <Game.h>

Game::Game()
{
	int WIDTH = 1280;
	int HEIGHT = 720;

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	m_Engine = new Engine(WIDTH, HEIGHT, "Game", renderTarget);

	m_Engine->Init();

	// Define entities and systems here

	flecs::entity player = m_Engine->m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.set<PlayerMovementComponent>({ 5.0f })
		.set<PlayerYawComponent>({ 1.0f });

	flecs::entity camera = m_Engine->m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({ glm::vec3(0.0f, 0.0f, 0.0f) })
		.add<CameraComponent>()
		.set<PlayerPitchComponent>({ 1.0f });

	//RGFW_getTime();

	//RGFW_getMonitors();

	m_Engine->InitSystems();
	m_Engine->MainLoop();
	m_Engine->Terminate();
	delete renderTarget;
}

Game::~Game()
{
	delete m_Engine;
}