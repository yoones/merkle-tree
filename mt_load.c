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

#include <stdio.h>
#include <string.h>
#include "mt.h"

/*
** TODO: add NO_DUPLICATE option
**
** mt->tree.root = addr
** instead of:
** memove(mt->tree.root, addr, ...)
*/
/*
** TODO: handle endianess
*/
int		mt_tree_load(t_mt *mt, t_mt_file *mtf)
{
  char		*w;

  w = mtf->addr;
  memmove(&mt->filesize, w, sizeof(unsigned int));
  w += sizeof(unsigned int);
  memmove(&mt->blocksize, w, sizeof(unsigned int));
  w += sizeof(unsigned int);
  memmove(mt->file_digest, w, DIGEST_LENGTH);
  w += DIGEST_LENGTH;
  if (btarr_init(&mt->tree, DIGEST_LENGTH, mt_get_effective_nb_blocks(mt), NULL))
    return (-1);
  memmove(mt->tree.root, w, DIGEST_LENGTH * btarr_get_nb_nodes(&mt->tree));
  return (0);
}
