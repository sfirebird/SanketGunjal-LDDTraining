# Understanding the and building the flow of the pir_driver

#### Initialization:      
Whatever pin that you want to use for the pir sensor and the led, you need to the following in initializaiton:
- request the gpio_pin that you are using
- set the direction of the requested gpio_pin
- register the driver