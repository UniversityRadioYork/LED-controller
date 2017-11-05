const app = require('express')()
const http = require('http').Server(app)
const io = require('socket.io')(http)
const bodyParser = require('body-parser')
const expressValidator = require('express-validator')
const serial = require('./controller.js')
const formHandler = require('./reqhandler.js')
let arduino_online = false

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(expressValidator())

app.get('/form', function(req, res){
  let commands = formHandler.getCommands(req,res)
  if(commands !== false){
    for(let i in commands){
      serial.sendCMD(commands[i])
    }
  }
})

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