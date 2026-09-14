/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibaya <ibaya@student.1337.ma>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/13 02:35:29 by ibaya             #+#    #+#             */
/*   Updated: 2026/09/13 02:35:30 by ibaya            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	sift_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (compare_requests(&heap->array[index],
				&heap->array[parent], heap->scheduler))
		{
			swap_requests(&heap->array[index], &heap->array[parent]);
			index = parent;
		}
		else
			break ;
	}
}

void	sift_down(t_heap *heap, int index)
{
	int	min_index;
	int	left;
	int	right;

	min_index = index;
	left = index * 2 + 1;
	right = index * 2 + 2;
	if (left < heap->size && compare_requests(&heap->array[left],
			&heap->array[min_index], heap->scheduler))
		min_index = left;
	if (right < heap->size && compare_requests(&heap->array[right],
			&heap->array[min_index], heap->scheduler))
		min_index = right;
	if (index != min_index)
	{
		swap_requests(&heap->array[index], &heap->array[min_index]);
		sift_down(heap, min_index);
	}
}

void	heap_push(t_heap *heap, t_request req)
{
	if (heap->size >= heap->capacity)
		return ;
	heap->array[heap->size] = req;
	sift_up(heap, heap->size);
	heap->size++;
}

t_request	heap_pop(t_heap *heap)
{
	t_request	root;

	root.arrival_time = 0;
	root.coder_id = 0;
	root.deadline = 0;
	if (heap->size <= 0)
		return (root);
	root = heap->array[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->array[0] = heap->array[heap->size];
		sift_down(heap, 0);
	}
	return (root);
}
