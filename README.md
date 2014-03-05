hubo-ach-readonly
-----------------
This is a Node.js module to retrieve the Hubo state channel data from hubo-ach.
This module NEVER writes to the ach channels, so it is completely safe to use.

To compile, run

```
npm build .
```

To test, run
```
npm install performance-now
node run.js
```

Release Notes:
--------------
0.1.4 - Bugfix release.
0.1.3 - Added support for power board and time.
0.1.2 - Added joint status (error flags) support
0.1.1 - Added IMU support
0.1.0 - Initial release
