/* Author: Steffen Viken Valvaag <steffenv@cs.uit.no> */
#include "common.h"
#include "list.h"
#include "printing.h"

#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a platform-independent path separator.
#ifdef _WIN32
#include <windows.h>
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

// Make our errors fatal (i.e. kill the program immediately); see printing.h.
#ifndef ERROR_FATAL
#define ERROR_FATAL
#endif

void tokenize_file(FILE *file, list_t *list)
{
    char *word;
    char buf[101];
    buf[100] = 0;

    while (!feof(file))
    {
        /* Skip non-letters */
        fscanf(file, "%*[^a-zA-Z0-9'_]");
        /* Scan up to 100 letters */
        if (fscanf(file, "%100[a-zA-Z0-9'_]", buf) == 1)
        {
            word = strdup(buf);
            if (word == NULL)
                ERROR_PRINT("out of memory\n");
            list_addlast(list, word);
        }
    }
}

struct list *find_files(char *root)
{
    // Initialise the list of files we will return.
    list_t *files;
    files = list_create(compare_strings);

    // Initialise variables needed to iterate through the directory.
    // PATH_MAX is a platform-independent constant from <limits.h>.
    DIR *d;
    struct dirent *ent;
    char path[PATH_MAX];
    char *final_path;

    // Attempt to open the root dir.
    if ((d = opendir(root)) != NULL)
    {
        // To be on the safe side, check that the root dir name isn't
        // too long. We add one because we know we'll have to add a
        // tailing slash and filename later on.
        size_t path_len = strlen(root);
        if (path_len + 1 >= PATH_MAX)
        {
            ERROR_PRINT("directory name too long\n");
        }

        // Start the file path with the root dir name.
        strncpy(path, root, PATH_MAX - 1);

        // Now add a trailing slash (forward- or backslash depending
        // on platform).
        if (path[path_len - 1] != PATH_SEPARATOR)
        {
            path[path_len] = PATH_SEPARATOR;
            // path[path_len + 1] = '\0';
            path_len++;
        }

        // We're ready to iterate over the contents of the directory
        while ((ent = readdir(d)) != NULL)
        {
            // Skip current and parent directory.
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            {
                continue;
            }

            // Build complete path to current file. First check that
            // the complete name wouldn't be too long.
            size_t filename_length = strlen(ent->d_name);
            if (path_len + filename_length + 1 >= PATH_MAX)
            {
                ERROR_PRINT("file name too long\n");
            }
            // Copy filename into the "path" string at the right
            // location, that is, offset "path_len" bytes from the
            // start of the string.
            strncpy(path + path_len, ent->d_name, filename_length + 1);

            // Duplicate the complete path and put it in the list.
            final_path = strdup(path);
            if (final_path == NULL)
            {
                ERROR_PRINT("out of memory\n");
            }
            list_addlast(files, final_path);

            // NOTE: "ent" could be either a file or a directory; here
            // we basically assume that there's only files in the
            // "root" dir.
        }

        // Be a good citizen and close the directory.
        closedir(d);
    }
    else
    {
        ERROR_PRINT("could not open directory\n");
    }

    return files;
}

int compare_strings(void *a, void *b)
{
    return strcmp(a, b);
}
