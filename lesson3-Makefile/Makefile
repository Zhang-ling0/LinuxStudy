BIN=text.exe
#SRC=$(shell ls *.c) #方法一
SRC=$(wildcard *.c) #wildcard函数，获取当前目录下的所有原文件
OBJ=$(SRC:.c=.o) #将.c换成.o
CC=gcc
Echo=echo
Rm=rm -f

$(BIN):$(OBJ)
	@$(CC) -o $@ $^ #交代一下各个.o文件应该怎么编.o文件当前并不存在
	@$(Echo) "linking $^ to $@...done"
%.o:%.c
	@$(CC) -c $<
	@$(Echo) "compling $< to $@...done"

.PHONY:clean
clean:
	$(Rm) $(OBJ) $(BIN)
.PHONY:text
text:
	@$(Echo) "Debug---"
	@$(Echo) $(SRC);
	@$(Echo) "Debug---"
	@$(Echo) $(OBJ);
	@$(Echo) "Debug---"
#BIN=text.exe
#SRC=text.c
#
#$(BIN):$(SRC)
#	@echo "开始编译代码"
#	gcc -o $@ $^
#	@echo "编译完成！"
#.PHONY:clean
#clean:
#	@echo "清理工程。。。"
#	rm -f $(BIN)
#	@echo "清理完毕。。。"


