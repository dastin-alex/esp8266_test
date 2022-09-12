#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//                             НАСТРОЙКИ БИБЛИОТЕК, ПРОЦЕССОВ, МЕТОДОВ, ФУНКЦИЙ
// глобальные определения
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

#ifndef STASSID
#define STASSID "TP-LINK_70CCF2"
#define STAPSK "19598932"
#endif

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// КОНСТАНТЫ
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

const char *ssid = STASSID;                      // имя сети WIFI
const char *pass = STAPSK;                       // пароль сети WIFI
const char *ntpServerName = "time.nist.gov";     // DNS-адрес NTP сервера time.nist.gov
const int NTP_PACKET_SIZE = 48;                  // размер пакета 48 байт
const unsigned long seventyYears = 2208988800UL; // количество секунд в 70 годах
const unsigned long secsDay = 86400L;            // количество секунд в одном дне
const int serial1_led = 2;                       // светодиод. количество повторений серии 0 (30/600)
const int serial2_led = 2;                       // светодиод. количество повторений серии 1 (500/500)

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// ПЕРЕМЕННЫЕ
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

unsigned int localPort = 2390;      // контроллер NTP. локальный порт для приёма UDP пакетов
IPAddress timeServerIP;             // контроллер NTP. IP-адрес NTP сервера time.nist.gov
byte packetBuffer[NTP_PACKET_SIZE]; // контроллер NTP. буфер для хранения принятых и исходящих пакетов
WiFiUDP udp;                        // контроллер NTP. экземпляр контроллера UDP для приёма/передачи UDP-пакетов
int cb;                             // контроллер NTP. длина принятого пакета
unsigned long highWord;             // контроллер NTP. буфер хранения байтов 40,41 принятого пакета
unsigned long lowWord;              // контроллер NTP. буфер хранения байтов 42,43 принятого пакета
unsigned long secsSince1900;        // контроллер NTP. время NTP. количество секунд, прошедших от даты Jan 1 1900
unsigned long epoch;                // контроллер NTP. время UNIX. количество секунд, прошедших от даты Jan 1 1970
unsigned long tdelta = 3;           // контроллер NTP. время GMT+3. количество часов смещения от GMT
unsigned long gmt3;                 // контроллер NTP. время GMT+3. время UNIX + смещение от GMT
int thrs, tmin, tsec;               // контроллер NTP. время GMT+3. количество часов, минут, секунд

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// ПРОГРАММНЫЕ ТАЙМЕРЫ
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

static uint32_t tmr_led, tmr_key;   // таймеры светодиода и кнопки
static uint32_t tmr_ts09, tmr_ts19; // таймеры задач потока timescan
