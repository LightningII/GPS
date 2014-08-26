#include <string.h>
#include <ctype.h>

int i=0;
unsigned char count = 0;
char rxPin = 0;
char txPin = 1;
char incoming[300];
char* query_base = "$PSRF103";

// @param for <msg>
#define GGA 0
#define GLL 1
#define GSA 2
#define GSV 3
#define RMC 4
#define VTG 5
#define MSS 6
#define ZDA 8

// @param for <mode>
#define SETRATE 0
#define QUERY 1
#define ABP_ON 2
#define ABP_OFF 3

// @param for <checksum>
#define disable 0
#define enable 1

struct  {
  char msg;
  char mode;
  char rate;
  char cksumEnable;
}Query;

struct  {
  const int number_of_sector = 15;
  int UTC_hour;
  int UTC_minute;
  int UTC_seconds;
  float Latitude;
  char Direction_NS;
  float Longitude;
  char Direction_EW;  
  int satellites_used;
  float MSL_altitude;
  int Position_Fix_Indicator;
  float Geoid_separation;
}GPS_Data;

void setup()  {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(4800);
  Serial.write(gps_query(GGA, QUERY, 1, disable));
  for(int j=0; j<300; j++)
    incoming[j] = ' ';
}

void loop()  {
  i=0;
  if(Serial.available() > 0)  {
    *(incoming+i) = Serial.read();
    i++;
    if((*incoming+i) == '\n')  {
      gps_decode(incoming);
      i = 0;
    }
  }
  
}

char* gps_query(unsigned char msg, unsigned char mode, unsigned char rate, unsigned char cksumEnable)  {
  char* query_str = query_base;
  Query.msg = msg;
  Query.mode = mode;
  Query.rate = rate;
  Query.cksumEnable = cksumEnable;
  
  sprintf(query_str, "%s,%2.0d", query_str, Query.msg);
  sprintf(query_str, "%s,%2.0d", query_str, Query.mode);
  sprintf(query_str, "%s,%2.0d", query_str, Query.rate);
  sprintf(query_str, "%s,%2.0d", query_str, Query.cksumEnable);
  
  return query_str;
}

void gps_decode(char* str)  {
  unsigned char BUFFER[10]={0};
  int h=0;
  count=0;
  while(str++)  {
    if(*str==",")
      count++; 
    switch(count)  {
     case 1: {*(BUFFER+h) = *str; h++; break;}
     case 2: {convert_hour(BUFFER); h=0; break;}
     case 3: GPS_Data.Direction_NS = *str; break;
     case 5: GPS_Data.Direction_EW = *str; break;
    }
    i++;
  }
}

void convert_hour(char* ptr)  {
  char buf[2];
  buf[0] = ptr[0];
  buf[1] = ptr[1];
  GPS_Data.UTC_hour = atoi(buf); 
  buf[0] = ptr[2];
  buf[1] = ptr[3];
  GPS_Data.UTC_minutes = atoi(buf); 
  buf[0] = ptr[4];
  buf[1] = ptr[5];
  GPS_Data.UTC_seconds = atoi(buf); 
}

