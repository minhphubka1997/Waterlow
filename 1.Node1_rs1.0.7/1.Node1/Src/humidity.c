#include "main.h"
#include "app.h"
#include "sim.h"
#include "lora.h"


extern uint8_t Humidity ;
extern ADC_HandleTypeDef hadc1;
extern bool SensorValueChanged;
void humidity_warning_check(void);
extern uint16_t ID;
extern bool FlagSendDataToSVViaSMS ;
bool GWPumpNeedOn	= false;	

#define HUM_MEASURE_NUM			20
#define HUM_MEASURE_INTVL		50


uint16_t HLog[HUM_MEASURE_NUM];
uint8_t  HLogCount = 0;
uint32_t HMeasureTime = 0;

void hm_log_shif(void)
{
	for (int i = 0; i< HUM_MEASURE_NUM - 1; i++)
		HLog[i] = HLog[i+1];
	if (HLogCount < HUM_MEASURE_NUM) HLogCount ++;
}
//

uint8_t hm_average(void)
{
	uint32_t ave = 0;
	for (int i = 0; i< HUM_MEASURE_NUM; i++)
		ave += HLog[i];
	return (ave/HUM_MEASURE_NUM);
}
//

void humidity_measure_process(void)
{
	if (HAL_GetTick()  -  HMeasureTime >= HUM_MEASURE_INTVL)
	{
	// Read Humidity
		uint16_t H = 0;
		if (HAL_ADC_PollForConversion(&hadc1,500) == HAL_OK)
		{
			H = HAL_ADC_GetValue(&hadc1);
		}
		
		H = 4095 - H;
		if (H < 810) H = 0;
		else
			H = H - 810; 		// Offset = 810

		H = (H * 100)/ 3285;
		
		hm_log_shif();		// Xóa dữ liệu của lần đo cũ nhất, để có chỗ trống lưu trữ lần đo này
		HLog[HUM_MEASURE_NUM-1] = H;
		
		if (HLogCount >= HUM_MEASURE_NUM) 
		{
			H = hm_average(); // Tính trung bình các lần đo
		
			if (Humidity != H)
			{
				Humidity = H;
				SensorValueChanged = true;
			}
		}
		
		HMeasureTime = HAL_GetTick();
	}
	
	humidity_warning_check();
}
//



#define HUMIDITY_CHECK_INTVL		1000//(15*60 * 1000)	// 15minute interval
#define WARNING_TIME						30000//(3 * 24*3600*1000)	// 3 ngày: Thời gian độ ẩm < 10% cần gửi tin nhắn cho user
#define WARNING_LEVEL						10 							// 10% Humidity: Mức độ ẩm cảnh báo


uint32_t humidity_check_time = 0;
int Oldhumidity = 0;
uint32_t humidity_wning_time = 0;

bool humidity_on_warning = false;

void humidity_warning_check(void)
{
	if (HAL_GetTick() - humidity_check_time >= HUMIDITY_CHECK_INTVL)
	{
		// Lúc khởi động mà độ ẩm < 10, lấy luôn thời gian lúc đó
		if ((Humidity < 10)&&(humidity_wning_time == 0))
		{
			humidity_wning_time = HAL_GetTick();
		}	
		
		// Nếu độ ẩm down xuống từ 1 giá trị > 10, lấy thời gian lúc đó
		if ((Oldhumidity > 10) && (Humidity < 10))	// down to under 10% event
		{
			humidity_wning_time = HAL_GetTick();
		}
		
		// Nếu độ ẩm > 20%, tắt bơm
		if (Humidity >= 20)
		{
			if (GWPumpNeedOn == true)
			{
				GWPumpNeedOn = false;
				FlagSendDataToSVViaSMS = true;	// Gửi yêu cầu tắt bơm qua tin nhắn
			}
		}
		
		
		// Nếu độ ẩm > 10, Tắt cảnh báo 
		if (Humidity >= 10) // Độ ẩm > 10, reset lại các biến
		{
			humidity_wning_time = 1;
			humidity_on_warning = false;
		}
		
		// Nếu Humidity < 10
		// Trong suốt 3 ngày liên tiếp
		// Và chưa gửi tin nhắn
		if 	((Humidity < 10)&&
				(HAL_GetTick()- humidity_wning_time > WARNING_TIME)&&
				(humidity_wning_time != 1)
				)	// It's time to notify User that the humidity is very low value
		{
			humidity_on_warning = true;
			if (sim_is_init_ok()&&(sim_is_sendingMsg() == false))
			{
				char ac[100];
				sprintf(ac, "ID%03d canh bao, do am < 10%c trong 3 ngay lien", ID,'%');
				sim_async_sendmsg(USER_PNB, ac);
				
				if (GWPumpNeedOn == false)
				{
					GWPumpNeedOn = true;
					FlagSendDataToSVViaSMS = true;	// Gửi yêu cầu bật bơm qua tin nhắn
				}
				
				humidity_wning_time = 1;
		
			}
		}
		

		Oldhumidity = Humidity;
		humidity_check_time = HAL_GetTick();
	}
}
//
