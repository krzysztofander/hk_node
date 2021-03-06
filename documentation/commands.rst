Commands
**************************

ref `http://www.tablesgenerator.com/text_tables`


+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| Type        | Command         | Type       | Returns | Brief        | Description                         |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| - Optional  |                 | - [Q]uery  |         |              |                                     |
| - Mandatory |                 | - [S]et    |         |              |                                     |
| - Debug     |                 | - [R]esp.  |         |              |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| O           | CTR number      | QSR        |         | Configure    | Configures precision of temperature |
|             |                 |            |         | Temperature  | measurements in bits                |
|  **N/A YET**|  **N/A YET**    |            |         | Resolution   |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CTP number      | QSR        |         | Configure    | in seconds                          |
|             |                 |            |         | Temperature  |                                     |
|             |                 |            |         | Period       |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CBP number      | QR         |         | Configure    | in seconds                          |
|             |                 |            |         | Blinker      |                                     |
|             |                 |            |         | Period       |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CPP number      | QSR        |         | Configure    | Sets period of supply voltage       |
|             |                 |            |         | Power        | measurement (in seconds)            |
|  **N/A YET**|  **N/A YET**    |            |         | Period       |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CST number      | QSR        |         | Configure    |                                     |
|             |                 |            |         | System       |                                     |
|             |                 |            |         | Time         |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CSC number      | QR         |         | Configure    | Queries for node capabilities       |
|             |                 |            |         | System       | 0x1 - temperature measurement ver A |
|             |                 |            |         | Capabilities | 0x100 - batery level measurement    |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CNN string      | QSR        |         | Configure    |                                     |
|             |                 |            |         | Node         |                                     |
|             |                 |            |         | Name         |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CRS numer       | S          |         | Configure    | - 0 - warm boot                     |
|             |                 |            |         | Reset        | - 1 - BT reset                      |
|             |                 |            |         | System       | - 2 - total factory reset           |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CRV numer       | QSR        |         | Configure    | Value of bandgap reference          |
|             |                 |            |         | Reference    | voltage in milivolts.               |
|             |                 |            |         | Voltage      | Used for ADC measuremnts e.g. batery|
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CSM number      | QSR        |         | Configure    | - 0 - low                           |
|             |                 |            |         | Saving       | - 1 - medium                        |
|             |                 |            |         | Mode         | - 2 - high                          |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | CSA number      | QSR        |         | Configure    | Applicable only when CSM==2:        |
|             |                 |            |         | Saving       | - number of WD ticks for how        |
|             |                 |            |         | Activity     | long system will use CSM==1         |
|             |                 |            |         |              | after recieving character           |
|             |                 |            |         |              | from serial                         |
|             |                 |            |         |              |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | AVI string      | QR         |         | Aux:         |                                     |
|             |                 |            |         | Version      |                                     |
|             |                 |            |         | Infomation   |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | RTH             | Q          | see VTH | Read         | Make a measurement right now        |
|             |                 |            |         | Temperature  | and add that to history             |
|             |                 |            |         | History      |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | RTH number      | Q          | see VTH | --//--       | Return historical measurements      |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| O           | RPM             | Q          | see VPM | Read         | Makes a supply voltage measurement  |
|             |                 |            |         | Power (bat)  |                                     |
|             |                 |            |         | Measurement  |                                     |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | RPH             | Q          | see VPM | Read         | Makes a supply voltage measurement  |
|             |                 |            |         | Power (bat)  | and add that to history             |
| **N/A YET** | **N/A YET**     |            |         | History      | **NOT AVAILABLE YET**               |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| D           | DE[a-Z]         |            | DR[a-Z] |              | Simple echo                         |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| D           | DL0             |            | //todo  |              | Debug led off                       |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| D           | DL1             |            | //todo  |              | Debug led on                        |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | VTH             | R          |         |              | Delivers historical                 |
|             | (number,number) |            |         |              | values of temperature               |
|             | (number,number) |            |         |              | measurements                        |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+
| M           | VPM             | R          |         |              | Delivers historical                 |
|             | (number,number) |            |         |              | values of power supply              |
|             | (number,number) |            |         |              | measurements.                       |
+-------------+-----------------+------------+---------+--------------+-------------------------------------+