List of components:
1.SW 6x6mm 4 Pin x4
2.Buzzer
3.BUTTON 3x4mm 4 Pins x2
4.S8411-45R(BATTERY HOLDER COIN 12MM SMD)
5.0603 0.1uF x15
6.0603 10uF x6
7.DIODE SMD
8.Diode SMD
9.ESP32_WROOM
10.16P 2.0mm x8
11.USB Type-C SMD x3
12.MMC-SD CARD
13.C1815 FP-SOT23-IPC_C
14.0603 10K x9
15.0603 100K
16.0603 470
17.SPX3819M5-L-3-3 x2
18.DS3231SN
19.WS2812b x12
20.LCD SPI 13 Pins x6

The lcds and esp32 communicate via spi communication.
All spi signal and dc, rst in lcds are connected together, except cs signal
which lcd that you want to print on it, you need to active low that lcd's cs signal
I just can share you only those common information,
If you need more, please email to me via elecchannel.ec@gmail.com.