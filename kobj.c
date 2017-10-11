#include <linux/init.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include "kobj.h"

static struct kobj_type *my_ktype;
static struct kobject *root;
static struct kobject *left;
static struct kobject *right;

static struct sysfs_ops ops =
{
	.show = &kobj_show,
	.store = &kobj_store
}; // end ops

static int f1 = 10;
static int f2 = 20;
//const mode_t mode = 0664;

static attr_map map[] =
{
	{&f1,{.name = "f1", .mode = 0664}},
	{&f2,{.name = "f2", .mode = 0664}},
	{NULL, {.name = 0, .mode = 0}}
}; // end map[]

ssize_t kobj_show(struct kobject *kobj, struct attribute *attr, char *buffer)
{
	int *object;
	int i = 0;
	for (i = 0; map[i].attr.name; i++)
	{
		if (strcmp(attr->name, map[i].attr.name) == 0)
		{
			object = map[i].object;
			return sprintf(buffer, "%d\n", *object);
		} // end if
	} // end for
	
	// error
	return -1;
} // end kobj_show()


ssize_t kobj_store(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size)
{
	int object;
	kstrtoint(buffer, 10, &object);
	
	int i = 0;
	for (i = 0; map[i].attr.name; i++)
	{
		if (strcmp(attr->name, map[i].attr.name) == 0)
		{
			*map[i].object = object;
			return size;
			
		} // end if
	} // end for		
	
	// error 
	return -1;
} // end kobj_store()


void kobj_release( struct kobject *kobj)
{
	;
} // end kobj_release()
static int __init kobj_init(void)
{
	// set up the kobject type
	struct kobj_type tk1;
	my_ktype = &tk1;
	memset(my_ktype, 0, sizeof(*my_ktype));
	my_ktype->release = &kobj_release;
	my_ktype->sysfs_ops = &ops;
	my_ktype->default_attrs = NULL;
	
	// set up the root kobject
	struct kobject t;
	root = &t;
	memset(root, 0, sizeof(*root));
	root->kset = NULL;
	kobject_init_and_add(root, my_ktype, kernel_kobj, "t");
	
	// set up the left sub-kobject
	struct kobject l;
	left = &l;
	memset(left, 0, sizeof(*left));
	left->kset = NULL;
	kobject_init_and_add(left, my_ktype, root, "l");
	
	// set up the right sub-kobject
	struct kobject r;
	right = &r;
	memset(right, 0, sizeof(*right));
	right->kset = NULL;
	kobject_init_and_add(right, my_ktype, root, "r");
	
	
	// add attributes to the left kobject
	int i;	
	for (i = 0; map[i].object; i++)
	{
		sysfs_create_file(left, &map[i].attr);
	} // end for
	printk(KERN_INFO "Done init.\n");
	return 0;
} // end kobj_init()

static void __exit kobj_exit(void)
{

	// remove attributes from the left sub-kobject
	int i;
	for (i = 0; map[i].object; i++)
	{
		sysfs_remove_file(left, &map[i].attr);
	} // end for
	
	// delete the kobjects themselves
	//kobject_put(right);
	kobject_del(right);
	//kobject_put(left);
	kobject_del(left);
	//kobject_put(root);
	kobject_del(root);
	printk(KERN_INFO "Done exit.\n");
} // end kobj_exit()

module_init(kobj_init);
module_exit(kobj_exit);

MODULE_DESCRIPTION("kobj");
MODULE_AUTHOR("Joe Nathan Abellard");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
