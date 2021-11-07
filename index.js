
var express = require('express');
var app = express();
var server = require("http").Server(app);

var io = require("socket.io")(server);

var serialPort = require("serialport");
const Readline = require('@serialport/parser-readline');
var serial = new serialPort('COM6',{"baudRate":9600});
const parser = serial.pipe(new Readline({ delimiter: '\r\n' }));



serial.on('open', function(err){
	if(err!=null){
		console.log("No hay conexión con el sistema arduino");
	}
	else{
		console.log("Conexión exitosa con el sistema arduino");
	}
})


app.use(express.static('public'));



app.get('/', function(req,res){
	res.sendfile("index.html");
});


io.on('connection', function (socket) {
  parser.on('data',function(data){
	  socket.emit('news', { serial: data.toString() });
});
 
});

server.listen(80);