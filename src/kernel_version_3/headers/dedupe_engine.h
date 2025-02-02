/*
 * THIS  SOFTWARE   CONTAINS  CONFIDENTIAL  INFORMATION
 * AND TRADE SECRETS OF N.L. VERMA / DATUMSOFT SYSTEMS.
 * USE, DISCLOSURE, COPY OR  REPRODUCTION IS PROHIBITED
 * WITHOUT  THE  PRIOR  EXPRESS  WRITTEN  PERMISSION OF
 * NL VERMA / DATUMSOFT SYSTEMS.
 */

#ifndef _DEDUPE_ENGINE_H_
#define _DEDUPE_ENGINE_H_

#include "types.h"
#include "macros.h"
#include "metadata_engine.h"

//MD Disk Block (MDD_BLOCK) Used for keeping metadata on the MD disk.
//It is sizeof(MDD_BLOCK) and rounded off to next 64 bytes
#define METADATA_BLOCK_SIZE		64 			

#define PBA_HASH_COLLISION_TAG_START		101

#define	LALL	LOGI("lba_table_lock acquired\n")
#define	LRLL	LOGI("lba_table_lock released\n")

#define	LAPL	LOGI("pba_table_lock acquired\n")
#define	LRPL	LOGI("pba_table_lock released\n")

#define	LAHL	LOGI("hash_table_lock acquired\n")
#define	LRHL	LOGI("hash_table_lock released\n")

#ifdef DZ_KERNEL_VERSION_4
#define DZ_ZERO_PAGE(bio)						\
	{											\
		struct bvec_iter iter;					\
		BIOVEC	bvec;							\
		PVOID   	spagebuf= NULL;				\
		bio_for_each_segment(bvec, (bio), iter) {\
			spagebuf = kmap(bvec.bv_page);		\
			spagebuf += bvec.bv_offset;		\
			memset(spagebuf, 0, bvec.bv_len);	\
			kunmap(bvec.bv_page);				\
		} 										\
	}

#else

#define DZ_ZERO_PAGE(bio)						\
	{											\
		INT i;									\
		PBIOVEC	bvec		= NULL;				\
		PVOID   	spagebuf= NULL;				\
		bio_for_each_segment(bvec, (bio), i) {	\
			spagebuf = kmap(bvec->bv_page);		\
			spagebuf += bvec->bv_offset;		\
			memset(spagebuf, 0, bvec->bv_len);	\
			kunmap(bvec->bv_page);				\
		} 										\
	}

#endif



#define DZ_DDE_FETCH_PBA_ENTRY(pba_entry, pba_index)	\
	(pba_entry) =	GET_PBA_ENTRY((pba_index));	

#define DZ_DDE_FETCH_FREE_PBA_ENTRY(pba_entry, free_pba_index)	\
	free_pba_index	= dz_get_free_pba_index();			\
	(pba_entry) =	GET_PBA_ENTRY((free_pba_index));	\
	if ((pba_entry)->ref_cnt >= 1) {					\
		LOGE("Ref count for free pba can not be >= 1\n");\
		BUG_ON(1);										\
	}

#define DZ_DDE_UPDATE_HASH_TABLE	UPDATE_HBA_TABLE
#define UPDATE_HBA_TABLE(hba_entry, pba_index)			\
		dz_update_hash_entry((hba_entry), (pba_index))

#define DZ_DDE_UPDATE_LBA_TABLE(lba_entry, hash_index, collision_index)			\
	dz_update_lba_entry((lba_entry), (hash_index), (collision_index))


#define UPDATE_LBA_TABLE_NEW(lba_entry, hash_index)				\
	DZ_DDE_UPDATE_LBA_TABLE(lba_entry, hash_index, PBA_HASH_COLLISION_TAG_START)

#define DZ_DDE_UPDATE_LBA_TABLE_FOR_NEW_PBA(lba_entry, hash_index)				\
	DZ_DDE_UPDATE_LBA_TABLE(lba_entry, hash_index, PBA_HASH_COLLISION_TAG_START)

#define UPDATE_PBA_TABLE_NEW(pba_entry)		\
	DZ_DDE_UPDATE_PBA_TABLE(pba_entry, PBA_HASH_COLLISION_TAG_START)

#define DZ_DDE_UPDATE_PBA_TABLE_FOR_NEW_PBA(pba_entry)		\
	DZ_DDE_UPDATE_PBA_TABLE(pba_entry, PBA_HASH_COLLISION_TAG_START)

#define DZ_DDE_UPDATE_PBA_TABLE(pba_entry, collision_index)		\
	dz_update_pba_entry((pba_entry), collision_index, PAGE_SIZE, 1, 0); 

#define	DZ_DDE_ADD_NEW_PBA_TO_HASH_COLLISION_LIST(last_pba_index, free_pba_index)	\
			(GET_PBA_ENTRY((last_pba_index)))->next_entry_in_collision_list = (free_pba_index)




#define DUPLICATE_DATA_FOUND			1	//Data is duplicate with other PBAs hash
#define DUPLICATE_OVERWRITE_FOUND		2	//Data is duplicate with same PBA hash i.e. overwrite with same data on same lba
#define	HASH_COLLISION_FOUND			3	//Data is NOT duplicate but its hash is same, so collision occurs

#define DZ_DDE_WRITE_TYPE_DEDUPE			1	//Data is duplicate with other PBAs hash
#define DZ_DDE_WRITE_TYPE_OVERWRITE		2	//Data is duplicate with same PBA hash i.e. overwrite with same data on same lba
#define	DZ_DDE_WRITE_TYPE_HASH_COLLISON	3	//Data is NOT duplicate but its hash is same, so collision occurs

#define OVERWRITE_WITH_SAME_DATA		1
#define OVERWRITE_WITH_DIFFERENT_DATA	2
#ifdef DZ_BIT_SPIN_LOCK

#define	LBA_LOCK(lbab)		bit_spin_lock(0, &(lbab)->donotchange)
#define	LBA_UNLOCK(lbab)	bit_spin_unlock(0, &(lbab)->donotchange)

#define	PBA_LOCK(pbab)		bit_spin_lock(0, &(pbab)->donotchange)
#define	PBA_UNLOCK(pbab)	bit_spin_unlock(0, &(pbab)->donotchange)

#define	HASH_LOCK(hashb)	bit_spin_lock(0, &(hashb)->donotchange)
#define	HASH_UNLOCK(hashb)	bit_spin_unlock(0, &(hashb)->donotchange)

#else

//TODO: We need to rework on these locks if the above bit spin locks dont work
#define	LBA_LOCK(lba) 		spin_lock(&lba_table_lock)
#define	LBA_UNLOCK(lba) 	spin_unlock(&lba_table_lock)

#define	PBA_LOCK(lba) 		spin_lock(&pba_table_lock)
#define	PBA_UNLOCK(lba) 	spin_unlock(&pba_table_lock)

#define	HASH_LOCK(lba) 		spin_lock(&hash_table_lock)
#define	HASH_UNLOCK(lba) 	spin_unlock(&hash_table_lock)

#endif //End of DZ_BIT_SPIN_LOCK

#ifdef DZ_BIT_SPIN_LOCK

	//Note: We have used lba_lock, hash_lock and pba_lock as bit fields without increasing
	//the size of their corresponding structures.
	//Instead of using a full spinlock we are going to use a spinlock which is based on bits
	//Hope that it will not add any latency as compared to normal spinlocks
	//TODO:We need to verify that with extenstive tests
	//
	//LBA Block main data structure
	typedef union dz_lba_block_s {
		struct  {
			BOOL 	fec_lock:1; 	// Used for locking by fec engine but without normal spinlocks
									// This lock is used to protect ioq_head which is for serving reads
			BOOL 	lba_lock:1; 	// Used for locking this structure but without normal spinlocks
#define	FEC_LBA_LOCK_BIT	0
#define	DEDUPE_LBA_LOCK_BIT	1
			BOOL	allocated:1;	// LBA has not not received any IO on it
			BOOL	in_use:1;		// LBA is under process
			U32		hash_index:31;	// Used for getting hash_pba_index from hash_table
			U64 	hash_collision_tag:33; 	//Used in case of dedupe or hash collision. 
			U64 	pba_index:33; 	//Used in case of dedupe or hash collision. 
			// Basically it is an index into  multiple pba_entries which are linked during hash collision
			U64 	sector;			// Sector into DataDisk where actual data is stored
									// Used for reading purpose to achieve fast lookups. TODO:
			LIST_HEAD	ioq_head;	// Used by fec engine for queuing buffers for serving reads.
									// The moment a write IO is issued. It will be first stored in this ioq_head.
									// During the processing of write IO, if any reads comes in, then this queue
									// will be checked first. All this operation will happen in fec engine context
									// and fec_lock will be used to protect this ioq_head field 
		}__attribute__((__packed__));
		//}__attribute__((__packed__)) LBA_BLOCK, *PLBA_BLOCK;
		ULONG	donotchange; //Do not change this variable as it is used for bit_spin_lock
	}__attribute__((__packed__)) LBA_BLOCK, *PLBA_BLOCK;

	//HASH Block main structure
	typedef union dz_hash_block_s {
		struct {
			BOOL 	hash_lock:1;
			BOOL 	in_use:1;
			union {
				U64 	pba_index:33;		//This index denotes an entry in pba_table
				U64 	hash_pba_index:33;
			};
		}__attribute__((__packed__));
		//}__attribute__((__packed__)) HASH_BLOCK, *PHASH_BLOCK; 
		ULONG	donotchange; //Do not change this variable as it is used for bit_spin_lock
	}__attribute__((__packed__)) HASH_BLOCK, *PHASH_BLOCK, HBA_BLOCK, *PHBA_BLOCK; 

	//PBA Block main structure
	typedef union dz_pba_block_s {
		struct {
			BOOL 	pba_lock:1;
			BOOL 	in_use:1;
			BOOL	bad:1;
			union {
				SECTOR	sector:33;
				U64 	pba_index:33;		//This index denotes an entry in pba_table
				
			};
			U64 	hash_collision_tag:33; // This is the ID given to this pba to standout in case of collision
			U32 	size:13;
			U64 	ref_cnt:34;
			U64 	next_entry_in_collision_list:33; // Index of Next PBA in a collision list
		}__attribute__((__packed__));
		//}__attribute__((__packed__)) PBA_BLOCK, *PPBA_BLOCK; 
		ULONG	donotchange; //Do not change this variable as it is used for bit_spin_lock
	}__attribute__((__packed__)) PBA_BLOCK, *PPBA_BLOCK; 

#else // BIT_SPIN_LOCK


#endif // DZ_BIT_SPIN_LOCK

struct dz_thread_pool_s; // Forward Declaration / Proactive Declaration

typedef struct dz_completion_s {
	union {
		struct completion   completion;
		struct completion   io_completion;
	};
	INT					error;

}DZ_COMPLETION, *PDZ_COMPLETION;

typedef struct dz_data_pages_s {
	union {
		PBIOVEC	pagevec;
		PBIOVEC	bio_vec;
		PBIOVEC	biovec;
	};
	UINT	pgcnt;
}DATAPAGES, DATAPAGES_T, DATA_PAGES_T, *PDATA_PAGES_T, *PDATAPAGES_T, *PDATA_PAGES;

typedef struct dz_dedupe_info_s {
	union {
		U32 hash 	  [4];
		U32 hash32 	  [4];
		U64 hash64	  [2];
	};
	PLBA_BLOCK				lba_entry;
	PHBA_BLOCK				hba_entry;
	PPBA_BLOCK				pba_entry;

	U64						new_hash_index;
	U64						new_pba_index;

	PPBA_BLOCK				free_pba_entry;
	U64						free_pba_index;
	union { 
	// Entries are mutually exclusive. Either dedupe_pba_index
	// will be set or last_pba_collision_pba_index
	
		U64					dedupe_pba_index;
		U64					last_pba_hash_collision_index;
		U64					pba_index; //Used for Metadata updation
	};
} DD_INFO, *PDD_INFO, DEDUPE, *PDEDUPE;

// Main IO request structure
//We are not adding a bdev_name field in this structure deliberately because 
// we have single data device and meta device so we can handle it globally
// and reduce the occurence of extra space which will be occupied by this field.
union fec_data_write_s;
struct dz_iorequest_s;

//Dedupe VOL (DDVOL) Block main structure
typedef struct dz_ddvol_lba_block_s {
	union {
		U64 	lba_index; 	//Used for Data Volumes
							// OR (Mutually Exclusive)
		U64 	pba_index; 	//Used for Data Volume Read Only Snapshots
	};
	U32		lba_ref_cnt;
}__attribute__((__packed__)) DDVOL_LBA_BLOCK, *PDDVOL_LBA_BLOCK;


typedef struct dz_bitmap_s {
	SPINLOCK	bitmap_lock;
	ULONG		*bitmap;
	ATOMIC64	num_items;     
	U32			num_of_longs;
} BITMAP, *PBITMAP;


//Tables such as LBA, HASH and PBA related Function Declarations
RPVOID 	dz_create_lba_table(SIZE table_size);
RVOID 	dz_destroy_lba_table(PVOID ptr);
RPVOID 	dz_create_md_lock_table(SIZE table_size);
RVOID 	dz_destroy_md_lock_table(PVOID ptr);

RPVOID 	dz_create_pba_table(SIZE table_size);
RVOID 	dz_destroy_pba_table(PPBA_BLOCK ptr);
VOID 	dz_update_pba_entry(PPBA_BLOCK pba_entry, U64 colidx, U32 size, U64 ref_cnt, U64 next);
VOID 	dz_update_lba_entry(PLBA_BLOCK lba_entry, U32 hash_index, U64 colidx);
VOID 	dz_update_hash_entry(PHASH_BLOCK hash_entry, U64 pba_index);

RPVOID 	dz_create_hash_table(SIZE table_size);
RVOID 	dz_destroy_hash_table(PVOID ptr);

// Rnd Work / Testing
RVOID test_bitmap(VOID);
//
//PBA Bitmaps
RINT dz_pba_bitmap_init(VOID);
U64 dz_get_free_pba_index(VOID);
RINT dz_put_pba_index(U64 pba_index);
RVOID dz_print_bitmap(BITMAP *bitmap);

RINT dz_lba_bitmap_init(VOID);
S64 dz_get_free_lba_index(VOID);
RINT dz_put_lba_index(U64 lba_index);

RINT dz_process_tables(PPAGE page, SECTOR sector, LBA lba, UINT data_size);
RINT dz_check_dedupe(PIOREQUEST iorequest, U64 hash_pba_index, U64 *last_pba_index, U64 *next_collision_index );
PPBA_BLOCK dz_find_mapped_pba(U64 hash_collision_tag, U64 pba_index);
RVOID dz_dedupe_write_io(PIOREQUEST iorequest);
RINT dz_dedupe_new_write(PIOREQUEST iorequest);
RINT dz_dedupe_overwrite(PIOREQUEST iorequest);
RINT dz_dedupe_write_io_overwrite_with_same_hash(PIOREQUEST iorequest);
PHASH dz_dedupe_write_io_hash(PIOREQUEST iorequest);
RINT dz_dedupe_write_io_overwrite_with_different_hash(PIOREQUEST iorequest);
VOID dz_dedupe_write_io_overwrite_update_old_entries(U32 lba_hash_index, U64 lba_hash_collision_index);
RINT dz_dedupe_write_io_for_dedupe_and_collision_detection(PIOREQUEST iorequest);
RVOID dz_read_and_print_metadata_block(UINT offset, U64 sector, BOOL all);
RVOID dz_write_block_to_raw_disk_sync(PIOREQUEST , SECTOR , PLBA_BLOCK );
RINT	 dz_write_block_to_raw_disk_sync_internal(PIOREQUEST , SECTOR , PLBA_BLOCK );

RVOID 	dz_read_block_from_raw_disk_sync(PIOREQUEST , SECTOR, PLBA_BLOCK );
RVOID 	dz_read_block_from_raw_disk_sync_internal(PIOREQUEST , SECTOR, PLBA_BLOCK );
RVOID  	dz_dedupe_io(PIOREQUEST );
//RVOID 	dz_dedupe_write_io(PIOREQUEST piorequest);
RVOID 	dz_dedupe_io(PIOREQUEST iorequest);
RVOID 	dz_dedupe_write_io(PIOREQUEST iorequest);
RVOID 	dz_dedupe_read_io(PIOREQUEST piorequest);
RINT  	dz_dedupe_map_io(PIOREQUEST );

// Hash / Checksum Related Function Declarations
VOID 	test_hash(PVOID data, UINT size);
VOID 	test_hash2(char *, char *);
BOOL 	dz_same_hash(PHASH hash1, PHASH hash2);
U32 	dz_get_hash_index(U32 hash);

INT 	dz_dedupe_engine_init(VOID);
RVOID 	dz_dedupe_engine_exit(VOID);

VOID 	dz_dedupe_print_counters(VOID);
VOID 	dz_dedupe_init_counters(VOID);
RINT dz_process_dedupe_and_collision_detection(PIOREQUEST );
RINT dz_process_overwrite_for_dedupe_and_collision_detection(PIOREQUEST );
RINT dz_process_new_write_for_dedupe_and_collision_detection(PIOREQUEST );
VOID dz_dedupe_overwrite_update_old_entries(PIOREQUEST, U32 , U64 );

RVOID 	_ACQUIRE_LBA_LOCK_READ(PLBA_BLOCK lba_entry);
RVOID	_RELEASE_LBA_LOCK_READ(PLBA_BLOCK lba_entry);
RVOID 	_ACQUIRE_LBA_LOCK(PLBA_BLOCK lba_entry);
RVOID	_RELEASE_LBA_LOCK(PLBA_BLOCK lba_entry);
RVOID	_ACQUIRE_PBA_LOCK(PPBA_BLOCK pba_entry);
RVOID	_DZ_DDE_RELEASE_PBA_LOCK(PPBA_BLOCK pba_entry);
RVOID	_ACQUIRE_HBA_LOCK(PHBA_BLOCK hash_entry);
RVOID	_DZ_DDE_RELEASE_HBA_LOCK(PHBA_BLOCK hash_entry);

RVOID 	_ACQUIRE_LBA_LOCK_DEBUG(PLBA_BLOCK lba_entry);
RVOID	_RELEASE_LBA_LOCK_DEBUG(PLBA_BLOCK lba_entry);
RVOID	_ACQUIRE_PBA_LOCK_DEBUG(PPBA_BLOCK pba_entry);
RVOID	_RELEASE_PBA_LOCK_DEBUG(PPBA_BLOCK pba_entry);
RVOID	_ACQUIRE_HBA_LOCK_DEBUG(PHBA_BLOCK hash_entry);
RVOID	_RELEASE_HBA_LOCK_DEBUG(PHBA_BLOCK hash_entry);

U64 	dz_set_garbage_pba_index(U64 pba_index);

RINT 	dz_create_dedupe_pool(UINT num_dedupes);
RVOID 	dz_destroy_dedupe_pool(UINT num_dedupes);
RPVOID 	dz_dedupe_alloc(VOID);
RVOID 	dz_dedupe_free(PVOID dedupe);
RINT 	dz_create_hash_pool(UINT num_hashs);
RVOID 	dz_destroy_hash_pool(UINT num_hashs);
RPVOID 	dz_hash_alloc(VOID);
RVOID 	dz_hash_free(PVOID hash);
RVOID 	dz_destroy_dedupe_thread_pool(VOID);
RINT 	dz_create_dedupe_thread_pool(INT num_threads);
PLBA_BLOCK	GET_LBA_ENTRY(U64 lba);
PPBA_BLOCK	GET_PBA_ENTRY(U64 pba);
PHASH_BLOCK	GET_HBA_ENTRY(U64 hash);
PHASH_BLOCK	GET_HASH_ENTRY(U64 hash);
PMD_BLOCK   GET_MDB_ENTRY(U64 lba);

U64 dz_dedupe_get_lba_table_size(VOID);
U64 dz_dedupe_get_pba_table_size(VOID);
U64 dz_dedupe_get_hash_table_size(VOID);
#endif // _DEDUPE_ENGINE_H_
