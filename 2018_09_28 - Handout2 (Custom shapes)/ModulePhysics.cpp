#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"

#include "Box2D/Box2D/Box2D.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));

	// big static circle as "ground" in the middle of the screen
	int x = SCREEN_WIDTH / 2;
	int y = SCREEN_HEIGHT / 1.5f;
	int diameter = SCREEN_WIDTH / 2;

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(diameter) * 0.5f;

	b2FixtureDef fixture;
	fixture.shape = &shape;
	b->CreateFixture(&fixture);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	return UPDATE_CONTINUE;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}
		}
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

PhysBody* ModulePhysics::CreateCircle(float sentRadius) {
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));
	b2Body* b = world->CreateBody(&body);

	float radius = PIXEL_TO_METERS(sentRadius);
	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(sentRadius);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	b->CreateFixture(&fixture);

	PhysBody* bodyToAdd = new PhysBody(b, sentRadius, sentRadius);
	return bodyToAdd;
}

PhysBody* ModulePhysics::CreateSquare(float width, float height) {
	// TODO 1: When pressing 2, create a box on the mouse position
	// To have the box behave normally, set fixture's density to 1.0f
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));

	b2PolygonShape shape;
	shape.SetAsBox(PIXEL_TO_METERS(width), PIXEL_TO_METERS(height));

	b2Body* b = world->CreateBody(&body);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	b->CreateFixture(&fixture);

	PhysBody* bodyToAdd = new PhysBody(b, width, height);//Change this values to the correct ones
	return bodyToAdd;
}

PhysBody* ModulePhysics::CreateRickHead() {
	// TODO 2: Create a chain shape using those vertices
	// remember to convert them from pixels to meters!
	int rick_head[66] = {
		43, 38,
		14, 36,
		29, 62,
		0, 76,
		28, 90,
		9, 102,
		31, 115,
		23, 124,
		39, 126,
		34, 136,
		44, 132,
		53, 142,
		66, 147,
		81, 148,
		94, 138,
		98, 126,
		104, 125,
		105, 120,
		100, 116,
		102, 104,
		110, 100,
		104, 94,
		109, 87,
		109, 76,
		115, 67,
		105, 61,
		111, 34,
		94, 39,
		93, 20,
		87, 4,
		84, 19,
		75, 32,
		41, 0
	};


	b2Vec2* vecArr = new b2Vec2[33];
	int j = 0;
	for (int i = 0; i < 33; i++) {
		vecArr[i].x = PIXEL_TO_METERS(rick_head[j++]);
		vecArr[i].y = PIXEL_TO_METERS(rick_head[j++]);
	}

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));
	b2Body* b = world->CreateBody(&body);

	b2ChainShape chain;
	chain.CreateLoop(vecArr, 33);

	b2FixtureDef fixture;
	fixture.shape = &chain;
	b->CreateFixture(&fixture);

	PhysBody* bodyToAdd = new PhysBody(b, 0, 0);//Change this values to the correct ones
	return bodyToAdd;
}

//Physbody methods
PhysBody::PhysBody(b2Body *body, int width, int height) :body(body), width(width), height(height) {};

p2Point<float> PhysBody::GetPosition() const{
	p2Point<float> pos;
	pos.x = METERS_TO_PIXELS(body->GetPosition().x) - width;
	pos.y = METERS_TO_PIXELS(body->GetPosition().y) - height;
	return pos;
}

float PhysBody::GetRotation() const {
	return RADTODEG * body->GetAngle();
}