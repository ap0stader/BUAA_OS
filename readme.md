# Lab 0 Exam

## 准备工作：创建并切换到 `lab0-exam` 分支

请在**自动初始化分支后**，在开发机依次执行以下命令：

```console
$ cd ~/学号/
$ git fetch
$ git checkout lab0-exam
$ bash init.sh  # 请务必执行该命令，题目涉及的文件均由 init.sh 产生
```

本题完成之后，可以通过 `bash clean.sh` 清理本题产生的各文件，避免这些文件影响到其他的分支提交。

## 题目要求

## GCC & Makefile Quiz

完成初始化后，仓库根目录下已经生成好了 `Makefile` 、 `check.c` 、 `src` 文件夹和 `out` 文件夹。

在仓库根目录（如 `~/23xxxxxx` ）下编写 `Makefile` 。要求实现的功能如下：

| 执行命令     | 实现功能                                                       |
| ------------ | ------------------------------------------------------------ |
| `make check`     | 利用 `gcc` 将**同目录**下的 `check.c` 编译成名为 `check.o` 的未链接的目标文件，并放在同目录下。 |
| `make` | **先完成 `make check` 要求的功能**。然后利用 `gcc` 将 **`src` 目录**下的 `main.c`、`output.c` 编译成名为 `main` 的可执行文件，并放在 **`out` 目录**下。 |
| `make run`   | 直接运行可执行文件 `out/main` 。 |
| `make clean` | 删除目标文件 `check.o` 、可执行文件 `out/main`。 |

注意：实现 `make` 时，可能需要**指定头文件目录**。

## GCC & Bash Quiz

完成初始化后，仓库根目录下已经生成好了 `exam_1.sh` 至 `exam_9.sh` （从1开始的连续整数命名的.sh文件，共9个）、 `run_exam.sh` 、 `stderr.txt` 和 `origin` 文件夹。

在仓库根目录（如 `~/23xxxxxx` ）下编写 `exam_1.sh` 至 `exam_9.sh` 。要求 `exam_x.sh` **实现且仅实现**下述的第 x 点功能。	

这些脚本将被 `run_exam.sh` 调用执行。使用 `run_exam.sh` 的命令格式为 `bash run_exam.sh [n] [s] [t]` ，其中 `n` 为小于等于 9 的正整数，表示将**按顺序依次执行** `exam_1.sh` 至 `exam_n.sh` 。若 `n` 未给出，将默认 `n` 为 9。可选的 `s` 和 `t` 用于传递给 `exam_9.sh` ，具体要求见第 9 点功能。（注意：根据命令格式，若要指定 `s` 和 `t` ，必须先指定 `n` ）

1. 创建一个新的目录，名为 `result` 。再在 `result` 目录下创建两个新的目录，名为 `code` 和 `backup` 。

2. 将 `origin` 目录下的 `basic.c` 的文件中含有 `hello` （区分大小写）的行输出。

3. 将 `origin` 目录下的 `basic.c` 文件移动到 `result` 目录下。

4. 将 `origin` 目录下的 `code` 目录及其内容拷贝到 `result/backup` 目录下。

5. 将 `origin/code` 目录下的 `0.c` `1.c` `2.c` …… `20.c` （从0开始的**连续整数**命名的.c文件，共21个）文件中的**所有** `REPLACE` （区分大小写）替换为文件名（**不含扩展名**），替换的结果保存到 `result/code` 目录下的同名文件中。

   如对于这样的 `origin/code/1.c`

   ```c
   int main() {
     return REPLACE;
   }
   ```
   
   替换的结果如下，并且这个结果保存在 `result/code/1.c` 中
   
   ```c
   int main() {
     return 1;
   }
   ```
   
6. 将 `result/code` 目录下的**全部** `.c` 文件一起编译为可执行文件 `result/verify` 。

7. 运行可执行文件 `result/verify` ，将其**标准错误输出**的内容**追加**到仓库根目录下的 `stderr.txt` 文件。

8. 将 `stderr.txt` 文件的权限修改为 `r--r-----` 。

9. `run_exam.sh` 以 `bash exam_9.sh [s] [t]` 的格式向 `exam_9.sh` 传入两个参数 `s` , `t` （正整数，且 `s < t` ）。

   + 两个均缺省时，输出 `stderr.txt` 的所有内容

   + 仅缺省 `t` 时，输出 `stderr.txt` 自第 `s` 行开始（行号从 `1` 开始编号，输出**含**第 `s` 行）至结尾的内容

   + 两个均指定时，输出 `stderr.txt` 自第 `s` 行至第 `t` 行的内容（输出**含**第 `s` 行，**不含**第 `t` 行）

## 提示

- 下发文件中包含一个初始化脚本 `init.sh`，如果你想将你的工作目录下除了 `Makefile` 和 `exam_1.sh` 至 `exam_9.sh` 之外的所有文件恢复到题目下发时的状态，可以使用命令 `bash init.sh` 执行该脚本。
- 将多个文件一起编译，可以借助通配符 `*` 。如强制删除当前目录下的所有扩展名为 `.txt` 的文件可以使用 `rm -f *.txt` 。

## 参考输出

GCC & Makefile Quiz，使用下发的 `init.sh` 初始化后，执行 `make` 后再执行 `make run` 的参考结果为

```
>>>>>QUIZ OUTPUT.C CAUGHT 998244353<<<<<
```

对于GCC & Bash Quiz，使用下发的 `init.sh` 初始化后，执行 `bash run_exam.sh` 的参考结果为

```
............
    printf("RIGHT! hello world\n");
............
RIGHT! APPEND
>>>>QUIZ 1.C
>>>>QUIZ 2.C
>>>>QUIZ 3.C
>>>>QUIZ 4.C
>>>>QUIZ 5.C
>>>>QUIZ 6.C
>>>>QUIZ 7.C
>>>>QUIZ 8.C
>>>>QUIZ 9.C
>>>>QUIZ 10.C
>>>>QUIZ 11.C
>>>>QUIZ 12.C
>>>>QUIZ 13.C
>>>>QUIZ 14.C
>>>>QUIZ 15.C
>>>>QUIZ 16.C
>>>>QUIZ 17.C
>>>>QUIZ 18.C
>>>>QUIZ 19.C
>>>>QUIZ 20.C
```

*可能会有其他类似于 `exam_x.sh done` 的指示脚本执行情况信息，这些信息是由 `run_exam.sh` 输出到标准错误输出的。可在本地调试时作为参考，不会影响提交评测。*

## 提交评测 & 评测标准

请在开发机中执行下列命令后，**在课程网站上提交评测**。

```console
$ cd ~/学号/
$ git add -A
$ git commit -m "message" # 请将 message 改为有意义的信息
$ git push
```

测试点所对应的评测内容和分数分布如下：

| 测试点序号 | 评测内容                                       | 分值  |
| ---------- | ---------------------------------------------- | ----- |
| 1          | `GCC & Makefile Quiz` 的 `make check` 命令     | 7 分  |
| 2          | `GCC & Makefile Quiz` 的 `make` 命令           | 12 分 |
| 3          | `GCC & Makefile Quiz` 的 `make run` 命令       | 3 分  |
| 4          | `GCC & Makefile Quiz` 的 `make clean` 命令     | 5 分  |
| 5          | `GCC & Bash Quiz` 的要求 `1`                   | 3 分  |
| 6          | `GCC & Bash Quiz` 的要求 `2`                   | 8 分  |
| 7          | `GCC & Bash Quiz` 的要求 `3`                   | 4 分  |
| 8*         | `GCC & Bash Quiz` 的要求 `4`                   | 6 分  |
| 9          | `GCC & Bash Quiz` 的要求 `5`                   | 15 分 |
| 10         | `GCC & Bash Quiz` 的要求 `6`                   | 12 分 |
| 11         | `GCC & Bash Quiz` 的要求 `7`                   | 6 分  |
| 12         | `GCC & Bash Quiz` 的要求 `8`                   | 4 分  |
| 13         | `GCC & Bash Quiz` 的要求 `9` ，缺省 `s` 和 `t` | 3 分  |
| 14         | `GCC & Bash Quiz` 的要求 `9` ，仅缺省 `t`      | 5 分  |
| 15         | `GCC & Bash Quiz` 的要求 `9` ，不缺省          | 7 分  |

*测试点8的评测结果分两点展示，两点均为正确本测试点才能得分。*

评测只关注 `Makefile` 和 `exam_1.sh` 至 `exam_9.sh` ，修改其他文件不会对评测产生影响。

测试用例保证：

- 被操作的源文件均存在。
- 给 `exam_9.sh` 传入的参数 `s` 和 `t` 保证为正整数且不会定位到不存在的行。
- 不单独执行 `exam_1.sh` 至 `exam_9.sh` 的各文件，仅使用 `run_exam.sh` 按照题面所述格式按顺序执行。
