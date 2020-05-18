# What is this?

This is a small application that can read a circuit from input and evaluate the output of the circuit.

# Input format

## Netlist format

The `.bench` file described the netlist. Please specify all the inputs and outputs first by using the statement `INPUT(<SIG_NAME>)` or `OUTPUT(<SIG_NAME>)`, than begin describing all the gates. Gates should follow the following format:

`<OUTPUT_SIG> = <GATE_TYPE>(<INPUT_SIG1>, <INPUT_SIG2>, ..., <INPUT_SIGN>)`

Where the following gates are supported (capitalization doesn't matter):

```
AND: 2 or more inputs
OR: 2 or more inputs
NAND: 2 or more inputs
NOR: 2 or more inputs
XOR: 2 or more inputs
XNOR: 2 or more inputs
NOT: 1 input
BUF: 1 input
```

These formats are all valid:

```
X5=AND(X1,X2,X3)
X5 = AND(X1, X2, X3)
X5=AND( X1,X2,X3 )
```

## Input value format

Use `<INPUT_SIG> <VALUE>` to specify all the input values. Values are restricted to `0` or `1`.

# How to invoke

Compile the file first, then use `./main <netlist-file> <input-value-file>`. An `.out` file will be generated with output values.

# Error checking

The program will not check syntax errors for you. Remember to follow the format!
