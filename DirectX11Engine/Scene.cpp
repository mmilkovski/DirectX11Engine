#include "Scene.h"
#include "Graphics.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace VectorMath;

Scene::Scene()
{}

//Scene::~Scene()
//{}

bool Scene::Initialize()
{
	/////// INIT CAMERA ////////
	_Camera.reset(new Actor);
	_Camera->InitializeMovement(false);
	_Camera->GetMovementComponent()->SetPosition(XMFLOAT3(0.f, 5.f, -20.f));
	_Camera->SetPosition(XMFLOAT3(0.f, 100.f, -200.f));
	_Camera->SetOrientation(XMFLOAT3(30, 0, 0));

	/////// INIT ACTORS ////////
	InitializeActors();

	/////// INIT LIGHTS ////////
	InitializeLights(_Actors);

	return true;
}

void Scene::InitializeActors()
{
	vector < std::pair<string, XMFLOAT3>> actorPairs =
	{
		std::make_pair("Columns", XMFLOAT3(0.f, -2.5f, 5.0f)),
		std::make_pair("Rock", XMFLOAT3(0.0f, 0.0f, 0.0f)),
		std::make_pair("Platform", XMFLOAT3(0.0f, 0.0f, 0.0f)),
		std::make_pair("Moai", XMFLOAT3(0.0f, 0.0f, 20.0f)),
		std::make_pair("Fountain", XMFLOAT3(0.0f, 0.0f, -5.0f)),
		std::make_pair("Water", XMFLOAT3(0.0f, 0.0f, -5.0f)),
		std::make_pair("GlassColumns", XMFLOAT3(0.f, -2.5f, -20.0f))
	};

	// Scene scale
	XMFLOAT3 sceneScale(.5, .5, .5);

	// Initialize contiguously allocated pool of actors for faster performance
	
	// Then set each address of these to the pointers in the map
	// TODO: if grow it or shrink it, fix up all the pointers

	_actorPool.resize(actorPairs.size());

	// Instantiate map of actors and update their names if duplicates
	int duplicateIdx = 0;
	string DuplicateBaseName = actorPairs[0].first;

	for (int i = 0; i < actorPairs.size(); ++i)
	{
		if (i > 0)
		{
			// Decide whether to reset duplicate reference name
			if (actorPairs[i - 1].first == actorPairs[i].first)
			{
				duplicateIdx = 0;
				DuplicateBaseName = actorPairs[i].first;
			}

			// Check each name against the latest duplicate name and number it if it matches
			if (actorPairs[i].first == DuplicateBaseName)
			{
				if (actorPairs[i - 1].first == DuplicateBaseName)
				{
					actorPairs[i - 1].first += to_string(duplicateIdx);
				}

				duplicateIdx++;
				actorPairs[i].first += to_string(duplicateIdx);
			}
		}

		// Instantiate each actor with the updated name
		//unique_ptr<Actor> pActor = std::make_unique<Actor>(actorPairs[i].first);
		//pActor->InitializeMovement(VISIBLE);
		//pActor->SetPosition(actorPairs[i].second);
		//_Actors.emplace(pActor->Name, std::move(pActor));
		//_Actors[actorPairs[i].first]->SetScale(sceneScale);

		// @OPTIMIZE - actor pool version
		//_actorPool[i] = Actor(actorPairs[i].first);
		//_actorPool[i].InitializeMovement(VISIBLE);
		//_actorPool[i].SetPosition(actorPairs[i].second);
		//_actorPool[i].SetScale(sceneScale);
		//unique_ptr<Actor> pActor(&_actorPool[i]);
		//_Actors.emplace(_actorPool[i].Name, std::move(pActor));

		// @OPTIMIZE 2 - actor pool version
		_actorPool[i] = Actor(actorPairs[i].first);
		_actorPool[i].InitializeMovement(VISIBLE);
		_actorPool[i].SetPosition(actorPairs[i].second);
		_actorPool[i].SetScale(sceneScale);
		_Actors.emplace(_actorPool[i].Name, &_actorPool[i]);
	}

	// Custom tweaks on actors
	_Actors["Columns"]->SetScale(XMFLOAT3(1.f, 1.f, 1.f));
	_Actors["GlassColumns"]->SetScale(XMFLOAT3(.7f, 1.f, .7f));
	_Actors["GlassColumns"]->SetOrientation(XMFLOAT3(0.f, 180.f, 0.f));
	//_Actors["Platform"]->SetScale(XMFLOAT3(.7f,.7f,.7f));
	//_Actors["Rock"]->SetScale(XMFLOAT3(.7f, .7f, .7f));
	//_Actors["Platform"]->SetPosition(XMFLOAT3(0.f, 1.0f, 0.f));
}

void Scene::InitializeLights(map<string, /*unique_ptr<Actor>*/Actor*>& actors)
{
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		//_LightActors.push_back(unique_ptr<Actor>());
		//_LightActors[i].reset(new Actor("Light" + to_string(i + 1)));
		//_LightActors[i]->InitializeMovement(true);
		//_LightActors[i]->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));

		_LightActors.push_back(unique_ptr<Actor>());
		_LightActors[i].reset(new Actor("Light" + to_string(i + 1)));
		_LightActors[i]->InitializeMovement(true);
		_LightActors[i]->SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));
	}

	//_LightActors[0]->SetPosition(XMFLOAT3(-20, 15.f, 0.f));
	_LightActors[0]->SetPosition(XMFLOAT3(-20, _lightHeight, -20.f));
	//_LightActors[0]->SetLookAt((actors["Rock"]->GetPosition() - _LightActors[0]->GetPosition()));
	_LightActors[0]->SetLookAt((actors["Moai"]->GetPosition() - _LightActors[0]->GetPosition()));
	//_LightActors[0]->SetLookAt(XMFLOAT3(0, 0, 0));
	//_LightActors[0]->SetLookAt(-1*XMFLOAT3(10.f, 10.0f, 0.f));
	//_LightActors[0]->SetPosition(XMFLOAT3(20.f, 25.0f, 0.f));
	//_LightActors[0]->SetLookAt((-1.f*_LightActors[0]->GetPosition()));
	//_LightActors[0]->SetLookAt(actors["Rock"]->GetPosition());

	//_LightActors[1]->SetPosition(XMFLOAT3(0, 15.0f, 0.f));
	_LightActors[1]->SetPosition(XMFLOAT3(0, _lightHeight*2.f, -40.f));
	//_LightActors[1]->SetPosition(XMFLOAT3(0, _lightHeight*2.f, 40.f));
	//_LightActors[1]->SetLookAt((actors["Moai"]->GetPosition() - _LightActors[1]->GetPosition()));
	//_LightActors[1]->SetLookAt((actors["Columns"]->GetPosition() - _LightActors[1]->GetPosition() + XMFLOAT3(0, 50, 0)));
	_LightActors[1]->SetLookAt((actors["Columns"]->GetPosition() - _LightActors[1]->GetPosition()));
	//_LightActors[1]->SetLookAt(XMFLOAT3(0, 0, 0));
	//_LightActors[1]->SetLookAt(-1*XMFLOAT3(-10.f, 10.0f, 0.f));
	//_LightActors[1]->SetPosition(XMFLOAT3(-10.f, 25.0f, 0.f));
	//_LightActors[1]->SetLookAt((-1.f*_LightActors[1]->GetPosition()));
	//_LightActors[1]->SetLookAt(actors["Sphere1"]->GetPosition());

	//_LightActors[2]->SetPosition(XMFLOAT3(0.f, 15.0f, -20));
	_LightActors[2]->SetPosition(XMFLOAT3(20.f, _lightHeight, -20));
	_LightActors[2]->SetLookAt((actors["Moai"]->GetPosition() - _LightActors[2]->GetPosition()));
	//_LightActors[2]->SetLookAt((actors["Fountain"]->GetPosition() - _LightActors[2]->GetPosition()));
	//_LightActors[2]->SetLookAt(XMFLOAT3(0,0,0));
	//_LightActors[2]->SetLookAt(-1*XMFLOAT3(0.f, 10.0f, 10.f));
	//_LightActors[2]->SetPosition(XMFLOAT3(0.f, 25.0f,20.f));
	//_LightActors[2]->SetLookAt((-1.f*_LightActors[2]->GetPosition()));
	//_LightActors[2]->SetLookAt(actors["Columns"]->GetPosition());
}

void Scene::Tick(float deltaTime, Input* pInput)
{
	ProcessInput(deltaTime, pInput);

	UpdateActors(deltaTime);

	// Update Lights
	//// light 1
	//if (_lightPositionX > 1.0f)
	//{
	//	_lightPositionX = -1.f;
	//}
	//if (_lightPositionZ > 1.0f)
	//{
	//	_lightPositionZ = -1.f;
	//}
	//_lightPositionX += _lightPosIncrement;
	//_lightPositionZ += _lightPosIncrement;
	////_LightActors[0]->SetPosition(XMFLOAT3(cos(_lightPositionX)*20.f, 20.0f, sin(_lightPositionZ)*20.f));
	//_LightActors[0]->SetPosition(XMFLOAT3(cos(_lightPositionX)*20.f, _lightHeight, sin(_lightPositionZ)*20.f));

	//// light 2
	//if (_lightPositionX2 > 20.0f || _lightPositionX2 < -20.0f)
	//{
	//	_increment2 *= -1;
	//}
	//_lightPositionX2 += _increment2;

	////_LightActors[1]->SetPosition(XMFLOAT3(_lightPositionX2, 20.0f, sin(_lightPositionZ)*-20.f));
	//_LightActors[1]->SetPosition(XMFLOAT3(_lightPositionX2, _lightHeight, sin(_lightPositionZ)*-20.f));

	//// light 3
	//if (_lightPositionX3 > 20.0f || _lightPositionX3 < -20.0f)
	//{
	//	_increment3 *= -1;
	//}
	//_lightPositionX3 += _increment3;

	//_LightActors[2]->SetPosition(XMFLOAT3(_lightPositionX3, _lightHeight, sin(_lightPositionZ)*20.f));
}

void Scene::ProcessInput(float deltaTime, Input* pInput)
{
	_Camera->GetMovementComponent()->SetFrameTime(deltaTime);

	// working old method
	//int mouseX = 0.f; int mouseY = 0.f;
	//pInput->GetMouseLocation(mouseX, mouseY);
	//_Camera->SetOrientation(XMFLOAT3((float)mouseX, (float)mouseY, 0.f));

	// Bitshifting method
	uint32_t x = pInput->GetMouseXY() >> 32;
	uint32_t y = pInput->GetMouseXY() & 0xffffffff;
	_Camera->GetMovementComponent()->SetOrientation(XMFLOAT3(
		static_cast<float>(*reinterpret_cast<int *>(&(y))), 
		static_cast<float>(*reinterpret_cast<int *>(&(x))), 
		0.f));

	_Camera->GetMovementComponent()->MoveForward(pInput->IsKeyDown(DIK_W));
	_Camera->GetMovementComponent()->MoveBack(pInput->IsKeyDown(DIK_S));
	_Camera->GetMovementComponent()->MoveLeft(pInput->IsKeyDown(DIK_A));
	_Camera->GetMovementComponent()->MoveRight(pInput->IsKeyDown(DIK_D));

	////////// MOVE LIGHTS (@TEST) ////////////
	//for (int i = 0; i < NUM_LIGHTS; ++i)
	//{
	//	_LightActors[i]->GetMovementComponent()->SetFrameTime(deltaTime);
	//}
	//_LightActors[0]->GetMovementComponent()->MoveForward(pInput->IsKeyDown(DIK_UP));
	//_LightActors[0]->GetMovementComponent()->MoveBack(pInput->IsKeyDown(DIK_DOWN));
	//_LightActors[0]->GetMovementComponent()->MoveLeft(pInput->IsKeyDown(DIK_LEFT));
	//_LightActors[0]->GetMovementComponent()->MoveRight(pInput->IsKeyDown(DIK_RIGHT));
}

void Scene::UpdateActors(float deltaTime)
{}