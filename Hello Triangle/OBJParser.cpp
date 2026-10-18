#include "OBJParser.h"

void OBJParser::readFromFile(std::string fileName)
{
	std::ifstream inFile;
	inFile.open(fileName, std::ios::in);
	std::stringstream stringStream;
	std::string line;
	std::string tag;
	std::string mtlFileName;

	dx::XMFLOAT3 tempV;
	dx::XMFLOAT2 tempVT;
	dx::XMFLOAT3 tempVN;
	dx::XMINT3 tempIndex;
	unsigned int index = 0;
	unsigned int counter = 0;
	unsigned int fCounter = 0;

	unsigned int vertexCount = 0;
	unsigned int uvCount = 0;
	unsigned int normalCount = 0;

	bool turnOn_Off_Cout = false;

	if (inFile.is_open() == false)
	{
		std::cout << "file is not open" << std::endl;
	}
	else
	{
		while (!inFile.eof() && std::getline(inFile, line))
		{
			stringStream.clear();
			stringStream.str(line);
			stringStream >> tag;

			if (line.size() > 0)
			{
				if (tag != "#")
				{
					if (tag == "mtllib")
					{
						stringStream  >> mtlFileName;
						readMtlFromFile(mtlFileName);
					}
					else if (tag == "v")
					{
						stringStream >> tempV.x >> tempV.y >> tempV.z;
						v.push_back(dx::XMFLOAT3(tempV.x, tempV.y, tempV.z));

						if (turnOn_Off_Cout == true)
						{
							std::cout << "v " << tempV.x << " " << tempV.y << " " << tempV.z << std::endl;
						}

						vertexCount = vertexCount + 1;
					}
					else if (tag == "vt")
					{
						stringStream >> tempVT.x >> tempVT.y;
						vt.push_back(dx::XMFLOAT2(tempVT.x, 1 - tempVT.y));

						if (turnOn_Off_Cout == true)
						{
							std::cout << "vt " << tempVT.x << " " << tempVT.y << std::endl;
						}

						uvCount = uvCount + 1;
					}
					else if (tag == "vn")
					{
						stringStream >> tempVN.x >> tempVN.y >> tempVN.z;
						vn.push_back(dx::XMFLOAT3(tempVN.x, tempVN.y, tempVN.z));

						if (turnOn_Off_Cout == true)
						{
							std::cout << "vn " << tempVN.x << " " << tempVN.y << " " << tempVN.z << std::endl;
						}

						normalCount = normalCount + 1;
					}
					else if (tag == "f")
					{
						counter = 0;

						while (stringStream >> index)
						{
							if (counter == 0)
							{
								tempIndex.x = index;
							}
							else if (counter == 1)
							{
								tempIndex.y = index;
							}
							else if (counter == 2)
							{
								tempIndex.z = index;

								if (turnOn_Off_Cout == true)
								{
									std::cout << "Face " << tempIndex.x << " " << tempIndex.y << " " << tempIndex.z << std::endl;
								}

								f.push_back(tempIndex);
							}

							if (stringStream.peek() == '/')
							{
								counter = counter + 1;
								stringStream.ignore(1,'/');

							}
							else if (stringStream.peek() == ' ')
							{
								counter = counter + 1;
								stringStream.ignore(1, ' ');
							}

							if (counter > 2)
							{
								counter = 0;
							}
						}

						fCounter++;
					}
				}
			}
		}
		
		vertexList.resize(f.size(), Vertex());

		if (vertexCount < 100)
		{
			printObjInformation();

		}
		
		std::cout << "vertexList size" << vertexList.size() << std::endl;
		std::cout << "f Counter size " << fCounter << std::endl;
		std::cout << "v size" << v.size() << std::endl;

		for (size_t i = 0; i < vertexList.size(); i++)
		{
			vertexList[i].setPos(v[f[i].x-1]);
			vertexList[i].setUV(vt[f[i].y-1]);
			vertexList[i].setNormal(vn[f[i].z-1]);
		}

		if (vertexCount < 100)
		{
			writeOutTestInformation(vertexCount, uvCount, normalCount);
		}

		for (size_t i = 0; i < vertexList.size(); i = i + 3)
		{
			Vertex vertex1 = vertexList[i];
			Vertex vertex2 = vertexList[i + 1];
			Vertex vertex3 = vertexList[i + 2];

			//deltaPos
			dx::XMVECTOR deltaPos1 = dx::XMVectorSet(vertex2.getPos().x - vertex1.getPos().x, vertex2.getPos().y - vertex1.getPos().y, vertex2.getPos().z - vertex1.getPos().z,0); // edge1
			dx::XMVECTOR deltaPos2 = dx::XMVectorSet(vertex3.getPos().x - vertex1.getPos().x, vertex3.getPos().y - vertex1.getPos().y, vertex3.getPos().z - vertex1.getPos().z, 0);// edge2
			
			//deltaUV
			dx::XMVECTOR deltaUV1 = dx::XMVectorSet(vertex2.getUV().x - vertex1.getUV().x, vertex2.getUV().y - vertex1.getUV().y, 0, 0);
			dx::XMVECTOR deltaUV2 = dx::XMVectorSet(vertex3.getUV().x - vertex1.getUV().x, vertex3.getUV().y - vertex1.getUV().y, 0, 0);

			float f = 1.0f / (dx::XMVectorGetX(deltaUV1) * dx::XMVectorGetY(deltaUV2) - dx::XMVectorGetY(deltaUV1) * dx::XMVectorGetX(deltaUV2));

			dx::XMVECTOR tangent = dx::XMVectorSubtract(dx::XMVectorScale(deltaPos1, DirectX::XMVectorGetY(deltaUV2)), dx::XMVectorScale(deltaPos2, DirectX::XMVectorGetY(deltaUV1)));
			
			tangent = dx::XMVectorScale(tangent, f);
			tangent = dx::XMVector3Normalize(tangent);

			tangentVec.push_back(dx::XMFLOAT3(dx::XMVectorGetX(tangent), dx::XMVectorGetY(tangent), dx::XMVectorGetZ(tangent)));
			tangentVec.push_back(dx::XMFLOAT3(dx::XMVectorGetX(tangent), dx::XMVectorGetY(tangent), dx::XMVectorGetZ(tangent)));
			tangentVec.push_back(dx::XMFLOAT3(dx::XMVectorGetX(tangent), dx::XMVectorGetY(tangent), dx::XMVectorGetZ(tangent)));
		}
	}

	for (size_t i = 0; i < vertexList.size(); i++)
	{
		vertexList[i].setTangent(tangentVec[i]);
	}

	inFile.close();
}

void OBJParser::readMtlFromFile(std::string fileName)
{
	fileName = "../" + fileName;
	std::ifstream inFile;
	inFile.open(fileName, std::ios::in);
	std::stringstream stringStream;
	std::string line;
	std::string tag;

	if (inFile.is_open() == false)
	{
		std::cout << "file is not open" << std::endl;
	}
	else
	{
		while (!inFile.eof() && std::getline(inFile, line))
		{
			stringStream.clear();
			stringStream.str(line);
			stringStream >> tag;

			if (line.size() > 0)
			{
				if (tag == "Kd")
				{
					std::cout << "Kd" << std::endl;
					stringStream >> kd.x >> kd.y >> kd.z;
					std::cout << kd.x << " " << kd.y << " " << kd.z << std::endl;
				}
				else if (tag == "Ka")
				{
					stringStream >> ka.x >> ka.y >> ka.z;
					std::cout << "Ka" << std::endl;
					std::cout << ka.x << " " << ka.y << " " << ka.z << std::endl;

				}
				else if (tag == "Ks")
				{
					stringStream >> ks.x >> ks.y >> ks.z;
					std::cout << "Ks" << std::endl;
					std::cout << ks.x << " " << ks.y << " " << ks.z << std::endl;
				}
				else if (tag == "Ns")
				{
					stringStream >> ns;
					std::cout << "Ns" << std::endl;
					std::cout << ns << std::endl;
				}
				else if (tag == "Tf")
				{
					stringStream >> tf.x >> tf.y >> tf.z;
					std::cout << "Tf" << std::endl;
					std::cout << tf.x << " " << tf.y << " " << tf.z << std::endl;

				}
				else if (tag == "Ni")
				{
					stringStream >> ni;
					std::cout << "Ni" << std::endl;
					std::cout << ni << std::endl;
				}
				else if (tag == "map_Kd")
				{
					stringStream >> map_Kd;
					std::cout << "map_Kd" << std::endl;
					std::cout << map_Kd << std::endl;
					hasATexture = true;
				}
				else if (tag == "bump")
				{
					stringStream >> bump;
					std::cout << "bump" << std::endl;
					std::cout << bump << std::endl;
					hasABumpMap = true;
				}
			}
		}
	}

	material.setAmbient(kd);
	material.setDiffuse(ka);
	material.setSpecular(ks);
	material.setTf(tf);
	material.setNs(ns);
	material.setNi(ni);
	material.setTextureName(map_Kd);
	material.setNormalTextureName(bump);
	material.setHasTexture(hasATexture);
	material.setHasABumpMap(hasABumpMap);

	inFile.close();
}



void OBJParser::printObjInformation()
{
	std::cout << "Vertex size " << vertexList.size() << std::endl;

	for (size_t i = 0; i < v.size(); i++)
	{
		std::cout << "v " << v[i].x << " " << v[i].y << " " << v[i].z << std::endl;
	}

	for (size_t i = 0; i < vt.size(); i++)
	{
		std::cout << "vt " << vt[i].x << " " << vt[i].y << std::endl;
	}

	for (size_t i = 0; i < vn.size(); i++)
	{
		std::cout << "vn " << vn[i].x << " " << vn[i].y << " " << vn[i].z << std::endl;
	}

	for (size_t i = 0; i < f.size(); i++)
	{
		std::cout << "f " << f[i].x << " " << f[i].y << " " << f[i].z << std::endl;

	}

	for (size_t i = 0; i < f.size(); i++)
	{
		std::cout << "f minus " << f[i].x - 1 << " " << f[i].y - 1 << " " << f[i].z - 1 << std::endl;

	}
}

void OBJParser::writeOutTestInformation(unsigned int vertexCount, unsigned int uvCount, unsigned int normalCount)
{
	std::cout << "Nr of Vertex " << vertexList.size() << std::endl;
	std::cout << "Vertex Count " << vertexCount << std::endl;
	std::cout << "UV Count" << uvCount << std::endl;
	std::cout << "Normal Count" << normalCount << std::endl;
}

std::vector<Vertex> &OBJParser::getVertexList()
{
	return this->vertexList;
}

Material &OBJParser::getMaterial()
{
	return this->material;
}

std::wstring OBJParser::getTextureName() const
{
	std::wstring name;
	name.assign(map_Kd.begin(), map_Kd.end());
	return name;
}

std::wstring OBJParser::getBumpName() const
{
	std::wstring name;
	name.assign(bump.begin(), bump.end());
	return name;
}

bool OBJParser::returnHasATexture() const
{
	return this->hasATexture;
}

bool OBJParser::returnHasABumpMap() const
{
	return this->hasABumpMap;
}

