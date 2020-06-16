#include "main.h"
#include "sim.h"
#include "app.h"

extern float Thermal ;
extern bool SensorValueChanged;
extern uint16_t ID ;

float DS18B20_ReadTemperature(void);
void temp_warning_check(void);


#define TEM_MEASURE_NUM			20
#define TEM_MEASURE_INTVL		50


uint16_t TempLog[TEM_MEASURE_NUM];
uint8_t  TempLogCount = 0;
uint32_t TempMeasureTime = 0;

void temp_log_shif(void)
{
	for (int i = 0; i< TEM_MEASURE_NUM - 1; i++)
		TempLog[i] = TempLog[i+1];
	if (TempLogCount < TEM_MEASURE_NUM) TempLogCount ++;
}
//

float temp_average(void)
{
	float ave = 0;
	for (int i = 0; i< TEM_MEASURE_NUM; i++)
		ave += TempLog[i];
	return (ave/TEM_MEASURE_NUM);
}
//

void temp_measure_process(void)
{
	if (HAL_GetTick() - TempMeasureTime >= TEM_MEASURE_INTVL)
	{
	// Read thermal
		
		float tempther = DS18B20_ReadTemperature();
		temp_log_shif();
		TempLog[TEM_MEASURE_NUM - 1] = tempther;
		
		if (TempLogCount >= TEM_MEASURE_NUM)
		{
			float ave = temp_average();
			if ((int)(ave*10) != (int)(Thermal*10))
			{
				SensorValueChanged = true;
				Thermal = tempther;
			}
		}
			
		TempMeasureTime = HAL_GetTick();
	}
	
	temp_warning_check();
}
//


#define THERMAL_WARNING_CHECK_INTVL		(3600 * 1000)	// 1hour
#define WARNING_TEMP									45					// nhiệt độ cảnh báo qua tin nhắn

uint32_t temp_warning_time_check = 0;

void temp_warning_check(void)
{
	if (HAL_GetTick() - temp_warning_time_check >= THERMAL_WARNING_CHECK_INTVL)
	{
		if ((Thermal > WARNING_TEMP)&&(Thermal < 120))
		{
			if (sim_is_init_ok() && (sim_is_sendingMsg() == false))
			{
				char ab[100];
				sprintf(ab, "ID%03d canh bao, nhiet do cao: %03d do C",ID, (int)Thermal);
				sim_async_sendmsg(USER_PNB, ab);
			}
			else return;
		}
		
		temp_warning_time_check = HAL_GetTick();
	}
}
//