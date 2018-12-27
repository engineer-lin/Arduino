#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
const int TrigPin = 2; 
const int EchoPin = 3; 
float distance;
RH_RF95 rf95;
float frequency = 868.0;  //frequency settings
String datastring1="";
String datastring2="";
char distance_1[8]={"\0"};   
char *node_id = "<5678>";  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here. 
uint8_t datasend[36];
unsigned int count = 1; 

void setup()
{
      Serial.begin(9600);
      pinMode(TrigPin, OUTPUT); 
      pinMode(EchoPin, INPUT);
      Serial.println(F("Start MQTT Example"));
          if (!rf95.init())
      Serial.println(F("init failed"));
         rf95.setFrequency(frequency);
         rf95.setTxPower(13);
}

void distance_read()
{
      digitalWrite(TrigPin, LOW); //低高低电平发一个短时间脉冲去TrigPin 
      delayMicroseconds(2); 
      digitalWrite(TrigPin, HIGH); 
      delayMicroseconds(10); 
      digitalWrite(TrigPin, LOW); 

      distance = pulseIn(EchoPin, HIGH) / 58.0; //将回波时间换算成cm 
      distance = (int(distance * 100.0)) / 100.0; //保留两位小数 
      Serial.print("Distance is ");
      Serial.print(distance); 
      Serial.print("cm"); 
      Serial.println(); 
      delay(1000);
}
void distance_write()
{
    char data[50] = "\0";
    for(int i = 0; i < 50; i++)
    {
       data[i] = node_id[i];
    }

    dtostrf(distance,0,1,distance_1);
  
    // Serial.println(distance_1);
     strcat(data,"field1=");
     strcat(data,distance_1);
     strcpy((char *)datasend,data);
     
   //Serial.println((char *)datasend);
    //Serial.println(sizeof datasend);
      
}

void SendData()
{
    //  Serial.println(F("Sending data to LG01"));
           
   
      rf95.send((char *)datasend,sizeof(datasend));  
      rf95.waitPacketSent();  // Now wait for a reply
    
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);

     if (rf95.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
     
      Serial.print("got reply from LG01: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);    
  }
    else
    {
     Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is LoRa server running?");
  }
  delay(5000);
}    
    


void loop()
{
    Serial.print("###########    ");
    Serial.print("COUNT=");
    Serial.print(count);
    Serial.println("    ###########");
     count++;
     distance_read();
     distance_write();
     SendData();
}

