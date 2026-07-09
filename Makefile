text1:text1.c
	gcc text1.c -o text1
.PHONY:clean #表示常调用
clean:
	@rm -f text1
