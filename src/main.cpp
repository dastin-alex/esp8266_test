// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// ------------------------------------------------------------------------------------------------- module
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --                                                                                                    --
// --                                                                                                    --
// --             "main"                                                                                 --
// --                                                                                                    --
// --                                                                                                    --
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// ---------------------------------------------------------------------------------------------- component
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --                                                                                                    --
// --                                                                                                    --
// -- title       : esp8266_test                                                                         --
// -- version     : 2.0                                                                                  --
// -- framework   : arduino                                                                              --
// -- author      : goldobin a.                                                                          --
// -- company     : cardboard.ru                                                                         --
// -- data        : 08/09/2022                                                                           --
// --                                                                                                    --
// --                                                                                                    --
// -- description :                                                                                      --
// --                                                                                                    --
// --                                                                                                    --
// --             (1) осуществляется ОДНОВРЕМЕННОЕ выполнение трёх потоков задач                         --
// --                 каждому потоку задач соответствует отдельный ПРОЦЕСС                               --
// --                                                                                                    --
// --                 BL (blink)    - непрерывная световая сигнализация                                  --
// --                 KP (keypress) - контроль нажатия встроенной кнопки                                 --
// --                 TS (timescan) - периодический вывод в UART значения времени                        --
// --                                                                                                    --
// --             (2) BL (blink) - это ПРОЦЕСС непрерывной световой сигнализации.                        --
// --                 для сигнализации используется единственный встроенный светодиод LED_BUILTIN.       --
// --                 бесконечно исполняемый цикл описывается таблицей                                   --
// --                                                                                                    --
// --                 Задача       Интервал        LED_BUILTIN                                           --
// --                 BL01         20 мс           HIGH                                                  --
// --                 BL02         300 мс          LOW                                                   --
// --                 BL03         20 мс           HIGH                                                  --
// --                 BL04         300 мс          LOW                                                   --
// --                 BL05         500 мс          HIGH                                                  --
// --                 BL06         500 мс          LOW                                                   --
// --                 BL07         500 мс          HIGH                                                  --
// --                 BL08         500 мс          LOW                                                   --
// --                                                                                                    --
// --             (3) задача BL00 исполняются однократно.                                                --
// --                 поток задач BL01 .. BL08 исполняются непрерывно.                                   --
// --                                                                                                    --
// --             (4) KP (keypress) - контроль нажатия встроенной кнопки "FLASH"                         --
// --                 она подключена к земле GPIO0. её можно использовать как обычную кнопку.            --
// --                 если программно подтянуть вывод GPIO0 с помощью внутреннего подтягивающего         --
// --                 резистора к высокому уровню, то появление LOW на этом выводе означает НАЖАТИЕ.     --
// --                                                                                                    --
// --                 при коротком нажатии - в UART выводится строка "short click"                       --
// --                 при долгом нажатии   - в UART выводится строка "long click"                        --
// --                                                                                                    --
// --                 справка о контакте контроллера/платы для кнопки "FLASH"                            --
// --                 15 (IO0, GPIO0, SPICS2) - в режиме INPUT_PULLUP                                    --
// --                                                                                                    --
// --             (5) БИБЛИОТЕКИ, сопряжённые с проектом                                                 --
// --                                                                                                    --
// --                         Arduino.h                                                                  --
// --                         ESP8266WiFi.h                                                              --
// --                         WiFiUdp.h                                                                  --
// --                                                                                                    --
// --             (6) МЕТОДЫ, которые вызываются из сопряжённых с проектом библиотек                     --
// --                                                                                                    --
// --                         Serial.begin          начальная настройка модуля UART                      --
// --                         Serial.println        вывод строки в UART                                  --
// --                         Serial.print          вывод символов в UART                                --
// --                         WiFi.begin            регистрация в сети WIFI                              --
// --                         WiFi.mode             начальная настройка модуля WIFI                      --
// --                         WiFi.status           состояние соединения WiFi                            --
// --                         WiFi.scanNetworks     поиск активных точек подключения WIFI                --
// --                         WiFi.hostByName       выбор IP адреса хоста по имени                       --
// --                         WiFi.disconnect       отключение модуля WIFI                               --
// --                         udp.begin             начало работы с протоколом UDP                       --
// --                         udp.read              чтение кадра UDP                                     --
// --                         udp.write             запись данных в кадр UDP                             --
// --                         udp.beginPacket       начало кадра UDP                                     --
// --                         udp.endPacket         конец кадра UDP                                      --
// --                         udp.parsePacket       синтаксический разбор кадра UDP                      --
// --                                                                                                    --
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --------------------------------------------------------------------------------------------------------
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

#include <Arduino.h>

#include "esp8266_test.h"
#include "esp8266_timescan.h"
#include "esp8266_control.h"

#define BUTTON 15

time_scan TS;
task_control<3> TC;

// ---------------------------------------------------------------------- delay 3000
//                                                                       ЗАДАЧА
// периодический запрос точного времени на сервере NTP.
// ---------------------------------------------------------------------- public

void ntp_time()

{

  TS.task_ts05();
  TS.task_ts07(TS.task_ts06());
}

void setup()

{

  pinMode(LED_BUILTIN, OUTPUT);  // светодиод. режим OUTPUT
  pinMode(BUTTON, INPUT_PULLUP); // кнопка. режим INPUT_PULLUP

  TC.attach(0, ntp_time, 3000);

  bool no_wifi = TS.task_ts02(TS.task_ts01());
  bool no_connect = TS.task_ts04(TS.task_ts03());

  if (no_wifi || no_connect)
    TC.stop(0);
}

void loop()

{

  TC.tick();
}

///////////////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------------------
//                                                        РЕГУЛЯРНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
// непрерывная световая сигнализация                                              blink
// управление встроенным светодиодом LED_BUILTIN
// ------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------------------

// static bool mode_led = false; // светодиод. переключение серий (0 - 30/600, 1 - 500/500)
// static bool flag_led = false; // светодиод. только в серии 0. переключение интервалов
// static int count_led = 0;     // светодиод. счётчик повторений в серии

// if ((!mode_led && (count_led >= serial1_led)) || (mode_led && (count_led >= serial2_led)))

// ----------------------------------------------------------------------------
// обновление настроек
// при достижении счётчиком серии предельного значения
// ----------------------------------------------------------------------------

// {
//   mode_led = !mode_led;
//   flag_led = false;
//   count_led = 0;
// }
// else

//     if (mode_led)

// ----------------------------------------------------------------------------
// серия 2
// переключение состояния светодиода при mode_led = 1 (500/500)
// ----------------------------------------------------------------------------

// {
//   if (millis() - tmr_led >= 500)
//   {
//     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     tmr_led = millis();
//     count_led++;
//   }
// }

// else

// ----------------------------------------------------------------------------
// серия 1
// переключение состояния светодиода при mode_led = 0 (30/600)
// ----------------------------------------------------------------------------

// {
//   if ((millis() - tmr_led) >= (flag_led ? 20 : 300))
//   {
//     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     tmr_led = millis();
//     flag_led = !flag_led;
//     count_led++;
//   }
// }

///////////////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------------------
//                                                        РЕГУЛЯРНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
// контроль нажатия встроенной кнопки                                          keypress
// опрос, обработка состояния встроенной кнопки FLASH (IO0, GPIO0, SPICS2) - 15
// ------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////

// static uint32_t tmr_key;      // таймер кнопки
// static bool flag_key = false; // кнопка. активация
// static bool state_key;        // кнопка. состояние

// state_key = !digitalRead(15); // кнопка. состояние

// ----------------------------------------------------------------------------
// нажатие кнопки
// интервал 100 мс для защиты от дребезга
// ----------------------------------------------------------------------------

// if (state_key && !flag_key && millis() - tmr_key > 100)

// {
//  flag_key = true;
//  tmr_key = millis();
//  Serial.println("short click");
// }

// ----------------------------------------------------------------------------
// нажатие кнопки
// интервал 500 мс для контроля удержания
// ----------------------------------------------------------------------------

// if (state_key && flag_key && millis() - tmr_key > 500)

//  Serial.println("long click");

// ----------------------------------------------------------------------------
// отпускание кнопки
// интервал 100 мс для защиты от дребезга
// ----------------------------------------------------------------------------

// if (!state_key && flag_key && millis() - tmr_key > 100)

// {
//  flag_key = false;
//  tmr_key = millis();
// }
