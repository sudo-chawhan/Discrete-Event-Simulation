# Discrete-Event-Simulation
Implementing Rate-Monotonic Scheduling &amp; Earliest Deadline First Scheduling through Discrete Event Simulation

## For Linux only

* Clone the repository, using

 ```
 $ git clone https://github.com/sudo-chawhan/Discrete-Event-Simulation.git
 ```
 
* In terminal, change the current directory to cloned repo using

```
$ cd Discrete-Event-Simulation
```

* Edit input.txt to the desired inputs
 
* in the terminal, compile the .cpp files using

```
$ g++ Assgn1RMS_CS16BTECH11037.cpp|cat input.txt|./a.out" 
$ g++ Assgn1EDF_CS16BTECH11037.cpp|cat input.txt|./a.out"
```
  -- this will take input.txt file as the input so you can edit it for different inputs.

* five ".txt" files named "rms_log", "rms_stat", "edf_log", "edf_stat" and average_times.txt" will be generated
