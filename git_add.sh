#!/bin/sh
ADD_FILE=.git.add
for FILE in `cat $ADD_FILE`
do
  echo "$FILE"
  git add $FILE
done
