# hk_node

Introduction
===============

This is the firmware for the Home Control node.
It is targeted for Arduino Nano

System description
====================

HW platform
---------------
The software should be run with the following HW platform:

@todo 

	image

Overview
----------
Software runs bare metal. 
In a main loop there are following operations:
- check for the command from serial,
- calculate time since last loop,
- execute operations scheduled by executor,
- go to power saving mode,

Serial is responding on commands send from master controller.
Commands are described in  `Commands` chapter

System time
------------
Up time is calculated basing on periodic interrupts on the 
watchdog timer. 
Watchdog is configured to raise interrupts every second.
In the future this may be made configurable.

Executor
----------

Executor have pre-defined number of tasks to execute.
Each task is invoked periodically with pre-defined or
configurable number of system time ticks (currently seconds)

Current up to version 0.9 there will be 2 executors:
- blinker
- temperature measurement

Version 1.0 and above will also have the executor
measuring battery level as well as 1-3 AC inputs.

The priority of executors is hard coded
highest - blinker
then - measurement of temperature 
then - battery measurement

Power saving mode
-------------------

There are 3 selectable power saving modes 
- IDLE
- STANBY
- POWER_DOWN

Refer to the ATMega documentation for reference
Default is the STANDBY. It does immediate recognition for serial commands.

In POWED_DOWN mode serial needs to be woken up (it depends on system oscillator 
which gets powered down). In this mode at least 1 character on serial is getting lost.

When serial command is not pending e.g. nothing is being send/read the
device goes to sleep and the watchdog's interrupts wakes it up.

When there is any character available in the receiver device does
not go to sleep up to receiving and handing full command.

@todo
	Add timeout
	
Serial nor power saving mode does not influence executor e.g. it executes tasks periodically
regardless of characters in receiver

Main Loop
-----------
Main loop executes functions one after another.
There is no pre-emption. 
Main loop first handles serial commands if any and then executor
When there is only part of command available main loop continues with executor 
e.g. does not wait with execution of task for full command.
@note
	There is only one executor task executed in a single loop. Ones its done device
	either goes to sleep or if there is another/same executor scheduled 
	loop round, handles serial, and executes a task belonging to highest
	priority executor. 

@warning
	Setting up the execution time to 0 will cause all lower priority
	executors to starve.

If a task takes longer than the system time tick it will not cause any starvation
as long as there will be period of time when lower priority executors could be 
invoked e.g. there will be gaps between particular executor activity

One Wire
-------------
Tho libraries are used 
OneWire
DS18B20

Communication
===============
Communication is build on Bluethooth 4.0 (BLE)


Commands
=========

Overview
---------
Commands are build with 3 letter ASCII command identifier followed by variable type and size data.
Data is always ASCII. Numbers are send as human readable hexadecimal notation (ASCII). 
Command may be prefixed by preamble:

Command structure:
	[preamble] 3_letter_cmd_it data termination_sign(s)

Commands are divided into categories. First letter of command prefix describes a 
category:
	- D: Debug
	- C: Configuration
	- R: Request to read measurement or configuration parameter
	- V: Value returned in response to 'R' command
	- A: Auxiliary, e.g. commands requesting some action

Preamble
-----------
The preamble should be used in order to wake up the device from
POWER_DOWN sleep mode. It is required so the command identifier is not lost
when device is being powered up.

Preamble is a series of 1 up to 10 '!' sign(s). It is recommended to use series of 3 signs ('!!!').

Command termination
---------------------

When device receives the command it must be terminated with '\x0d'  sign.
Device itself terminates its own command (mostly responses) with series of '\x0d''\x0a' 

@todo
accept and ignore 0xa on receive

Data types
----------------
Possible values:
	- [u]int[8,16,32,64]_t: hexadecimal value in ASCII characters. Only lowercase are used.
	- {u,s}Ni.Nf: fixed point e.g s12.4 means signed fixed point 12bit integer 4 bit fractional
	- string: a series of ASCII 
	
Negative values are U2 encoded.
	
Commands 
---------------

Notation
+++++++++++

*command identifier* [*data*]

Where possible configuration, read and values(responses) commands are grouped e.g.
	CTM, RTH, VTH

	
	
Preamble and termination are is not included in this notation



CTM, RTH, VTH
++++++++++++++



 


Other control
===============

References
=============
 http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf



//TODO
