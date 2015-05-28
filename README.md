# CS360
Labs and Final Project for CS360 (Systems Programming) at WSU taught by K. C. Wang during the Spring 2014 semester.

These projects are all done in C (and occasionally some Assembly) and were compiled under a Linux distro (specifically Ubuntu 12.04). I make no guarentees that any of these will work perfectly with every system or work perfectly according to the project specifications.

Labs:
- Lab 1: See Lab1VS for the complete project. This was some initial work on the project.
- Lab 1VS: An sh simulator with basic commands such as cd and ls. Uses a tree data structure rather than working on an actual file system.
- Lab 2: 
  - Part 1: Implementation of a custom printf statement that acts the same as regular printf.
  - Part 2: Modifications of the stack so that the function returns to a different place than it was originally going to.
- Lab 3: An shell implementation that can interact with actual Linux commands and works on an actual file system. Has some built-in commands, but these use system calls to work. 
- Lab 4: Implements a client-server shell system. I wasn't able to find my completed version of the code, so there is only a version where local commands were implemented.
- Lab 4 Prelab: A simple client-server networking system sending back and forth some simple data.
- Lab 5: Prints disk information on a ext2 file system.
- Lab 5 Prelab: Prints disk information on a ext2 file system (less than the actual lab above).
- Lab 6: A preliminary setup of the end project implementing some basic commands.
- End: Implementation of a shell working on an ext2 file system. I only had time to implement Level 1, so it is missing some more advanced commands like cat, cp, etc., but what is there worked nearly perfectly.
