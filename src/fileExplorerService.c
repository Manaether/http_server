#include "fileExplorerService.h"
#include "http.h"

char *workSpacePath = ".";

char *getFullUri(char *uriPart)
{
    unsigned char *fullPath = calloc(strlen(workSpacePath) + strlen(uriPart) + 1, 1);

    if (workSpacePath[strlen(workSpacePath) - 1] == '/')
        strncpy(fullPath, workSpacePath, strlen(workSpacePath) - 1);
    else
        strcpy(fullPath, workSpacePath);

    if (uriPart[strlen(uriPart) - 1] == '/')
        strncat(fullPath, uriPart, strlen(uriPart) - 1);
    else
        strcat(fullPath, uriPart);

    return fullPath;
}

enum pathType getServiceIsAvailable(char *uri)
{
    struct stat sb;

    if (strlen(uri) > 1 && strstr(uri, "..") != NULL)
        return isNothing;

    if (stat(uri, &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode))
            return isDirectory;
        if (S_ISREG(sb.st_mode))
            return isFile;
    }

    return isNothing;
}

void getFileContent(char *uri, Response* resp)
{
    unsigned char *buffer = NULL;

    FILE *fp;
    fp = fopen(uri, "r");
    long size = 0;

    if (fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        logger_info("FILE","size[%d]", size);

        if(size > 0)
        {
            buffer = calloc(size + 1, 1);
            if (buffer)
            {
                fread(buffer, 1, size, fp);
            }
        }
        else
        {
            buffer = calloc(2, 1);
            buffer[0] = ' ';
            buffer[1] = '\0';
        }
        fclose(fp);
    }

    resp->content.content = malloc(size + 1);
    resp->content.content_length = size;
    strcpy(resp->content.content, buffer);

    free(buffer);
}

char *getFileName(char *uri)
{
    return strrchr(uri, '/');
}

char *getDirectoryContent(char *local_path, char *uri)
{
    DIR *FD;

    unsigned char *listbuffer = NULL;
    unsigned char *linkString = NULL;
    unsigned char *itemString = NULL;
    unsigned char *finalString = NULL;
    int n = 0;
    int total_char = 0;

    struct dirent *in_file;

    if ((FD = opendir(local_path)) == NULL)
        return NULL;

    const unsigned  char list_structure[] = "<ul>\r\n" \
                    "%s\r\n" \
                    "<ul>\r\n";

    const unsigned char item_structure[] = "<li><a href=\"%s\">%s</a></li>\r\n";

    while ((in_file = readdir(FD)))
    {
        /*
        if (!strcmp(in_file->d_name, "."))
            continue;
        if (!strcmp(in_file->d_name, ".."))
            continue;
        */

        if (strlen(uri) == 1 && uri[0] == '/') {
            linkString = calloc(strlen(in_file->d_name) + 1 + 1, 1);
            sprintf(linkString, "%s%s", "/", in_file->d_name);
        } else if(uri[strlen(uri) - 1]  == '/') {
            linkString = calloc(strlen(uri) + strlen(in_file->d_name) + 1, 1);
            sprintf(linkString, "%s%s", uri, in_file->d_name);
        } else {
            linkString = calloc(strlen(in_file->d_name) + strlen(uri) + 1 + 1, 1);
            sprintf(linkString, "%s%s%s", uri, "/", in_file->d_name);
        }

        n = snprintf(NULL, 0, item_structure, linkString, in_file->d_name);
        total_char += (unsigned int) n;

        itemString = calloc(n + 1, 1);
        sprintf(itemString, item_structure, linkString, in_file->d_name);

        if (listbuffer == NULL) {
            listbuffer = calloc(total_char + 1, 1);
            strcpy(listbuffer, itemString);
        } else {
            char *tmp = realloc(listbuffer, total_char + 1);
            if (tmp == NULL) {
                free(listbuffer);
                free(itemString);
                free(linkString);
                return NULL;
            }
            listbuffer = tmp;

            sprintf(listbuffer, "%s%s", listbuffer, itemString);
        }

        free(itemString);

        free(linkString);
    }

    if(total_char > 0)
    {
        n = snprintf(NULL, 0, list_structure, listbuffer);
        finalString = calloc(n + 1, 1);
        sprintf(finalString, list_structure, listbuffer);
    }
    else
    {
        finalString = calloc(2, 1);
        finalString[0] = ' ';
        finalString[1] = '\0';
    }

    free(listbuffer);
    closedir(FD);

    return finalString;
}
