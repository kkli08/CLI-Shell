# CLI-Shell
Build a mini Command line shell interface.

`[systems programming]` `[process communication]` `[resource management]`


### Author
Ke Li

### Description
This is a School Project Using C and Linux.


Run program by:\
`make`\
`./shell379`


### Commands
`exit` 
* End the execution of shell379. Wait until all processes initiated by the
shell are complete. Print out the total user and system time for all
processes run by the shell.

`jobs`
* Display the status of all running processes spawned by shell379. See
the print format below in the example.

`kill <pid>`
* Kill process `<pid>`.

`resume <pid>`
* Resume the execution of process `<pid>`. This undoes a suspend.

`sleep <pid>`
* Sleep for `<int>` seconds.

`suspend <pid>`
* Suspend execution of process `<pid>`. A resume will reawaken it.

`wait <pid>`
* Wait until process `<pid>` has completed execution.

`<cmd> <arg>`
* Spawn a process to execute command `<cmd>` with 0 or more arguments `<arg>`. `<cmd>` and `<arg>` are each one or more sequences of nonblank characters.

