
#include "node.h"


void nodeTH:: remove(void)
{
  Id = 0;
  _Thermal = 0;
  _Humidity = 0;
}


void nodeTH:: setThermal(float _thermal)
{
	_Thermal = _thermal;
}

void nodeTH:: setHumidity(int _humidity)
{
	_Humidity = _humidity;
}


float nodeTH::getThermal()
{
  return _Thermal;
}


int nodeTH :: getHumidity()
{
  return _Humidity;
}








void nodeDP:: remove(void)
{
  Id = 0;
  _PH = 0;
  _Distance = 0;
}

void nodeDP:: setPH(float _ph)
{
	_PH = _ph;
}

void nodeDP:: setDistance(int _distance)
{
	_Distance = _distance;
}


float nodeDP ::getPH()
{
	return _PH;
}

int nodeDP ::getDistance()
{
	return _Distance;
}
