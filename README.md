Scriptdog
=========

Configurable watchdog timer that interrupts V8 from executing Javascript
code that is either stuck in an infinite loop or is just taking too long.

Installation
------------

```
npm install scriptdog
```

Usage
-----

```javascript
var Scriptdog = require('scriptdog').Scriptdog;
var sd = new Scriptdog(1000);
while (true) {}
```

In the above example, the script would continue executing endlessly, but
the Scriptdog timer expires after 1000ms and terminates execution of the
endless loop.

The `Scriptdog` constructor takes one argument -- milliseconds until
timeout. The object also exposes a `dispose()` function, which should
always be called after successful execution.

```javascript
var Scriptdog = require('scriptdog').Scriptdog;
var vm = require('vm');
var sd = new Scriptdog(1000);

vm.runInNewContext('var i = 0; while (i++ < 10000000) {}');

sd.dispose();
```


Implementation
--------------

When constructed, a thread is created which runs a separate `libuv` event
loop with a timer set with the specified timeout value.

When the timer expires, `V8::TerminateExecution` is called to terminate
execution inside the default `Isolate`.

When `dispose()` is called, or when the object is collected, the timer is
cancelled, the loop is deleted, and the thread is joined.

License
-------

MIT License
