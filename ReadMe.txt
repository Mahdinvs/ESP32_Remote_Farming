Warning: Unlike arduino board, you don't need to define SerialClock & SerialData pins for AHT10 humidity and temperature sensord or OLED monitor.

1- Connect LDR to D4 pin of ESP32 or any other analogue input and change value in code
2- ESP32 default SCL is D22 and SDA is D21 or you can change ddefaults with a simple search in Google or even better, ask CHATGPT :D
3- Connect Sim800 Rx to UART Tx2 (D17) , and Tx to UART Rx2 (D16) OR you may connect Tx to UART Rx0 (D3) and Rx to UART Tx0 (D1), you should change: "HardwareSerial sim800(2);" to this:
"HardwareSerial sim800(0);"
Warning: I DIDN'T test this one, so if you want to change to this, have a copy of my *.ino file somewhere!
4- You can connect LEDs with Resistor for output, or better job is to use relay driver circuit (I used 5V Relays, but you can use higher voltage relays if you want) or use a multiple relay module which has optocoupler on it and
you don't need to design seperate circuit for it.

LDR or Photocell = D4
Valve = D18
Cooler = D5
Steam = D1 (Or Rx0)
Light = D23
Heater = D13

5- Be careful not to damage sim800 with over 4.4 V supply, best way is to use a 12 V adapter and LM2596 DC to DC convertor.
