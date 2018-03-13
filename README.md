# Linux_IO_Scheduling_Comparison
Tests and compares different I/O schedulers for Linux.

## Run test

```bash
# Clone this repository
git clone https://github.com/Johnstedt/Linux_IO_Scheduling_Comparison.git
# Go into the repository
cd Linux_IO_Scheduling_Comparison
# Compile
gcc -o bench benchmarkPolicies.c -lpthread -lm
# Run the app
./bench
# or run the script loopsched that runs ./bench for cfq, deadline and noop
bash loopsched.sh

```

