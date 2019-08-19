# Monscape (alpha)

Monscape is a program to create a monitoring interface easely and cheaper using arduino and esp32 as puzzle brain and a RPI as conductor

Monscape is a system client <-> server for escape game.

each client are a µcontroller (aruino, uno, nano, compatible... or ESP32)
and the server is a raspberry pi 

for now they can communicate via RS485 but i want to develope it with different way to communicate

They send a json each other to communicate.


## Getting Started


Copy the server directory on your RPI

launch inside the directory

```
node server.js
```



### Prerequisites

install the library in arduino directory

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

* [OnOff](https://www.npmjs.com/package/onoff) - GPIO access and interrupt detection with Node.js
* [Socket.io](https://socket.io/) - FEATURING THE FASTEST AND MOST RELIABLE REAL-TIME ENGIN
* [ArduinoJSON](https://arduinojson.org/) - A JSOn arduino library

## Contributing


## Authors

* **Ulric Denis** - *Initial work* - [hyndruide](https://github.com/hyndruide)


## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
