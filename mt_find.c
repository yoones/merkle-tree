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

#include <string.h>
#include "mt.h"

static int	_get_value_of(char c, char *base);
static char	*_convert_hex_to_decimal(char *dest, char *input);

unsigned int	mt_digest_get_index(t_mt *mt, char *digest)
{
  unsigned int	i;
  unsigned int	nb_nodes;
  unsigned char	*w;
  char		buf[DIGEST_LENGTH];

  if (_convert_hex_to_decimal(buf, digest) == NULL)
    return (-1);
  nb_nodes = btarr_get_nb_nodes(&mt->tree);
  w = mt->tree.root;
  for (i = 1; i <= nb_nodes; i++)
    {
      if (!memcmp(w, buf, DIGEST_LENGTH))
	return (i);
      w += DIGEST_LENGTH;
    }
  return (0);
}

int		mt_digest_exists(t_mt *mt, char *digest)
{
  return ((mt_digest_get_index(mt, digest) > 0) ? 1 : 0);
}

int		mt_data_find_by_digest_index(t_mt *mt, unsigned int index,
					     unsigned int *offset, unsigned int *size)
{
  unsigned int	first_data_block_idx;
  unsigned int	most_left_node_idx;
  unsigned int	most_right_node_idx;

  first_data_block_idx = btarr_get_most_left_leaf_idx(&mt->tree, 1);
  most_left_node_idx = btarr_get_most_left_leaf_idx(&mt->tree, index) - first_data_block_idx;
  most_right_node_idx = btarr_get_most_right_leaf_idx(&mt->tree, index) - first_data_block_idx;
  *offset = most_left_node_idx * mt->blocksize;
  *size = (most_right_node_idx - most_left_node_idx + 1) * mt->blocksize;
  if (*offset > mt->filesize) /* > ou >= ??? */
    {
      *offset = 0;
      *size = 0;
      return (0);
    }
  if (most_right_node_idx * mt->blocksize > mt->filesize)
    *size = mt->filesize - *offset;
  return (0);
}

static int	_get_value_of(char c, char *base)
{
  int		i;

  for (i = 0; base[i] && base[i] != c; i++)
    ;
  return (base[i] == c ? i : -1);
}

static char	*_convert_hex_to_decimal(char *dest, char *input)
{
  char		base16[] = "0123456789abcdef";
  int		i;
  int		a, b;

  if (strlen(input) != (DIGEST_LENGTH * 2))
    return (NULL);
  for (i = 0; input[i]; i += 2)
    {
      a = _get_value_of(input[i], base16);
      b = _get_value_of(input[i + 1], base16);
      if (a == -1 || b == -1)
	return (NULL);
      *dest = ((a * 16) + b);
      dest++;
    }
  return (dest);
}
