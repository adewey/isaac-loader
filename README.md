isaac-loader
============

a platform for loading plugins in the binding of isaac: rebirth

included projects:
* injector - injector.exe loads the main dll (loader.dll) into isaac's memory
* loader - loader.dll handles all of the framework for:
* * plugins
* * commands
* * hooks
* * data structures
* template - template.dll is an example of how you would create a plugin and use the framework to read, and write values in the memory structures of isaac
* tracker - tracker.dll talks to isaactracker.com to upload data from your run live, so that you can share it, or capture it for your stream
* races - races.dll talks to isaacraces.com and allows you to queue for races in game
