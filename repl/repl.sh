#!/bin/bash
echo "" > tmp.pn;

while [ true ]
do
  read -p "> " cmd;
  if [ "$cmd" == ".clear" ]; then
    echo "" > tmp.pn;
    clear;
    echo "[cleared]";

  elif [ "$cmd" == ".exit" ]; then
    exit 0;
  elif [ "$cmd" == ".read" ]; then
    cat tmp.pn;
  else
    echo $cmd >> tmp.pn;
    echo " -- ";
    ./pinecone tmp.pn;
  fi
done