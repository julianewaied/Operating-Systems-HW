#include <linux/ctype.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>  	
#include <linux/slab.h>
#include <linux/fs.h>       		
#include <linux/errno.h>  
#include <linux/types.h> 
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#include "encdec.h"

#define MODULE_NAME "encdec"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Us Probably");

int 	encdec_open(struct inode *inode, struct file *filp);
int 	encdec_release(struct inode *inode, struct file *filp);
int 	encdec_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

ssize_t encdec_read_caesar( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

ssize_t encdec_read_xor( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

int memory_size = 0;

MODULE_PARM(memory_size, "i");

int major = 0;
char* buffer_xor;
char* buffer_caesar;

struct file_operations fops_caesar = {
	.open 	 =	encdec_open,
	.release =	encdec_release,
	.read 	 =	encdec_read_caesar,
	.write 	 =	encdec_write_caesar,
	.llseek  =	NULL,
	.ioctl 	 =	encdec_ioctl,
	.owner 	 =	THIS_MODULE
};

struct file_operations fops_xor = {
	.open 	 =	encdec_open,
	.release =	encdec_release,
	.read 	 =	encdec_read_xor,
	.write 	 =	encdec_write_xor,
	.llseek  =	NULL,
	.ioctl 	 =	encdec_ioctl,
	.owner 	 =	THIS_MODULE
};

typedef struct {
	unsigned char key;
	int read_state;
} encdec_private_data;

int init_module(void)
{
	// arbitrirally to find a major, could've used xor
	major = register_chrdev(major, MODULE_NAME, &fops_caesar);
	if(major < 0)
	{	
		return major;
	}
	buffer_caesar = kmalloc(sizeof(char)*memory_size);
	if(!buffer_caesar) exit(-1);
	buffer_xor = kmalloc(sizeof(char)*memory_size);
	if(!buffer_xor) exit(-1);
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(major,MODULE_NAME);
	kfree(buffer_caesar);
	kfree(buffer_xor);
}

int encdec_open(struct inode *inode, struct file *filp)
{
	if(!inode) exit(-1);
	if(!filp) exit(-1);
	int minor = MINOR(inode->i_rdev);

	if(minor == 0)		filp->f_op = fops_caesar;
	else if(minor == 1)	filp->f_op = fops_xor;
	else
	exit(-1);

	filp->private_data = kmalloc(sizeof(encdec_private_data));
	if(!flip->private_data) exit(1); 
	return 0;
}

int encdec_release(struct inode *inode, struct file *filp)
{
	if(!filp) exit(-1);
	kfree(filp->private_data);
	return 0;
}

void zerotize(char* buffer)
{
	int i;
	for(i=0; i<memory_size; i++)
	{
		buffer[i] = 0;
	}
}

int encdec_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	if((!filp) || (!inode)) exit(-1);
	switch(cmd):
	{
		case (ENCDEC_CMD_CHANGE_KEY): filp->private_data->key = (char) arg; break;
		case (ENCDEC_CMD_SET_READ_STATE): filp->private_data->read_state = arg; break;
		case (ENCDEC_CMD_ZERO): zerotize(buffer_caesar); zerotize(buffer_xor); break;
	}
	return 0;
}


ssize_t encdec_read_caesar( struct file *filp, char *buf, size_t count, loff_t *f_pos )
{
	if(!filp || !buf || count<0 || *f_pos <0) exit(-1);
	int k = filp->private_data->key;
	int i = 0;
	if(memory_size==(*f_pos)) return -EINVAL;
	char* data = kmalloc(sizeof(char)*count);
	if(filp->private_data->read_state == ENCDEC_READ_STATE_RAW)
	for(i = 0; i < count; i++)
		data[i] = (buffer_caesar[i+*f_pos]+k)%128;
		if(*fpos + i ==memory_size)	break;
	
	else if(filp->private_data->read_state == ENCDEC_READ_STATE_DECRYPT)
	for(i = 0; i < count; i++)
		data[i] = buffer_caesar[*f_pos + i];
		if(*fpos + i == memory_size)	break;
	copy_to_user(buf,data,i);
	*f_pos += i;
	kfree(data)
	if(i!=count) return -EINVAL;
	return count;
}

ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	if(!filp || !buf || count<0 || *f_pos <0) exit(-1);
	int i;
	data = kmalloc(sizeof(char)*count);
	if(!data) exit(-1);
	copy_from_user(data,buf,count)
	if((memory_size - *f_pos) < count) return -EINVAL;
	for(i=0; i<count; i++)
	{
		buffer_caesar[*f_pos + i] = data[i];
	}
	*f_pos += count;
	return count;
}

ssize_t encdec_read_xor( struct file *filp, char *buf, size_t count, loff_t *f_pos );

ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos);