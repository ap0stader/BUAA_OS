# 1
mkdir ./test
# 2
cp -r ./code ./test/code
# 3
cat ./code/14.c
# 4
i=0
while [ $i -le 15 ]
do
	gcc -c "./test/code/$i.c" -o "./test/code/$i.o"
	let i=i+1
done
# 5
gcc ./test/code/*.o -o ./test/hello
# 6
./test/hello 2> ./test/err.txt
# 7
mv ./test/err.txt ./err.txt
# 8
chmod 655 ./err.txt 
# 9
if [ $# -eq 0 ]
then
	n1=1
	n2=1
elif [ $# -eq 1 ]
then
	n1=$1
	n2=1
else
	n1=$1
	n2=$2
fi
let n=n1+n2
sed -n "${n}p" ./err.txt >&2
