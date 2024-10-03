#pragma once
#include"Particel.h"
#include<list>
#include<vector>
#include<set>
#include<iostream>
class SP
{
private:
	const float cellSize;
	std::vector<std::list<Particel*>> grid;
    public:
	SP(const float c = 0.1f) : cellSize(c) {}
	
	void SetSize(int xs = 20, int ys = 20, int zs = 20)
	{
		grid.resize(xs * ys * zs);
	}

	size_t Hash(float xi, float yi, float zi)
	{///(0.2 / cellSize)
		int x = static_cast<int>(std::floor(xi / cellSize));
		int y = static_cast<int>(std::floor(yi / cellSize));
		int z = static_cast<int>(std::floor(zi / cellSize));

		return (std::abs((x * 73856093)) ^ std::abs((y * 19349663)) ^ std::abs((z * 83492791))) % grid.size();
	}

	void StoreGrid(std::vector<Particel*> allparticles)
	{
		for (int i = 0; i < allparticles.size(); ++i)
		{
			grid[Hash(allparticles[i]->GetPosition().x, allparticles[i]->GetPosition().y, allparticles[i]->GetPosition().z)].push_back(allparticles[i]);
		}
	}
	void found_neighbor(Particel* P, float smradius = 0.2)
	{

		std::set<Particel*> newN;


		for (float cellz = -smradius; cellz < smradius; cellz += cellSize)///Z ////ZAXODIM
		{
			for (float celly = -smradius; celly < smradius; celly += cellSize)///Y ///// OTSASIVAEM
			{
				for (float cellx = -smradius; cellx < smradius; cellx += cellSize)///X //// VIXODIM
				{
					for (std::list<Particel*>::iterator a = grid[Hash(P->GetPosition().x + cellx, P->GetPosition().y + celly, P->GetPosition().z + cellz)].begin(); a != grid[Hash(P->GetPosition().x + cellx, P->GetPosition().y + celly, P->GetPosition().z + cellz)].end(); ++a)
					{
						//std::cout << "R" << glm::length(P->GetPosition() - (*a)->GetPosition()) << " " << smradius << '\n';
						if (P != *a && glm::length(P->GetPosition() - (*a)->GetPosition())  <= smradius)
						{
							//std::cout << "R" << glm::length(P->GetPosition() - (*a)->GetPosition()) << " " << smradius << '\n';
							newN.insert(*a);
						}
					}
				}
			}
			P->SetNeighboors(newN);
		}
		//for (std::set<Particel*>::iterator i = newN.begin(); i != newN.end(); ++i)
		//{
		//	std::cout << "   " << (*i)->GetPosition().x << " " << (*i)->GetPosition().y << " " << (*i)->GetPosition().z << '\n';
		//}
	}
};

