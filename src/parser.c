/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/10 18:24:44 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/14 16:13:27 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_numeric(const char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

int	check_num_args(char **argv)
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!is_numeric(argv[i]))
			return (i);
		i++;
	}
	return (0);
}

int	check_scheduler(char *scheduler, t_sim *sim)
{
	if (strcmp(scheduler, "fifo") == 0)
	{
		sim->scheduler = FIFO;
		return (0);
	}
	else if (strcmp(scheduler, "edf") == 0)
	{
		sim->scheduler = EDF;
		return (0);
	}
	return (1);
}

int	print_errors(int error_code, int arg)
{
	if (error_code == 1)
		fprintf(stderr, "Error: Invalid number of arguments.\n");
	else if (error_code == 2)
		fprintf(stderr, "Error: Argument %d is not a valid"
			" positive integer.\n", arg);
	else if (error_code == 3)
		fprintf(stderr, "Error: number_of_coders must be greater than 0.\n");
	else if (error_code == 4)
		fprintf(stderr, "Error: scheduler must be 'fifo' or 'edf'.\n");
	return (1);
}

int	parse_args(int argc, char **argv, t_sim *sim)
{
	int	i;

	if (argc != 9)
		return (print_errors(1, 0));
	i = check_num_args(argv);
	if (i)
		return (print_errors(2, i));
	sim->nb_coders = ft_atoll(argv[1]);
	sim->time_to_burnout = ft_atoll(argv[2]);
	sim->time_to_compile = ft_atoll(argv[3]);
	sim->time_to_debug = ft_atoll(argv[4]);
	sim->time_to_refactor = ft_atoll(argv[5]);
	sim->nb_compiles_req = ft_atoll(argv[6]);
	sim->dongle_cooldown = ft_atoll(argv[7]);
	sim->heap_cap = 2;
	if (sim->nb_coders <= 0)
		return (print_errors(3, 0));
	if (check_scheduler(argv[8], sim))
		return (print_errors(4, 0));
	return (0);
}
