// Mmmmmm imports
const express = require('express')
const app = express();
const http = require('http').Server(app);
const bodyParser = require('body-parser');
const expressValidator = require('express-validator');
const pug = require('pug');
const path = require('path');
const serial = require('./arduino.js');
const formHandler = require('./reqhandler.js');
const huwsModes = require("./huwModes.js");

//Allows the arduino time to turn on
let arduino_online = false;
let enable_color_changes = true;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
  extended: false
}));
app.use(expressValidator());
app.use('/public', express.static(path.join(__dirname, 'public')))

app.post('/', function (req, res) {
  let commands = formHandler.getCommands(req, res);
  if (commands !== false) {
    for (let i in commands) {
      serial.CMD(commands[i], arduino_online);
    };
  };
  res.render('index.pug', formHandler.currentSettings());
});

app.get('/', function (req, res) {
  res.render('index.pug', {
    mode: 'a',
    brightness: '25',
    delay: '20'
  });
});

http.listen(3000, function () {
  console.log('listening on localhost:3000');
  //Waits 6 seconds before enabling commands to be sent
  setTimeout(function () {
    arduino_online = true;
    setInterval(function () {
      huwsModes.DynamicStaticPolling(arduino_online)
    }, 3000);
    console.log("Ready.")
  }, 3000);
});