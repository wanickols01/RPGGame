#include "stdafx.h"
#include "GameState.h"
#include "TileMap.h"
#include "Graph.h"
#include "GraphicsSettings.h"
#include "PauseMenu.h"
#include "Player.h"
#include "EnemySpawner.h"
#include "PlayerGui.h"
#include "EnemyLibrary.h"
#include "ItemLibrary.h"
#include "Movement.h"
#include "Attribute.h"
#include "ItemComp.h"
#include "Weapon.h"
#include "Item.h"
#include "TextTagSystem.h"
#include "PhysicsDevice.h"
#include "PlayerGuiTabs.h"
#include "physicsComponent.h"
#include "UserInput.h"



void GameState::initDefferredRender()
{
	renderTexture.create(GraphicsSettings->resolution.width,
		GraphicsSettings->resolution.height,
		GraphicsSettings->contextSettings
	);

	renderSprite.setTexture(renderTexture.getTexture());
	renderSprite.setTextureRect(
		sf::IntRect(
			0,
			0,
			GraphicsSettings->resolution.width, 
			GraphicsSettings->resolution.height
		));
}

//initializer functions
void GameState::initView()
{
	view.setSize(sf::Vector2f((float)GraphicsSettings->resolution.width, (float)GraphicsSettings->resolution.height));
	//view.setCenter(GraphicsSettings->resolution.width / 2.f, GraphicsSettings->resolution.height / 2.f);
	
}

void GameState::initKeybinds()
{
	std::ifstream ifs("Config/gamestate_keybinds.ini");

	if (ifs.is_open()) {

		std::string key = "";
		std::string key2 = "";

		while (ifs >> key >> key2) {
			keybinds[key] = supportedKeys->at(key2);
		}

	}

	ifs.close();


}

void GameState::initPhysics()
{
	pDevice = std::make_shared<PhysicsDevice>(0.f,0.f);
	if (!pDevice->initialize())
		std::cout << "ERROR::GAMESTATE::PHSYICS COULD NOT INITIALIZE \n";

	float borderSize = (float)stateData->mapSize * stateData->gridSize;
	

	//Creating Borders of World
	sf::Vector2f vTopLeft({ 0.f, 0.f });
	sf::Vector2f vTopRight({ borderSize, 0.f });
	sf::Vector2f vBottomLeft(0.f, borderSize);
	sf::Vector2f vBottomRight(borderSize, borderSize);

	pDevice->createEdge(vTopLeft, vTopRight);
	pDevice->createEdge(vTopRight, vBottomRight);
	pDevice->createEdge(vBottomRight, vBottomLeft);
	pDevice->createEdge(vBottomLeft, vTopLeft);

	
}

void GameState::initTextures()
{
	if (!textures["PLAYER_SHEET"].loadFromFile("Resources/Images/Sprites/Player/PLAYER_SHEET.png")) {
		throw "ERROR::GAME_STATE::COULD_NOT_LOAD_PLAYER_SHEET_TEXTURE \n";
	}
	if (!textures["FIRE_BULLET"].loadFromFile("Resources/Images/Sprites/Bullets/FIRE_BULLET.png")) {
		throw "ERROR::GAME_STATE::COULD_NOT_LOAD_FIRE_BULLET_TEXTURE \n";
	}
}

void GameState::initTextTags()
{
	tts = std::make_shared<TextTagSystem>("Fonts/PixellettersFull.ttf");
}

void GameState::initPauseMenu()
{
	sf::VideoMode& vm = GraphicsSettings->resolution;
	pmenu = std::make_unique<PauseMenu>(*window, font, gui::p2pX(13.f, vm), gui::p2pY(6.9f, vm), characterSize);

	pmenu->addButton("SAVE", gui::p2pY(34.f, vm), "Save"); //Key, Y, 
	pmenu->addButton("STATS", gui::p2pY(50.f, vm), "Stats");
	pmenu->addButton("QUIT", gui::p2pY(83.f, vm), "Quit"); //Key, Y, text
}

void GameState::initShaders()
{
	
	if(!main_shader.loadFromFile("vertex_shader.vert", "fragment_shader.frag"))
	{
		std::cout << "ERROR::GAMESTATE::COULD_NOT_LOAD_SHADERS" << "\n";
	}


}

void GameState::initEnemies()
{
	enemyLib = std::make_shared<EnemyLibrary>(tts, player);
	enemyLib->initStateData(*stateData);
}
void GameState::initTileMap()
{
	
	
	map = std::make_shared<TileMap>("Save/mapfile", enemyLib, pDevice);
	graph = map->getGraph(); // init graph in map using the savefile info, then send it back to gamestate so it can use it for enemies
	//map = std::make_unique<TileMap>(stateData->gridSize, mapSize, mapSize, "Resources/Images/Tiles/tilesheet3.png");
	//map->loadFromFile("Save/mapfile");
}

void GameState::initPlayers()
{
	itemLib = std::make_shared<ItemLibrary>();
	player = std::make_shared<Player>(gui::p2pX(11.4f, GraphicsSettings->resolution), gui::p2pY(20.3f, GraphicsSettings->resolution), textures["PLAYER_SHEET"],pDevice);
	player->initItems(itemLib);
	//testEnemy = std::make_shared<Enemy>(gui::p2pX(11.4f, GraphicsSettings->resolution), gui::p2pY(20.3f, GraphicsSettings->resolution), textures["PLAYER_SHEET"]);
}



void GameState::initPlayerGui()
{
	playerGui = std::make_shared<PlayerGui>(player, font, GraphicsSettings->resolution);
	playerGuiMenuOpen = false;
}


//constructors
GameState::GameState(std::shared_ptr<StateData> state_data)
	: State(state_data)
{
	initDefferredRender();
	initView();
	initKeybinds();
	initPhysics();
	initTextures();
	initTextTags();
	initPauseMenu();
	initShaders();
	initPlayers();
	initEnemies();
	initTileMap();
	initPlayerGui();
}


GameState::~GameState()
{
}

//Functions

void GameState::updateView()
{
	//Sets view to center the player
	view.setCenter(
		std::floor(player->getPosition().x + (static_cast<float>(mousePosWindow.x) - static_cast<float>(stateData->GraphicsSettings->resolution.width/2))/10),
		std::floor(player->getPosition().y + (static_cast<float>(mousePosWindow.y) - static_cast<float>(stateData->GraphicsSettings->resolution.height/2))/10));


	//Makes sure view doesn't show outside world bounds.
	//X
	if (map->getMaxSizeF().x >= view.getSize().x)
	{
		if (view.getCenter().x - view.getSize().x / 2.f < 0.f)
		{
			view.setCenter(0.f + view.getSize().x / 2.f, view.getCenter().y);
		}
		else if (view.getCenter().x + view.getSize().x / 2.f > map->getMaxSizeF().x)
		{
			view.setCenter(map->getMaxSizeF().x - view.getSize().x / 2.f, view.getCenter().y);
		}
	}
	//Y
	if (map->getMaxSizeF().y >= view.getSize().y)
	{
		if (view.getCenter().y - view.getSize().y / 2.f < 0.f)
		{
			view.setCenter(view.getCenter().x, 0.f + view.getSize().y / 2.f);
		}
		else if (view.getCenter().y + view.getSize().y / 2.f > map->getMaxSizeF().y)
		{
			view.setCenter(view.getCenter().x, map->getMaxSizeF().y - view.getSize().y / 2.f);
		}
	}
	
	viewGridPosition.x = static_cast<int>(view.getCenter().x) / static_cast<int>(stateData->gridSize);
	viewGridPosition.y = static_cast<int>(view.getCenter().y) / static_cast<int>(stateData->gridSize);
}

void GameState::updatePlayerInput(const float& dt)
{
	
	player->getComponent<UserInput>()->handleInput(keybinds, dt);

	//Debugging
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L) && getKeyTime()) {
		player->getComponent<Attribute>()->loseHealth(10);
		player->getComponent<Attribute>()->loseEnergy(10);
		player->getComponent<Attribute>()->loseExp(10);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O) && getKeyTime()) {
		player->getComponent<Attribute>()->addHealth(20);
		player->getComponent<Attribute>()->addEnergy(20);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::G) && getKeyTime()) {
		player->getComponent<Attribute>()->addExp(50);
		this->tts->addTextTag(EXPERIENCE_TAG, this->player->getPosition().x - 40.f, this->player->getPosition().y - 30.f, 50, "+", " exp");
	}




}

void GameState::updatePlayerGui(const float& dt)
{
	playerGui->update(dt, sf::Vector2f((float)mousePosWindow.x, (float)mousePosWindow.y));
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(this->keybinds.at("Toggle_Charcter_Tab"))) && getKeyTime())
	{
		playerGui->toggleTab(PLAYER_TABS::CHARACTER_TAB);
	}

}

void GameState::updateInput(const float& dt)
{

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(keybinds.at("CLOSE"))) && getKeyTime()) {
		if (!paused)
			pauseState();
		else
			unpauseState();
	}
	
}

void GameState::updatePauseMenuButtons()
{
	if (pmenu->isButtonPressed("QUIT"))
		endState();
}

void GameState::updateTileMap(const float& dt)
{
	//std::shared_ptr<Entity> player1 = std::make_shared<Entity>(player);
	map->updateTiles(player, dt, enemyLib->getEnemies());
}

void GameState::updateEnemies(const float& dt)
{
	
	enemyLib->update(dt, player->getAttack(), player, map);
}

void GameState::updatePlayer(const float& dt)
{

	//map->updateWorldBounds(player);
	//map->updateTileCollision(player, dt);
	updatePlayerInput(dt);
	updatePlayerGui(dt);
	player->update(dt, mousePosView);
	player->updateBulletCollision(dt, map);
}

void GameState::updateCombat(const float& dt)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && player->getComponent<ItemComp>()->activeRune->getItemComponent<Weapon>()->getAttackTimer()) //&& this->player->getWeapon()->getAttackTimer()
		this->player->setAttack(true);
	else 
		this->player->setAttack(false);


}

void GameState::update(const float& dt)
{

	updateMousePositions(std::make_unique<sf::View>(view));
	updateKeyTime(dt);
	updateInput(dt);
	if (!paused && !playerGuiMenuOpen) {

		pDevice->update(dt);
		updateView();
		updateTileMap(dt);
		updatePlayer(dt);
		updateEnemies(dt);
		updateCombat(dt);
		tts->update(dt);
		
		//testEnemy->update(dt, mousePosView);
	}
	else if (paused) {
		pmenu->update(dt, sf::Vector2f((float)mousePosWindow.x, (float)mousePosWindow.y));
		updatePauseMenuButtons();
	}else	{
		//playerGuiMenu->update(sf::Vector2f((float)mousePosWindow.x, (float)mousePosWindow.y);
	}
}


void GameState::render(std::shared_ptr<sf::RenderTarget> target)
{
	if (!target)
		target = window;
	renderTexture.clear();

	//Player and Map
	renderTexture.setView(view);

	map->render(renderTexture, player->getGridPosition((int)stateData->gridSize), player->getCenterPosition(), &main_shader, false, true);
	player->render(renderTexture, &main_shader, player->getCenterPosition(), false);
	enemyLib->render(renderTexture, &main_shader, player->getPosition(), false);
	map->renderDeferred(renderTexture, player->getPosition(), &main_shader);

	tts->render(renderTexture);
	//Player Gui
	renderTexture.setView(renderTexture.getDefaultView());
	playerGui->render(renderTexture);

	//Pause Menu
	if (paused) {
		
		pmenu->render(renderTexture);
	}
	
	//Final Render
	renderTexture.display();
	renderSprite.setTexture(renderTexture.getTexture());
	target->draw(renderSprite);

	
	pDevice->render(true);
}





