// mystat.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#typedef unsigned short u16;
#typedef unsigned long  u32;

main(int argc, char *argv[])
{
	if (argc > 1)
	{
		// GET FILE STATS
		char folder[128];
		DIR *dir;
		getdir(folder, 128);
		dir = opendir(folder);
		if (dir != NULL)
		{
			struct dirent *entry;

			printf("File_Type%t|%tPermissions%t|%tUid%t|%tSize%t|%tCreation Time");
			while ((entry = readdir(dir)) != NULL)
			{
				int mode = 0; // 0 = reg file, 1 = dir, 2 = link
				struct lstat filestats;
				lstat(entry->d_name, &filestats);

				// FILE TYPE
				if ((filestats->st_mode & 0100000) == 0100000)
					printf("REG%t|%t");
				else if ((filestats->st_mode & 0040000) == 0040000)
				{
					mode = 1;
					printf("DIR%t|%t");
				}
				else
				{
					mode = 2;
					printf("LNK%t|%t");
				}

				// PERMISSIONS
				if ((filestats->st_mode & (1 << 8)))
					printf("r");
				if ((filestats->st_mode & (1 << 7)))
					printf("w");
				if ((filestats->st_mode & (1 << 6)))
					printf("x");
				printf("%t|%t");

				// UID
				printf("%d%t|%t", filestats->st_uid);

				// SIZE
				printf("%d%t|%t", filestats->st_size);

				// CREATION TIME
				printf("%d/%d/%d%t|%t", fstats->st_ctime->tm_mday, fstats->st_ctime->tm_mon, fstats->st_ctime->tm_year);

				if (mode) // print files in directory
				{
					struct dirent *subentry;
					while ((subentry = readdir(dir)) != NULL)
					{
						struct lstat filestats;
						lstat(entry->d_name, &filestats);

						// FILE TYPE
						if ((filestats->st_mode & 0100000) == 0100000)
							printf("REG%t|%t");
						else if ((filestats->st_mode & 0040000) == 0040000)
							printf("DIR%t|%t");
						else
							printf("LNK%t|%t");

						// PERMISSIONS
						if ((filestats->st_mode & (1 << 8)))
							printf("r");
						if ((filestats->st_mode & (1 << 7)))
							printf("w");
						if ((filestats->st_mode & (1 << 6)))
							printf("x");
						printf("%t|%t");

						// UID
						printf("%d%t|%t", filestats->st_uid);

						// SIZE
						printf("%d%t|%t", filestats->st_size);

						// CREATION TIME
						printf("%d/%d/%d%t|%t", fstats->st_ctime->tm_mday, fstats->st_ctime->tm_mon, fstats->st_ctime->tm_year);
					}
				}
				else if (mode == 2) // print the actual file
				{
					struct stat fstats;
					stat(entry->d_name, &fstats);

					// FILE TYPE
					if ((fstats->st_mode & 0100000) == 0100000)
						printf("REG%t|%t");
					else if ((fstats->st_mode & 0040000) == 0040000)
					{
						mode = 1;
						printf("DIR%t|%t");
					}
					else
					{
						mode = 2;
						printf("LNK%t|%t");
					}

					// PERMISSIONS
					if ((fstats->st_mode & (1 << 8)))
						printf("r");
					if ((fstats->st_mode & (1 << 7)))
						printf("w");
					if ((fstats->st_mode & (1 << 6)))
						printf("x");
					printf("%t|%t");

					// UID
					printf("%d%t|%t", fstats->st_uid);

					// SIZE
					printf("%d%t|%t", fstats->st_size);

					// CREATION TIME
					printf("%d/%d/%d%t|%t", fstats->st_ctime->tm_mday, fstats->st_ctime->tm_mon, fstats->st_ctime->tm_year);
				}
				return 0;
			}
			closedir(dir);
		}

		return 0;
	}
	else
		return 1;

}

