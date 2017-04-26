Overview
**************************

System description
====================

HW platform
---------------

The software should be run with the following HW platform:

.. todo::
    add image

.. seealso:: :doc:`board`

Overview
----------

Software runs bare metal. 

In a main loop there are following operations:
    - check for the command from serial,
    - calculate time since last loop,
    - execute operations scheduled by executor,
    - go to power saving mode,

Serial is responding on commands send from master controller.
see Commands 

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
measuring battery level

The priority of executors is hard coded"
    * highest - blinker
    * then - measurement of temperature 
    * then - battery measurement

Power saving mode
-------------------

There are 3 selectable power saving modes:
    - NONE
    - STANBY
    - POWER_DOWN

Refer to the ATMega documentation for reference
Default is the STANDBY. It does immediate recognition for serial commands.

In POWER_DOWN mode serial needs to be woken up (it depends on system oscillator 
which gets powered down). In this mode at least 1 character on serial is getting lost.

When serial command is not pending e.g. nothing is being send/read the device goes to sleep and the watchdog's interrupts wakes it up.

When there is any character available in the receiver device does
not go to sleep up to receiving and handing full command.
  
Serial nor power saving mode does not influence executor e.g. it executes tasks periodically
regardless of characters in receiver

Main Loop
-----------

Main loop executes functions one after another.
There is no pre-emption. 
Main loop first handles serial commands if any and then executor
When there is only part of command available main loop continues with executor 
e.g. does not wait with execution of task for full command.

.. note::
    There is only one executor task executed in a single loop. Ones its done device
    either goes to sleep or if there is another/same executor scheduled 
    loop round, handles serial, and executes a task belonging to highest
    priority executor. 

.. warning::
    Setting up the execution time to 0 will cause all lower priority
    executors to starve.

If a task takes longer than the system time tick it will not cause any starvation
as long as there will be period of time when lower priority executors could be 
invoked e.g. there will be gaps between particular executor activity.

One Wire
-------------
Tho libraries are used:
    - OneWire
    - DS18B20

Communication
===============
Communication is build on Bluethooth 4.0 (BLE) or raw logic serial


Commands
=========

Overview
---------
Commands are build with 3 letter ASCII command identifier followed by variable type and size data.
Data is always ASCII. Numbers are send as human readable hexadecimal notation (ASCII). 
Command may be prefixed by preamble:

Command structure
-------------------

    `preamble 3_letter_cmd_id data termination_sign`

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

Preamble is a series (at least 1) of any characters, followed by a `!` sign, followed by an `#` sign. 
Effectively all incoming data is ignored untill recieving `#!` sequence.

.. note::
    Any incoming data, including preample wakes system up from sleep. System then stays active (no power saving)
    for given (hard coded) number of seconds.

.. note::
    Each and every command must be prefixed with a preamble.

Command termination
---------------------

When device receives the command it must be terminated with `'\\x0d'`  sign.
Device itself terminates its own command (responses) with same sign: `'\\x0d'`

Commands description
---------------------
See :doc:`commands` 