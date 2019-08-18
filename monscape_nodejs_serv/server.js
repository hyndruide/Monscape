var SerialPort = require('serialport');





function switchOff(){
  RS_RE.writeSync(0);
  RS_DE.writeSync(0);
}



function Communication(data){
  RS_RE.writeSync(1);
  RS_DE.write(1, function(err) {
   if (err) {
    return console.log('Error on write: ', err.message)
  }
  value = JSON.stringify(data)
  console.log(value)
  value = value+ '\n'
  port.write(value, function(err) {

   if (err) {
    return console.log('Error on write: ', err.message)
  }
  setTimeout(switchOff, 15);
})

})

}




const Readline = require('@serialport/parser-readline');const port = new SerialPort('/dev/ttyS0',{
 baudRate: 74880
});

const parser = port.pipe(new Readline({ delimiter: '\r\n' }));// Read the port data


port.on("open", () => {
  console.log('serial port open');
});




var Gpio = require('onoff').Gpio;
var RS_RE = new Gpio(17, 'out');
var RS_DE = new Gpio(27, 'out');
RS_RE.writeSync(0)
RS_DE.writeSync(0)
var express = require('express');

var app = express();
app.use('/static', express.static('public'));
var server = require('http').Server(app);

app.get('/', function(req, res) {
var data = [
    // { name: "PG1", type : "Keypad", light: 1},
    // { name: "PG2", type : "Knob", light: 1},
    // { name: "PG3", type : "Speed", light: 1},
    { name: "PL1", type : "Light", light: 1},

];
 


  res.setHeader('Content-Type', 'text/html');
  res.render('tesjs.ejs',{data : data} );
});

// app.get('/P1/reset', function(req, res) {

//   RS_RE.writeSync(1);
//   RS_DE.write(1, function(err) {
//    if (err) {
//     return console.log('Error on write: ', err.message)
//   }
//   port.write('R\r\n', function(err) {
//    console.log("Reset")
//    if (err) {
//     return console.log('Error on write: ', err.message)
//   }
//   setTimeout(switchOff, 10);
// })

// })





// });

var io = require('socket.io').listen(server);

// Quand un client se connecte, on le note dans la console
io.sockets.on('connection', function (socket) {
  RS_RE.writeSync(0);
  RS_DE.writeSync(0);


  parser.on('data', function (data){
    console.log(data)
    socket.emit('message', data);
  });



  socket.on('Communication', function (data) {
    Communication(data);
  });



  socket.on('disconnect', onDisconnect);

  function onDisconnect() {
      console.log('Received Disconnect from client: ' + socket.id);
      socket.removeListener('disconnect', onDisconnect);
      socket.disconnect();
  }


});



server.listen(8080);
