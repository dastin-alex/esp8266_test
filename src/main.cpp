// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// ------------------------------------------------------------------------------------------------- module
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --                                                                                                    --
// --                                                                                                    --
// --             "esp8266_test"                                                                         --
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
// --             (1) ОДНОВРЕМЕННОЕ выполнение трёх задач                                                --
// --                 каждой задаче соответствует отдельный файл                                         --
// --                                                                                                    --
// --                 blink.cpp    - непрерывная световая сигнализация                                   --
// --                 keypress.cpp - контроль нажатия встроенной кнопки                                  --
// --                 timescan.cpp - периодический вывод в UART значения времени                         --
// --                                                                                                    --
// --             (2) СИГНАЛИЗАЦИЯ используется единственный встроенный светодиод LED_BUILTIN            --
// --                                                                                                    --
// --                 бесконечно исполняемый цикл описывается таблицей                                   --
// --                 Интервал        LED_BUILTIN                                                        --
// --                 20 мс           HIGH                                                               --
// --                 300 мс          LOW                                                                --
// --                 20 мс           HIGH                                                               --
// --                 300 мс          LOW                                                                --
// --                 500 мс          HIGH                                                               --
// --                 500 мс          LOW                                                                --
// --                 500 мс          HIGH                                                               --
// --                 500 мс          LOW                                                                --
// --                                                                                                    --
// --             (3) ВСТРОЕННАЯ КНОПКА "FLASH" используются для контроля нажатия.                       --
// --                 она подключена к земле GPIO0. её можно использовать как обычную кнопку.            --
// --                 если программно подтянуть вывод GPIO0 с помощью внутреннего подтягивающего         --
// --                 резистора к высокому уровню, то появление LOW на этом выводе означает НАЖАТИЕ.     --
// --                                                                                                    --
// --                 при коротком нажатии - в UART выводится строка "short"                             --
// --                 при долгом нажатии   - в UART выводится строка "long"                              --
// --                                                                                                    --
// --                 справка о полезном контакте контроллера/платы для кнопки "FLASH"                   --
// --                 15 (IO0, GPIO0, SPICS2) - в режиме INPUT_PULLUP                                    --
// --                                                                                                    --
// --             (1) timescan.cpp - периодический вывод в UART значения времени. Клиент UDP/NTP.        --
// --                 Считывание значения времени на сервере по протоколу NTP (Network Time Protocol).   --
// --                 Обмен осуществляется с помощью сетевых пакетов UDP.                                --
// --                 Подробная справка о серверах времени NTP и необходимом формате сообщений           --
// --                 http://en.wikipedia.org/wiki/Network_Time_Protocol                                 --
// --                                                                                                    --
// --             (2) КОНТРОЛЬ ВРЕМЕНИ исполняются непрерывно. через каждые 3 сек                        --
// --                 выполняется алгоритм                                                               --
// --                                                                                                    --
// --                 1 : соединение с сетью WiFi                          исполняется однократно        --
// --                 2 : запрос значения времени с сервера                исполняется периодически      --
// --                 3 : приём значения времени по UDP/NTP                исполняется периодически      --
// --                 4 : интерпретация принятого UDP пакета               исполняется периодически      --
// --                 5 : значение времени выводится в UART                исполняется периодически      --
// --                                                                                                    --
// --             (3) не следует явно указывать IP-адрес, иначе нет преимуществ пула.                    --
// --                 вместо этого указывается IP-адрес, соответстующий имени хоста.                     --
// --                 пример явного задания IP-адреса (NTP сервер time.nist.gov):                        --
// --                 IPAddress timeServer(129, 6, 15, 28);                                              --
// --                                                                                                    --
// --             (4) ФАЙЛЫ, сопряжённые с проектом                                                      --
// --                 esp8266_test.h - заголовок, объявление функций и макроопределений проекта          --
// --                 main.cpp       - корневой исполняемый файл                                         --
// --                 blink.cpp      - исполняемый файл, управление световой сигнализацией               --
// --                 keypress.cpp   - исполняемый файл, контроль нажатия кнопки                         --
// --                 timescan.cpp   - этот файл, печать в UART текущего времени                         --
// --                                                                                                    --
// --             (5) БИБЛИОТЕКИ, сопряжённые с проектом                                                 --
// --                 WiFiUdp.h                                                                          --
// --                                                                                                    --
// --             (6) МЕТОДЫ, которые вызываются из сопряжённых с проектом библиотек                     --
// --                 начальная настройка модуля UART                               Serial.begin         --
// --                 передача пакета UDP                                           sendPacket           --
// --                 приём пакета UDP                                              ReceivePacket        --
// --                                                                                                    --
// --             (5) ФАЙЛЫ, сопряжённые с проектом                                                      --
// --                 esp8266_test.h - заголовок, объявление функций и макроопределений проекта          --
// --                 main.cpp       - корневой исполняемый файл, содержит функции-процессы setup, loop  --
// --                 blink.cpp      - исполняемый файл, содержит функцию blink                          --
// --                 keypress.cpp   - исполняемый файл, содержит функцию keypress                       --
// --                 timescan.cpp   - исполняемый файл, содержит функцию timescan                       --
// --                                                                                                    --
// --             (6) БИБЛИОТЕКИ, сопряжённые с проектом                                                 --
// --                                                                                                    --
// --             (7) МЕТОДЫ, которые вызываются из сопряжённых с проектом библиотек                     --
// --                 начальная настройка модуля UART                               Serial.begin         --
// --                                                                                                    --
// --                 вывод строки в UART                                           Serial.println       --
// --                 вывод символов в UART                                         Serial.print         --
// --                 начальная настройка модуля WIFI                               WiFi.mode            --
// --                 отключение модуля WIFI                                        WiFi.disconnect      --
// --                 поиск активных точек подключения WIFI                         WiFi.scanNetworks    --
// --                                                                                                    --
// --                                                                                                    --
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --------------------------------------------------------------------------------------------------------
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

#include "esp8266_test.h"

void setup()

{

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  //                                               ОДНОКРАТНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
  // настройка UART
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  Serial.begin(9600); // МЕТОД Serial.begin : скорость 9600 бод
  Serial.println();   // МЕТОД Serial.println
  Serial.println();   // МЕТОД Serial.println

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  //                                               ОДНОКРАТНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
  // начальная настройка модуля WiFi.
  // подключение к сети по UDP
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  Serial.print("connect to "); // МЕТОД Serial.print
  Serial.println(ssid);        // МЕТОД Serial.println : значение SSID

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // включение модуля WIFI. режим station mode.
  // регистрация в сети WIFI
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  WiFi.mode(WIFI_STA);    // МЕТОД WiFi.mode  : режим station mode
  WiFi.begin(ssid, pass); // МЕТОД WiFi.begin : регистрация в сети WIFI

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // цикл опроса состояния соединения.
  // ПОКА состояние соединения != WL_CONNECTED: ТО пауза ожидания 500
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  while (WiFi.status() != WL_CONNECTED) // WiFi.status : состояние соединения

  {

    ///////////////////////////////////////////////////////////////////////////////
    // ---------------------------------------------------------------------- delay 500
    // пауза ожидания
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    delay(500);

    Serial.print("."); // МЕТОД Serial.print
  }

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // начало сеанса работы по протоколу UDP
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  udp.begin(localPort);

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // печать в UART отчёта.
  // информация о подключении к сети WIFI
  // информация о настройке протокола UDP
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  Serial.println("");               // МЕТОД Serial.println
  Serial.println("WIFI connected"); // МЕТОД Serial.println
  Serial.print("IP address: ");     // МЕТОД Serial.print
  Serial.println(WiFi.localIP());   // МЕТОД Serial.println, WiFi.localIP
  Serial.print("Local port: ");     // МЕТОД Serial.print
  Serial.println(udp.localPort());  // МЕТОД Serial.println, udp.localPort

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  //                                               ОДНОКРАТНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
  // настройка контактов
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  pinMode(LED_BUILTIN, OUTPUT); // контакт LED_BUILTIN             : режим OUTPUT
  pinMode(15, INPUT_PULLUP);    // контакт 15 (IO0, GPIO0, SPICS2) : режим INPUT_PULLUP
}

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//                                                              ФУНКЦИЯ-ПРОЦЕСС
// бесконечное исполнение                                                  loop
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void loop()

{

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  //                                                РЕГУЛЯРНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
  // периодический вывод в UART значения времени,                        timescan
  // полученного от сервера времени NTP
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // КОНСТАНТЫ
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  const unsigned long seventyYears = 2208988800UL; // количество секунд в 70 годах
  const unsigned long secsDay = 86400L;            // количество секунд в одном дне

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // ЛОКАЛЬНЫЕ ПЕРЕМЕННЫЕ
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  int cb;                      // длина принятого пакета
  unsigned long highWord;      // буфер хранения байтов 40,41 принятого пакета
  unsigned long lowWord;       // буфер хранения байтов 42,43 принятого пакета
  unsigned long secsSince1900; // время NTP. количество секунд, прошедших от даты Jan 1 1900
  unsigned long epoch;         // время UNIX. количество секунд, прошедших от даты Jan 1 1970
  unsigned long tdelta = 3;    // время GMT+3. количество часов смещения от GMT
  unsigned long gmt3;          // время GMT+3. время UNIX + смещение от GMT
  int thrs, tmin, tsec;        // время GMT+3. количество часов, минут, секунд

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // подключение к сети WIFI.
  // случайный выбор сервера NTP из пула
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  WiFi.hostByName(ntpServerName, timeServerIP);

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // отправка пакета NTP в выбранный сервер времени.
  // после этого ожидается ответ сервера на запрос
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  Serial.println("sending NTP packet...");

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // начальная установка битовых полей пакета запроса времени NTP.          шаг 1
  // установка всех байтов буфера пакета в 0.
  // NTP_PACKET_SIZE - количество байтов в пакете запроса
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // начальная установка битовых полей пакета запроса времени NTP.          шаг 2
  // установка необходимых байтов буфера пакета.
  // 8 байтов, 4 .. 11, соответствуют полям Root Delay и Root Dispersion
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  packetBuffer[0] = 0b11100011; // LI, версия, режим
  packetBuffer[1] = 0;          // уровень или тип часов
  packetBuffer[2] = 6;          // интервал
  packetBuffer[3] = 0xEC;       // точность (Peer Clock Precision)

  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // пакет запроса времени NTP сформирован.
  // передача пакета в порт 123.
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  udp.beginPacket(timeServerIP, 123);

  udp.write(packetBuffer, NTP_PACKET_SIZE);

  udp.endPacket();

  ///////////////////////////////////////////////////////////////////////////////
  // ---------------------------------------------------------------------- delay 1000
  // пауза ожидания ответа севера
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  delay(1000);

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // обработка результата, полученного из сети WIFI.
  // попытка разобрать пакет UDP
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  cb = udp.parsePacket();

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  // ЕСЛИ длина пакета = 0 : ТО пакет UDP не принят
  // ЕСЛИ длина пакета != 0 : ТО пакет UDP получен
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  if (!cb)

    Serial.println("no packet yet");

  else

  {

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // сохранение полученного пакета в буфер
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    udp.read(packetBuffer, NTP_PACKET_SIZE);

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // длина битового поля информации о времени равна четырем байтам (два слова).
    // начинается с 40-го байта принятого пакета.
    // старшее слово - байты 40 .. 41
    // младшее слов0 - байты 42 .. 43
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    highWord = word(packetBuffer[40], packetBuffer[41]);

    lowWord = word(packetBuffer[42], packetBuffer[43]);

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // время NTP. количество секунд, прошедших с даты Jan 1 1900
    // объединение четырех байтов в одно длинное целое число
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    secsSince1900 = highWord << 16 | lowWord;

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // время UNIX. количество секунд, прошедших с даты Jan 1 1970
    // вычитание из времени NTP количества секунд в 70 годах
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    epoch = secsSince1900 - seventyYears;

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // время GMT+3. значение времени UNIX в Москве
    // прибавление к времени UNIX количества секунд в tdelta часах
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    gmt3 = epoch + tdelta * 3600;

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // печать в UART информации о результатах расчёта времени
    // время GMT соответствует меридиану Гринвича (Greenwich Meridian time)
    // время GMT+3 соответствует текущему времени в Москве
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    Serial.print("packet length = ");
    Serial.println(cb);
    Serial.print("NTP time = ");
    Serial.println(secsSince1900);
    Serial.print("GMT time = ");
    Serial.println(epoch);
    Serial.print("GMT+3 time (Moscow) = ");
    Serial.println(gmt3);

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // расчёт числа часов, минут, секунд времени GMT+3.
    // применяется формат HH:MM:SS
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    thrs = int((gmt3 % secsDay) / 3600);

    tmin = int((gmt3 % 3600) / 60);

    tsec = int(gmt3 % 60);

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // печать в UART информации о количестве часов для времени GMT+3.
    // ЕСЛИ число часов < 10: ТО сначала необходимо напечатать '0'
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    Serial.print("Moscow time is ");

    if (thrs < 10)

      Serial.print('0');

    Serial.print(thrs);
    Serial.print(':');

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // печать в UART информации о количестве минут для времени GMT+3.
    // ЕСЛИ число минут < 10: ТО сначала необходимо напечатать '0'
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    if (tmin < 10)

      Serial.print('0');

    Serial.print(tmin);
    Serial.print(':');

    ///////////////////////////////////////////////////////////////////////////////
    // ----------------------------------------------------------------------------
    // печать в UART информации о количестве секунд для времени GMT+3.
    // ЕСЛИ число секунд < 10: ТО сначала необходимо напечатать '0'
    // ----------------------------------------------------------------------------
    ///////////////////////////////////////////////////////////////////////////////

    if (tsec < 10)

      Serial.print('0');

    Serial.println(tsec);
  }

  // ---------------------------------------------------------------------- delay 10000
  // пауза ожидания. перед новым запросом времени
  // ----------------------------------------------------------------------------

  delay(10000);

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  //                                                РЕГУЛЯРНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
  // непрерывная световая сигнализация                                      blink
  // управление встроенным светодиодом LED_BUILTIN
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  blink();

  ///////////////////////////////////////////////////////////////////////////////
  // ----------------------------------------------------------------------------
  //                                                РЕГУЛЯРНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
  // контроль нажатия встроенной кнопки                                  keypress
  // опрос встроенной кнопки FLASH (GPIO0)
  // ----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////

  keypress();
}
