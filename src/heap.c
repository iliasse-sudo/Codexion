/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 02:31:00 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/13 02:34:42 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	swap_requests(t_request *a, t_request *b)
{
	t_request	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

int	compare_requests(t_request *a, t_request *b, t_scheduler type)
{
	if (type == FIFO)
	{
		if (a->arrival_time != b->arrival_time)
			return (a->arrival_time < b->arrival_time);
		return (a->coder_id < b->coder_id);
	}
	else
	{
		if (a->deadline != b->deadline)
			return (a->deadline < b->deadline);
		if (a->arrival_time != b->arrival_time)
			return (a->arrival_time < b->arrival_time);
		return (a->coder_id < b->coder_id);
	}
}

int	init_heap(t_heap *heap, int capacity, t_scheduler type)
{
	heap->capacity = capacity;
	heap->size = 0;
	heap->scheduler = type;
	heap->array = malloc(sizeof(t_request) * capacity);
	if (!heap->array)
		return (1);
	return (0);
}

void	free_heap(t_heap *heap)
{
	if (heap->array)
		free(heap->array);
}
