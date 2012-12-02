#include <linux/version.h>
#include <linux/module.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Michal Antonczak");
MODULE_VERSION("0.1");
MODULE_DESCRIPTION("totally harmless module");

static int __init m_init(void){
	printk(KERN_INFO "rkit module loaded");
	return 0; 

}

static void __exit m_exit(void){
	printk(KERN_INFO "unloading");
	return;
}
module_init(m_init);
module_exit(m_exit);
