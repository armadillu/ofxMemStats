//
//  MemStats.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 26/03/15.
//
//

//this is most likely only osx
//taken from http://stackoverflow.com/questions/25609945/why-does-mstats-and-malloc-zone-statistics-not-show-recovered-memory-after-free

#ifndef BaseApp_MemStats_h
#define BaseApp_MemStats_h

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <mach/mach.h>
#include <mach/task.h>
#include <malloc/malloc.h>
#include <errno.h>

class MemStats{

public:

	//returns num used bytes
	static void printMemStats(bool inBytes = false){
		size_t bytesInUse, sizeAllocated, blocksInUse;
		printf("== HEAP STATS =========================================================\n");
		freeAsMuchAsPossible();
		heapInUse(&bytesInUse, &blocksInUse, &sizeAllocated);
		size_t bytesUsed = heapStats(inBytes);
		printf("=======================================================================\n");
		return bytesUsed;
	}

private:


	static void freeAsMuchAsPossible()
	{
		vm_address_t * zones;
		unsigned int count;
		unsigned int i;

		kern_return_t rc =
		malloc_get_all_zones(mach_task_self(), 0, &zones, &count);
		if (0 != rc)
		{
			fprintf(stderr, "rc was %d\n", rc);
		}
		for (i = 0; i < count; ++i)
		{
			malloc_zone_t * zone = (malloc_zone_t*)zones[i];
			char const * name = malloc_get_zone_name(zone);
			if (NULL == name)
			{
				continue;
			}
			malloc_zone_pressure_relief(zone, 0);
		}
	}

	/** heapStats()
	 * print the output from the mstats() function: total heap bytes,
	 * used bytes, and free bytes. returns bytes_used
	 */
	static size_t heapStats(bool inBytes)
	{
		struct mstats ms = mstats();
		malloc_printf(
					  inBytes ? "\nBytes > total: %d, used: %d, free: %d\nChunks > used: %d free: %d\n" :
					  "\nBytes > total: %y, used: %y, free: %y\nChunks > used: %d free: %d\n",
					  ms.bytes_total,
					  ms.bytes_used,
					  ms.bytes_free,
					  ms.chunks_used,
					  ms.chunks_free
					  );
		return ms.bytes_used;
	}

	/* heapInUse()
	 * Gather the heap usage metrics from each zone, using
	 * malloc_zone_statistics().
	 */
	static void heapInUse(
						  size_t * bytesInUse,
						  size_t * blocksInUse,
						  size_t * sizeAllocated
						  )
	{
		*bytesInUse = 0;
		*blocksInUse = 0;
		*sizeAllocated = 0;
		unsigned int i;
		vm_address_t * zones;
		unsigned int count;
		kern_return_t rc =
		malloc_get_all_zones(mach_task_self(), 0, &zones, &count);
		if (0 != rc)
		{
			fprintf(stderr, "rc was %d\n", rc);
		}
		for (i = 0; i < count; ++i)
		{
			malloc_zone_t * zone = (malloc_zone_t*)zones[i];
			char const * name = malloc_get_zone_name(zone);
			if (NULL == name)
			{
				continue;
			}
			malloc_statistics_t stats;
			stats.blocks_in_use = 0;
			stats.size_in_use = 0;
			stats.max_size_in_use = 0;
			stats.size_allocated = 0;
			malloc_zone_statistics(zone, &stats);
			*bytesInUse += stats.size_in_use;
			*blocksInUse += stats.blocks_in_use;
			*sizeAllocated += stats.size_allocated;
		}
	}
	

};

#endif
