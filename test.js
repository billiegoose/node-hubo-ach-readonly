var now = require('performance-now');
var hubo_ach = require('./build/Release/hubo_ach_readonly');
console.log("init() Test");
var r = hubo_ach.init();
console.warn(r);
if (r) {
	console.log("getState() Test");
	var state = hubo_ach.getState();
	console.log(state);
	console.log(state.joint[hubo_ach.LSP].ref);

	console.log("Polling Time Test");
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

	console.log("JointList Test");
	console.log(hubo_ach.JointList);

	// Test JointMap
	console.log("JointMap Test");
	for (var i = 1; i < 42; i++) {
		console.log(i + ': ' + hubo_ach.JointMap[i]);
	}
	for (var i = 1; i < hubo_ach.JointList.length; i++) {
		var name = hubo_ach.JointList[i];
		var index = hubo_ach.JointMap[name];
		console.log(name + ': ' + index);
	};
	for (var prop in hubo_ach.JointMap) {
		if (hubo_ach.JointMap.hasOwnProperty(prop)) {
			var assertion = (hubo_ach.JointMap[hubo_ach.JointMap[prop]] == prop) ? true : false;
			if (!assertion) {
				console.log("Doubly-linked assertion fail for '" + prop + "'");
			}
		}
	}
}
console.log('subsequent init() Test')
r = hubo_ach.init();
console.warn(r);
