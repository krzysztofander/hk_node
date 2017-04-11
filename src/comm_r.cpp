/************************************************************************************************************************
Copyright (c) 2016, Krzysztof Bartczak
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/


#include "hk_node.h"
#include "executor.h"
#include "serial.h"
#include "comm.h"
#include "supp.h"
#include "temp_measurement.h"
#include "comm_extra_records.h"
#include "comm_extra_rec_handlers.h"
#include "blinker.h"
#include "adc.h"

void HKComm::command_RTM(OutBuilder & bld)
{

    TempMeasure::TempMeasurement singleTempMeasurement = TempMeasure::getSingleTempMeasurement();

    HKCommExtraRecordsHDL::setNumRecords(0);
    HKCommExtraRecordsHDL::setDataReciever(&HKCommExtraHLRs::RTHdataReciever);

    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_VTM));

    bld.addMeasurement(0, singleTempMeasurement);

}


void HKComm::command_RTH(const InCommandWrap & inCmd, OutBuilder & bld)
{
    OutBuilder::ELogicErr err;
    uint16_t measurementsToReturn ;
    if (g_RecievedCmd.hasData())
    {
        measurementsToReturn =  g_RecievedCmd.getUint16(err);
    }
    else
    {
        err = OutBuilder::ELogicErr::None;
        measurementsToReturn = 0;
    }
    
    if (err != OutBuilder::ELogicErr::None )
    {
        bld.putErr(err);
    }
    else
    {
        if (measurementsToReturn == 0)
        {
            //special case, make a measurement now and store it.
            TempMeasure::measureTemperature();
            measurementsToReturn = 1; //so we return the last one.
        }

    //check for size correctness
        if (measurementsToReturn > TempMeasure::capacity())
        {
            //if its zero return all.
            measurementsToReturn = TempMeasure::capacity();
        }
        bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_VTM));

        //measurementsToReturn contains how many. First one returns difference of current to timestamp
        HKTime::UpTime diff = Sleeper::getUpTime();
        TempMeasure::TempRecord tempRecord = TempMeasure::getTempMeasurementRecord(0);
        diff = diff - (HKTime::UpTime)tempRecord.timeStamp;

        bld.addMeasurement(HKTime::SmallUpTime(diff), tempRecord.tempFPCelcjus);

        measurementsToReturn--; //one is returned in bld

                                //now set up records handler
        if (measurementsToReturn > 0)
        {
            HKCommExtraRecordsHDL::setNumRecords(measurementsToReturn);
            HKCommExtraRecordsHDL::setDataReciever(&HKCommExtraHLRs::RTHdataReciever);
        }
    }
}

void HKComm::commandRPM(const InCommandWrap & inCmd, OutBuilder & bld)
{
    OutBuilder::ELogicErr err;
    uint16_t measurementsToReturn ;
    if (g_RecievedCmd.hasData())
    {
        measurementsToReturn =  g_RecievedCmd.getUint16(err);
    }
    else
    {
        err = OutBuilder::ELogicErr::None;
        measurementsToReturn = 0;
    }

    if (err != OutBuilder::ELogicErr::None )
    {
        bld.putErr(err);
    }
    else
    {
        if (measurementsToReturn == 0)
        {
            //special case, make a measurement now and store it.
            int16_t val = ADCSupport::readBandgap();
            measurementsToReturn = 1; //so we return the last one.
            bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_VPM));
            bld.addInt(val);
        }
    }
}

void HKComm::command_AVI( OutBuilder & bld)
{
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_AVI));
    static const char v[] ={ ' ','0','.','7','.','0' };

    bld.addString(v, NUM_ELS(v));

    // Datasheet: http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf


    /*Releases
    0.5.0:
    + version information
    + shortened RTH
    + sending temperature readings set up by blinker

    0.5.1 Development
    0.5.1.1 Development
    + Improved current power saving algo. 
    + Starting serial now in main loop after non-wd power up 
    and waiting a delay afterwards
    0.5.1.2 Development
    +Improved blinker setting
    0.5.1.3 Development
    +Improved blinker setting, fixed auto send
    0.5.1.4 Development (11,280) bytes
    +Added compatible commands as macro
    !not tested on system
    0.5.1.5 Development (11,280)
    + 
    0.5.2.0
    improved power save mode
    //see http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/
    0.5.2.1
    LED off capability
    //see http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/

    0.6.0 change to protocol!. Not all previous commands are supported
    0.6.1 change to protocol!. Restored previous commands
    0.6.2 Fixed RTH, CNN, CSA, CBP, and others
0.6.3 Limited temperature results to available only (working)

    0.6.4 Abstracted serial (not tested)
    0.6.5 Added preamble and BT support. 
        preamble and BT handling seems to be working
        Have not checked whether BT module indeed goes to low power mode,
        But it seems to (getting some wake from serial) sent connection lost
    0.6.6 Timeout now is reset on any character recieved
        Setting up BT name
    0.6.7 BT works
    0.7.0 Measurement of batery, no history so far

    0.?.1
    + batery reading
    // see https://forum.arduino.cc/index.php?topic=38119.0
    0.?.1
    + Any AC value reading with autoscale
    0.?.1
    + Bluetooth settings
    0.?.1 
    + eeprom
    1.0.0 all above works



    */
 
}

//------------------------------------------------------------------

/*
Dobra, to teraz b�dzie co i jak:

1. Po wys�aniu komendy do BT musisz odczyta� jego odpowied�, je�li tego nie zrobisz, nie b�dzie dzia�a�.
2. Niekt�re komendy aktywuj� si� dopiero po AT+RESET

Teraz co chcemy, na pewno chcemy taki ci�g:

AT+RENEW, AT+MODE1, AT+PIO11, AT+RESET (Po ka�dej komendzie czytamy odpowied��)
Po renew warto odczeka� ze 100ms albo ponowi� AT+MODE1 a� odpowie ci ok.

Sekwencja powy�ej spowoduje �e led na BT zga�nie i �wieci� nie b�dzie.

Teraz co� co w teorii powinno zmniejszy� zu�ycie energii ale to trzeba pomierzy� czy rzeczywi�cie si� tak dzieje:
AT+ADVI4 - parameters mog� by� od 0 do F, organoleptycznie sprawdzone �e do 4 jest w miar� ok je�li chodzi o prac� z po��czeniem, mo�na jeszcze poeksperymentowa� ale warto�ci powy�ej 6-7 s� bardzo czasoch�onne.

W zale�no�ci jak daleko jeste�my od sensora mo�na przeprowadzi� dodatkow� optymalizacj�:
AT+POWEx - gdzie x mo�e by� 0-3 przy czym 0-1 to obni�enie mocy BT, 2-praca ze standardow� moc�, 3-wi�cej mocy w sygna�. Tutaj mo�na pokmini� aby baza sobie pogada�a z w�z�em i korzstaj�c z RSSI mo�e dopasowa� t� warto��. Wymaga dalszego rozpoznania.

AT+NAMExxxx - nadanie nazwy sensorowi, do 12 znak�w nazwa

Teraz co� bardziej zaawansowanego ale daj�ce lepsze rezultaty wg mnie:
AT+SLEEP - po tym HM10 wchodzi w sleepa ale jest widoczny dla innych, je�li chcesz go wybudzi� z arduino wysy�asz mu 80 losowych znak�w ale nie s�dz� aby to nam by�o do czegokolwiek potrzebne. HM-10 automatycznie si� wybudza jak kto� si� przy��czy. Wi�c chcemy tego u�ywa�, ale po za��czeniu si� gdy nast�pi roz��czenie zdalnego po��czenia HM10 pozostaje w trybie wybudzonym. Mo�na niby w��czy� autosleep przez komend� AT+PWRM0 ale to s�abo dzia�a, wi�c olewamy j�. Proponuje inne podej�cie. U�yjemy komendy AT+NOTI1 w efekcie gdy kto� si� do nas przy��czy dostajemy na UART linie �OK+CON� p�niej s� dane itd. gdy kto� si� roz��czy dostajesz �OK+LOST� po wykryciu tego komunikatu powiniene� wykona� od razu AT+SLEEP.

Docelowo wydaje mi si� �e chcemy u�y� nast�puj�cego zakl�cia przy boocie noda:
AT+RENEW [100-200ms wait], AT+MODE1, AT+PIO11, AT+NOTI1, AT+ADVI4, AT+NAMExxxx, AT+RESET [100-200ms wait], AT+SLEEP
W drugim milestone mo�emy zrobi� p�tle do pr�bkowania zasi�gu i dorzuci� jeszcze AT+POWEx.

Mam nadzieje �e cokolwiek zrozumia�e� z tego co napisa�em :D
Bart�omiej �arnowski
Senior Software Design Engineer
Imagination Technologies
www.imgtec.com

*/