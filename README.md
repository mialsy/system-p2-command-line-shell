# Project 2: Command Line Shell

**Author**: [Chuxi Wang](https://www.instagram.com/_mialsy_/ "click here to see awesome kitties, helps me survive this project :)") 

**Summmary**: This is the project 2 for CS 521 System Programming. In this project, I implemented a command line text-based interface shell named ```crash```, which allows the user to execute shell built-ins (inlcuding ```cd```, ```history```, ```jobs```, background execution, and history execution), as well as external executable programs in interactive mode or scripting mode. 

## About the project

Back in the dark age of computer without GUI, shell is the only easy way for human to interct with people. To get a better understanding of how things work in the acient days, we built a ```crash``` program that provide such function. 

### What is ```crash```? 

```crash``` is our own version of the command line interface shell. It basically allows user to interact with the computers in the following ways:

- Run built-in functions:
    - Including change the CWD, browsing past commands, comment, listing background jobs, and exiting the ```crash``` program.
    - For detail about running the ```crash```, please check [**Program Options**](https://github.com/usf-cs521-sp21/P2-mialsy#program-options).
- Run external executable functions:
    - This allows user to run external executable functions that are avaiable.
    - Background job are also supported in ```crash```, please review [**Program Options**](https://github.com/usf-cs521-sp21/P2-mialsy#program-options) for how to run background job.

The workflow of how ```crash``` works is as follow: 
<img src="https://github.com/usf-cs521-sp21/P2-mialsy/blob/main/res/p2_main_function.jpg" alt="crash main function workflow" width="900">

### Data Structure - clist and elist
#### What is a clist and why are we using it?

To support the built-in history browsing and retriving, an history list based on circular list (clist) structure is included. 

The clist is essencially a list that gets override when a limit has been reached. It has a size limit (also is the capacity for the list), when the clist reaches the limit, the newly added element will override the old ones.

Here is a domenstration of how clist works:
<img src="https://github.com/usf-cs521-sp21/P2-mialsy/blob/main/res/clist_demo.gif" alt="demo of the clist adding operation" width="700">

#### Elist and partial matching search in elist
For elist implementaion, please refer to [P1 readme file](https://github.com/usf-cs521-sp21/P1-mialsy#project-1-elastic-array--disk-analyzer).

A new feature has been added in this version of elist, partial matching of a elist element. This is implemented as we may need to have an indexing of the elist, for our case in this project, we need to search a job element in the jobs elist based on a job's process id (pid) rather than the entire job element (which has a pid and a command string).

To perform the partial matching, the pid was designed to be placed in the very begining of the job struct, thus if we only compare the size of pid in one elist element, we can know whether the pid is matched. 

The original search index method:
|stuct job_item|pid space| command space|
|---|---|---|
|compared|yes|yes|

The original search index method (only compare ```sizeof(pid)``` from start):
|stuct job_item|pid space| command space|
|---|---|---|
|compared|yes|**no**|

Note that for simplicity, this method only allows searching the first field of the struct as we only take input of the size to compare. It can be further extended to ```memcpy``` starting from an offset, in this way we can compare fields in the middle of the struct as well.

### Implemtation - process, built-in and external command
#### What is a process and why do we need a child process for external command?

A process is basically an instance of the running program. 

In the ```crash``` program, we fork a child process to execute an external command. 

The reason we need different process working on this is that we can do then have differnt handling of signal of the child process, and also allow our ```crash``` to run background jobs. Another reason is that by doing so, we would be able to set up enviroment of the execution of program in the child process, and would be able to redirect IO.

#### What is a signal? 

Signal is a way to pass information between proces, and allows process to interact with each other. A signal can be send to a proccesss, and handled by a signal handler. In the ```crash``` command line interface, ```SIGINT``` and ```SIGCHID``` are the two process got handled.

Handling ```SIGINT``` allows ```crash``` to quit a running child process without quiting the interface. 

And ```SIGCHLD``` is handled because we want to have jobs running in the background. As ```SIGCHLD``` handler send signal to parent when child process exits, we can keep track of process that is running in the backgdound.

#### Built-in commands

To implemet built-in commands, the input string would be tokenzied and compared with the built-in commands, and once we find a match, the built-in command would be executed. 

Unlike the external command, the built-in command will not need a fork of child process, however the status of running the built-in command would also reflects on the ui. 

## Program options

- Running ```crash```: no flag needs to be passed in. 
- Running built-ins in ```crash```:
    - ```cd```: used to changes CWD;
    - ```#<comment>```: this is ignored by crash;
    - ```history```: used to browse past commands;
    - ```!!```, ```!<history_num>```, ```!<history_prefix>```: used to execute previous command
    - ```jobs```: used to browse all running background jobs
    - ```<command> &```: used to execute command in the background

## Included files

Here is a list of files that is included:

- **Makefile**: Included to compile and run the programs.
- **shell.c** The driver for ```crash``` program. Includes main function and supportive functions and structs:
    - ```main()``` function that handles the program logic;
    - ```struct job_item``` that describe a background job;
    - supportive funtions to:
        - tokenize the input string;
        - handle built-in functions;
        - SIGINT and SIGCHLD handlers;
        - print usage and say goodbye.
- **history.c**: This includes struct and functions that are used to create a history list, which is basically a wrapper for clist. 
    - ```struct hist_clist``` that stores the history commands;
    - supprotive functions including:
        - creating and destory the hist_clist;
        - printing the history;
        - adding in the hist_list;
        - searching hist_list by index, or by prefix;
        - getting hist_list last added index;
        - validating if an index is valid in hist_list；
        - private method that supporting iteration in hist_clist.
- **ui.c**: This includes functions that support an text based command line ui. It includes:
    - initializing and destroying ui;
    - set status in prompt to display previous execution status;
    - key down and key up to roll back or move forward in history commands;
    - supprotive function to construct the prompt:
        - get current working directory, and truncated in shortcut version as needed;
        - get username, hostname, and home diretory. 
- **elist.c**: This include a elastic list data structure ```elist``` and functions that supports the ```elist``` operations.       
    - ```struct elist``` which stores the element and metadata of the elist.
    - elist opreation functions including:
        - creating, sorting, and destorying the elist;
        - clearing out the memory on elist;
        - getting and resetting the elist capacity;
        - getting the elist size;
        - adding, getting, setting, and searching one element in the elist;
        - **New feature:** partial match search is also supported in this version of elist. 
    - helper method ```idx_is_valid(struct elist *, size_t)``` to support checking if a given index is valid.
- **elist.h**: header files for elist.
- **logger.h**: Included for log output. 
- Header filess are also included for the c files.

For compiling the program, runs:
```console
[miasly@dirtmouth P2-mialsy]$ make
```

For running the ```crash``` in interative mode, runs:
```console
[miasly@dirtmouth P2-mialsy]$ ./crash
```

For running the ```crash``` in a scripting mode, either use a pipe  or input redirect.
```console
[miasly@dirtmouth P2-mialsy]$ cat <somefile> |./crash
```

Or: 

```console
[miasly@dirtmouth P2-mialsy]$ ./crash < <somefile>
```

## Program output

The example out put of running crash on scripting mode is as follow. 

```console
[miasly@dirtmouth P2-mialsy]$ cat << EOM | ./crash
ls
pwd
whoami
history
EOM
shell.c:131:main(): data piped in on stdin; entering script mode
cat.txt       elist.c  hist       history.o    logger.h    read       shell.c  ui.c  valgrind_out.txt
commands.txt  elist.h  history.c  libelist.so  Makefile    README.md  shell.o  ui.h
crash         elist.o  history.h  libshell.so  named_pipe  res.txt    tests    ui.o
/home/miasly/projects/P2-mialsy
miasly
  1 ls
  2 pwd
  3 whoami
  4 history
```

The example output of running crash on interactive mode is as follow. 

```console
[miasly@dirtmouth P2-mialsy]$ ./crash
ui.c:44:init_ui(): Initializing UI...
ui.c:47:init_ui(): Setting locale: en_US.UTF-8
============== welcome to crash ==============
crash is a command line shell that supports: 
        1. build in functions: 
                1.1 cd - for entering directory
                1.2 history - for review command history
                1.3 !<command number> 
                or !<command prefix> 
                or !! - for executing history command
                1.4 jobs - for listing currently-running background jobs
                1.5 exit - for exiting the crash
        2. & - background job
        3. # - comment
shell.c:137:main(): stdin is a TTY; entering interactive mode
>>-[😌]-[1]-[miasly@dirtmouth:~/projects/P2-mialsy]-> ls
crash    elist.h  history.c  history.o    libshell.so  Makefile   shell.c  tests  ui.h
elist.c  elist.o  history.h  libelist.so  logger.h     README.md  shell.o  ui.c   ui.o
>>-[😌]-[2]-[miasly@dirtmouth:~/projects/P2-mialsy]-> !!
crash    elist.h  history.c  history.o    libshell.so  Makefile   shell.c  tests  ui.h
elist.c  elist.o  history.h  libelist.so  logger.h     README.md  shell.o  ui.c   ui.o
>>-[😌]-[3]-[miasly@dirtmouth:~/projects/P2-mialsy]-> history
  1 ls
  2 ls
  3 history
>>-[😌]-[4]-[miasly@dirtmouth:~/projects/P2-mialsy]-> sleep fefef
sleep: invalid time interval ‘fefef’
Try 'sleep --help' for more information.
>>-[🤯]-[5]-[miasly@dirtmouth:~/projects/P2-mialsy]-> sleep 30 &
>>-[🤯]-[6]-[miasly@dirtmouth:~/projects/P2-mialsy]-> ls
crash    elist.h  history.c  history.o    libshell.so  Makefile   shell.c  tests  ui.h
elist.c  elist.o  history.h  libelist.so  logger.h     README.md  shell.o  ui.c   ui.o
>>-[😌]-[7]-[miasly@dirtmouth:~/projects/P2-mialsy]-> jobs
[0] 1859026 sleep 30 &
>>-[😌]-[8]-[miasly@dirtmouth:~/projects/P2-mialsy]-> jobs
[0] 1859026 sleep 30 &
>>-[😌]-[9]-[miasly@dirtmouth:~/projects/P2-mialsy]-> 
exit
 ================================ 
( Goodbye, have a wonderful day! )
 ================================ 
  o
    o
       /\__)o < 
      |       \  
      | O . O | 
       \_____/ 
[miasly@dirtmouth P2-mialsy]$ 
```

## Testing

For running the test cases, use ```make test```. For updating test cases, runs ```make testupdate```. 

For more detailed testing, please refer to below:

- Run all tests:
    ```console
    [miasly@dirtmouth P2-mialsy]$ make test
    ```
- Run one test (let's say we want to run test x):
    ```console
    [miasly@dirtmouth P2-mialsy]$ make test run=[x]
    ```

- Run a few tests (let's say we want to run test x, y, z):
    ```console
    [miasly@dirtmouth P2-mialsy]$ make test run='[x] [y] [z]'
    ```

- **Note**: before run the test case, make sure you are in the correct directory with the makefile.
