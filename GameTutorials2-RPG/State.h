#pragma once

namespace sf {
	class Texture;
	class Sprite;
	class RenderTarget;
	class Font;
	class Text;
	class Color;
	class RectangleShape;
	class RenderWindow;
}

class gui::Button;
class GraphicsSettings;
class State;

class StateData
{
public:
	StateData() {};
	float gridSize = 0.f;
	unsigned mapSize = 0;
	std::shared_ptr<GraphicsSettings> GraphicsSettings;
	std::shared_ptr<sf::RenderWindow> window;
	std::shared_ptr<std::map<std::string, int>> supportedKeys;
	std::stack<std::shared_ptr<State>>* states = NULL;
	std::shared_ptr<sf::Font> font;
	//variables
};

class State
{
public:

	//Constructor|Deconstructor
	State(std::shared_ptr<StateData>);
	virtual ~State();

	void endState();
	void pauseState();
	void unpauseState();

	//accessors
	const bool& getQuit() const;
	const bool getKeyTime();

	//Functions
	virtual void update(const float& dt) = 0;
	virtual void updateMousePositions(std::unique_ptr<sf::View> view = NULL);
	virtual void updateKeyTime(const float& dt);
	virtual void updateInput(const float& dt) = 0;
	virtual void render(std::shared_ptr<sf::RenderTarget> target =  NULL) = 0;

	


protected:
	
	//Variables
	std::shared_ptr<StateData> stateData;
	std::stack< std::shared_ptr<State>>* states; // this is a pointer to a stack of pointers from the game class
	std::shared_ptr<GraphicsSettings> GraphicsSettings;
	sf::Font font; //States font.
	
	bool quit;
	bool paused;
	float gridSize;
	unsigned mapSize;
	float keyTime;
	float keyTimeMax;
	int characterSize;
	
	sf::Vector2i mousePosScreen; 
	sf::Vector2i mousePosWindow; 
	sf::Vector2f mousePosView;
	sf::Vector2i mousePosGrid;
	sf::Event sfEvent;

	//Resources
	std::map <std::string, sf::Texture> textures;
	std::shared_ptr<sf::RenderWindow> window;
	std::shared_ptr<std::map<std::string, int>> supportedKeys;
	std::map<std::string, int> keybinds;

	//Functions
	
	virtual void initFont();

	virtual void initKeybinds() = 0;
	virtual std::unique_ptr<gui::Button> addButton(float x, float y, const std::string text, float width = 250.f, float height = 75.f, short characterSize = 50); //Basic Button
	virtual std::unique_ptr<gui::Button> addButton(float x, float y, const std::string text, float width, float height, short characterSize,
		sf::Color text_idle_color, sf::Color text_hover_color, sf::Color text_active_color, gui::button_types type,
		sf::Color idle_color, sf::Color hover_color, sf::Color active_color
	); //Fully Modifiable button

private:
	
};


