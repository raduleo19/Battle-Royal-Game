# Copyright 2018 
# Rica Radu Leonard 315CA
# ElfUnknown's - Glacierfight

## Move
    In the function move I computed the new coords and I checked if player has 
    enough stamina to make the move. If an elf meets another one they will fight
    using the following condition:
    if an elf A fights and elf named B the hits needed to kill A are 
    (A.hp + B.dmg - 1) / B.dmg and the hits needed to kill B are 
    (B.hp + A.dmg - 1) / A.dmg so if A starts the hits needed to kill B 
    remains (B.hp + A.dmg - 1) / A.dmg - 1. The winner is the one who needs 
    fewer hits to kill the other one.

## Snowstorm
    For snowstorm I extracted the data from that package and for all players 
    alive on the glacier affected by the snowstorm I updated theirs hp.

## Scoreboard
    To print the scoreboard I sorted indexes of elfs stored in an auxilliary 
    array by required criteria (dry/wet, eliminated, lexicographically) 
    with bubble sort.

## Meltdown
    For meltdown I reallocated memory to keep only the current glacier.
    Also I kept track of meltdown operations executed.
    For the others functions I took into account that indexes are shifted 
    "meltdowns" positions to North-West.

## Data storage and miscellaneous
    I stored data in two structures, one for elfs and one for glacier.
    I allocated dinamically the elfs array, their names and the glacier 
    bidimensional array.
    At the end of program I released all the memory used.
    For best precision I used distances squared, to keep the variables 
    integers.

    
