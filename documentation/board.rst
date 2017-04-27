Board
**************************

Features
============

Ports
------

    - **4 Digital Inputs**
        - 2 of them 
            - opto isolated *OR*
            - raw logic 
        - 1 of them
            - raw logic *OR*
            - connected to on-board with positive voltage detection circuitry *OR*
            - pushbutton
        - 1 of them 
            - raw logic only
    - **4 Digital Outputs**

        Configurable options:
            - raw logic on all
            - 2 outputs opto isolated
            - open-drain N-FET
            - 2 outputs open-drain P-FET
            - 2 outputs triac circuitry
        Some of the options are exclusive.

    - **4 Analogue Inputs**
        - each with optional pull-up
        - 1 exchangeable with build-in opto transistor circuitry
        - 1 exchangeable with configurable op-amp circuitry
    - **opto-transistor**
        - An external opto transistor can connected. There is a build in circuitry for opto transistor
    - **Dedicated one-wire**
    - **Dedicated I2C**
    - **Serial/Bluetooth module**
        .. note::
            There is no RS232 converter.
    - **SPI programming interface**
        - Arduino compatible

Power supply
------------

    - **230VAC input**
        - requires soldering in a 230VAC / 5VDC converter
    - **4-24VDC input**
        - requires soldering a 5V DC stabilizer 
    - **raw 3-6 input/5V DC output**
        - direct supply input or an output from 5V DC stabilizer.

On board circuitry
------------------
    
Each circuitry is optional e.g. by soldering in/out particular elements.
Particular circuitry is exchangable. 

    - quartz oscillator for ATMega
    - one wire
        - just a required pull-up resistor
    - opto-transistor,
        - used to assess lightening conditions
        - connected to one of ADC inputs
    - op-amp
        - for high impedance, amplified ADC measurements
    - positive voltage detection
        - for detecting phase of ADC of mains power supply
    - triac high current **OR** opto isolated output
    - triac low current **OR** opto isolated output
    - up to 4 open drain outputs
    - 2 opto isolated inputs
    - 230VAC / 5VDC monolitic converter 
    - 5V voltage stablizer
    - LED 
    - Pushbutton

Pin selection
------------------

Digital Inputs
+++++++++++++++

    ===== ============= ======================================= ===================
    pin     type         alternative circuitry                   Special function
    ===== ============= ======================================= ===================
    PD2    raw                                                   INT0, PCINT18
    PD3    raw           positive voltage detection, pushbutton  INT1, PCINT19
    PD4    opto or raw                                           Timer T0 (8 bit)
    PD5    opto or raw                                           Timer T1 (16 bit)
    ===== ============= ======================================= ===================

Digital Outputs
++++++++++++++++

    ===== ============= ================================ ===================
    pin     type         alternative circuitry            Special function
    ===== ============= ================================ ===================
    PD6    raw           open drain N            , LED*    pwm 
    PB0    opto          triac, open drain N or P, LED*     
    PB1    opto          open drain N            , LED*    pwm           
    PB2    raw           triac, open drain N or P, LED*    pwm             
    ===== ============= ================================ ===================

    *For debug purposes, may not be present on final board.

Analog Inputs
+++++++++++++++

    ===== ============= ============================ ===================
    pin     type         alternative circuitry        Special function
    ===== ============= ============================ ===================
    ADC6   raw           opto transistor, pull up     analog only pin 
    ADC7   raw           op-amp,pull up               analog only pin
    PC2    raw           digital raw, pull up         PCINT10
    PC3    raw           ditigal raw, pull up         PCINT11
    ===== ============= ============================ ===================

I2C
+++++++++++++++

    ===== ============= ============================ ===================
    pin     type         alternative circuitry        Special function
    ===== ============= ============================ ===================
    PC4    raw                                         ADC4, PCINT12
    PC5    raw                                         ADC5, PCINT13
    ===== ============= ============================ ===================


One wire
+++++++++++++++

    ===== ============= ============================ ===================
    pin     type         alternative circuitry        Special function
    ===== ============= ============================ ===================
    PC0    raw                                         ADC0
    ===== ============= ============================ ===================

LED
+++++++++++++++

    ===== ============= ============================ ===================
    pin     type         alternative circuitry        Special function
    ===== ============= ============================ ===================
    PB5   no output     LED indicator                 SCK (SPI pin)
    ===== ============= ============================ ===================

SPI programming interface
+++++++++++++++++++++++++

Same as for Arduino nano

    ===== ============= ============================ ===================
    pin     type         alternative circuitry        Special function
    ===== ============= ============================ ===================
    PB5                   LED indicator  
    PB4 
    PB3 
    PC6 
    ===== ============= ============================ ===================
