/*
 * THIS  SOFTWARE   CONTAINS  CONFIDENTIAL  INFORMATION
 * AND TRADE SECRETS OF N.L. VERMA / DATUMSOFT SYSTEMS.
 * USE, DISCLOSURE, COPY OR  REPRODUCTION IS PROHIBITED
 * WITHOUT  THE  PRIOR  EXPRESS  WRITTEN  PERMISSION OF
 * NL VERMA / DATUMSOFT SYSTEMS.
 */

#include "prototypes.h"

PDZ_INTERVAL_NODE 	INTERVAL_TREE_ROOT = NULL;
PDZ_INTERVAL_NODE 	INTERVAL_TREE_HEAD = NULL;
PDZ_INTERVAL_NODE 	OVERLAP_NODE = NULL;
BOOL				INTERVAL_OVERLAP_FOUND = FALSE;

////////////////////////////////////////////////////////////
//Prototype Interval Node specific functions
////////////////////////////////////////////////////////////
PDZ_INTERVAL_NODE			dz_alloc_interval_node(VOID);
RVOID						dz_free_interval_node(PDZ_INTERVAL_NODE);
static RVOID 				dz_print_interval_tree_node(PDZ_INTERVAL_NODE node, BOOL);
static RVOID 				dz_print_interval(PDZ_INTERVAL interval);
static RVOID 				dz_print_interval_tree_level(PDZ_INTERVAL_NODE root, INT des, INT curr);
static PDZ_INTERVAL_NODE 	dz_find_minimum_interval(PDZ_INTERVAL_NODE root);
PDZ_INTERVAL_NODE 			dz_delete_interval(PDZ_INTERVAL_NODE root, PDZ_INTERVAL interval);

////////////////////////////////////////////////////////////
//Prototype Interval Tree specific functions
////////////////////////////////////////////////////////////
static RVOID dz_postorder_interval_tree(PDZ_INTERVAL_NODE node);
static RVOID dz_preorder_interval_tree(PDZ_INTERVAL_NODE node);
static RVOID dz_inorder_interval_tree(PDZ_INTERVAL_NODE node);
static BOOL	 dz_check_overlap(PDZ_INTERVAL interval, PDZ_INTERVAL interval2);
static PDZ_INTERVAL_NODE dz_insert_interval(PDZ_INTERVAL_NODE root, PDZ_INTERVAL interval);

////////////////////////////////////////////////////////////
//Interval Tree Node specific functions
////////////////////////////////////////////////////////////


static RVOID dz_print_interval_tree_node(PDZ_INTERVAL_NODE node, BOOL flag)
{
	if (!node) {
		LOGTGT("Node Print: Interval Node is NULL\n");
		return;
	}


	if (flag == TRUE) {
		LOGTGT("[-----------INTERVAL NODE----------]\n");
		LOGTGT("[                                  ]\n");
		LOGTGT("[ NODE           = %p]\n", node);
		LOGTGT("[ NODE->interval = [%lli - %lli]]\n", node->interval.low, node->interval.high);
		LOGTGT("[ NODE->max      = %lli]\n", node->max);
		LOGTGT("[ NODE->left     = %p]\n", node->left);
		LOGTGT("[ NODE->right    = %p]\n", node->right);
		LOGTGT("[ NODE->parent   = %p]\n", node->parent);
		LOGTGT("[----------------------------------]\n");
		//LOGTGT("node->parent   = %p\n", node->parent);
		//LOGTGT("node->root     = %p\n", node->root);
	} else {
		LOGTGT("[-----------INTERVAL------------]\n");
		LOGTGT("[ NODE->interval = [%lli - %lli]]\n", node->interval.low, node->interval.high);
		LOGTGT("[-------------------------------]\n");

	}

}

static RVOID dz_print_interval(PDZ_INTERVAL interval)
{
	LOGTGT("INTERVAL : [%lli - %lli]", interval->low, interval->high);
}

PDZ_INTERVAL_NODE dz_alloc_interval_node(VOID)
{
    PDZ_INTERVAL_NODE node = dz_kmalloc(sizeof(DZ_INTERVAL_NODE), GFP_KERNEL);
	if (!node) {
		LOGTGTE("%s","Error, Unable to allocate memory for interval tree node\n");
		RETURNN;
	}
	MEMSET(node);
	return node;
}

RVOID dz_free_interval_node(PDZ_INTERVAL_NODE node)
{
	if (!node) {
		LOGTGTE("%s","Error, Freeing an already freedup interval node\n");
		return;
	}
    dz_kfree(node, sizeof(DZ_INTERVAL_NODE));
	return;
}

////////////////////////////////////////////////////////////
//Interval Tree specific functions
////////////////////////////////////////////////////////////
static RVOID dz_postorder_interval_tree(PDZ_INTERVAL_NODE node)
{

	//dz_print_interval_tree_node(node);

	if (node->left) {
		dz_postorder_interval_tree(node->left);
	}

	if (node->right) {
		dz_postorder_interval_tree(node->right);
	}
}

static RVOID dz_preorder_interval_tree(PDZ_INTERVAL_NODE node)
{
	//dz_print_interval_tree_node(node);

	if (node->left) {
		dz_preorder_interval_tree(node->left);
	}

	if (node->right) {
		dz_preorder_interval_tree(node->right);
	}
}

static RVOID dz_inorder_interval_tree(PDZ_INTERVAL_NODE node)
{
	if (node->left) {
		dz_inorder_interval_tree(node->left);
	}
	dz_print_interval_tree_node(node, TRUE);

	if (node->right) {
		dz_inorder_interval_tree(node->right);
	}
}

static BOOL	 dz_check_overlap(PDZ_INTERVAL interval1, PDZ_INTERVAL interval2)
{
	if (interval1->low <= interval2->high && interval2->low <= interval1->high) {
		LOGTGT("OVERLAP FOUND: [%lli %lli] [%lli %lli]\n",
				interval1->low, interval1->high,
				interval2->low, interval2->high);

		INTERVAL_OVERLAP_FOUND = TRUE;
		OVERLAP_NODE = (PDZ_INTERVAL_NODE)interval1;
		dz_print_interval_tree_node(OVERLAP_NODE, TRUE);
        return TRUE;
	} else {
    	return FALSE;
	}
}

static PDZ_INTERVAL_NODE dz_find_minimum_interval(PDZ_INTERVAL_NODE root)
{
	 /* loop down to find the leftmost leaf */
    while (root->left != NULL)
        root = root->left;
 
    return root;
}

UINT	dz_height_interval_tree(PDZ_INTERVAL_NODE root)
{
	UINT lheight = 0;
	UINT rheight = 0;

	if (root) {
		lheight = dz_height_interval_tree(root->left);
		rheight = dz_height_interval_tree(root->right);

		if (lheight > rheight) {
			return (lheight + 1);
		} else {
			return (rheight + 1);
		}
	} else {
		return 0;
	}
}

static RVOID	dz_print_interval_tree_level(PDZ_INTERVAL_NODE root, INT des, INT curr)
{

	if (root) {
		if (des == curr) {
			dz_print_interval((PDZ_INTERVAL)root);
		} else {

			dz_print_interval_tree_level(root->left, des, curr + 1);
			dz_print_interval_tree_level(root->right, des, curr + 1);

		}


	}
	des = curr;
	return;
}

RVOID	dz_print_interval_tree(PDZ_INTERVAL_NODE root)
{
	int h, i;
	h = dz_height_interval_tree(root);
	LOGTGT("HEIGHT : %d\n", h);
	for (i = 0; i < h; i++) {
		LOGTGT("LEVEL %d  :", i);
		dz_print_interval_tree_level(root, i, 0);
		LOGTGT("\n");
    }
}

//This function is slightly different than a delete interval function.
//This does not traverse the full interval tree, instead it deletes the current node
//from the tree
//dz_delete_interval() function searches the complete tree for a given interval and then
//it deletes. Typically the search starts from root node, 

RVOID dz_delete_current_interval_node(PDZ_INTERVAL_NODE node)
{

	PDZ_INTERVAL_NODE temp = NULL;
	PDZ_INTERVAL_NODE parent = NULL;
	if (!node) {
		LOGTGT ("Node is NULL\n");
		return;
	}
	if (!node->parent) {
		LOGTGT ("Node Parent is NULL\n");
		return;
	}

	parent = node->parent;


	//find out if node lies in left side of parent
	if (parent->left == node) {
		if (node->left == NULL && node->right != NULL) {
			temp = node->right;
			dz_free_interval_node(node);
			parent->left = temp;
			temp->parent = parent;
		} else if (node->right == NULL && node->left != NULL) {
			temp = node->left;
			dz_free_interval_node(node);
			parent->left = temp;
			temp->parent = parent;
		} else {
			//Refer Figure 12.4 Page 263, Chapter 12 in Book  Introduction to Algorithms by Cormen
			temp = dz_find_minimum_interval(node->right);
			node->interval = temp->interval;
			node->right = dz_delete_interval(node->right, &(temp->interval));
		}
	} 


	//find out if node lies in right side of parent
	if (parent->right == node) {
		if (node->left == NULL && node->right != NULL) {
			temp = node->right;
			dz_free_interval_node(node);
			parent->right = temp;
			temp->parent = parent;
		} else if (node->right == NULL && node->left != NULL) {
			temp = node->left;
			dz_free_interval_node(node);
			parent->right = temp;
			temp->parent = parent;
		} else {
			//Refer Figure 12.4 Page 263, Chapter 12 in Book  Introduction to Algorithms by Cormen
			temp = dz_find_minimum_interval(node->right);
			node->interval = temp->interval;
			node->right = dz_delete_interval(node->right, &(temp->interval));
		}
	} 

	return;
}

PDZ_INTERVAL_NODE dz_delete_interval(PDZ_INTERVAL_NODE root, PDZ_INTERVAL interval)
{
	if (root == NULL) {
		return NULL;
	}

	if (interval->low < ((PDZ_INTERVAL)root)->low) {
		root->left 	= dz_delete_interval(root->left, 	interval);

	} else if (interval->low > ((PDZ_INTERVAL)root)->low) {
		root->right = dz_delete_interval(root->right, 	interval);

	} else if ((interval->low == ((PDZ_INTERVAL)root)->low) && (interval->high == ((PDZ_INTERVAL)root)->high)) {
		PDZ_INTERVAL_NODE temp = NULL;
		if (root->left == NULL && root->right == NULL) {
			dz_free_interval_node(root);
			root = NULL;

		} else if (root->left == NULL) {
			temp = root->right;
			temp->parent = root->parent;
			dz_free_interval_node(root);
			return temp;

		} else if (root->right == NULL) {
			temp = root->left;
			temp->parent = root->parent;
			dz_free_interval_node(root);
			return temp;

		} else {
			//Comes here when left and right children exists
			//Refer Figure 12.4 Page 263, Chapter 12 in Book  Introduction to Algorithms by Cormen

			//First find the minimum value of its right subtree
			temp = dz_find_minimum_interval(root->right);
			temp->parent = root->parent;
			root->interval = temp->interval; //Duplicate the Interval
			root->right = dz_delete_interval(root->right, &(temp->interval)); //Delete the duplicate interval

		}
	} else {
		root->right = dz_delete_interval(root->right, 	interval);
	}

	return root;
}

static PDZ_INTERVAL_NODE dz_insert_interval(PDZ_INTERVAL_NODE root, PDZ_INTERVAL interval)
{
	if (INTERVAL_OVERLAP_FOUND == TRUE) {
		return root;
	}

	if (root == NULL) {
		return (PDZ_INTERVAL_NODE)interval;
	}

	dz_check_overlap((PDZ_INTERVAL)root, interval);

	if (interval->low < ((PDZ_INTERVAL)root)->low) {
		root->left 	= dz_insert_interval(root->left, 	interval);
		if (root->left) {
			root->left->parent 	= root;
		}
	} else {
		root->right = dz_insert_interval(root->right, 	interval);
		if (root->right) {
			root->right->parent 	= root;
		}
	}

	if (root->max <  interval->high) {
		root->max =  interval->high;
	}
	
	return root;
}

LBA dz_findmax_interval_tree(LBA a, LBA b, LBA c)
{

	return a>b?(a>c?a:c):(b>c?b:c);

	RETURNS;
}

spinlock_t dz_spinlock;
rwlock_t dz_rwlock;
struct rw_semaphore dz_rwsemaphore;
struct semaphore dz_semaphore;
VOID dz_semaphore_test(VOID)
{
	sema_init(&dz_semaphore, 1);
	//LOG("RWSEM_WAITING_BIAS      = %ld, 0x%lx\n",RWSEM_WAITING_BIAS,RWSEM_WAITING_BIAS);
	//LOG("RWSEM_ACTIVE_READ_BIAS  = %ld, 0x%lx\n",RWSEM_ACTIVE_READ_BIAS,RWSEM_ACTIVE_READ_BIAS);
	//LOG("RWSEM_ACTIVE_WRITE_BIAS = %ld, 0x%lx\n",RWSEM_ACTIVE_WRITE_BIAS,RWSEM_ACTIVE_WRITE_BIAS);
	LOG("Initial Semaphore           count = %u, count = 0x%x\n", dz_semaphore.count, dz_semaphore.count);
	down(&dz_semaphore);
	LOG("After down() Semaphore      count = %u, count = 0x%x\n", dz_semaphore.count, dz_semaphore.count);
	up(&dz_semaphore);
	LOG("After up() Semaphore        count = %u, count = 0x%x\n", dz_semaphore.count, dz_semaphore.count);


}

VOID dz_spin_lock_test(VOID)
{
	spin_lock_init(&dz_spinlock);
	LOG("SPINLOCK =%u, = 0x%x\n", (UINT)dz_spinlock.rlock.raw_lock.head_tail, (UINT)dz_spinlock.rlock.raw_lock.head_tail);

}

VOID dz_read_write_spin_lock_test(VOID)
{
	rwlock_init(&dz_rwlock);
	LOG("Initial RW raw_lock.lock             = %lld, raw_lock.lock = 0x%llx\n", dz_rwlock.raw_lock.lock, dz_rwlock.raw_lock.lock);
	LOG("Initial RW raw_lock.read             = %u, raw_lock.read = 0x%x\n", dz_rwlock.raw_lock.read, dz_rwlock.raw_lock.read);
	LOG("Initial RW raw_lock.write            = %d, raw_lock.write  = 0x%x\n", dz_rwlock.raw_lock.write, dz_rwlock.raw_lock.write);

	read_lock(&dz_rwlock);
	LOG("After READ_LOCK RW raw_lock.lock     = %lld, raw_lock.lock = 0x%llx\n", dz_rwlock.raw_lock.lock, dz_rwlock.raw_lock.lock);
	LOG("After READ_LOCK RW raw_lock.read     = %u, raw_lock.read = 0x%x\n", dz_rwlock.raw_lock.read, dz_rwlock.raw_lock.read);
	LOG("After READ_LOCK RW raw_lock.write    = %d, raw_lock.write  = 0x%x\n", dz_rwlock.raw_lock.write, dz_rwlock.raw_lock.write);

	read_unlock(&dz_rwlock);
	LOG("After READ_UNLOCK RW raw_lock.lock   = %lld, raw_lock.lock = 0x%llx\n", dz_rwlock.raw_lock.lock, dz_rwlock.raw_lock.lock);
	LOG("After READ_UNLOCK RW raw_lock.read   = %u, raw_lock.read = 0x%x\n", dz_rwlock.raw_lock.read, dz_rwlock.raw_lock.read);
	LOG("After READ_UNLOCK RW raw_lock.write  = %d, raw_lock.write  = 0x%x\n", dz_rwlock.raw_lock.write, dz_rwlock.raw_lock.write);
	write_lock(&dz_rwlock);
	LOG("After WRITE_LOCK RW raw_lock.lock    = %lld, raw_lock.lock = 0x%llx\n", dz_rwlock.raw_lock.lock, dz_rwlock.raw_lock.lock);
	LOG("After WRITE_LOCK RW raw_lock.read    = %u, raw_lock.read = 0x%x\n", dz_rwlock.raw_lock.read, dz_rwlock.raw_lock.read);
	LOG("After WRITE_LOCK RW raw_lock.write   = %d, raw_lock.write  = 0x%x\n", dz_rwlock.raw_lock.write, dz_rwlock.raw_lock.write);
	write_unlock(&dz_rwlock);
	LOG("After WRITE_UNLOCK RW raw_lock.lock  = %lld, raw_lock.lock = 0x%llx\n", dz_rwlock.raw_lock.lock, dz_rwlock.raw_lock.lock);
	LOG("After WRITE_UNLOCK RW raw_lock.read  = %u, raw_lock.read = 0x%x\n", dz_rwlock.raw_lock.read, dz_rwlock.raw_lock.read);
	LOG("After WRITE_UNLOCK RW raw_lock.write = %d, raw_lock.write  = 0x%x\n", dz_rwlock.raw_lock.write, dz_rwlock.raw_lock.write);
}

VOID dz_read_write_semaphore_test(VOID)
{
	init_rwsem(&dz_rwsemaphore);
	LOG("RWSEM_WAITING_BIAS      = %ld, 0x%lx\n",RWSEM_WAITING_BIAS,RWSEM_WAITING_BIAS);
	LOG("RWSEM_ACTIVE_READ_BIAS  = %ld, 0x%lx\n",RWSEM_ACTIVE_READ_BIAS,RWSEM_ACTIVE_READ_BIAS);
	LOG("RWSEM_ACTIVE_WRITE_BIAS = %ld, 0x%lx\n",RWSEM_ACTIVE_WRITE_BIAS,RWSEM_ACTIVE_WRITE_BIAS);
	LOG("Initial RWSemaphore           count = %ld, count = 0x%lx\n", dz_rwsemaphore.count, dz_rwsemaphore.count);

	down_read(&dz_rwsemaphore);
	LOG("After down_read  RWSemaphore  count = %ld, count = 0x%lx\n", dz_rwsemaphore.count, dz_rwsemaphore.count);
	up_read(&dz_rwsemaphore);
	LOG("After up_read    RWSemaphore  count = %ld, count = 0x%lx\n", dz_rwsemaphore.count, dz_rwsemaphore.count);

	down_write(&dz_rwsemaphore);
	LOG("After down_write RWSemaphore  count = %ld, count = 0x%lx\n", dz_rwsemaphore.count, dz_rwsemaphore.count);
	up_write(&dz_rwsemaphore);
	LOG("After up_write   RWSemaphore  count = %ld, count = 0x%lx\n", dz_rwsemaphore.count, dz_rwsemaphore.count);

}


VOID dz_lock_test(VOID)
{

/*
typedef union {
        s64 lock;
        struct {
                u32 read;
                s32 write;
        };
} arch_rwlock_t;
*/


	//LOG("RW_LOCK_BIAS = 0x%lx\n", RW_LOCK_BIAS);
	//LOG("WRITE_LOCK_CMP = 0x%lx\n", WRITE_LOCK_CMP);

	LOG("CONFIG_NR_CPUS = %d, 0x%x\n", CONFIG_NR_CPUS, CONFIG_NR_CPUS);
	dz_spin_lock_test();
	dz_read_write_spin_lock_test();
	dz_semaphore_test();
	dz_read_write_semaphore_test();

}

PDZ_BINARY_NODE dz_binary_tree_insert(PDZ_BINARY_NODE root, INT data)
{
	if (root == NULL) {
		root = DZ_ALLOC_BINARY_NODE();
		MEMSET(root);
		root->data = data;
		return root;
	}

	if (data < root->data) {
		root->left = dz_binary_tree_insert(root->left, data);
	} else {
		root->right = dz_binary_tree_insert(root->right, data);
	}

	return root; //Always Top of the tree root
}

PDZ_BINARY_NODE dz_heap_max_insert(PDZ_BINARY_NODE root, INT data)
{
	if (root == NULL) {
		root = DZ_ALLOC_BINARY_NODE();
		MEMSET(root);
		root->data = data;
		return root;
	}

	if (data < root->data) {
		root->left = dz_binary_tree_insert(root->left, data);
	} else {
		root->right = dz_binary_tree_insert(root->right, data);
		//Since it is a heap max
		//Swap with root data
		data = root->data;
		root->data = root->right->data;
		root->right->data = data;

	}

	return root; //Always Top of the tree root
}


BOOL dz_process_interval_tree(LBA offset, UINT iosize)
{
	PDZ_INTERVAL_NODE node = dz_alloc_interval_node();

	dz_lock_test();

	if (INTERVAL_TREE_ROOT == NULL ) {
		INTERVAL_TREE_ROOT = node;
		node->interval.low    	= offset;
		node->interval.high   	= node->interval.low + iosize - 1;
		node->max    			= node->interval.high;
		dz_print_interval((PDZ_INTERVAL)node);
		LOGTGT("\n");
		RETURNT;
	} else {
		node->interval.low    	= offset;
		node->interval.high   	= node->interval.low + iosize - 1;
		node->max    			= node->interval.high;
	}

	//Allocation is done seperately so that we could avoid allocation in spinlock
	dz_print_interval((PDZ_INTERVAL)node);
	LOGTGT("\n");

	INTERVAL_TREE_ROOT = dz_insert_interval(INTERVAL_TREE_ROOT, (PDZ_INTERVAL)node);

	if (!INTERVAL_TREE_ROOT) {
		RETURNFA;
	}

	if (INTERVAL_OVERLAP_FOUND) {
		dz_free_interval_node(node);
	}

	LOGTGT("INTERVAL TREE -> IN-ORDER   TRAVERSAL\n");
	dz_inorder_interval_tree(INTERVAL_TREE_ROOT);

	LOGTGT("TREE ROOT\n");
	dz_print_interval_tree_node(INTERVAL_TREE_ROOT, TRUE);

	LOGTGT("INTERVAL TREE -> PRE-ORDER  TRAVERSAL\n");
	dz_preorder_interval_tree(INTERVAL_TREE_ROOT);

	LOGTGT("INTERVAL TREE -> POST-ORDER TRAVERSAL\n");
	dz_postorder_interval_tree(INTERVAL_TREE_ROOT);

	LOGTGT("INTERVAL TREE -> LEVEL-ORDER TRAVERSAL\n");
	dz_print_interval_tree(INTERVAL_TREE_ROOT);

	if (INTERVAL_OVERLAP_FOUND) {
		LOGTGT("DELETING OVERLAP NODE\n");
		dz_delete_current_interval_node(OVERLAP_NODE);
		dz_print_interval_tree(INTERVAL_TREE_ROOT);
		dz_inorder_interval_tree(INTERVAL_TREE_ROOT);
	}
	RETURNT;
}

EXPORT_SYMBOL(dz_process_interval_tree);