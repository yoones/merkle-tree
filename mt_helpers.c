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

char		*mt_convert_hex_to_decimal(char *dest, char *input)
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

static int	_get_value_of(char c, char *base)
{
  int		i;

  for (i = 0; base[i] && base[i] != c; i++)
    ;
  return (base[i] == c ? i : -1);
}

unsigned int	mt_get_real_nb_blocks(t_mt *mt)
{
  unsigned int	nb_blocks;

  nb_blocks = mt->filesize / mt->blocksize;
  if (mt->filesize % mt->blocksize)
    nb_blocks++;
  if (nb_blocks % 2)
    nb_blocks++;
  return (nb_blocks);
}

unsigned int	mt_get_effective_nb_blocks(t_mt *mt)
{
  unsigned int	nb_blocks;
  unsigned int	i;

  nb_blocks = mt_get_real_nb_blocks(mt);
  for (i = 1; i < nb_blocks; i *= 2)
    ;
  return (i);
}
