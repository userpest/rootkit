   obj-m := rootkit.o
#Kernel building system (include files mostly)
   KDIR := /lib/modules/`uname -r`/build
#current working directory - where to store the output
   PWD := `pwd`
#default build rule
default:
	make -C $(KDIR) M=$(PWD) modules
