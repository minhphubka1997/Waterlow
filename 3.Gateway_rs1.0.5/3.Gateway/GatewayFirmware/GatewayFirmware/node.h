
#ifndef _NODE_H
#define _NODE_H

#include "Arduino.h"

#define TYPE_TH   1
#define TYPE_DP   2
#define NODE_NUM  3

class nodeDP 
{
	public:
		uint8_t Id;
    uint32_t LastTimeGetMsg ;
    bool connected;
    uint8_t SimState ;
    bool GWPumpNeedOn;
    bool N1PumpNeedOn;
		
		void setPH(float _ph);
		void setDistance(int _distance);

		float getPH();
		int getDistance();
    void remove();
	private:
		float _PH;
		int _Distance;
};

class nodeTH
{
  public:
    uint8_t Id;
    uint32_t LastTimeGetMsg ;
    uint8_t PumpState;
    uint8_t SimState ;
    uint8_t PumpCmd;
    bool GWPumpNeedOn;
    bool connected;
    
    void setThermal(float _thermal);
    void setHumidity(int _humidity);
    
    float getThermal();
    int getHumidity();
    void remove();
  private:
    float _Thermal;
    int _Humidity;
};


bool node_is_exist(uint16_t id, uint8_t Type);
void node_check_alive(void);

void nodeth_setvalue(uint16_t id, float thermal, int humidity, bool GWpumpNOn, uint8_t N1PumpSTT, bool isconnected);
void nodeth_add(uint16_t id, float thermal, int humidity,  bool GWpumpNOn, uint8_t N1PumpSTT,  bool isconnected);
void nodeth_remove(uint16_t id);
uint8_t nodeth_count();
uint8_t nodeth_alive_count();


void nodedp_setvalue(uint16_t id, uint16_t distance, float ph, bool GWpumpNOn, bool N1pumpNOn,  bool isconnected);
void nodedp_add(uint16_t id, uint16_t distance, float ph, bool GWpumpNOn, bool N1pumpNOn, bool isconnected);
void nodedp_remove(uint16_t id);
uint8_t nodedp_count(void);
uint8_t nodedp_alive_count(void);

#endif
