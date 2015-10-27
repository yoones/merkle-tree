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
#include <stdlib.h>
#include <unistd.h>
#include "mt.h"

static int	_load_and_find(char *source_filepath, char *digest)
{
  t_mt_file	mtf;
  t_mt		mt;
  int		ret;
  unsigned int	index;
  unsigned int	offset;
  unsigned int	size;

  ret = EXIT_SUCCESS;
  if (mt_file_mmap(&mtf, source_filepath) != 0)
    {
      dprintf(2, "Error: failed to open '%s'\n", source_filepath);
      return (EXIT_FAILURE);
    }
  if (mt_tree_load(&mt, &mtf) != 0)
    {
      dprintf(2, "Error: failed to build a merkle tree.\n");
      dprintf(2, "Possible causes are memory exhaustion, "
	      "or an error while using the hashing function\n");
      ret = -1;
    }
  index = mt_digest_get_index(&mt, digest);
  if (index == 0)
    {
      dprintf(2, "digest not found\n");
      ret = 2;
    }
  else
    {
      mt_data_find_by_digest_index(&mt, index, &offset, &size);
      printf("index=%d\noffset=%d\nsize=%d\n", index, offset, size);
    }
  mt_tree_free(&mt);
  if (mt_file_munmap(&mtf) != 0)
    {
      dprintf(2, "Error: failed to close '%s'\nData might be corrupted!!\n", source_filepath);
      return (EXIT_FAILURE);
    }
  return (ret);
}

int		main(int argc, char **argv)
{
  if (argc != 3 || strlen(argv[1]) == 0 || strlen(argv[2]) == 0)
    {
      dprintf(2, "Usage: %s tree-filepath digest\n", argv[0]);
      return (EXIT_FAILURE);
    }
  if (_load_and_find(argv[1], argv[2]) != 0)
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
