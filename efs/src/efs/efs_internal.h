/*
 * Filename:         efs_internal.h
 * Description:      EOS file system internal APIs

 * Do NOT modify or remove this copyright and confidentiality notice!
 * Copyright (c) 2019, Seagate Technology, LLC.
 * The code contained herein is CONFIDENTIAL to Seagate Technology, LLC.
 * Portions are also trade secret. Any use, duplication, derivation,
 * distribution or disclosure of this code, for any reason, not expressly
 * authorized is prohibited. All other rights are expressly reserved by
 * Seagate Technology, LLC.

 This file contains EFS file system internal APIs
*/

#ifndef _EFS_INTERNAL_H
#define _EFS_INTERNAL_H

#include <dstore.h>
#include <nsal.h>
#include "efs.h"
#include "kvnode.h"

enum efs_sys_attr_type
{
   EFS_SYS_ATTR_SYMLINK = 1,
   EFS_SYS_ATTR_MAX
};

/**
 * A kvnode is identified by a 128 bit node id. However, currently efs uses
 * 64 bit inodes in it's apis to identify the entities. The following apis
 * are a temporary arrangement to convert 64 bit inodes to 128 bit node ids.
 * @TODO: Cleanup these apis when efs filehandle (or equivalent) is
 * implemented. The filehandles would eventually use 128bit unique fids.
 */
static inline int node_id_to_ino(const node_id_t *nid, efs_ino_t *ino)
{
	dassert(nid != NULL);
	dassert(ino != NULL);

	*ino = nid->f_hi;
	return 0;
}

static inline int ino_to_node_id(const efs_ino_t *ino, node_id_t *nid)
{
	dassert(ino != NULL);
	dassert(nid != NULL);

	nid->f_hi = *ino;
	nid->f_lo = 0;

	return 0;
}

/** Set the extstore object identifier (kfid) with kvsns inode as the key */
int efs_set_ino_oid(struct efs_fs *efs_fs, efs_ino_t *ino, dstore_oid_t *oid);

/** Get the extstore object identifier for the passed efs inode */
int efs_ino_to_oid(struct efs_fs *efs_fs, const efs_ino_t *ino, dstore_oid_t *oid);

/** Delete the ino-kfid key-val pair from the kvs. Called during unlink/rm. */
int efs_del_oid(struct efs_fs *efs_fs, const efs_ino_t *ino);

/* Initialize the kvnode with given parameters
 *
 * @param[in] node *    - Kvnode pointer which will be initialized using kvnode
 *                        API to hold node information
 * @param[in] tree *    - Kvtree pointer which will be stored in kvnode on init
 * @param[in] ino *     - Inode of a file - file identifier
 * @param[in] bufstat * - Stat attribute buffer, stored in kvnode
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int efs_kvnode_init(struct kvnode *node, struct kvtree *tree,
		    const efs_ino_t *ino, const struct stat *bufstat);

/* Load kvnode for given file identifier from disk
 *
 * @param[in] node *    - Kvnode pointer which will be initialized using kvnode
 *                        API to hold node information
 * @param[in] tree *    - Kvtree pointer which will be stored in kvnode on init
 * @param[in] ino *     - Inode of a file - file identifier
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int efs_kvnode_load(struct kvnode *node, struct kvtree *tree,
		    const efs_ino_t *ino);

/* Store the stat associated with particular file inode held by given kvnode
 *
 * @param[in] node *    - Kvnode pointer which is already initialized and hold
 *                        the stat attributes which needs to be stored
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int efs_set_stat(struct kvnode *node);

/* Extract the stat attribute for particular file held by given kvnode
 *
 * @param[in] node *     - Kvnode pointer which is already initialzed and having
 *                         stat attributes
 * @param[in] bufstat ** - Stat attribute buffer pointer to store stat attribute
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int efs_get_stat(struct kvnode *node, struct stat **bufstat);

/* Delete the stat associated with particular file inode held by given kvnode
 *
 * @param[in] node *    - Kvnode pointer which is initialized and needs to be
 *                        deleted
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int efs_del_stat(struct kvnode *node);

/* Update the stat hold by given kvnode for given flag and re-store it back to
 * the disk
 *
 * @param[in] node *   - Kvnode pointer which is already initialzed and having
 *                       stat attributes
 * @param[in] flags    - Flag mask for which stats needs to be updated
 *
 * @return - 0 on sucess on failure error code given by kvnode APIs
 */
int efs_update_stat(struct kvnode *node, int flags);
/*
 * This API will store any type of system attributes associated with
 * given node
 *
 * @param[in] node *    - node for which system attributes to be stored
 * @param[in] value    - Buffer which needs to be be stored
 * @param[in] attr_type - Type of system attirbutes to be stored
 *
 * @return - 0 on success else error code return by kvnode APIs
 */
int efs_set_sysattr(const struct kvnode *node, const buff_t value,
		    enum efs_sys_attr_type attr_type);

/*
 * This API will fetch any type of system attributes associated with
 * given node
 *
 * @param[in] node *    - node for which system attributes to be fetched
 * @param[in] value *   - Ponter to buffer to store attributes
 * @param[in] attr_type - Type of system attirbutes to be fetched
 *
 * @return - 0 on success else error code return by kvnode APIs
 */
int efs_get_sysattr(const struct kvnode *node, buff_t *value,
		    enum efs_sys_attr_type attr_type);

/*
 * This API will delete any type of system attributes associated with
 * given node
 *
 * @param[in] node *    - node for which system attributes to be deleted
 * @param[in] attr_type - Type of system attirbutes to be deleted
 *
 * @return - 0 on success else error code return by kvnode APIs
 */
int efs_del_sysattr(const struct kvnode *node,
		    enum efs_sys_attr_type attr_type);
#endif
