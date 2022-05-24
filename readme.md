
<center>

# Votex CLI
Votex is a simple, robust and friendly command line tool. It is developed as an experimental project to get good grasp of the fundamentals.

</center>


## Technologies and Languages:

* C++
* Makefile

## Compilation Command:

* `g++ *.cpp -o main`
* `./main`


## Commands Provided: 


* `exit`
  * exit the command line

* `cd <dirname>`
  * change directory given the dirrectory name

* `pwd`
  * print working directory

* `ls`
  * list the directories and files in working directory 
  * directories will be displayed with parentheses

* `ll`
  * list the details of the directories and files in working directory

* `mkdir <dirname>`
  * make a new directory under working directory

* `touch <filename> <size> <tag>`
  * make a new file under working directory

* `rm [-r] <name>`
  * delete a file / directory under the working directory given its name
  * the "-r" flag is optional, which means to recursively remove

* `mv <target> <destination>`
  * move the target (file / directory) to the destination (directory)

* `tag <Tag>`
  * list the details of the files of a certain tag, will search the working directory recursively

* `tree`
  * display the file system in a tree view, will search the working directory recursively

* `help`
  * list all the commands and their usages

---


<!-- ## Implemented

- rm <filename>         == DONE
- touch                 == DONE
- mkdir                 == DONE
- rm -r directory       == DONE
- mv
- tag <name> -->