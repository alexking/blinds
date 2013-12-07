SmartThings Mini Blinds
=======================

I'm using this github repository just to document the SmartThings project I've been working on lately – controlling my window blinds via Zigbee - in case anyone else wants to build something similar, or apply any of it to another project.

I read this [article on Makezine](http://makezine.com/projects/mini-blind-minder/) while trying to find motorized blinds (because turning a handle is waaay too hard). I loved the simplicity of the mechanism, so I decided to build my own version that integrated with SmartThings (a fantastic smart home platform that allows you to connect a ton of various third party devices). 

They offer a fantastic [SmartThings Arduino shield](https://shop.smartthings.com/#/products/smartthings-shield-arduino), which makes it incredibly easy to enable an arduino to connect via Zigbee. You'll also need an [Arduino Uno of some kind](https://www.sparkfun.com/products/11224). 

Since our version of the project will be controlled wirelessly, most of the parts needed for the original Makezine version aren't needed. You will need these items though -

- [Continuous Rotation Servo](http://www.parallax.com/product/900-00008)
- Rubber Band – although I've had more success using hair elastics with one rubber side)
- Project Case – I used an iPhone 5 box 

I also integrated the following parts I had laying around -

- [DHT11 Temperature and Humidity Sensor](https://www.adafruit.com/products/386) - I wouldn't recommend using this, it's not very accurate, but I figured it'd be fun to have an extra input
- [Photo resistor](http://bildr.org/2012/11/photoresistor-arduino/) - again, I'd [recommend something better if you can get it](http://bildr.org/2011/09/tsl230r-arduino/)

Note - you probably wouldn't do poorly by following the original project more closely than I have - many of my changes actually have more to do with the parts I had available at the time (my local RadioShack isn't very well stocked). In particular, in future versions I plan to add buttons to control the blinds. I didn't have any buttons.


### Thanks!

- To the [original Makezine article](http://makezine.com/projects/mini-blind-minder/) that inspired the design
- To the [SmartThings community](http://build.smartthings.com) site for helping me figure out the device handler and Arduino code 
