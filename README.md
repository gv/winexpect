winexpect
---

A helper tool for GUI testing automation to use along with
ActivateApp/SendKeys. The use case is when you want to SendKeys but are
not sure if the control you're targeting has yet appeared. winexpect.exe
takes a string and waits until there is a window on its desktop with
window text containing given string.

Example JScript code
---

```js
function print(x) {
	return WScript.Echo(x);
}

function fail(x) {
	print(x + " (press a key to quit...)");
	WScript.StdIn.Read(1);
	WScript.Quit(1);
}

function run(x) {
	print("Running " + x + " ...");
	return sh.Run(x, 1, true);
}

var wexpect = WScript.ScriptFullName.replace(/[^\\]+$/, "winexpect.exe");
function expect(str) {
	var s = run(wexpect + ' "' + str + '"');
	if(s) 
		fail("winexpect failed, error " + s);
}

sh.AppActivate("Setup -");
sk("abcde");
sk(["{ENTER}", "{ENTER}"]);
expect("Configure server");
sk(["{ENTER}", "{TAB}", "{TAB}", "{ENTER}"]);
```

Bugs
---

- long argument strings are cut off sloppily in the UI
