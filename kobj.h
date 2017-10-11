#ifndef KOBJ_H
#define KOBJ_H



typedef struct _attr_map 
{
	int *object;
	struct attribute attr;
}attr_map; // end struct _attr_map

ssize_t kobj_show(struct kobject *kobj, struct attribute *attr, char *buffer);
ssize_t kobj_store(struct kobject *kobj, struct attribute *attr, const char *buffer, size_t size);
void kobj_release( struct kobject *kobj);


#endif // KOBJ_H
