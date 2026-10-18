#pragma once
#include "Mesh.h"

class GameManager
{
private:

	int score = 0;
	int numberOfTargets = 0;
	bool gameOver;
	std::vector<Mesh*> gameTargets;

public:

	GameManager();
	GameManager(int numberOfTargets);

	void placeTargets(ID3D11Device * devicePtr,float yPos, float zPos);

	void checkIfGameOver(std::vector<Mesh*> &gameTargets);
	bool checkIfGameOver() const;

	void moveGameTargets();

	void setScore(int score);
	void addScore(int score);
	int getScore() const;

	void setGameOver(bool gameOver);
	bool getGameOver() const;

	void release();

	std::vector<Mesh*> &getGameTargets();

	int getNumberOfTargets() const;
};

