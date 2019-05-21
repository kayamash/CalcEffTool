#!/bin/bash
branch="master"
#file="CalcEfficiency"
#add="git add CalcEfficiency"
add="git add -A"
message="add README"
push="git push origin "

eval $add
#eval $add$file
git commit -m "${message}"
eval $push$branch
