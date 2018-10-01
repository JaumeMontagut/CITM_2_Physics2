#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModulePhysics.h"
#include <list>

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// TODO 4: Move all creation of bodies on 1,2,3 key press here in the scene
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->physics->physicsBody.push_back(App->physics->CreateCircle());
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->physics->CreateSquare();
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->physics->CreateRickHead();
	}
	// TODO 6: Draw all the circles using "circle" texture
	SDL_Texture* circleTx = App->textures->Load("pinball/wheel.png");
	for (std::list<PhysBody>::iterator iterator = App->physics->physicsBody.begin(); iterator != App->physics->physicsBody.end(); iterator++) {
		App->renderer->Blit(circleTx,iterator->GetPosition().x, iterator->GetPosition().y);
	}
	App->textures->Unload(circleTx);
	return UPDATE_CONTINUE;
}