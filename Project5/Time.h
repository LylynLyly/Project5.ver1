#pragma once
#include"glm/glm.hpp"
class Time
{
private:
	float time_s, cfl_value;
public:
	Time(float ts,float cfl) : time_s(ts),cfl_value(cfl){}
	void ComputeCFL(glm::vec3 velocity,float sr)
	{
		cfl_value = 0.4*(sr / glm::length(velocity));
	}
	void IncrementTime_s(float a)
	{
		time_s += a;
	}
	void DecrementTime_s(float a) 
	{
		time_s -= a;
	}
	float GetTime_s()
	{
		return time_s;
	}
	bool IsCFLC()
	{
		if (cfl_value < 1.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
