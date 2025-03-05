if [ $# -eq 1 ]; then
  sed -n "${1},\$p" stderr.txt
elif [ $# -eq 2 ]; then
  sed -n "${1},$(($2-1))p" stderr.txt
else
  cat stderr.txt
fi
