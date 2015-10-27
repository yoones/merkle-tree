/*
** This file is part of libmerkle-tree.
**
** libmerkle-tree is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** libmerkle-tree is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with libmerkle-tree.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <openssl/sha.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "mt.h"

static int	_hash_data_blocks(t_mt *mt, t_mt_file *mtf);
static int	_build_tree(t_mt *mt);

int	mt_file_mmap(t_mt_file *mtf, char *filepath)
{
  if ((mtf->fd = open(filepath, O_RDONLY)) == -1
      || (int)(mtf->filesize = lseek(mtf->fd, 0, SEEK_END)) == -1
      || mtf->filesize == 0
      || (int)lseek(mtf->fd, 0, SEEK_SET) ==  -1
      || (mtf->addr = mmap(NULL, mtf->filesize, PROT_READ, MAP_PRIVATE, mtf->fd, 0)) == MAP_FAILED)
    {
      if (mtf->fd != -1)
	close(mtf->fd);
      return (-1);
    }
  return (0);
}

int	mt_file_munmap(t_mt_file *mtf)
{
  if (munmap(mtf->addr, mtf->filesize) == -1
      || close(mtf->fd) == -1)
    return (-1);
  return (0);
}

int	mt_tree_build(t_mt *mt, t_mt_file *mtf, unsigned int blocksize)
{
  mt->filesize = mtf->filesize;
  mt->blocksize = blocksize;
  if (btarr_init(&mt->tree, DIGEST_LENGTH, mt_get_effective_nb_blocks(mt), NULL))
    return (-1);
  if (_hash_data_blocks(mt, mtf) == -1
      || _build_tree(mt) == -1)
    {
      free(mt->tree.root);
      return (-1);
    }
  return (0);
}

int	mt_tree_free(t_mt *mt)
{
  free(mt->tree.root);
  return (0);
}

static int	_hash_data_blocks(t_mt *mt, t_mt_file *mtf)
{
  unsigned int	remaining;
  SHA_CTX	ctx_file;
  SHA_CTX	ctx_chunk;
  unsigned char	*chunk;
  char		*data;
  unsigned int	nb_chunks;
  unsigned char	empty_block[DIGEST_LENGTH];

  data = mtf->addr;
  chunk = btarr_get_most_left_leaf(&mt->tree, 1); /* root is 1 or 0 ?? */
  remaining = mt->filesize;
  nb_chunks = 0;
  /* hash data */
  if (SHA1_Init(&ctx_file) != 1)
    return (-1);
  while (remaining >= mt->blocksize)
    {
      if (SHA1_Init(&ctx_chunk) != 1
	  || SHA1_Update(&ctx_chunk, data, mt->blocksize) != 1
	  || SHA1_Final(chunk, &ctx_chunk) != 1
	  || SHA1_Update(&ctx_file, data, mt->blocksize) != 1)
	return (-1);
      chunk += DIGEST_LENGTH;
      data += mt->blocksize;
      remaining -= mt->blocksize;
      nb_chunks++;
    }
  if (remaining)
    {
      if (SHA1_Init(&ctx_chunk) != 1
	  || SHA1_Update(&ctx_chunk, data, remaining) != 1
	  || SHA1_Final(chunk, &ctx_chunk) != 1
	  || SHA1_Update(&ctx_file, data, remaining) != 1)
	return (-1);
      chunk += DIGEST_LENGTH;
      nb_chunks++;
    }
  if (SHA1_Final(mt->file_digest, &ctx_file) != 1)
    return (-1);
  /* complete with empty blocks to make a complete balanced binary tree */
  if (SHA1_Init(&ctx_chunk) != 1
      || SHA1_Final(empty_block, &ctx_chunk) != 1)
    return (-1);
  while (nb_chunks < mt->tree.nb_leafs)
    {
      memcpy(chunk, empty_block, DIGEST_LENGTH);
      chunk += DIGEST_LENGTH;
      nb_chunks++;
    }
  return (0);
}

static int	_build_tree(t_mt *mt)
{
  SHA_CTX	ctx;
  unsigned int	i;
  unsigned int	most_left_read_node_idx;
  unsigned int	most_left_write_node_idx;
  unsigned char	*read_node;
  unsigned char	*write_node;

  most_left_read_node_idx = btarr_get_most_left_leaf_idx(&mt->tree, 1);
  while (most_left_read_node_idx > 1)
    {
      most_left_write_node_idx = btarr_get_parent_idx(&mt->tree, most_left_read_node_idx);
      read_node = btarr_get(&mt->tree, most_left_read_node_idx);
      write_node = btarr_get(&mt->tree, most_left_write_node_idx);
      for (i = most_left_write_node_idx; i < most_left_read_node_idx; i++)
	{
	  if (SHA1_Init(&ctx) != 1
	      || SHA1_Update(&ctx, read_node, DIGEST_LENGTH * 2) != 1
	      || SHA1_Final(write_node, &ctx) != 1)
	    return (-1);
	  write_node += DIGEST_LENGTH;
	  read_node += (DIGEST_LENGTH * 2);
	}
      most_left_read_node_idx = most_left_write_node_idx;
    }
  return (0);
}
