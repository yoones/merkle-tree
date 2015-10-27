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

static int	_extract_data(t_mt_file *mtf, unsigned int offset, unsigned int size)
{
  char		*w;
  int		i;

  w = mtf->addr + offset;
  while (size > 0)
    {
      i = write(1, w, size > 4096 ? 4096 : size);
      if (i < 1)
	{
	  perror(NULL);
	  return (-1);
	}
      size -= i;
      w += i;
    }
  return (0);
}

static int	_get_position(char *tree_filepath, char *digest, unsigned int *offset, unsigned int *size)
{
  t_mt_file	mtf;
  t_mt		mt;
  unsigned int	index;
  int		ret;

  if (mt_file_mmap(&mtf, tree_filepath) != 0)
    {
      dprintf(2, "Error: failed to open '%s'\n", tree_filepath);
      return (-1);
    }
  ret = mt_tree_load(&mt, &mtf);
  if (mt_file_munmap(&mtf) != 0)
    {
      dprintf(2, "Error: failed to close '%s'\nData might be corrupted!!\n", tree_filepath);
      return (-2);
    }
  if (ret != 0)
    {
      dprintf(2, "Error: failed to load merkle tree.\n");
      dprintf(2, "Possible causes are memory exhaustion, "
	      "or an error while using the hashing function\n");
      return (-3);
    }
  ret = 0;
  index = mt_digest_get_index(&mt, digest);
  if (index == 0)
    {
      dprintf(2, "Error: digest not found\n");
      ret = -2;
    }
  else
    mt_data_find_by_digest_index(&mt, index, offset, size);
  mt_tree_free(&mt);
  return (ret);
}

static int	_load_find_and_extract(char *tree_filepath, char *source_filepath, char *digest)
{
  t_mt_file	mtf;
  int		ret;
  unsigned int	offset;
  unsigned int	size;

  if (_get_position(tree_filepath, digest, &offset, &size) != 0)
    return (EXIT_FAILURE);
  if (mt_file_mmap(&mtf, source_filepath) != 0)
    {
      dprintf(2, "Error: failed to open '%s'\n", source_filepath);
      return (EXIT_FAILURE);
    }
  ret = (_extract_data(&mtf, offset, size) == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
  if (mt_file_munmap(&mtf) != 0)
    {
      dprintf(2, "Error: failed to close '%s'\nData might be corrupted!!\n", source_filepath);
      ret = EXIT_FAILURE;
    }
  return (ret);
}

int		main(int argc, char **argv)
{
  if (argc != 4 || strlen(argv[1]) == 0 || strlen(argv[2]) == 0 || strlen(argv[3]) == 0)
    {
      dprintf(2, "Usage: %s tree-filepth source-filepath digest\n", argv[0]);
      return (EXIT_FAILURE);
    }
  if (_load_find_and_extract(argv[1], argv[2], argv[3]) != 0)
    return (EXIT_FAILURE);
  return (EXIT_SUCCESS);
}
