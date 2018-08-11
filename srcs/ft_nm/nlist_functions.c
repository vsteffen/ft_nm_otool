#include "ft_nm_otool.h"

int8_t		get_location(int8_t n_type)
{
	if ((n_type & N_STAB) > 0)
		return (0); // symbol is for debugging
	if ((n_type & N_EXT) != N_EXT && (n_type & N_PEXT) != N_PEXT)
		return (1); // symbol is non external
	if ((n_type & N_EXT) == N_EXT)
		return (2); // symbol is external
	return (3); // symbol is private external
}

char		get_correct_letter(char letter, int8_t stab_ext_p_local)
{
	if (stab_ext_p_local == 1)
		return (letter + 32);
	return (letter);
}

struct s_sect_64		*get_section_64(struct s_sect_64 *sect_list, uint32_t ordinal)
{
	while (sect_list)
	{
		if (sect_list->ordinal == ordinal)
			return (sect_list);
		sect_list = sect_list->next;
	}
	return (NULL);
}

struct s_sect_32		*get_section_32(struct s_sect_32 *sect_list, uint32_t ordinal)
{
	while (sect_list)
	{
		if (sect_list->ordinal == ordinal)
			return (sect_list);
		sect_list = sect_list->next;
	}
	return (NULL);
}
