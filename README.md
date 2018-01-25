# Parser
## 1. Motivation/Aim
Program a simple Syntax Analyzer.
## 2. Content description
+ Input1: Stream of characters
>	- Assume that they are given in the file "./sample/Input.txt" 
+ Input2: Context-Free Grammar 
>	- Assume that they are defined in the file "./sample/Language.txt"
>
>	- Also, additonal priority relationship are defined in the file "./sample/Priority.txt"
>
+ Output: Sequence of reductions
> - The result is displayed in the file “./sample/Analysis Result.txt”
## 3. Ideas/Method
I have implemented a canonical LR(1) Parser using Soft Coding:
> 1. User can customize the productions being used by modified file "./sample/Language.txt".
> following the pattern "Production $(items in productions, seperated by space)$".
> 2. User can change the priority of the terminals.
> 3. Program constructs a "Production" object for each production.
> 4. Program merges the objects and finally constructs a "Language" object.
> 5. Program generates a LR_DFA for the "Language" object.
> 6. Program constructs a LR_Parser which includes a LR_DFA
> 7. Program calls the LR_Parser's "parseLine" method for dealing with a line of input sequence.
> 8. Program displays an analysis result for each input sequence.
## 4. Assumptions
The basic assumptions are given as follow:
> 1. There are not any "shift-shift" conflicts and "reduce-reduce" conflicts for constructing a parsing
table (even though the LR parsing table is not explicitly given).
> 2. The production whose id is 0 should contain an augmented-grammar starting label (i.e. S'), and
any other productions should not include the starting label as a component in its right handside part.
## 5. Core algorithms
> 1. Computing "first": referring to Dragon Book 4.4.2. ("first" method in file "Language.hpp")
> 2. Computing "clousure": referring to Dragon Book Algorithm 4.53. ("clousure" method in file "LR_DFA.hpp")
> 3. Constructing "LR_DFA": referring to Dragon Book Algorithm 4.53. ("construct" method in file "LR_DFA.hpp")
> 4. Parsing: referring to Dragon Book Figure 4-36. ("parseLine" method in file "LR_Parser.hpp")
## 6. Other important issues:
> + Q1: How to deal with ambiguity in an LR grammar? (For example, the "dangling else" problem)
>
> + A1: I use a terminal label stack ("terminal_stack" in "parseLine" method) to store previously 
appearing terminal labels. When constructing a LR_DFA, the program will maintain those cores where protential
conflicts will occur. And when parsing a sequence, the program will first find out the outing edges, 
then get all possible actions(a shift, a reduce or a shift and a reduce), then:
>> If a terminal label "a" corresponds to action "SHIFT" and the prority of label "a" is higher than 
the terminal label "b" which is the top element in terminal_stack, then the current action should be 
modified to "SHIFT".
>
> + Q2: Is the LR paring table indispensable for my LR parser?
>
> + A2: Not exactly. Program can be run merely based on previously constructed LR_DFA, because it 
contains all the information needed in parsing(like the label corresponding to an action, the next 
state for shifiting, and so on).
## 7. Use case in running
See the files in directory "sample".
