#!/bin/bash
# appname 程序名称
# inputname 输入文件名
# outputname 输出文件名
# resultname 程序控制台输出结果重定向文件名

appname='gbn'
inputname='input.txt'
outputname='output1.txt'
resultname='result1.txt'

for ((i=1;i<=10;i++))
do
echo Test $appname $i
./$appname > $resultname 2>&1
cmp $inputname $outputname
echo Test $i over
done

echo 'Test GBN over'

appname='sr'
outputname='output2.txt'
resultname='result2.txt'
for ((i=1;i<=10;i++))
do
echo Test $appname $i
./$appname > $resultname 2>&1
cmp $inputname $outputname
echo Test $i over
done

echo 'Test SR over'

appname='tcp'
outputname='output3.txt'
resultname='result3.txt'
for ((i=1;i<=10;i++))
do
echo Test $appname $i
./$appname > $resultname 2>&1
cmp $inputname $outputname
echo Test $i over
done

echo 'Test TCP over'