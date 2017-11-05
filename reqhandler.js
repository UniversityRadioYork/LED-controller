// Handle Genre create on POST
const zpad = require('zpad')
var currentMode = 'a'
var currentBrightness = 25
var currentDelay = 20;
exports.getCommands = function(req, res) {
  
  //Check that the name field is not empty
  req.checkBody('mode', 'Genre name required').notEmpty()
  req.sanitize('mode').escape()
  req.sanitize('mode').trim()
  req.checkBody('brightness', 'Invalid brightness').notEmpty().isInt()
  req.checkBody('delay', 'Invalid step delay').notEmpty().isInt()
  
  var errors = req.validationErrors();

  var command_data = new Command({ 
    mode: req.body.mode,
    brightness: req.body.brightness,
    delay: req.body.delay
  })
  
  if (errors) {
    res.send(errors)
    return false;
  } else {
    let commands = []
    //Adds mode command if needed
    let mode = command_data.mode
    if(mode != currentMode){
      currentMode = mode
      commands.push("/M"+mode+";")
    }
    //Adds brightness command if needed
    let brightness = command_data.brightness
    if(brightness != currentBrightness){
      currentBrightness = brightness
      commands.push("/B"+zpad(brightness,3)+";")
    }
    //Adds delay command if needed
    let delay = command_data.delay
    if(delay != currentDelay){
      currentDelay = delay
      commands.push("/D"+zpad(delay,3)+";")
    }

    return commands
  }
};