#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include <vector>
#include <memory>
#include <GraphNode.h>
#include <TextRenderer.h>

enum STATE {
	IN_MENU,
	IN_GAME
};

using NodePtr = shared_ptr<GraphNode>;
class GameManager
{
private:
	
	float COOLDOWN = 0;
	float ASTEROID_COOLDOWN = 0;
	float PLAYER_SPEED = 12.0f;
	const float BULLET_SPEED = 15.0f;
	int PLAYER_LIFES = 3;
	int SCORE = 0;
	float FRAME_TIME = 0.016f;
	bool gameInit = true;

	
	NodePtr sceneGraph;
	NodePtr player;
	NodePtr bullet;
	shared_ptr<TextRenderer> Text;
	NodePtr asteroid;
	float* horizontalDirection;
	float* verticalDirection;
	vector<NodePtr> bulletList;
	vector<NodePtr> asteroidList;

	bool spacebar;
	bool enter;
	
	STATE GAME_STATE = IN_MENU;
	bool SpacebarIsPushed();
	bool EnterIsPushed();
	void removeObjectOutsideTheCamera(vector<NodePtr>&);
	void removeAsteroidsOutsideTheCamera(vector<NodePtr>& v);
	void removeNode(vector<NodePtr>& , NodePtr&);
	void ShootIfPossible();
	bool CheckCollision(GraphNode*, GraphNode*, bool bulletMode = false);
	void DoCollision();
	void spawnAsteroid();
	void addBullet(glm::vec3&, glm::vec3 &, GraphNode*);
	void movePlayer();
	void moveObjects(vector<NodePtr>&);
public:
	GameManager(NodePtr graph, float* , float*);
	~GameManager();
	void setPlayer(GraphNode* playerPtr);
	void setBullet(GraphNode* bulletPtr);
	void SetTextRenderer(shared_ptr<TextRenderer>& text);
	void SetAsteroid(GraphNode* ast);
	void spacebarPushed(bool);
	void enterPushed(bool);
	void GameOps();
	void ResetGame();
	void SetFrameTime(float frameTime);
	void RotateAsteroids();
	STATE GetState();
};
#endif // !GAMEMANAGER_H
