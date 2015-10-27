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

#ifndef MT_H_
# define MT_H_

# include <openssl/sha.h>
# include "btarr.h"

# define DIGEST_LENGTH (SHA_DIGEST_LENGTH)

typedef struct	s_mt_file
{
  int		fd;
  char		*addr;
  unsigned int	filesize;
}		t_mt_file;

typedef struct	s_mt
{
  unsigned int	filesize;		    /* size of source file (bytes) */
  unsigned int	blocksize;		    /* maximum size (bytes) of each data block */
  unsigned char	file_digest[DIGEST_LENGTH]; /* file sha1sum */
  t_btarr	tree;			    /* merkle tree */
}		t_mt;

char		*mt_convert_hex_to_decimal(char *dest, char *input);
unsigned int	mt_get_real_nb_blocks(t_mt *mt);
unsigned int	mt_get_effective_nb_blocks(t_mt *mt);

int		mt_file_mmap(t_mt_file *mtf, char *filepath);
int		mt_file_munmap(t_mt_file *mtf);

int		mt_tree_build(t_mt *mt, t_mt_file *mtf, unsigned int blocksize);
int		mt_tree_free(t_mt *mt);

unsigned int	mt_digest_get_index(t_mt *mt, char *digest);
int		mt_digest_exists(t_mt *mt, char *digest);

int		mt_data_get_by_digest(t_mt *mt, t_mt_file *mtf,
				      char *digest, char **addr, unsigned int *size);
int		mt_data_get_by_digest_index(t_mt *mt, t_mt_file *mtf,
					    unsigned int index, char **addr, unsigned int *size);
int		mt_data_find_by_digest_index(t_mt *mt, unsigned int index,
					     unsigned int *offset, unsigned int *size);

int		mt_tree_load(t_mt *mt, t_mt_file *mtf);

#endif
