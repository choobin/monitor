#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include <gtk/gtk.h>

#include "interface.h"

#define INTERFACE_PATH "/sys/class/net"

GList*
interface_list()
{
    DIR *dir;
    struct dirent *dirent;
    GList *list = NULL;

    dir = opendir(INTERFACE_PATH);

    assert(dir);

    while ((dirent = readdir(dir))) {
        if (strcmp(dirent->d_name, ".") == 0) continue;
        if (strcmp(dirent->d_name, "..") == 0) continue;

        list = g_list_append(list, g_strdup(dirent->d_name));
    }

    closedir(dir);

    return list;
}
