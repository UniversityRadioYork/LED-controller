const request = require('request');
const ColorToHex = require('colornames');
const Serial = require('./arduino.js');
const studio_colours = ["#FF0000", "#00FF00", "#0000FF"]
let current_color = false;
let current_studio = 0;

exports.pollTimelord = function () {
	//Get current song data
	request('http://ury.org.uk/audio/status-json.xsl', function (error, response, body) {
		let data = JSON.parse(body);
		let title = data.icestats.source[5].title;
		//Split title into words
		let words = title.split(" ");
		//look for colours
		for (let i = 0; i < words.length; i++) {
			let found_color = ColorToHex(words[i])
			if (found_color != undefined) {
				//Set found colour
				current_color = found_color;
				return;
			}
		};
		current_color = false;
	});
	//Get studio selector status
	request('https://ury.org.uk/api/v2/selector/statusattime?api_key=IMNOTAREALKEY', function (error, response, body) {
		let data = JSON.parse(body);
		current_studio = data.payload.studio;
		//Lookup the colour for the currently on-air studio
		if (Colour === false && 0 < current_studio <= 3) {
			current_color = studio_colours[current_studio]
		}
	});
};

exports.DynamicStaticPolling = function (online) {
	let old_colour = current_color;
	pollTimelord();
	if (old_colour != current_color) {
		sendCmd("/C" + current_color.slice(1));
	}
	console.log(Colour);
}

exports.currentStudio = function () {
	return current_studio
}