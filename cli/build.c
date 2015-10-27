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

/*
** TODO: handle endianess
*/
/*
** TODO: add magic
*/
static int	_dump_mt(t_mt *mt)
{
  unsigned int	size;
  int		i;
  unsigned char	*w;

  write(1, &mt->filesize, sizeof(unsigned int));
  write(1, &mt->blocksize, sizeof(unsigned int));
  write(1, mt->file_digest, DIGEST_LENGTH);
  size = btarr_get_nb_nodes(&mt->tree) * DIGEST_LENGTH;
  w = mt->tree.root;
  while (size > 0)
    {
      i = write(1, w, size > 4096 ? 4096 : size);
      switch (i)
	{
	case (-1):
	  perror(NULL);
	  return (-1);
	case (0):
	  return (-2);
	default:
	  w += i;
	  size -= i;
	}
    }
  return (0);
}

static int	_build_and_dump(char *source_filepath, unsigned int blocksize)
{
  t_mt_file	mtf;
  t_mt		mt;
  int		ret;

  ret = EXIT_SUCCESS;
  if (mt_file_mmap(&mtf, source_filepath) != 0)
    {
      dprintf(2, "Error: failed to open '%s'\n", source_filepath);
      return (EXIT_FAILURE);
    }
  if (mt_tree_build(&mt, &mtf, blocksize) != 0)
    {
      dprintf(2, "Error: failed to build a merkle tree.\n");
      dprintf(2, "Possible causes are memory exhaustion, "
	      "or an error while using the hashing function\n");
      ret = -1;
    }
  if (_dump_mt(&mt) != 0)
    {
      dprintf(2, "Error: failed to write completely the merkle tree in stdout\n");
      ret = EXIT_FAILURE;
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
  unsigned int	blocksize;

  blocksize = 4096;
  if (argc == 3)
    {
      if (atoi(argv[2]) < 1)
	{
	  dprintf(2, "Error: invalid blocksize\n");
	  return (EXIT_FAILURE);
	}
      blocksize = (unsigned int)atoi(argv[2]);
    }
  if (argc < 2 || argc > 3 || strlen(argv[1]) == 0)
    {
      dprintf(2, "Usage: %s source-filepath (blocksize)\n", argv[0]);
      return (EXIT_FAILURE);
    }
  if (_build_and_dump(argv[1], blocksize) != 0)
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
