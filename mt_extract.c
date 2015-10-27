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

#include <stdlib.h>
#include <string.h>
#include "mt.h"

int		mt_data_get_by_digest(t_mt *mt, t_mt_file *mtf,
				      char *digest, char **addr, unsigned int *size)
{
  unsigned int	index;

  index = mt_digest_get_index(mt, digest);
  if (index == 0)
    return (-1);
  return (mt_data_get_by_digest_index(mt, mtf, index, addr, size));
}

int		mt_data_get_by_digest_index(t_mt *mt, t_mt_file *mtf,
					    unsigned int index, char **addr, unsigned int *size)
{
  unsigned int	offset;

  if (mt_data_find_by_digest_index(mt, index, &offset, size) != 0)
    return (-1);
  *addr = malloc(sizeof(char) * (*size));
  if (!*addr)
    return (-1);
  memmove(*addr, mtf->addr + offset, *size);
  return (0);
}
