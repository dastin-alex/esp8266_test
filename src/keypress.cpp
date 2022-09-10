// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// ------------------------------------------------------------------------------------------------- module
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --                                                                                                    --
// --                                                                                                    --
// --             "keypress.cpp"                                                                         --
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
// --             (1) keypress.cpp - контроль нажатия встроенной кнопки                                  --
// --                                                                                                    --
// --             (2) ВСТРОЕННАЯ КНОПКА "FLASH" используются для контроля нажатия.                       --
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
// --             (3) ФАЙЛЫ, сопряжённые с проектом                                                      --
// --                 esp8266_test.h - заголовок, объявление функций и макроопределений проекта          --
// --                 main.cpp       - корневой исполняемый файл, содержит функции-процессы setup, loop  --
// --                 blink.cpp      - исполняемый файл, содержит функцию blink                          --
// --                 keypress.cpp   - исполняемый файл, содержит функцию keypress                       --
// --                 timescan.cpp   - исполняемый файл, содержит функцию timescan                       --
// --                                                                                                    --
// --             (4) БИБЛИОТЕКИ, сопряжённые с проектом                                                 --
// --                                                                                                    --
// --             (5) МЕТОДЫ, которые вызываются из сопряжённых с проектом библиотек                     --
// --                 начальная настройка модуля UART                               Serial.begin         --
// --                                                                                                    --
// --                                                                                                    --
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --------------------------------------------------------------------------------------------------------
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

#include "keypress.h"

///////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
//                                                                      ФУНКЦИЯ
// контроль нажатия встроенной кнопки                                  keypress
// ----------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

void keypress()

{
}
