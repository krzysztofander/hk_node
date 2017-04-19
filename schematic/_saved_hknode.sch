EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA328P-AU U1
U 1 1 58EF8CF5
P 3800 2700
F 0 "U1" H 3050 3950 50  0000 L BNN
F 1 "ATMEGA328P-AU" H 4200 1300 50  0000 L BNN
F 2 "TQFP32" H 3800 2700 50  0001 C CIN
F 3 "" H 3800 2700 50  0001 C CNN
	1    3800 2700
	1    0    0    -1  
$EndComp
$Comp
L BSS806 Q1
U 1 1 58EF93F2
P 6500 1600
F 0 "Q1" H 6700 1675 50  0000 L CNN
F 1 "BSS806" H 6700 1600 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 6700 1525 50  0001 L CIN
F 3 "" H 6500 1600 50  0001 L CNN
	1    6500 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR2
U 1 1 58EF9B7B
P 6600 1900
F 0 "#PWR2" H 6600 1650 50  0001 C CNN
F 1 "GND" H 6600 1750 50  0000 C CNN
F 2 "" H 6600 1900 50  0001 C CNN
F 3 "" H 6600 1900 50  0001 C CNN
	1    6600 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR1
U 1 1 58EF9B93
P 2850 3950
F 0 "#PWR1" H 2850 3700 50  0001 C CNN
F 1 "GND" H 2850 3800 50  0000 C CNN
F 2 "" H 2850 3950 50  0001 C CNN
F 3 "" H 2850 3950 50  0001 C CNN
	1    2850 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 3950 2850 3700
Wire Wire Line
	2850 3700 2900 3700
Wire Wire Line
	2900 3800 2850 3800
Connection ~ 2850 3800
Wire Wire Line
	2900 3900 2850 3900
Connection ~ 2850 3900
Wire Wire Line
	6600 1900 6600 1800
$EndSCHEMATC
