#include "GameManager.h"

GameManager::GameManager()
{
}

GameManager::GameManager(int numberOfTargets)
{
	this->numberOfTargets = numberOfTargets;
}

void GameManager::placeTargets(ID3D11Device * devicePtr, float yPos, float zPos)
{
	float offSet = 0;
	float offSetIncrease = 0;
	offSet = ((numberOfTargets * 2.0f) - 2) / -2;
	offSetIncrease = 2;
	
	for (int i = 0; i < numberOfTargets; i++)
	{
		gameTargets.push_back(new Mesh(devicePtr));
		gameTargets[i]->readModelFromeFile("../box.obj");
		gameTargets[i]->readTextureFromeFile(L"../Texture/bump01.png");
		gameTargets[i]->setTranslation(dx::XMFLOAT3(offSet, yPos, zPos));
		gameTargets[i]->setScale(dx::XMFLOAT3(1, 1, 1));
		gameTargets[i]->setRotation(dx::XMFLOAT3(0, 0, 0));
		gameTargets[i]->setPickable(true);

		offSet = offSet + offSetIncrease;
	}
}

void GameManager::checkIfGameOver(std::vector<Mesh*>& gameTargets)
{
	int check = 0;

	for (size_t i = 0; i < gameTargets.size(); i++)
	{
		if (gameTargets[i]->getPickable() == false)
		{
			check = check + 1;
		}
	}

	if (check == gameTargets.size())
	{
		gameOver = true;
	}
	else
	{
		gameOver = false;
	}
}

bool GameManager::checkIfGameOver() const
{
	return this->gameOver;
}

void GameManager::moveGameTargets()
{
	static float limit = 1.0f;

	for (size_t i = 0; i < gameTargets.size(); i++)
	{
		if (gameTargets[i]->getPickable() == true)
		{
			if (gameTargets[i]->getTranslation().y <= -limit || gameTargets[i]->getTranslation().y >= limit)
			{
				gameTargets[i]->setSpeed(gameTargets[i]->getSpeed() * -1);
			}

			if (i % 2 == 0)
			{
				gameTargets[i]->setTranslation(dx::XMFLOAT3(gameTargets[i]->getTranslation().x, gameTargets[i]->getTranslation().y - gameTargets[i]->getSpeed(), gameTargets[i]->getTranslation().z));

			}
			else if (i % 2 == 1)
			{
				gameTargets[i]->setTranslation(dx::XMFLOAT3(gameTargets[i]->getTranslation().x, gameTargets[i]->getTranslation().y + gameTargets[i]->getSpeed(), gameTargets[i]->getTranslation().z));

			}
		}
	}
}

void GameManager::setScore(int score)
{
	this->score = score;
}

void GameManager::addScore(int score)
{
	this->score = this->score + score;
}

int GameManager::getScore() const
{
	return this->score;
}

void GameManager::setGameOver(bool gameOver)
{
	this->gameOver = gameOver;
}

bool GameManager::getGameOver() const
{
	return this->gameOver;
}

void GameManager::release()
{
	for (size_t i = 0; i < gameTargets.size(); i++)
	{
		gameTargets[i]->release();
		delete gameTargets[i];
	}
}

std::vector<Mesh*> &GameManager::getGameTargets()
{
	return this->gameTargets;
}

int GameManager::getNumberOfTargets() const
{
	return this->numberOfTargets;
}


