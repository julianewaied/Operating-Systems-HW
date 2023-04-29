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
#define CAESAR 0
#define XOR 1
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Us Probably");

int 	encdec_open(struct inode *inode, struct file *filp);
int 	encdec_release(struct inode *inode, struct file *filp);
int 	encdec_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

ssize_t encdec_read(struct file *filp, char *buf, size_t count, loff_t *f_pos, char* data_buffer, int encryption);
ssize_t encdec_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos, char* data_buffer, int encryption);

ssize_t encdec_read_caesar( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

ssize_t encdec_read_xor( struct file *filp, char *buf, size_t count, loff_t *f_pos );
ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

int memory_size = 20;

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
	buffer_caesar = kmalloc( sizeof(char)*memory_size, GFP_KERNEL);
	if(!buffer_caesar) return -1;
	buffer_xor = kmalloc(sizeof(char)*memory_size, GFP_KERNEL);
	if(!buffer_xor) return -1;
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
	if(!inode) return -1;
	if(!filp) return -1;
	int minor = MINOR(inode->i_rdev);

	if(minor == 0)
		filp->f_op = &fops_caesar;
	else if(minor == 1)
		filp->f_op = &fops_xor;
	else
	return -1;

	filp->private_data = (void *) kmalloc(sizeof(encdec_private_data), GFP_KERNEL);
	if(!filp->private_data) return -1; 
	((encdec_private_data*)(filp->private_data))->key = 0;
	return 0;
}

int encdec_release(struct inode *inode, struct file *filp)
{
	if(!filp) return -1;
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
	if((!filp) || (!inode)) return -1;
	encdec_private_data* pd = (encdec_private_data*)(filp->private_data);
	if(cmd == ENCDEC_CMD_CHANGE_KEY)
		pd->key = arg;
	if(cmd == ENCDEC_CMD_SET_READ_STATE)
	{
		pd->read_state = arg;
	}
	if(cmd == ENCDEC_CMD_ZERO) 
	{
		zerotize(buffer_caesar); 
		zerotize(buffer_xor);
	}
	return 0;
}

ssize_t encdec_read(struct file *filp, char *buf, size_t count, loff_t *f_pos, char* data_buffer, int encryption)
{
	if(!filp || !buf || count<0 || *f_pos <0) return -EINVAL;
	encdec_private_data* pd = ((encdec_private_data*)filp->private_data);
	int k = pd->key;
	int i = 0;
	if(memory_size==(*f_pos)) return -EINVAL;
	char* data = kmalloc(sizeof(char)*count, GFP_KERNEL);
	if(pd->read_state == ENCDEC_READ_STATE_DECRYPT)
	{
		if(encryption == CAESAR)
			for(i = 0; i < count; i++)
			{
				if(*f_pos + i == memory_size)	break;
				data[i] = (data_buffer[i+*f_pos]-k + 128)%128;
			}
		else if(encryption == XOR)
			for(i = 0; i < count; i++)
			{
				if(*f_pos + i ==memory_size)	break;
				data[i] = data_buffer[i+*f_pos] ^ k;
			}
	}
	
	else if(pd->read_state == ENCDEC_READ_STATE_RAW)
	{
		for(i = 0; i < count; i++)
		{
			if(*f_pos + i == memory_size)	break;
			data[i] = data_buffer[*f_pos + i];
		}
	}
	copy_to_user(buf,data,i);
	*f_pos += i;
	kfree(data);
	if(i!=count) return -EINVAL;
	return count;
}

ssize_t encdec_write(struct file *filp, const char *buf, size_t count, loff_t* f_pos, char* data_buffer,int encryption)
{
	if(!filp || !buf || count<0 || *f_pos <0) return 1;
	int i = 0;
	encdec_private_data* pd = (encdec_private_data*) filp->private_data;
	if(!pd) return 1;
	int key = pd->key;
	if(memory_size == *f_pos) return -ENOSPC;
	char* data = kmalloc(sizeof(char)*count, GFP_KERNEL);
	if(!data) return 1;
	copy_from_user(data,buf,count);
	if(encryption == CAESAR)
	{
		for(i=0; i<count; i++)
		{
			if(*f_pos + i == memory_size) break;
			data_buffer[*f_pos + i] = (data[i]+key)%128;
		}
	}
	else if(encryption==XOR)
	{
		for(i=0; i<count; i++)
		{
			if(*f_pos + i == memory_size) break;
			data_buffer[*f_pos + i] = (data[i])^key;
		}
	}
	kfree(data);
	*f_pos = *f_pos + i;
	if(i<count) return -ENOSPC;
	return count;
}

ssize_t encdec_read_caesar( struct file *filp, char *buf, size_t count, loff_t *f_pos )
{
	return encdec_read(filp,buf,count,f_pos,buffer_caesar,CAESAR);
}

ssize_t encdec_write_caesar(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	return encdec_write(filp,buf,count,f_pos,buffer_caesar,CAESAR);
}

ssize_t encdec_read_xor( struct file *filp, char *buf, size_t count, loff_t *f_pos )
{
	return encdec_read(filp,buf,count,f_pos,buffer_xor,XOR);
}

ssize_t encdec_write_xor(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	return encdec_write(filp,buf,count,f_pos,buffer_xor,XOR);
}
