# Project 2: Command Line Shell

**Author**: [Chuxi Wang](https://www.instagram.com/_mialsy_/ "click here to see awesome kitties, helps me survive this project :)") 

**Summmary**: This is the project 2 for CS 521 System Programming. In this project, I implemented a command line text-based interface shell named ```crash```, which allows the user to execute shell built-ins (inlcuding ```cd```, ```history```, ```jobs```, background execution, and history execution), as well as external executable programs in interactive mode or scripting mode. 

## About the project

Back in the dark age of computer without GUI, shell is the only easy way for human to interct with people. To get a better understanding of how things work in the acient days, we built a ```crash``` program that provide such function. 

### What is ```crash```? 

```crash``` is our own version of the command line interface shell. It basically allows user to interact with the computers in the following ways:

- Run built-in functions:
    - Including change the CWD, browsing past commands, comment, listing background jobs, and exiting the ```crash``` program.
    - For detail about running the ```crash```, please check [Program Options]().
- Run external executable functions:
    - This allows user to run external executable functions that are avaiable.
    - Background job are also supported in ```crash```, please review [Program Options]() for how to run background job.

The workflow of how ```crash``` works is as follow: 

### Data Structure - clist and elist
#### What is a clist and why are we using it?

To support the built-in history browsing and retriving, an history list based on circular list (clist) structure is included. 

The clist is essencially a list that gets override when a limit has been reached. It has a size limit (also is the capacity for the list), when the clist reaches the limit, the newly added element will override the old ones.

Here is a domenstration of how clist works:


#### Elist and partial matching search in elist
For elist implementaion, please refer to [P1 readme file]().

A new feature has been added in this version of elist, partial matching of a elist element. This is implemented as we may need to have an indexing of the elist, for our case in this project, we need to search a job element in the jobs elist based on a job's process id (pid) rather than the entire job element (which has a pid and a command string).

To perform the partial matching, the pid was designed to be placed in the very begining of the job struct, thus if we only compare the size of pid in one elist element, we can know whether the pid is matched. 

The original search index method:
|stuct job_item|pid space| command space|
|---|---|---|
|comapred|yes|yes|

The original search index method (only compare ```sizeof(pid)``` from start):
|stuct job_item|pid space| command space|
|---|---|---|
|comapred|yes|**no**|

Note that for simplicity, this method only allows searching the first field of the struct as we only take input of the size to compare. It can be further extended to ```memcpy``` starting from an offset, in this way we can compare fields in the middle of the struct as well.

### Implemtation - process, built-in and external command
#### What is a process?

#### What is a signal? 

#### Built-in "crash" functions

#### How to execute an external command?

## Program options

- Running ```crash```:
- Running built-ins in ```crash```:
    - ```cd``` that changes CWD;
    - ```#<comment>``` that is ignored by crash;
    - ```history``` to browse past commands;
    - ```!!```, ```!<history_num>```, ```!<history_prefix>``` to execute previous command
    - ``````

## Included files

Here is a list of files that is included:

- **Makefile**: Included to compile and run the programs
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

Header filess are also included for the c files.

For compiling the program, runs:
```console
[miasly@dirtmouth P2-mialsy]$ make
```

For runing the crahs, runs:
```console

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