const app = require('express')()
const http = require('http').Server(app)
const io = require('socket.io')(http)
const serial = require('./controller.js')
let arduino_online = false

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html')
})

io.on('connection', function(socket){
  console.log("A client connected")
  socket.on('test', function(msg){
    console.log('test recieved');
  });
})

http.listen(3000, function(){
  console.log('listening on localhost:3000')
  setTimeout(function(){arduino_online = true},6000)
})