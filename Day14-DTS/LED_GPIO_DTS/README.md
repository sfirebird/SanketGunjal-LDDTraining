### Understanding the Device Tree

Device Tree Source files (hardware description files) are used to provide the structure used to define a hardware description for the Linux Device Tree.

### LED GPIO DTS file:

        /dts-v1/;
        /plugin/;
        /{
            compatible="brcm,bcm2835";
            fragment@0{
                target=<&gpio>;
                __overlay__{
                    led_gpio=<&gpio 17 0>;
                };
            };
        };

- The ```/dts-v1/``` specifies the version of the Device Tree Source format being used.
- The version is always ```v1```.
- The ```/plugin/``` indicates that this file is a Device Tree overlay, which modifies or extends an existing base Device Tree.

### Doubts/Questions:

#### Q. What is the need to create a pointer of type ```struct device_node```?
- This is required to hold the address of the node which will contain the configuration for the GPIO, as overlayed by the gpio_led.dts file.

#### Q. Is it possible to get the information of the GPIO without using the of_get_named_gpio() function?
