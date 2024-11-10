#include <Engine.h>
#include <GameRenderTarget.h>

int main()
{
	int WIDTH = 1280;
	int HEIGHT = 720;

	GameRenderTarget* renderTarget = new GameRenderTarget(WIDTH, HEIGHT);
	Engine engine = Engine(WIDTH, HEIGHT, "Game", renderTarget);

	engine.Init();

	// Define entities and systems here

	flecs::entity player = engine.m_World.entity("Player")
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, 0.0f)})
		.set<PlayerMovementComponent>({5.0f})
		.set<PlayerYawComponent>({1.0f});

	flecs::entity camera = engine.m_World.entity("Camera")
		.child_of(player)
		.add<TransformComponent, Global>()
		.set<TransformComponent, Local>({glm::vec3(0.0f, 0.0f, 0.0f)})
		.add<CameraComponent>()
		.set<PlayerPitchComponent>({1.0f});
	
	engine.InitSystems();
	engine.MainLoop();
	engine.Terminate();
	delete renderTarget;
	return 0;
}