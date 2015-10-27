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

static int	_load_and_dump(char *source_filepath)
{
  t_mt_file	mtf;
  t_mt		mt;
  int		ret;
  unsigned int	i, j;
  unsigned char	*w;

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
      return (-1);
    }
  w = mt.tree.root;
  for (i = btarr_get_nb_nodes(&mt.tree); i > 0; i--)
    {
      /* convert_hex_to_decimal(buf, w); */
      for (j = 0; j < DIGEST_LENGTH; j++)
	printf("%02x", w[j]);
      printf("\n");
      w += DIGEST_LENGTH;
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
  if (argc != 2 || strlen(argv[1]) == 0)
    {
      dprintf(2, "Usage: %s tree-filepath\n", argv[0]);
      return (EXIT_FAILURE);
    }
  if (_load_and_dump(argv[1]) != 0)
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
