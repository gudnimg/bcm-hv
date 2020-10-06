# bcm-hv
Library for Vicor's BCM4414 DC/DC regulator.

# Background
Many EV Formula Student teams use 600V tractive system voltage. The higher the voltage, the thinner the cables. 
To power the low voltage electronics you can do either of 2 things. 

1. A separate low voltage battery pack. 
2. A DC/DC regulator. (Usually 12V or 24V).

It is difficult to find a rule compliant regulator. One such regulator is Vicor's BCM4414. 

# Usage

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
