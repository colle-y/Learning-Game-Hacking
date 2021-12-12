# Smuggler
Join me in the process of learning game hacking


## About
Smuggler is currently a simple LoadLibraryA dll injector designed with the goal of evolving it into becoming an actual "good" tool for gamehacking.

I am new to c++ and gamehacking and I plan to evolve this project over time while learning more about the WINAPI and memory.

I will be heavily commenting this repository in order to allow for begginners like myself to get a better bearing of what is actually going on in the program instead of just giving you the file and telling you it works.


## Original Commit - 12/11/21
Smuggler is currently a very basic LoadLibraryA injector that also hooks IsDebuggerPresent and sets it to return false to defeat very basic anti-debugging techniques.

## Pending Commit - TBD
I am working on making a manually mapped loader opposed to LoadLibraryA. LoadLibrary injection is the simplist possible method of injection, but also the most insecure. This is the most widely used technique, however it has some problems:

## LoadLibraryA:
The DLL must be loaded off the disk of the host system, this stops us from loading a buffer of bytes/byte array.
The file cannot be modified while it is injected.
Callback of the function can be detected by an anti-cheat.
Enumerable, will always be in PEB and detectable via iteration.

## Manual Mapping:
Doing what LoadLibraryA does essentially without more callback or linking to the PEB.
Possible to load via byte buffer or any format, does not need to be present on the host system(eg. loading from a web server)
Not *Easily* Enumerable

Think of injecting via LoadLibraryA as shooting your wall with a pistol, it's loud and it leaves a hole, but you achieved your goal. Alternatively think of manually mapping as cracking open the wall, putting something inside, then patching it up.

These methods will not be capable of bypassing any actual competently developed anti-cheat. That being said, it'll work fine on VAC due to it being a ring 3 ac. An anticheat using a kernel driver will easily be able to see what you are doing upon use of either of these methods using a multitude of WIN32 functions via the family of CreateNotifyRoutines.

