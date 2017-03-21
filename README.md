# hk_node
[toc]
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

    add image

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
  * highest - blinker
  * then - measurement of temperature 
  * then - battery measurement

Power saving mode
-------------------

There are 3 selectable power saving modes 
  - IDLE
  - STANBY
  - POWER_DOWN

Refer to the ATMega documentation for reference
Default is the STANDBY. It does immediate recognition for serial commands.

In POWER_DOWN mode serial needs to be woken up (it depends on system oscillator 
which gets powered down). In this mode at least 1 character on serial is getting lost.

When serial command is not pending e.g. nothing is being send/read the device goes to sleep and the watchdog's interrupts wakes it up.

When there is any character available in the receiver device does
not go to sleep up to receiving and handing full command.

> todo Add timeout
    
Serial nor power saving mode does not influence executor e.g. it executes tasks periodically
regardless of characters in receiver

Main Loop
-----------
Main loop executes functions one after another.
There is no pre-emption. 
Main loop first handles serial commands if any and then executor
When there is only part of command available main loop continues with executor 
e.g. does not wait with execution of task for full command.
> @note
    There is only one executor task executed in a single loop. Ones its done device
    either goes to sleep or if there is another/same executor scheduled 
    loop round, handles serial, and executes a task belonging to highest
    priority executor. 

> @warning
    Setting up the execution time to 0 will cause all lower priority
    executors to starve.

If a task takes longer than the system time tick it will not cause any starvation
as long as there will be period of time when lower priority executors could be 
invoked e.g. there will be gaps between particular executor activity

One Wire
-------------
Tho libraries are used 
- OneWire
- DS18B20

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

> [preamble] 3_letter_cmd_it data termination_sign(s)

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

> @todo
    Accept and ignore 0xa on receive?

Data types
----------------
Possible values:
  - [u]int[8,16,32,64]_t: hexadecimal value in ASCII characters. Only lowercase are used.
  - {u,s}Ni.Nf: fixed point e.g s12.4 means signed fixed point 12bit integer 4 bit fractional
  - string: a series of ASCII 
  - combined (string and data)
    
Negative values are U2 encoded.

Response
---------------

- Response to 'C' command is 
 -- same Command identified repeated followed by a string ` ok`
    for example for the command `CTM0010` response is `CTM ok`
 -- error code (see error codes)
- Response to 'R' command is either
 -- corresponding 'V' command
 -- error code (see error codes)
- Response to other command is command specific


Error codes
---------------
There are following error codes defined: 
  - `Cun`    :    unrecognised 'C' command
  - `C[A-Z]u`:     unrecognized third letter of 'C' command
  - `Run`    :    unrecognized 'R' command
  - `R[A-Z]u`:
  - `Dun`    :     unrecognized 'D' command
  - `D[A-Z]u`:
  - `ERR{:space:}{last cmd}-{error code}` where,
   -- {last cmd} is whatever device seen as last command. 
      Non visible characters are presented as backslash followed by hexadecimal code 
   -- error code is hexadecimal value. Meanings are as follows: 
    
Commands List
---------------

###Notation###
 
> *command identifier* [*data*]

or in case of common data:

> *{CRV} command identifier remained* *data*

A dot ('.') indicates any character.
A curly braces indicates alternative.

> @warning
    Unless stated otherwise, there is no space between command identifier and data
    Here, for visibility of notation only, data format is stated after space.
    
Preamble and termination are is not included in this notation.

### DE.
> Debug Echo 

    Parameter: none
    Response:  DR(same third letter)

### DR.
> Debug Reply

See DE.

###CTM {uint16_t, uint32_t}###
> Configure Temperature Measurement

See CTP. This command is deprecated and would be removed.

### CTP
> Configure Temperature (measurement) Period

    Parameter: Setting of temperature period in system watchdog's ticks.
               Parameter can be either 16 or 32 bits (4 or 8 hex digits)
    Response: See `Response` section

### VTM uint32_t
> Value Temperature Measurement

See VTP. This command is deprecated and would be removed.

### {RV}TP uint32_t
> Read/Value Temperature (measurement) Period

    Parameter: None on read, value of temperature measurement period.

### RTC
> Read Temperature Configuration (of measurements)

See {RV}TP. This command is deprecated and would be removed.

    
### {RV}VI {*none*,string}
> Read/Value Version Information

    Parameter: None on read, a string containing version information on reply (VVI command)

### RTM
> Read Temperature Measurement

Request to make a single temperature measurement immediately
The measurement is not stored in history.

    Parameter: None
    Response: VTM

### RTH uint16_t
> Read Temperature History

Request for historical measurements.

    Parameter: Number of measurements to read
               Value of 0 or above capacity will request all capacity

### RTH
> Read Temperature History

Request for 16 last measurements.

    Parameter: None

### {CRV}ST uint64_t
> Configure/Read/Value System Time

    Parameter: None on read, new value to set or current value of system tick counter

### {RV}BP uint32_t
> Read Blinker Period

Request/Value of the period of blinker.
There is no configuration command as it is fixed

    Parameter: None on read, value of blinker in system ticks
    
### {CRV}BA uint8_t
> Configure/Read/Value Blinker pAttern

Blinker pattern is the mode of blinking.
Pattern is processed left to right
Every 1 gives a blink, every 0 gives a delay as long as there are any 1's on more 
significant bits
    
    Parameter: Blink pattern.

### {CRV}PM uint8_t
> Configure/Read/Value Power Mode

Handles power saving modes. 

    Parameter:
                00 - IDLE
                01 - STANDBY (default)
                02 - POWER_DOWN
    Note: When in POWER_DOWN mode the clock gets shut down, therefore subsequent command
          Have to be prefixed with preamble.

### {CRV}PA uint_8_t
> Configure/Read/Value Power Activity (time)
Activity is a parameter saying for how many system ticks device
stays awake if woken up by something else than watchdog timer (system tick)

    Parameter:
                00 - do not stay awake
                01 - stay awake till next system tick (default)
                02 - stay awake for one tick after next system tick
                ...
    Note: The following tick may still happen immediately after wake coming from something 
          else than system watchdog (source of ticks)


### VTH combined
> Values Temperature History

Delivers historical records in a format (no spaces in real format):
> '(' uint32_t ',' s12.4 ')'  *delimiter* ...
  
    Parameter:
                - First parameter is a time difference is system ticks since previous measurement.
                  If its a first record it is a difference to current time.
                - Second parameter is temperature value in fixed point
                - delimiter is an end of line sequence

    
    
Other control
===============

Blinker can be set also by holding pushbutton prior to blink.
Every time pattern is changes in following sequence:
> 01(one blink) -> 03(two blinks) -> .. -> 0f (four blinks) -> 00 -> 01 -> ...

Blinker patterns have special meanings:
  * 00 (blinking off) - none
  * 01 (1 blink)  - none
  * 03 (2 blinks) - send A and VTM command without request every blink period
  * 07 (3 blinks) - for future use
  * 0f (4 blinks) - factory reset
  * 1F (1 blink, long pause, 1 blink) - set blinker pattern to 0 
  * every pattern matching xxx11111b - same as 1F

References
=============
 http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf

