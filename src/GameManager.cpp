#include <GameManager.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/vector_angle.hpp>
#include <sstream>

GameManager::GameManager(NodePtr graph, float* hDir, float* vDir) {
	sceneGraph = graph;
	horizontalDirection = hDir;
	verticalDirection = vDir;
	spacebar = false;
}

GameManager::~GameManager()
{
	//SoundEngine->drop();
}

void GameManager::GameOps()
{
	stringstream ss;
	if (GAME_STATE == IN_GAME) 
	{
		ShootIfPossible();
		spawnAsteroid();
		RotateAsteroids();
		movePlayer();
		moveObjects(bulletList);
		moveObjects(asteroidList);
		DoCollision();
		removeObjectOutsideTheCamera(bulletList);
		removeAsteroidsOutsideTheCamera(asteroidList);
		ss << PLAYER_LIFES;
		Text.get()->RenderText("Lives: " + ss.str(), 10.0f, 10.0f, 1.0f);
		ss.str("");
		ss << SCORE;
		Text.get()->RenderText("Score: " + ss.str(), 400.0f, 10.0f, 1.0f);
	}
	if(GAME_STATE == IN_MENU)
	{
		Text->RenderText("Press ENTER to start", 500.0f, 360.0f, 1.0f);
		if (!gameInit)
		{
			ss << SCORE;
			Text.get()->RenderText("Score: " + ss.str(), 590.0f, 400.0f, 1.0f);
		}
		if (EnterIsPushed()) {
			ResetGame();
		}

	}
}

void GameManager::ResetGame()
{
	sceneGraph->GetChildren().clear();
	bulletList.clear();
	asteroidList.clear();
	player->setPosition(-10.0f, 0.0f, 0.0f);
	PLAYER_LIFES = 3;
	SCORE = 0;
	GAME_STATE = IN_GAME;
	gameInit = false;
}

void GameManager::SetFrameTime(float frameTime)
{
	FRAME_TIME = frameTime;
}

void GameManager::setPlayer(GraphNode* playerPtr)
{
	player = shared_ptr<GraphNode>(playerPtr);
}

void GameManager::setBullet(GraphNode* bulletPtr)
{
	bullet = NodePtr(bulletPtr);
}

void GameManager::SetTextRenderer(shared_ptr<TextRenderer>& text) {
	Text = text;
}

void GameManager::SetAsteroid(GraphNode * ast)
{
	asteroid = NodePtr(ast);
}

void GameManager::addBullet(glm::vec3& position, glm::vec3& direction, GraphNode* shootingObject)
{
		NodePtr oneBullet = std::make_shared<GraphNode>(bullet.get());
		oneBullet->setPosition(position.x, position.y, position.z + 1.0f);
		oneBullet->SetDirection(direction);
		oneBullet->SetSpeed(BULLET_SPEED);
		oneBullet->SetShootingObject(shootingObject);
		float angleRadians = glm::angle(glm::vec3(1, 0, 0), direction);

		float angle = angleRadians * (float)(180 / M_PI);
		if (player->getPosition().y > shootingObject->getPosition().y)
			oneBullet.get()->Rotate(angle, glm::vec3(0, 0, 1));
		else
			oneBullet.get()->Rotate(-angle, glm::vec3(0, 0, 1));


		bulletList.push_back(oneBullet);
		sceneGraph->AddChild(oneBullet.get());
}

void GameManager::spawnAsteroid()
{
	float time = (float)glfwGetTime();
	if (time >= ASTEROID_COOLDOWN)
	{
		ASTEROID_COOLDOWN = (float)glfwGetTime() + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + 1.5f;;
		float y = -11.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (10.0f - (-11.0f))));
		float z = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (-5.0f - (-1.0f))));
		NodePtr tmp = std::make_shared<GraphNode>(asteroid.get());
		tmp->setPosition(20.0f, y, z);
		tmp->SetSpeed(static_cast <float> (((rand()) / static_cast <float> (RAND_MAX)))* 30.0f);
		float i = static_cast <float> ((rand()) / static_cast <float> (RAND_MAX)) + 1;
		float j = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (-1.0f))));
		j /= 2;
		tmp->SetDirection(glm::vec3(-i, j, 0));
		asteroidList.push_back(tmp);
		sceneGraph->AddChild(tmp.get());
	}
}

void GameManager::movePlayer()
{
	glm::vec3 shipPosition = player->getPosition();
	float xVal = shipPosition.x + *horizontalDirection * PLAYER_SPEED * FRAME_TIME;
	float yVal = shipPosition.y + *verticalDirection * PLAYER_SPEED * FRAME_TIME;
	if (xVal >= -19.0f && xVal <= 16.0f && yVal <= 11.0f && yVal >= -11.0f) {
		player->setPosition(xVal, yVal, 0.0f);
	}

}

void GameManager::moveObjects(vector<NodePtr>& objectContainer)
{
	for (NodePtr& gameObject : objectContainer) {
		glm::vec3 gameObjectPos = gameObject->getPosition();
		float x = gameObjectPos.x + gameObject->GetDirection().x * gameObject->GetSpeed() * FRAME_TIME;
		float y = gameObjectPos.y + gameObject->GetDirection().y * gameObject->GetSpeed() * FRAME_TIME;
		gameObject->setPosition( x, y, gameObjectPos.z);
	}
}

void GameManager::spacebarPushed(bool pushed)
{
	spacebar = pushed;
}

void GameManager::enterPushed(bool pushed)
{
	enter = pushed;
}

void GameManager::removeObjectOutsideTheCamera(vector<NodePtr>& v)
{
	for (NodePtr& object : v) 
	{
		glm::vec3 xPos = object->getPosition();
		
		if (xPos.x < -30.0f || xPos.x > 30.0f) {
			sceneGraph->RemoveNode(object.get());
			removeNode(v, object);
			break;
		}
	}
}

void GameManager::removeAsteroidsOutsideTheCamera(vector<NodePtr>& v)
{
	for (NodePtr& object : v)
	{
		glm::vec3 xPos = object->getPosition();

		if (xPos.x < -100.0f || xPos.x > 100.0f) {
			sceneGraph->RemoveNode(object.get());
			removeNode(v, object);
			break;
		}
	}
}

void GameManager::removeNode(vector<NodePtr>& v, NodePtr& node) {

	std::vector<NodePtr>::iterator i = std::find_if(v.begin(), v.end(),
		[&node](NodePtr& x) //lambda
	{
		return x == node;
	});

	if (i != v.end()) {
		v.erase(i);
	}
}

void GameManager::ShootIfPossible()
{
	static volatile bool playerShot = false;
	if (playerShot) {
		COOLDOWN = (float)glfwGetTime() + 0.4f;
	}
	if (SpacebarIsPushed() && (float)glfwGetTime() >= COOLDOWN)
	{
        glm::vec3 tempPos(player->getPosition());
        glm::vec3 tempDirection(1, 0, 0);
		addBullet(tempPos, tempDirection, player.get());
		playerShot = true;
	}
	else
	{
		playerShot = false;
	}
}

void GameManager::DoCollision()
{
    for (NodePtr& asteroid : asteroidList) {
        for (NodePtr& bullet : bulletList) {
            if (CheckCollision(bullet.get(), asteroid.get())) {
                bullet->Active(false);
                asteroid->Active(false);
                SCORE += 10;
                break;
            }
        }

        if (CheckCollision(player.get(), asteroid.get())) {
            asteroid->Active(false);
            PLAYER_LIFES -= 1;
            cout << "Player has been hit" << endl;
            if (PLAYER_LIFES == 0)
                GAME_STATE = IN_MENU;
            break;
        }

    }
}

bool GameManager::CheckCollision(GraphNode* one, GraphNode* two, bool bulletMode)
{
	if(bulletMode)
		if (one->GetShootingObject() == two)
			return false;

	if (one->IsActive() == false || two->IsActive() == false)
		return false;

	float d1x = two->GetMin().x - one->GetMax().x;
	float d1y = two->GetMin().y - one->GetMax().y;
	float d2x = one->GetMin().x - two->GetMax().x;
	float d2y = one->GetMin().y - two->GetMax().y;

	if (d1x > 0.0f || d1y > 0.0f)
		return false;

	if (d2x > 0.0f || d2y > 0.0f)
		return false;

	return true;
}

bool GameManager::SpacebarIsPushed()
{
	if (spacebar)
		return true;
	else
		return false;
}

bool GameManager::EnterIsPushed()
{
	if (enter)
		return true;
	else
		return false;
}

void GameManager::RotateAsteroids()
{
	for (NodePtr& asteroid : asteroidList) {
		asteroid->Rotate(0.01f * glfwGetTime(), glm::vec3(0.3f , 0.4f, 0.5f));
	}
}

STATE GameManager::GetState()
{
	return GAME_STATE;
}
