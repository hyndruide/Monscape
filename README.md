# Monscape (alpha)

Monscape is a library to create a monitoring interface easily and cheaper using Arduino and esp32 as puzzle brain and a RPI as conductor

Monscape is a system client <—> server for escape game.

each client is a µcontroller (Arduino Uno, Nano, compatible … or ESP32)
and the server is a raspberry pi 

for now they can communicate via RS485 but I want to develop it with different ways to communicate

They send a json each other to communicate.


## Getting Started


Copy the server directory on your RPI

launch inside the directory

```
node server.js
```



### Prerequisites

install the library in Arduino directory

install nodejs on the raspberry pi

```
sudo apt-get install nodejs
sudo apt-get install npm
npm insatll socket.io
npm install onoff
```



### Installing

Copy the server directory on your RPI

launch inside the directory

```
node server.js
```


## Built With

* [OnOff](https://www.npmjs.com/package/onoff)—GPIO access and interrupt detection with Node.js
* [Socket.io](https://socket.io/)—FEATURING THE FASTEST AND MOST RELIABLE REAL-TIME ENGIN
* [ArduinoJSON](https://arduinojson.org/)—JSON Arduino library

## Contributing


## Authors

* **Ulric Denis** - *Initial work* - [hyndruide](https://github.com/hyndruide)


## License

This project is licensed under the MIT License

