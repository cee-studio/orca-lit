#!/bin/bash
for i in *.c;do
    ${CC} $i -c
done
