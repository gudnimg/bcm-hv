# bcm-hv
Library for Vicor's BCM4414 DC/DC regulator. There is a separate folder for Arduino and STM32F4.

# Background
Many EV Formula Student teams use 600V tractive system voltage. The higher the voltage, the thinner the cables. 
To power the low voltage electronics you can do either of 2 things. 

1. A separate low voltage battery pack. 
2. A DC/DC regulator. (Usually 12V or 24V).

It is difficult to find a rule compliant regulator. One such regulator is Vicor's BCM4414. 

----

# Hardware connection
External pull-ups are required to get good signal edges. You can use either 3.3V or 5V, either works fine.

You can choose your own ADDR resistor value (R3) by looking up the table in the datasheet. Using 1000 Ohms and depicted below yields a device address 0x51.
![](images/bcm_master_connection.PNG)

# Usage

## Arduino
Start by creating an instance of the class `Vicor`. It takes two inputs, a `TwoWire` instance and an I2C address (see *Device Address* in BCM datasheet). You can choose whether you want to use the your own TwoWire instance or not, the Vicor class will create one for you.

```cpp
// Method 1
Vicor bcm;       // Not inputs. The instance uses default settings.

// Method 2
Vicor bcm(0x51); // Custom I2C address 0x51.

// Method 3
TwoWire pWire;           // Custom Wire class for I2C
Vicor bcm(&pWire, 0x51); // Custom I2C address 0x51 and TwoWire instance.

```

## STM32F4

So far I had trouble using C++ so I've stuck with C for the time being meaning I can't use the class method I used for the Arduino.

Start by declaring an instance of the global struct like this:

```c
BCM4414 bcm; // Global BCM variable.
```
This struct includes all the data possible to get out of the modules.

You must manually call the other functions included in vicor.c to specify which data to get. The data is stored in the BCM4414 struct.

**Example:** Status CML 

```c
read_status_cml(&hi2c1); // Read Status CML byte from the BCM module.

// You can either get the entire byte in one:
bcm.status_cml.all // <-- raw byte (includes all bits)

// Or you can go further and read individually defined bits:
bcm.status_cml.cmd_stat_rx  // <-- 0 or 1 for bit 7
bcm.status_cml.data_stat_rx // <-- 0 or 1 for bit 6
```
