#include "stdafx.h"
#include "Tile.h"
#include "EnemySpawner.h"
#include "Entity.h"
#include "Enemy.h"
#include "EnemyLibrary.h"
#include "TileMap.h"
#include "physicsComponent.h"
#include "Graph.h"

void TileMap::initTileMap(std::shared_ptr<PhysicsDevice> p_device)
{

	pDevice = p_device;
}

TileMap::TileMap(float grid_size, int width, int height, std::string texture_file, std::shared_ptr<PhysicsDevice> p_device)
	: textureFile(texture_file), fromX(0), toX(0), fromY(0), toY(0), layer(0)
{
	pDevice = nullptr;

	//enemySpawner.loadFromFile("Resources/Images/Tiles/EnemyTile");

	if (!tileSheet.loadFromFile(texture_file))
		std::cout << "ERROR::TILEMAP::FAILED TO LOAD TILETEXTURESHEET " << '\n';

	

	gridSizeF = grid_size;
	gridSizeI = static_cast<int>(grid_size);
	layers = 1;
	maxSize = { width, height };
	maxSizeWorldF.x = static_cast<float>(width) * gridSizeF;
	maxSizeWorldF.y = static_cast<float>(height) * gridSizeF;
	maxSizeWorldI.x = width * gridSizeI;
	maxSizeWorldI.y = height * gridSizeI;


	collisionBox.setSize(sf::Vector2f(gridSizeF, gridSizeF));
	collisionBox.setFillColor(sf::Color::Transparent);
	collisionBox.setOutlineColor(sf::Color::Red);
	collisionBox.setOutlineThickness(1.f);


}

TileMap::TileMap(const std::string file_name, std::shared_ptr<EnemyLibrary> enemy_lib, std::shared_ptr<PhysicsDevice> p_device)
: fromX(0), toX(0), fromY(0), toY(0), layer(0)
{
	enemyLib = enemy_lib;
	initTileMap(p_device);


	initLoadFromFile(file_name);

	gridSizeF = (float)gridSizeI;
	maxSizeWorldF.x = static_cast<float>(maxSize.x) * gridSizeF;
	maxSizeWorldF.y = static_cast<float>(maxSize.y) * gridSizeF;
	maxSizeWorldI.x = maxSize.x * gridSizeI;
	maxSizeWorldI.y = maxSize.y * gridSizeI;

	//if (!tileSheet.loadFromFile(textureFile))
		//std::cout << "ERROR::TILEMAP::FAILED TO LOAD TILETEXTURESHEET " << '\n';

	collisionBox.setSize(sf::Vector2f(gridSizeF, gridSizeF));
	collisionBox.setFillColor(sf::Color::Transparent);
	collisionBox.setOutlineColor(sf::Color::Red);
	collisionBox.setOutlineThickness(1.f);
	
	
	obstacles = std::make_shared<std::vector<sf::Vector2i>>();
	//graph for enemies
	if (enemyLib != nullptr) {
		sf::Vector2i intPos;
		if (!map.empty()) {
			for (unsigned short x = 0; x < maxSize.x; x++)
			{
				for (unsigned short y = 0; y < maxSize.y; y++)
				{
					for (unsigned short z = 0; z < layers; z++)
					{
						for (size_t k = 0; k < map[x][y][z].size(); k++) {
							if (map[x][y][z][k] != nullptr) {
								if (map[x][y][z][k]->collision) {
									intPos.x = (int)map[x][y][z][k]->getPosition().x / 64;
									intPos.y = (int)map[x][y][z][k]->getPosition().y / 64;
									obstacles->push_back(intPos);

								}
							}
						}
					}
				}
			}
			enemyLib->initGraph(graph, obstacles);
		}
	}
}

TileMap::~TileMap()
{
}


const bool TileMap::tileEmpty(const int x, const int y, const int layer) const
{
	if (x >= 0 && x < static_cast<int>(map.size()))
	{
		if (y >= 0 && y < static_cast<int>(map[x].size()))
		{
			if (layer >= 0 && layer < static_cast<int>(map[x][y].size()))
			{
				return map[x][y][layer].empty();
			}
		}
	}
	
	return false;
}

//Accessors
const int TileMap::getLayerSize(const int x, const int y, const int layer) const
{
	if (x >= 0 && x < static_cast<int>(map.size()))
	{
		if (y >= 0 && y < static_cast<int>(map[x].size()))
		{
			if (layer >= 0 && layer < static_cast<int>(map[x][y].size()))
			{
				return map[x][y][layer].size();
			}
		}
	}

	return -1;
}

std::shared_ptr<Graph> TileMap::getGraph()
{
	if (graph != nullptr)
		return graph;
	else
		throw("TILEMAP:: GRAPH EMPTY!");
}

const sf::Texture* TileMap::getTileSheet() const
{
	return &tileSheet;
}

const sf::Vector2f& TileMap::getMaxSizeF() const
{
	return maxSizeWorldF;
}


//Functions
void TileMap::clear()
{
	if (!map.empty()) {
		for (unsigned short x = 0; x < maxSize.x; x++)
		{
			for (unsigned short y = 0; y < maxSize.y; y++)
			{
				for (unsigned short z = 0; z < layers; z++)
				{
					for (size_t k = 0; k < map[x][y][z].size(); k++) {
						while (!map[x][y][z].empty()) //sees if tile there to remove
						{
							map[x][y][z].pop_back();
						}
					}
				}
			}
		}
	}
}

void TileMap::addTile(const int x, const int y, const int z, const sf::IntRect& texture_rect, const bool collision, const short type)
{
	//Take two indicies and checks to see if in array size.
	if (x >= 0 && x < maxSize.x && //x
		y >= 0 && y < maxSize.y && //y
		z >= 0 && z < layers)//z 
	{
			map[x][y][z].push_back(std::make_shared<Tile>(x * gridSizeF, y * gridSizeF, tileSheet, texture_rect, graph, pDevice, collision, type));
	}
}

void TileMap::addEnemyTile(const int x, const int y, const int z, const sf::IntRect& texture_rect, int enemy_type, int max_spawned, int time_to_spawn, float max_distance, int level)
{
	
	//Take two indicies and checks to see if in array size.
	if (x >= 0 && x < maxSize.x && //x
		y >= 0 && y < maxSize.y && //y
		z >= 0 && z < layers)//z 
	{
			map[x][y][z].push_back(std::make_shared<EnemySpawner>(x * gridSizeF, y * gridSizeF, tileSheet, texture_rect, graph, *enemyLib->find(enemyLib->translateType(enemy_type)), enemy_type, max_spawned, time_to_spawn, max_distance, level, enemyLib,pDevice));
	}
}

void TileMap::removeTile(const int x, const int y, const int z, bool type_check)
{
	

		if (x >= 0 && x < maxSize.x && //x
			y >= 0 && y < maxSize.y && //y
			z >= 0 && z < layers)//z 
		{
			if (!map[x][y][z].empty()) //sees if tile there to remove
			{
				if (type_check) {
					switch (map[x][y][z].back()->getType())
					{
					case(ENEMYSPAWNER):
						break;
					default:
						return;
						break;
					}
				}
				for (size_t k = 0; k < map[x][y][z].size(); k++)
					map[x][y][z].pop_back();
			}
		}
		
	
	//Take two indicies and checks to see if in array size.
		
	
}

void TileMap::saveToFile(const std::string file_name)
{
	/*Saves the tilemape to a text file
	Format:
	Basic:
	Size x y
	gridSize
	layers
	texture file

	All tiles
	type,Texture rect x y, collision
	x y z
	EnemyTiles:
	type,Texture rect x y, collision
	enemy_type, max_spawned, time_to_spawn, max_distance
	x y z
	*/

	std::ofstream of;
	of.open(file_name);

	if (of.is_open())
	{
		of << maxSize.x << " " << maxSize.y << "\n"
			<< gridSizeI << "\n"
			<< layers << "\n"
			<< textureFile << "\n";
		graph = std::make_shared<Graph>(gridSizeI);
		for (unsigned short x = 0; x < maxSize.x; x++)
		{
			for (unsigned short y = 0; y < maxSize.y; y++)
			{
				for (unsigned short z = 0; z < layers; z++)
				{
					for (size_t k = 0; k < map[x][y][z].size(); k++) {
						if (!map[x][y][z].empty()) {
							if (map[x][y][z][k] != NULL) {
								map[x][y][z][k]->getStringTile(of);
								of << " " << x << " " << y << " " << z << "\n";
								of;
							}
							
						}
					}
				}

			}

		}

	}
	else
	{
		std::cout << "TILEMAP SAVE COULD OPEN TILEMAP FILE: " << file_name << "\n";
	}
	of.close();


}

void TileMap::loadFromFile(const std::string file_name)
{
	clear();
	std::ifstream is;
	is.open(file_name);
	if (is.is_open())
	{
		//Map
		is >> maxSize.x >> maxSize.y
			>> gridSizeI
			>> layers
			>> textureFile;
	
		//Tile
		short type;
		int x;
		int y;
		int z;
		int trX;
		int trY;
		bool collision;
		

		while (is >> type >> trX >> trY >> collision) {
			sf::IntRect texture_rect;
			texture_rect.left = trX; texture_rect.top = trY;
			texture_rect.width = (int)gridSizeF; texture_rect.height = (int)gridSizeF;

			switch(type)
			{
			case(TileType::ENEMYSPAWNER):
				int enemy_type, max_spawned, time_to_spawn, level;
				float max_distance;
				is >> enemy_type >> max_spawned >> time_to_spawn >> max_distance >> level
					>> x >> y >> z;
				map[x][y][z].emplace_back(std::make_shared<EnemySpawner>(x * gridSizeF, y * gridSizeF, tileSheet, texture_rect, graph, *enemyLib->find(enemyLib->translateType(enemy_type)), enemy_type, max_spawned, time_to_spawn, max_distance, level , enemyLib,pDevice));
				break;
			default:
				//std::cout << x << y << z << trX << trY << collision << type
				is >> x >> y >> z;
				map[x][y][z].emplace_back(std::make_shared<Tile>(x * gridSizeF, y * gridSizeF, tileSheet,  texture_rect, graph, pDevice, collision, type));
				break;
			}
		}
	}
	else
	{
		std::cout << "TILEMAP LOAD COULD OPEN TILEMAP FILE: " << file_name << "\n";
	}
	is.close();
}

void TileMap::initLoadFromFile(const std::string& file_name)
{
	std::ifstream is;
	is.open(file_name);
	if (is.is_open())
	{
		//Map
		is >> maxSize.x >> maxSize.y
			>> gridSizeI
			>> layers
			>> textureFile;
		gridSizeF = (float)gridSizeI;
		graph = std::make_shared<Graph>(gridSizeI);
		map.reserve(maxSize.x);
		for (unsigned short x = 0; x < maxSize.x; x++)
		{
			map.emplace_back(std::vector<std::vector<std::vector<std::shared_ptr<Tile>>>>());
			map[x].reserve(maxSize.y);
			for (unsigned short y = 0; y < maxSize.y; y++)
			{
				map[x].emplace_back(std::vector<std::vector<std::shared_ptr<Tile>>>());
				map[x][y].reserve(layers);
				for (unsigned short z = 0; z < layers; z++)
				{
					map[x][y].emplace_back(std::vector<std::shared_ptr<Tile>>());
					map[x][y][z].reserve(map[x][y][z].size());
					for (size_t k = 0; k < map[x][y][z].size(); k++) {
						map[x][y][z].emplace_back(nullptr);
					}
				}
			}
		}

		if (!tileSheet.loadFromFile(textureFile))
			std::cout << "ERROR::TILEMAP::FAILED TO LOAD TILETEXTURESHEET " << '\n';
	}
	is.close();
	loadFromFile(file_name);
	
}

bool TileMap::updateWorldBounds(std::shared_ptr<Entity> entity)
{
	return false;
}



void TileMap::updateTiles(std::shared_ptr<Entity> entity, const float& dt, std::vector<std::shared_ptr<Enemy>>& enemies)
{
	layer = 0;
	//Update around the player
	fromX = entity->getGridPosition(gridSizeI).x - 23;
	if (fromX < 0)
		fromX = 0;
	else if (fromX > gridSizeI)
		fromX = gridSizeI;

	toX = entity->getGridPosition(gridSizeI).x + 23;
	if (toX < 0)
		toX = 0;
	else if (toX > gridSizeI)
		toX = gridSizeI;

	fromY = entity->getGridPosition(gridSizeI).y - 17;
	if (fromY < 0)
		fromY = 0;
	else if (fromY > gridSizeI)
		fromY = gridSizeI;

	toY = entity->getGridPosition(gridSizeI).y + 17;
	if (toY < 0)
		toY = 0;
	else if (toY > gridSizeI)
		toY = gridSizeI;
	//loop through nearby objects
	for (int x = fromX; x < toX; x++)
	{
		for (int y = fromY; y < toY; y++)
		{

			for (size_t k = 0; k < map[x][y][layer].size(); k++) {
				if (map[x][y][layer][k] != nullptr) {
					if (map[x][y][layer][k]->getType() == TileType::ENEMYSPAWNER) 
					{
						EnemySpawner* es = dynamic_cast<EnemySpawner*>(map[x][y][layer][k].get());
						if(es)
						{
							if (es->canSpawn()) {
								enemies.push_back(es->spawn());
							}
						}
					}	
						map[x][y][layer][k]->update(dt);
					}
				}
			}
		}

}



void TileMap::update(std::shared_ptr<Entity> entity, const float& dt)
{
	updateWorldBounds(entity);
	//updateTiles(entity, dt);
}


void TileMap::render(sf::RenderTarget& target, const sf::Vector2i& gridPosition, const sf::Vector2f playerPos, sf::Shader* shader, const bool show_collision, const bool show_EnemySpawner)
{
		layer = 0;
		//Render boxes only around the player. (just enough to never show rendering on screen)
		fromX = gridPosition.x - 23;
		if (fromX < 0)
			fromX = 0;
		else if (fromX > gridSizeI)
			fromX = gridSizeI;

		toX = gridPosition.x + 23;
		if (toX < 0)
			toX = 0;
		else if (toX > gridSizeI)
			toX = gridSizeI;

		fromY = gridPosition.y - 17;
		if (fromY < 0)
			fromY = 0;
		else if (fromY > maxSizeWorldI.y)
			fromY = gridSizeI;

		toY = gridPosition.y + 17;
		if (toY < 0)
			toY = 0;
		else if (toY > gridSizeI)
			toY = gridSizeI;


		for (int x = fromX; x < toX; x++)
		{
			for (int y = fromY; y < toY; y++)
			{
				for (size_t k = 0; k < map[x][y][layer].size(); k++) {
					if (map[x][y][layer][k] != NULL) {

						if (map[x][y][layer][k]->getType() == TileType::DEFERRED)
						{
							deferredRenderStack.push(map[x][y][layer][k]);
						}
						else {
							if (show_EnemySpawner) { //Enemy Spawners rendered or not
								if (shader)
									map[x][y][layer][k]->render(target, playerPos, shader);
								else
									map[x][y][layer][k]->render(target);
							}
							else {
								if (map[x][y][layer][k]->getType() != TileType::ENEMYSPAWNER) {
									if (shader)
										map[x][y][layer][k]->render(target, playerPos, shader);
									else
										map[x][y][layer][k]->render(target);
								}
							}
						}

						if (map[x][y][layer][k]->getCollision()) { //Collision box rendered or not
							collisionBox.setPosition(map[x][y][layer][k]->getPosition());
							if (show_collision)
								target.draw(collisionBox);
						}

					
					
					}

				}
			}

		}
}


void TileMap::renderDeferred(sf::RenderTarget& target, const sf::Vector2f player_position, sf::Shader* shader)
{
	while (!deferredRenderStack.empty())
	{
		if (shader)
			deferredRenderStack.top()->render(target, player_position, shader);
		else
			deferredRenderStack.top()->render(target);
		deferredRenderStack.pop();
	}
}




