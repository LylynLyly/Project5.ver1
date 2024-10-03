#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<cmath>
#include <set>
#include<iostream>
class Particel
{
private:
	float mass,density,pressure,ckf,sr,k;
	glm::vec3 position,velocity,Fpressure,Fvic,Force,Ftension;
	std::vector<Particel*> neighborhood;
	int index;
public:
	Particel(glm::vec3 pos,glm::vec3 v_,float m, float c_k_f, float s_r,float k_,glm::vec3 f,int i) :position(pos), velocity(v_), mass(m), ckf(c_k_f), sr(s_r), k(k_),Force(f),index(i) {}
	float GetMass()
	{return mass;}
	glm::vec3 GetPosition()
	{return position;}
	glm::vec3 GetVelocity()
	{return velocity;}
	void SetVelocity(glm::vec3 a)
	{velocity = a;}
	void SetNeighboors(std::set<Particel*> fe)
	{neighborhood.assign(fe.begin(), fe.end());}
	float GetDensity()
	{return density;}
	float GetPressure()
	{return pressure;}
	glm::vec3 force()
	{ return Force; }
	int GetInt()
	{
		return index;
	}
	void Neighboors()
	{
		for (int i = 0; i < neighborhood.size(); i++)
		{
			std::cout << index << "  Nindex  " << neighborhood[i]->GetInt() << '\n';
		}
	}
	float kernel_function_poly(glm::vec3 a_position,glm::vec3 n_position)
	{
		float q = glm::length(n_position - a_position);
		//std::cout << "Q" << q << '\n';
		if (0.0 <= q && q < sr)
		{
			return  (315.0 / (64.0 * 3.14 * pow(sr, 9))) * pow((pow(sr, 2) - pow(q, 2)), 3);
		}
		else
		{
			return 0;
		}
	}
	float grad_kernel_function_spiky(glm::vec3 a_position, glm::vec3 n_position)
	{
		float q = glm::length(n_position - a_position);
		if (0.0 <= q && q < sr)
		{
			return  (45.0 / (3.14 * pow(sr, 6))) * pow((sr - q), 2);
		}
		else
		{
			return 0;
		}
	}
	float laplace_kernel_function_vicosiy(glm::vec3 a_position, glm::vec3 n_position)
	{
		float q = glm::length(n_position - a_position);
		if (0.0 <= q && q < sr)
		{
			return  (15.0 / (2 * 3.14 * pow(sr, 3))) * (-((3 * q) / pow(sr, 3)) + (2 / pow(sr, 2)));
		}
		else
		{
			return 0;
		}
	}
	glm::vec3 getViscosity() 
	{
		
		const float baseViscosity = 0.001f; 
		const float dampingFactor = 0.01f;   
		float viscosityMagnitude = baseViscosity / (1 + dampingFactor * glm::length(velocity));
		return viscosityMagnitude * glm::normalize(velocity);
	}
	void ComputeDensity()
	{
		density = 0.0f;
		if (neighborhood.empty()) 
		{
			std::cout << "ZERO" << '\n';
		}
		for (int i = 0; i < neighborhood.size(); i++)
		{
			std::cout << "H" << kernel_function_poly(position, neighborhood[i]->GetPosition()) << '\n';
			density += neighborhood[i]->GetMass() * kernel_function_poly(position, neighborhood[i]->GetPosition());
		}
    }
	void ComputePressure()
	{
		pressure = 0;
		pressure = 500 * (density - 1000.0f);
	}
	glm::vec3 InterpolationPressure()
	{
		glm::vec3 Interpressure(0, 0, 0);
		for (int j = 0; j < neighborhood.size(); ++j)
		{
			Interpressure.x += (neighborhood[j]->GetMass() / neighborhood[j]->GetDensity()) * neighborhood[j]->GetPressure() * kernel_function_poly(position, neighborhood[j]->GetPosition());
			Interpressure.y += (neighborhood[j]->GetMass() / neighborhood[j]->GetDensity()) * neighborhood[j]->GetPressure() * kernel_function_poly(position, neighborhood[j]->GetPosition());
			Interpressure.z += (neighborhood[j]->GetMass() / neighborhood[j]->GetDensity()) * neighborhood[j]->GetPressure() * kernel_function_poly(position, neighborhood[j]->GetPosition());
		}

		return Interpressure;
	}
	glm::vec3 InterpolationVelocity()
	{
		glm::vec3 Intervicosiy(0, 0, 0);
		for (int j = 0; j < neighborhood.size(); ++j)
		{
			//std::cout << " f " << neighborhood[j]->GetMass() / neighborhood[j]->GetDensity() << '\n';
			//std::cout << " fd " << neighborhood[j]->GetMass() << " " << neighborhood[j]->GetDensity() << '\n';
			Intervicosiy.x += neighborhood[j]->GetMass() / neighborhood[j]->GetDensity() * neighborhood[j]->GetVelocity().x * kernel_function_poly(position, neighborhood[j]->GetPosition());
			Intervicosiy.y += neighborhood[j]->GetMass() / neighborhood[j]->GetDensity() * neighborhood[j]->GetVelocity().y * kernel_function_poly(position, neighborhood[j]->GetPosition());
			Intervicosiy.z += neighborhood[j]->GetMass() / neighborhood[j]->GetDensity() * neighborhood[j]->GetVelocity().z * kernel_function_poly(position, neighborhood[j]->GetPosition());
		}
		return Intervicosiy;
	}
	void ComputeFPressure()
	{
		Fpressure = glm::vec3(0, 0, 0);
		glm::vec3 IntorP = InterpolationPressure();
		for (int j = 0; j < neighborhood.size(); j++)
		{
			glm::vec3 IntorPN = neighborhood[j]->InterpolationPressure();
			Fpressure += neighborhood[j]->GetMass() * ((IntorP + IntorPN) / (2 * neighborhood[j]->GetDensity())) * grad_kernel_function_spiky(position, neighborhood[j]->GetPosition());
		}
		//std::cout << Fpressure.y << '\n';
		Fpressure *= -mass;
	}
	void ComputeFVicosiy()
	{
		Fvic = glm::vec3(0, 0, 0);
		glm::vec3 IntorP = InterpolationVelocity();
		for (int j = 0; j < neighborhood.size(); j++)
		{
			glm::vec3 posdif = position - neighborhood[j]->GetPosition();
			glm::vec3 IntorDif = IntorP - neighborhood[j]->InterpolationVelocity();
			Fvic += neighborhood[j]->GetMass() * (IntorDif / neighborhood[j]->GetDensity()) * laplace_kernel_function_vicosiy(position, neighborhood[j]->GetPosition());
		}
		//std::cout << Fvic.y << '\n';
		Fvic *= getViscosity();
	}
	void ComputeColorField()
	{
		Ftension = glm::vec3(0, 0, 0);
		for (int i = 0; i < neighborhood.size(); i++)
		{
			Ftension += (neighborhood[i]->GetMass() / neighborhood[i]->GetDensity()) * kernel_function_poly(position, neighborhood[i]->GetPosition());
		}
	}
	void SurfaceFTension()
	{

	}
	void ComputeAc()
	{
		Force = Fpressure + Fvic + glm::vec3(0, -mass * (9.8), 0);
		//std::cout << glm::length(Fpressure + Fvic) << '\n';
	}
	void Transform(float time_step)
	{
		velocity = velocity + (time_step * (Force / mass));
		position = (position + (time_step * velocity));
	}
	void SetPosition(glm::vec3 a)
	{
		position = a;
	}

	void ComputeColision(float e)
	{
		for (int i = 0; i < neighborhood.size(); i++)
		{
			// Визначення відстані між частинками
			glm::vec3 neighborPosition = neighborhood[i]->GetPosition();
			float distance = glm::length(position - neighborPosition);

			// Перевірка на зіткнення
			if (distance <= sr)
			{
				//std::cout << "Colision" << '\n';
				// Визначення нормалі до поверхні зіткнення
				glm::vec3 normal = glm::normalize(position - neighborPosition);

				// Обчислення нової швидкості з урахуванням відновлення
				float velocityDotNormal = glm::dot(velocity, normal);
				if (velocityDotNormal < 0) // Переконуємось, що частинка рухається в напрямку нормалі
				{
					velocity -= normal * velocityDotNormal * (1 + e); // Відновлення швидкості
				}
			}
		}
	}
	void ConvertToOpenGL(std::vector<float>& v)
	{
		v.push_back(position.x);
		v.push_back(position.y);
		v.push_back(position.z);
	}
};

