// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// ------------------------------------------------------------------------------------------------- module
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --                                                                                                    --
// --                                                                                                    --
// --             "esp8266_key"                                                                          --
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
// --             (1) KP (keypress) - контроль нажатия встроенной кнопки "FLASH"                         --
// --                 она подключена к земле GPIO0. её можно использовать как обычную кнопку.            --
// --                 если программно подтянуть вывод GPIO0 с помощью внутреннего подтягивающего         --
// --                 резистора к высокому уровню, то появление LOW на этом выводе означает НАЖАТИЕ.     --
// --                                                                                                    --
// --             (2) при коротком нажатии - в UART выводится строка "short click"                       --
// --                 при долгом нажатии   - в UART выводится строка "long click"                        --
// --                                                                                                    --
// --             (3) справка о контакте контроллера/платы для кнопки "FLASH"                            --
// --                 15 (IO0, GPIO0, SPICS2) - в режиме INPUT_PULLUP                                    --
// --                                                                                                    --
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
// --------------------------------------------------------------------------------------------------------
// -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

#include <Arduino.h>

class key_press

{

public:
    void run()
    {
        ///////////////////////////////////////////////////////////////////////////////////////
        // ------------------------------------------------------------------------------------
        //                                                        РЕГУЛЯРНО ИСПОЛНЯЕМЫЙ ПРОЦЕСС
        // контроль нажатия встроенной кнопки                                          keypress
        // опрос, обработка состояния встроенной кнопки FLASH (IO0, GPIO0, SPICS2) - 15
        // ------------------------------------------------------------------------------------
        ///////////////////////////////////////////////////////////////////////////////////////

        static uint32_t tmr;      // таймер кнопки
        static bool flag = false; // кнопка. активация
        static bool state;        // кнопка. состояние

        state = !digitalRead(15); // кнопка. состояние

        // ----------------------------------------------------------------------------
        // нажатие кнопки
        // интервал 100 мс для защиты от дребезга
        // ----------------------------------------------------------------------------

        if (state && !flag && millis() - tmr > 100)

        {
            flag = true;
            tmr = millis();
            Serial.println("short click");
        }

        // ----------------------------------------------------------------------------
        // нажатие кнопки
        // интервал 500 мс для контроля удержания
        // ----------------------------------------------------------------------------

        if (state && flag && millis() - tmr > 500)

            Serial.println("long click");

        // ----------------------------------------------------------------------------
        // отпускание кнопки
        // интервал 100 мс для защиты от дребезга
        // ----------------------------------------------------------------------------

        if (!state && flag && millis() - tmr > 100)

        {
            flag = false;
            tmr = millis();
        }
    }

private:
};
