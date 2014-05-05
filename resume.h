/* resume.h */

#ifndef RESUME_H
#define RESUME_H

/* Size of a buffer used for data storage */
#ifndef RESUME_BUFFER_SIZE
#define RESUME_BUFFER_SIZE 4000
#endif

/* Maxumum length of a block that can be read or written in one operation */
#ifndef RESUME_BLOCK_SIZE
#define RESUME_BLOCK_SIZE 512
#endif

/* The structure to represent 'resume' device. 
 *  data - data buffer;
 *  buffer_size - size of the data buffer;
 *  block_size - maximum number of bytes that can be read or written 
 *    in one call;
 *  resume_mutex - a mutex to protect the fields of this structure;
 *  cdev - character device structure.
 */
struct resume_dev {
	unsigned char *data;
	unsigned long buffer_size; 
	unsigned long block_size;  
	struct mutex resume_mutex; 
	struct cdev cdev;
};
#endif /* RESUME_H */
