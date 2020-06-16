
#include "lora.h"
#include "ring_buffer.h"
#include "node.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>

//extern node node1, node2;
extern float PH, Temp ;
extern int Distance, Humidity;

extern nodeTH nodeth[5];
extern nodeDP nodedp[5];

BUFFER_typdef       lora_data_line;
ringbuffer          lora_data_buffer;

/*---- Private function declaration-------*/
void lora_getdata(char str[]);
void lora_buffer_process(void );  



bool GWPumpNeedOn = false;
uint8_t GWPumpState = 0;
uint8_t N1PumpState = 0;



String loraTemp;
uint32_t KeepConnectionTime = 0;

void link_rules(void)
{
  if (node_isany_needGWpumpon()) GWPumpNeedOn = true; else GWPumpNeedOn = false;
  if (node_isany_needN1pumpon()) nodeth_setallpump(true); else nodeth_setallpump(false);
}


void lora_process(void)
{
  /*------- Đọc dữ liệu từ UART------- */
  while (Serial2.available())
  {
    String str = Serial2.readString();
    if (str.length() > 0)
    {
      for (int i = 0; i< str.length(); i++) lora_data_buffer.push(str[i]);
    }
  }

  lora_buffer_process();  // Nhận dữ liệu từ các node
  node_check_alive();     // Check các node có còn kết nối lora hay không
  link_rules();           // Từ dữ liệu nhận được từ các node bắt đầu đưa ra quyết định: Bật tắt bơm GW, bơm NodeTH nào đó


  /*-------- Gửi bản tin thông báo cho các node biến là node nào đã kết nối lora vào Gateway ----------------
   ----------Từ đó các node biết thực sự là node đó có kết nối hay không để biết sử dụng đến SIM
   Cấu trúc bản tin như sau:
   #GWTH[N1][ID_TH1][P1][ID_TH2][P2]...[ID_THn][Pn]DP[N2][ID_DP1][ID_DP2]...[ID_DPn]!
   Trong đó:
   TH: nghĩa là node loại TH (Thermal - Humidity)
   DP: nghĩa là node loại DP (Distance, PH)
   ID_THx: là ID của node loại TH thứ x đã kết nối vào GW
   ID_DPx: là ID của node loại DP thứ x đã kết nối vào GW
   P1: là lệnh bật tắt bơm cho node ID_TH1, tương tự P2 là lệnh bật/tắt bơm cho ID_TH2

   Các node sẽ nhận được dữ liệu này và tự so sánh xem ID của nó có nằm trong danh sách này không, nếu không có chứng tỏ
   là nó chưa có kêt nối với Gateway. Từ đó nếu có hoạt động gì nó sẽ sử dụng SIM
  */
  if (millis() - KeepConnectionTime >= 5000)
  {
//    Serial.print("NodeTH count: ");
//    Serial.print(nodeth_count());
//    Serial.print(" -- NodeDP count: ");
//    Serial.println(nodedp_count());
    
    char as[100];
    Serial.println();
    Serial2.print("#GWTH");
    Serial.print("GWTH");
    sprintf(as, "%02d", nodeth_alive_count());
    Serial2.print(as);  // number of alive TH node
    Serial.print(as);
    for (int i = 0; i< NODE_NUM; i++) 
    {
      if ((nodeth[i].Id>0)&&(nodeth[i].connected))
      {
        sprintf(as, "%03d%d", nodeth[i].Id, nodeth[i].PumpCmd);
        Serial2.print(as);// print ID and PumpCMD total 4byte
        Serial.print(as);
      }
    }
    Serial2.print("DP");
    Serial.print("DP");
    sprintf(as, "%02d", nodedp_alive_count());
    Serial2.print(as);  // number of alive DP node
    Serial.print(as);
    for (int i = 0; i< NODE_NUM; i++)
    {
      if ((nodedp[i].Id>0)&&(nodedp[i].connected))
      {
        sprintf(as, "%03d", nodedp[i].Id);
        Serial2.print(as);// print ID and PumpCMD total 4byte
        Serial.print(as);
      }
    }
    Serial2.print("!");
    Serial.println("!");
  }


}



void lora_buffer_process(void )	
{
	char c;
  
	while ((c=lora_data_buffer.pop()) >0)
	{
		if (c != 0)
		{
			if (c == START_CODE)
      {
        lora_data_line.length = 0;
        lora_data_line.buffer[lora_data_line.length++] = c;
      }
      else
      if (c == END_CODE)
      {
        lora_getdata(lora_data_line.buffer);
        lora_data_line.length = 0;
      }
      else
      {
        lora_data_line.buffer[lora_data_line.length++] = c;
        if (lora_data_line.length >= LINE_LEN) lora_data_line.length = 0;
      }
		}
	}
}
//


void lora_getdata(char str[])
{

//  Serial.println(str);
//  Serial.println("--");
  // Lấy ID
  char ID[4] ;
  int id = 0;
  ID[0] =  str[3];
  ID[1] =  str[4];
  ID[2] =  str[5];
  ID[3] = 0;
  
  id = atoi(ID);
  Serial.print("ID: ");
  Serial.print(id);
  
  // Lấy Type
  char Type[3] = {0,0,0};
  Type[0] = str[6];
  Type[1] = str[7];


  
  if ((Type[0] == 'T')&&(Type[1] == 'H'))
  {
    bool GWpumpon = false;
    uint8_t N1pumpstae = 0;
    
    // Lấy nhiệt độ
    char Tm[5];
    Tm[0] = str[9];
    Tm[1] = str[10];
    Tm[2] = str[11];
    Tm[3] = str[12];
    Tm[4] = 0;

    Temp = atoi(Tm)/10.0;
    Serial.print("---Thermal:  ");
    Serial.print(Temp);


    // Lấy độ ẩm
    char Hm[4] ;
    Hm[0] = str[14];
    Hm[1] = str[15];
    Hm[2] = str[16];
    Hm[3] = 0;

    Humidity = atoi(Hm);
    Serial.print("  --   Humidity: ");
    Serial.print(Humidity);

    char Pp[2];
    Pp[0] = str[19];
    Pp[1] = str[20];
    if (Pp[0] == '1') GWpumpon = true; else GWpumpon = false;
    if (Pp[1] == '1') N1pumpstae = 1; else N1pumpstae = 0;

    N1PumpState = N1pumpstae;
    
    Serial.print("  --  GWCMDPump: ");
    Serial.print(GWpumpon);
    Serial.print("  --  N1PumpState: ");
    Serial.println(N1pumpstae);

    if (node_is_exist(id, TYPE_TH) == false)
    {
      nodeth_add(id, Temp, Humidity, GWpumpon, N1pumpstae, true);
    }
    else
    {
      nodeth_setvalue(id, Temp, Humidity, GWpumpon, N1pumpstae, true);
    }
  }

  if ((Type[0] == 'D')&&(Type[1] == 'P'))
  {
    bool GWpumpon = false;
    bool N1pumpon = false;
    
    // Lấy Khoảng cách
    char Ds[5];
    Ds[0] = str[9];
    Ds[1] = str[10];
    Ds[2] = str[11];
    Ds[3] = str[12];
    Ds[4] = 0;
    int dis = atoi(Ds);
    if ((dis != 0)&&(dis <250))
    Distance = dis;
    
    Serial.print("---Distance:   ");
    Serial.print(Distance);

    // Lấy độ PH
    char Ph[4] ;
    Ph[0] = str[14];
    Ph[1] = str[15];
    Ph[2] = str[16];
    Ph[3] = 0;
    
    int phval = atoi(Ph);
    PH = phval / 10.0;
    Serial.print("  --   PH:     ");
    Serial.print(PH);

    char Pp[2];
    Pp[0] = str[19];
    Pp[1] = str[20];
    if (Pp[0] == '1') GWpumpon = true; else GWpumpon = false;
    if (Pp[1] == '1') N1pumpon = true; else N1pumpon = false;

    Serial.print("  --  GWCMDPump: ");
    Serial.print(GWpumpon);
    Serial.print("  --  N1CMDPump: ");
    Serial.println(N1pumpon);

     if (node_is_exist(id, TYPE_DP) == false)
      {
        nodedp_add(id, Distance, PH, GWpumpon, N1pumpon, true);
      }
      else
      {
        nodedp_setvalue(id, Distance, PH, GWpumpon, N1pumpon, true);
      }
  }
}

bool lora_is_connected(void)
{
  
}
