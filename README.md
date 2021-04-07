# Project 2: Command Line Shell

**Author**: [Chuxi Wang](https://www.instagram.com/_mialsy_/ "click here to see awesome kitties, helps me survive this class :)") 

**Summmary**: This is the project 2 for CS 521 System Programming.

## About the project

### What is crash? 

### Data Structure - clist and elist
#### What is a clist and why are we using it?
#### Elist recap:
Please refer to: 

### Implemtation - process, built-in and external command
#### What is process?

#### Built-in "crash" functions

#### How to execute an external command?

## Program options

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
        - adding, getting, setting, and searching one element in the elist.
    - helper method ```idx_is_valid(struct elist *, size_t)``` to support checking if a given index is valid.
- **util.c**: This includes utilly methods for converting raw size in byte and time in millosecond to human readble format. 
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
>>-[😌]-[9]-[miasly@dirtmouth:~/projects/P2-mialsy]-> shell.c:493:sigchild_handler(): job done: 1859026
shell.c:494:sigchild_handler(): status: 0
exit
[miasly@dirtmouth P2-mialsy]$ 
```


## Testing

For running the test cases, use ```make test```. For updating test cases, runs ```make testupdate```. 

For more detailed testing, please refer to below:

- Run all tests:
    ```console
    [miasly@dirtmouth P1-mialsy]$ make test
    ```
- Run one test (let's say we want to run test x):
    ```console
    [miasly@dirtmouth P1-mialsy]$ make test run=[x]
    ```

- Run a few tests (let's say we want to run test x, y, z):
    ```console
    [miasly@dirtmouth P1-mialsy]$ make test run='[x] [y] [z]'
    ```

- **Note**: before run the test case, make sure you are in the correct directory with the makefile.

<>