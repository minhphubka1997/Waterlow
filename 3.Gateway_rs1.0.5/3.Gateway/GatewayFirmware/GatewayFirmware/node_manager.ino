#include "node.h"



nodeTH nodeth[5];
nodeDP nodedp[5];


bool node_is_exist(uint16_t id, uint8_t Type)
{
  if (Type == TYPE_TH)
  {
    for (int i = 0; i< NODE_NUM; i++) 
      if (nodeth[i].Id == id) return true;
  }

  if (Type == TYPE_DP)
  {
    for (int i = 0; i< NODE_NUM; i++) 
      if (nodedp[i].Id == id) return true;
  }
  
  return false;
}
//

void node_print_infor_all(void)
{
  Serial.println();
  Serial.println("All nodeTH infor");
  for (int i = 0; i< NODE_NUM; i++)
  {
    Serial.print("ID: "); Serial.print(nodeth[i].Id);
    Serial.print(" -- connected: "); Serial.println(nodeth[i].connected);
  }
  Serial.println("All nodeDP infor");
  for (int i = 0; i< NODE_NUM; i++)
  {
    Serial.print("ID: "); Serial.print(nodedp[i].Id);
    Serial.print(" -- connected: "); Serial.println(nodedp[i].connected);
  }
  
}

uint8_t nodeth_count()
{
  uint8_t count = 0;
  for (int i = 0; i< NODE_NUM; i++) 
    if (nodeth[i].Id > 0) count ++;
  return count;
}

uint8_t nodeth_alive_count()
{
  uint8_t count = 0;
  for (int i = 0; i< NODE_NUM; i++) 
  {
    if (nodeth[i].Id > 0)
    {
      if (nodeth[i].connected == true) count ++;
    }
  }
   
  return count;
}

void nodeth_setvalue(uint16_t id, float thermal, int humidity, bool GWpumpNOn, uint8_t N1PumpSTT, bool isconnected)
{
//  Serial.print("Set nodeth, ID: ");
//  Serial.println(id);
  if (node_is_exist(id, TYPE_TH))
  {
    for (int i = 0; i< NODE_NUM; i++) 
    {
      if (nodeth[i].Id == id)
      {
        nodeth[i].setThermal(thermal);
        nodeth[i].setHumidity(humidity);
        nodeth[i].GWPumpNeedOn = GWpumpNOn;
        nodeth[i].connected = isconnected;
        nodeth[i].PumpState = N1PumpSTT;
        nodeth[i].LastTimeGetMsg = millis();
        //node_print_infor_all();
        return;
      }
    }
  }
}
//

void nodeth_add(uint16_t id, float thermal, int humidity, bool GWpumpNOn, uint8_t N1PumpSTT, bool isconnected)
{
//  Serial.print("Add nodeth, ID: ");
//  Serial.println(id);
  if (node_is_exist(id, TYPE_TH))
  {
    for (int i = 0; i< NODE_NUM; i++) 
    {
      if (nodeth[i].Id == id)
      {
        nodeth[i].setThermal(thermal);
        nodeth[i].setHumidity(humidity);
        nodeth[i].GWPumpNeedOn = GWpumpNOn;
        nodeth[i].connected = isconnected;
        nodeth[i].PumpState = N1PumpSTT;
        nodeth[i].LastTimeGetMsg = millis();
        //node_print_infor_all();
        return;
      }
    }
  }
  else
  {
    for (int i = 0; i< NODE_NUM; i++) 
    {
      if (nodeth[i].Id == 0)
      {
        nodeth[i].Id = id;
        nodeth[i].setThermal(thermal);
        nodeth[i].setHumidity(humidity);
        nodeth[i].GWPumpNeedOn = GWpumpNOn;
        nodeth[i].connected = isconnected;
        nodeth[i].PumpState = N1PumpSTT;
        nodeth[i].LastTimeGetMsg = millis();
        //node_print_infor_all();
        return;
      }
    }
  }
}

void nodeth_remove(uint16_t id)
{
  Serial.print("Remove ID: ");
  Serial.println(id);
  for (int i = 0; i< NODE_NUM; i++) 
    if (nodeth[i].Id == id)
    {
      nodeth[i].remove();
      return;
    }
}







uint8_t nodedp_count(void)
{
  uint8_t count = 0;
  for (int i = 0; i< NODE_NUM; i++) 
    if (nodedp[i].Id > 0) count ++;
  return count;
}


uint8_t nodedp_alive_count(void)
{
  uint8_t count = 0;
  for (int i = 0; i< NODE_NUM; i++) 
    if ((nodedp[i].Id > 0)&&(nodedp[i].connected)) count ++;
  return count;
}


void nodedp_setvalue(uint16_t id, uint16_t distance, float ph, bool GWpumpNOn, bool N1pumpNOn,  bool isconnected)
{
//  Serial.print("NodeDP Setvalue, ID: ");
//  Serial.println(id);
  if (node_is_exist(id, TYPE_DP))
  {
    for (int i = 0; i< NODE_NUM; i++) if (nodedp[i].Id == id)
    {
      nodedp[i].setPH(ph);
      nodedp[i].setDistance(distance);
      nodedp[i].connected = isconnected;
      nodedp[i].GWPumpNeedOn = GWpumpNOn;
      nodedp[i].N1PumpNeedOn = N1pumpNOn;

      nodedp[i].LastTimeGetMsg = millis();
      //node_print_infor_all();
      return;
    }
  }
}
//


void nodedp_add(uint16_t id, uint16_t distance, float ph, bool GWpumpNOn, bool N1pumpNOn,  bool isconnected)
{
//  Serial.print("Add ID: ");
//  Serial.println(id);
  
 if (node_is_exist(id, TYPE_DP))
  {
    for (int i = 0; i< NODE_NUM; i++) 
    {
      if (nodedp[i].Id == id)
      {
        nodedp[i].setPH(ph);
        nodedp[i].setDistance(distance);
        nodedp[i].connected = isconnected;
        nodedp[i].GWPumpNeedOn = GWpumpNOn;
        nodedp[i].N1PumpNeedOn = N1pumpNOn;
        nodedp[i].LastTimeGetMsg = millis();
        //node_print_infor_all();
        return;
      }
    }
  }
  else
  {
    for (int i = 0; i< NODE_NUM; i++) 
    {
      if (nodedp[i].Id == 0)
      {
        nodedp[i].Id = id;
        nodedp[i].setPH(ph);
        nodedp[i].setDistance(distance);
        nodedp[i].connected = isconnected;
        nodedp[i].GWPumpNeedOn = GWpumpNOn;
        nodedp[i].N1PumpNeedOn = N1pumpNOn;
        nodedp[i].LastTimeGetMsg = millis();
        //node_print_infor_all();
        return;
      }
    }
  }
}

void nodedp_remove(uint16_t id)
{
  for (int i = 0; i< NODE_NUM; i++) 
  if (nodedp[i].Id == id)
  {
    nodedp[i].remove();
    return;
  }
}


void node_check_alive(void)
{
  for (int i = 0; i< NODE_NUM; i++)
  {
    if (nodeth[i].Id != 0)
    {
      if (millis() - nodeth[i].LastTimeGetMsg >= 30000) // 30s Lost connect to node
      {
        nodeth[i].connected = false;
      }

//      if (millis() - nodeth[i].LastTimeGetMsg >= (3 * 60 * 1000)) // 3minute Lost connect -> remove node
//      {
//        nodeth[i].remove();
//      }
    }

    if (nodedp[i].Id != 0)  
    {
      if (millis() - nodedp[i].LastTimeGetMsg >= 30000) // 30s Lost connect to node
      {
        nodedp[i].connected = false;
      }

//      if (millis() - nodedp[i].LastTimeGetMsg >= (3 * 60 * 1000)) // 3minute Lost connect -> remove node
//      {
//        nodedp[i].remove();
//      }
    }
  }
}
//


bool node_isany_needGWpumpon(void)
{
  for (int i = 0; i< NODE_NUM; i++)
  {
    if ((nodeth[i].Id> 0)&&(nodeth[i].GWPumpNeedOn)) return true;
    if ((nodedp[i].Id> 0)&&(nodedp[i].GWPumpNeedOn)) return true;
  }
  return false;
}
//


bool node_isany_needN1pumpon(void)
{
  for (int i = 0; i< NODE_NUM; i++)
  {
    if ((nodedp[i].Id> 0)&&(nodedp[i].N1PumpNeedOn)) return true;
  }
  return false;
}
//

void nodeth_setallpump(bool pumpcmd)
{
  for (int i = 0; i< NODE_NUM; i++) 
  if (nodeth[i].Id> 0)
  {
    nodeth[i].PumpCmd = pumpcmd;
  }
}
