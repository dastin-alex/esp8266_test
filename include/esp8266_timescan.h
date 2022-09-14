// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// ------------------------------------------------------------------------------------------------- module
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --                                                                                                    --
// --                                                                                                    --
// --             "esp8266_timescan"                                                                     --
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
// --             (1) TS (timescan) - это задача периодического вывода в UART значения времени.          --
// --                 Клиент UDP/NTP. Считывание значения времени на сервере по протоколу NTP.           --
// --                 Обмен осуществляется с помощью сетевых пакетов UDP.                                --
// --                 Подробная справка о серверах времени NTP и необходимом формате сообщений           --
// --                 http://en.wikipedia.org/wiki/Network_Time_Protocol                                 --
// --                                                                                                    --
// --             (2) поток задач TS00 .. TS04 исполняются однократно.                                   --
// --                 поток задач TS05 .. TS07 исполняются непрерывно. через каждые 3 сек                --
// --                                                                                                    --
// --             (3) не следует явно указывать IP-адрес, иначе нет преимуществ пула.                    --
// --                 вместо этого указывается IP-адрес, соответстующий имени хоста.                     --
// --                 пример явного задания IP-адреса (NTP сервер time.nist.gov):                        --
// --                 IPAddress timeServer(129, 6, 15, 28);                                              --
// --                                                                                                    --
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --------------------------------------------------------------------------------------------------------
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#define STASSID "TP-LINK_70CCF2"
#define STAPSK "19598932"
#define ATTEMPTS 20

const char *ssid = STASSID;                      // имя сети WIFI
const char *pass = STAPSK;                       // пароль сети WIFI
const unsigned int localPort = 2390;             // локальный порт для приёма UDP пакетов
const char *ntpServerName = "time.nist.gov";     // DNS-адрес NTP сервера time.nist.gov
const int NTP_PACKET_SIZE = 48;                  // размер пакета 48 байт
const unsigned long seventyYears = 2208988800UL; // количество секунд в 70 годах
const unsigned long secsDay = 86400L;            // количество секунд в одном дне

class time_scan

{

public:
    // ----------------------------------------------------------------------------
    //                                                                        МЕТОД : ЗАДАЧА TS01
    // включение, настройка UART
    // начало печати отчёта. режим station mode модуля WiFi
    // сканирование точек доступа WiFi.
    // флаг no_wifi - количество имеющихся сетей
    // ---------------------------------------------------------------------- public

    int task_ts01()

    {

        Serial.begin(9600); // МЕТОД Serial.begin : скорость 9600 бод
        Serial.println();   // МЕТОД Serial.println

        WiFi.mode(WIFI_STA);        // МЕТОД WiFi.mode  : режим station mode
        WiFi.disconnect();          // МЕТОД WiFi.disconnect : отключение модуля WIFI
        return WiFi.scanNetworks(); // МЕТОД WiFi.scanNetworks : поиск активных точек подключения WIFI
    }

    // ----------------------------------------------------------------------------
    //                                                                        МЕТОД : ЗАДАЧА TS02
    // печать отчёта в UART.
    // перечисляются SSID имеющихся сетей WiFi.
    // ---------------------------------------------------------------------- public

    bool task_ts02(int networks)

    {

        if (!networks)
        {

            Serial.println("no networks found"); // МЕТОД Serial.println
            WiFi.disconnect();                   // МЕТОД WiFi.disconnect : отключение модуля WIFI
        }

        else

            for (
                int i = 0;
                i < networks;
                ++i)

            {

                Serial.print(i + 1);          // МЕТОД Serial.print : номер в списке
                Serial.print(" : ");          // МЕТОД Serial.print
                Serial.println(WiFi.SSID(i)); // МЕТОД Serial.println : значение i-ого WiFi.SSID
            }

        return networks ? false : true;
    }

    // ----------------------------------------------------------------------------
    //                                                                        МЕТОД : ЗАДАЧА TS03
    // цикл опроса состояния соединения.
    // ПОКА состояние соединения != WL_CONNECTED: ТО пауза ожидания 500
    // ---------------------------------------------------------------------- public

    int task_ts03()

    {

        int nc = ATTEMPTS;
        //  int *pc = &nc;

        Serial.println();            // МЕТОД Serial.println
        Serial.print("connect to "); // МЕТОД Serial.print
        Serial.println(ssid);        // МЕТОД Serial.println : значение SSID

        WiFi.begin(ssid, pass); // МЕТОД WiFi.begin : регистрация в сети WIFI

        while (nc && (WiFi.status() != WL_CONNECTED)) // WiFi.status : состояние соединения

            task_ts31(&nc);

        Serial.println(); // МЕТОД Serial.println
        Serial.println(); // МЕТОД Serial.println
        return nc;
    }

    // ----------------------------------------------------------------------------
    //                                                                        МЕТОД : ЗАДАЧА TS04
    // печать отчёта в UART.
    // информация о подключении к сети WIFI и настройке протокола UDP
    // ---------------------------------------------------------------------- public

    bool task_ts04(int nc)

    {

        udp.begin(localPort); // МЕТОД udp.begin : начало работы с протоколом UDP

        if (!nc)
        {

            Serial.println();
            Serial.println("no WiFi connection"); // МЕТОД Serial.println
            WiFi.disconnect();                    // МЕТОД WiFi.disconnect : отключение модуля WIFI
        }

        else

        {

            Serial.println("");               // МЕТОД Serial.println
            Serial.println("WIFI connected"); // МЕТОД Serial.println
            Serial.print("IP address: ");     // МЕТОД Serial.print
            Serial.println(WiFi.localIP());   // МЕТОД Serial.println, WiFi.localIP
            Serial.print("Local port: ");     // МЕТОД Serial.print
            Serial.println(udp.localPort());  // МЕТОД Serial.println, udp.localPort
            Serial.println();                 // МЕТОД Serial.println
        }

        Serial.println(); // МЕТОД Serial.println
        return nc ? false : true;
    }

    // ----------------------------------------------------------------------------
    //                                                                        МЕТОД : ЗАДАЧА TS05
    // подключение к сети WIFI.
    // случайный выбор сервера из пула NTP
    // начальная установка битовых полей пакета запроса времени NTP.
    // установка необходимых байтов буфера пакета.
    // байты 4 .. 11 (8 байтов) установлены в 0.
    // отправка пакета NTP в выбранный сервер времени.
    // используется порт 123.
    // пауза ожидания ответа севера 1000
    // ---------------------------------------------------------------------- public

    void task_ts05()

    {

        WiFi.hostByName(ntpServerName, timeServerIP); // МЕТОД WiFi.hostByName : выбор хоста по имени

        memset(packetBuffer, 0, NTP_PACKET_SIZE);

        packetBuffer[0] = 0b11100011; // LI, версия, режим
        packetBuffer[1] = 0;          // уровень или тип часов
        packetBuffer[2] = 6;          // интервал
        packetBuffer[3] = 0xEC;       // точность (Peer Clock Precision)

        packetBuffer[12] = 49;
        packetBuffer[13] = 0x4E;
        packetBuffer[14] = 49;
        packetBuffer[15] = 52;

        udp.beginPacket(timeServerIP, 123);       // МЕТОД udp.beginPacket : начало кадра UDP
        udp.write(packetBuffer, NTP_PACKET_SIZE); // МЕТОД udp.write : запись данных в кадр UDP
        udp.endPacket();                          // МЕТОД udp.endPacket : конец кадра UDP
    }

    // ----------------------------------------------------------------------------
    //                                                                        МЕТОД : ЗАДАЧА TS06
    // синтаксический разбор пакета, полученного из сети WIFI.
    // ЕСЛИ длина пакета = 0  : ТО пакет UDP не принят
    // ЕСЛИ длина пакета != 0 : ТО пакет UDP получен
    // ---------------------------------------------------------------------- public

    int task_ts06(void)

    {

        return udp.parsePacket(); // МЕТОД udp.parsePacket : синтаксический разбор кадра UDP
    }

    // ----------------------------------------------------------------------------
    //                                                                        МЕТОД : ЗАДАЧА TS07
    // сохранение информации о времени из пакета в буфер.
    // длина битового поля информации о времени равна четырем байтам.
    // старшее слово - байты 40 .. 41 пакета
    // младшее слов0 - байты 42 .. 43 пакета
    // расчёт времени. число часов, минут, секунд времени GMT+3.
    // время NTP     - количество секунд, прошедших с даты Jan 1 1900.
    //                 одно четырёхбайтовое слово.
    // время UNIX    - количество секунд, прошедших с даты Jan 1 1970.
    //                 из времени NTP вычиется количество секунд в 70 годах.
    // время GMT+3.  - соответствует времени UNIX в Москве.
    //                 к времени UNIX прибавляется количество секунд в трёх часах
    // печать в UART отчёта о времени. применяется формат HH:MM:SS
    // время GMT     - соответствует меридиану Гринвича (Greenwich Meridian time).
    // время GMT+3   - соответствует текущему времени в Москве.
    // пауза ожидания 2 сек перед новым запросом времени.
    // ---------------------------------------------------------------------- public

    void task_ts07(int flag)

    {

        if (flag)

        {
            udp.read(packetBuffer, NTP_PACKET_SIZE); // МЕТОД udp.read : чтение кадра UDP

            highWord = word(packetBuffer[40], packetBuffer[41]);
            lowWord = word(packetBuffer[42], packetBuffer[43]);
            secsSince1900 = highWord << 16 | lowWord;

            epoch = secsSince1900 - seventyYears;
            gmt3 = epoch + tdelta * 3600;

            thrs = int((gmt3 % secsDay) / 3600);
            tmin = int((gmt3 % 3600) / 60);
            tsec = int(gmt3 % 60);

            Serial.print("NTP time   = ");   // МЕТОД Serial.print
            Serial.println(secsSince1900);   // МЕТОД Serial.println
            Serial.print("GMT time   = ");   // МЕТОД Serial.print
            Serial.println(epoch);           // МЕТОД Serial.println
            Serial.print("GMT+3 time = ");   // МЕТОД Serial.print
            Serial.println(gmt3);            // МЕТОД Serial.println
            Serial.print("Moscow time is "); // МЕТОД Serial.print

            if (thrs < 10)

                Serial.print('0'); // МЕТОД Serial.print

            Serial.print(thrs); // МЕТОД Serial.print
            Serial.print(':');  // МЕТОД Serial.print

            if (tmin < 10)

                Serial.print('0'); // МЕТОД Serial.print

            Serial.print(tmin); // МЕТОД Serial.print
            Serial.print(':');  // МЕТОД Serial.print

            if (tsec < 10)

                Serial.print('0'); // МЕТОД Serial.print

            Serial.println(tsec); // МЕТОД Serial.println
            Serial.println();     // МЕТОД Serial.println
        }
    }

private:
    WiFiUDP udp;                        // экземпляр контроллера UDP для приёма/передачи UDP-пакетов
    IPAddress timeServerIP;             // IP-адрес NTP сервера time.nist.gov
    byte packetBuffer[NTP_PACKET_SIZE]; // буфер для хранения принятых и исходящих пакетов
    unsigned long highWord;             // буфер хранения байтов 40,41 принятого пакета
    unsigned long lowWord;              // буфер хранения байтов 42,43 принятого пакета
    unsigned long secsSince1900;        // время NTP. количество секунд, прошедших от даты Jan 1 1900
    unsigned long epoch;                // время UNIX. количество секунд, прошедших от даты Jan 1 1970
    unsigned long tdelta = 3;           // время GMT+3. количество часов смещения от GMT
    unsigned long gmt3;                 // время GMT+3. время UNIX + смещение от GMT
    int thrs, tmin, tsec;               // время GMT+3. количество часов, минут, секунд

    // ---------------------------------------------------------------------- delay 500
    //                                                                        МЕТОД : ЗАДАЧА TS31
    // печать прогресса в UART.
    // ---------------------------------------------------------------------- private

    void task_ts31(int *cnt)

    {

        cnt--;
        Serial.print("."); // МЕТОД Serial.print
        delay(500);
    }
};