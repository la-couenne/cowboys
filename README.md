# Shooting game, let's knock out the Cowboys
![img00](https://user-images.githubusercontent.com/38251711/119187865-2edddb00-ba7a-11eb-9315-796d3319474f.jpg)

Hello everyone !!
Here's a cool target game, powered by a Nerf dart rifle, to take out Cowboys: D

# Demo:
https://youtu.be/ealsSJ5FleY

# Equipment:
![schema](https://user-images.githubusercontent.com/38251711/119188084-78c6c100-ba7a-11eb-920d-46a636356756.jpg)

1 Arduino UNO

1 Nerf Stryfe rifle (battery-powered semi-auto so you don't have to reload)

1 COB power LED

1 Amplifier for the speaker (Adafruit 20W MAX9744)

1 433MHz radio transmission modules

1 Drawing of a cowboy laminated

1 microphone module

2 1.5v AA battery pack

2 NO limit switches

2 Engines

3 Relay 5v 125ohms with a diode

10 Full of Nerf Darts :)

How it works: I used the metal construction modules and motors of the Makeblock kits to make the structure, an Arduino raises the targets, then "listen" through the microphone to the possible noise of a Nerf arrow, which would cause the descent of the target.

If no arrow hits the target, we play the sound of a shot and send a flash with a high power LED to signal that the cowboy shot first and that we are Game Over.
I spiced it up a bit with a 433MHz module in order to synchronize the targets with the start / end of the game.

In this diagram the switches of the motor circuit are NO, that is to say they open the circuit when the target is at the end of its travel in order to stop the motor.
So when output # 8 is HIGH the relay closes, the motor runs until the target hits the switch which then cuts power to the motor.

I forgot to draw them on the diagram, add on the pins of the relay coil: a rectifier diode!

# Sources:
Coloring picture site (cowboys): https://www.hugolescargot.com

Modules 433Mhz: https://www.carnetdumaker.net/articles/communiquer-sans-fil-en-433mhz-avec-la-bibliotheque-virtualwire-et-une-carte-arduino-genuino

VirtualWire library: http://www.airspayce.com/mikem/arduino/VirtualWire

Building Modules: https://www.makeblock.com

Power led: https://www.distrelec.ch/fr/led-cob-blanc-froid-14-everlight-electronics-ju1215-kt657n7-12507-090t/p/11050014?track=true&no-cache=true
