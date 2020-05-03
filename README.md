# Dissector
This is an IDA plugin used to analyze and gut files during debugging.

![https://github.com/GShabanov/Dissector/blob/master/images/mainapp.jpg](https://github.com/GShabanov/Dissector/blob/master/images/mainapp.jpg)

## Note:
_This plugin is my army knife for working with packed files. Unfortunately, 
I did not find suitable tools for IDA, which would allow to make file 
modifications. Therefore, this project was created. 
It allows you to flush portions of code onto the disk without leaving 
the environment, as well as change program data both on disk and in memory._

_This program is useful to malware researchers, as well as other researchers 
working with packaged code._


## Version 
   **1.0.0** _first public working code_

## Supported features

* Dump portions of memory from debugged process
* Patch physical data on disk
