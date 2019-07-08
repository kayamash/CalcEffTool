#!/bin/bash
branch="CloseByMuon"
#file="CalcEfficiency"
add="git add -A"
message="add new method"
push="git push origin "

eval $add
#eval $add$file
git commit -m "${message}"
eval $push$branch
