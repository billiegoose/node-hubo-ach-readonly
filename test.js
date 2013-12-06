var now = require('performance-now');
var hubo_ach = require('./build/Release/hubo_ach_readonly');
var r = hubo_ach.init();
console.warn(r);
if (r) {
	// output stuff
	var state = hubo_ach.getState();
	console.log(state);
	console.log(state.joint[hubo_ach.LSP].ref);

	// polling time test
	var timeavg = 0;
	for (var i = 1; i < 1001; i++) {
		start = now();
		var state = hubo_ach.getState();
		end = now();
		timeavg = timeavg*(i-1)/i + (end-start)*1/i;
		if (i % 100 == 0) {
			console.log(timeavg + ' milliseconds');
		}
	}
}
