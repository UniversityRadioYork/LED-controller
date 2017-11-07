const request = require('request');
const ColourToHex = require('colornames')
const Serial = require('./arduino.js')
var Title = "";
var Colour = -1;
var Studio = 0;

function pollTimelord()
{
	request('http://ury.org.uk/audio/status-json.xsl', function (error, response, body) {
		var data = JSON.parse(body);
		Title = data.icestats.source[5].title; 
		//split title into words
		Words = [];
		StringStream = "";
		for(var i = 0; i < Title.length; i++)
		{
			if(Title[i] != ' ')
			{
				StringStream += Title[i];
			}
			else
			{
				Words.push(StringStream);
				StringStream = "";
			}
		}
		Words.push(StringStream);
		Words.push("END");
		//look for colours
		for(var i = 0; i < Words.length; i++)
		{
			if(ColourToHex(Words[i]) != undefined)
			{
				Colour = ColourToHex(Words[i]);
				break;
			}
			else if(Words[i] == "END")
			{
				Colour = -1;
				break;
			}
		}
		//
		//console.log(Colour);
	});
	request('https://ury.org.uk/api/v2/selector/statusattime?api_key=IMNOTAREALKEY', function (error, response, body) {
		var data = JSON.parse(body);
		Studio = data.payload.studio;
		if(Colour == -1)
		{
			switch(Studio)
			{
				case 1:
					Colour = ColourToHex("RED");
					break;
				case 2:
					Colour = ColourToHex("GREEN");
					break;
				case 3:
					Colour = ColourToHex("BLUE");
					break;
			}
		}
	});
};

function DynamicStaticPolling(online)
{
	var oldColour = Colour;
	pollTimelord();
	if(oldColour != Colour)
	{
		sendCmd("/c" + Colour.slice(1));
	}
	
	console.log(Colour);
}

module.exports = {
		pollTimelord: pollTimelord,
		DynamicStaticPolling: DynamicStaticPolling
}