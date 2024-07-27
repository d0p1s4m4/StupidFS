#include <stdlib.h>
#include <libstpdfs/stpdfs.h>

static struct stpdfs_file *head = NULL;

struct stpdfs_file *
stpdfs_file_alloc(void)
{
	struct stpdfs_file *f;

	f = (struct stpdfs_file *)malloc(sizeof(struct stpdfs_file));
	if (f == NULL)
	{
		return (NULL);
	}

	f->refcount = 1;
	f->ip = NULL;
	f->offset = 0;
	f->next = head;

	head = f;

	return (f);	
}

void
stpdfs_file_close(struct stpdfs_file *f)
{
	struct stpdfs_file *tmp;

	f->refcount--;
	if (f->refcount <= 0)
	{
		for (tmp = head; tmp != NULL; tmp = tmp->next)
		{
			if (tmp->next == f)
			{
				tmp->next = f->next;
				break;
			}
		}
		free(f);
	}
}
