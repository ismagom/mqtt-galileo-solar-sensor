# mqtt-galileo-solar

This program periodically reads analog inputs A0, A1 and A2 from the Galileo board and sends MQTT message to a remote broker. It uses the libemqtt library: https://github.com/menudoproblema/libemqtt

To be used with the Solar Panel Shield v2.0. Connect VBAT and solar panel to the A0 and A1 pins in your Galileo Board. 

Use web app https://github.com/ismagom/mqtt-panel-galileo to see the messages in a browser. 

# Compile and Run

make
./test ip_address

where ip_address is the IP address of the MQTT broker. 



