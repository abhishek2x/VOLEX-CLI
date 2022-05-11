/**
 * This file contains the file system functions that you need to implement.
 * You can define helper functions in this file.
 *
 * Remember to remove the initial statements in the functions.
 * They are only used so that your semi-finished codes can compile.
 */

#include <iostream>
#include "file_sys.h"
#include "utils.h"
using namespace std;

//--------------------- START: Utility functions

bool isPresent(Dir *dir, const char *name)
{
    Dir *dirIterator = dir->subdir;
    File *fileIterator = dir->subfile;
    while (dirIterator)
    {
        if (strcmp(dirIterator->name, name) == 0)
        {
            return true;
        }
        dirIterator = dirIterator->next;
    }

    while (fileIterator)
    {
        if (strcmp(fileIterator->name, name) == 0)
        {
            return true;
        }
        fileIterator = fileIterator->next;
    }
    delete (dirIterator);
    delete (fileIterator);

    return false;
}

bool isNameLegal(Dir *dir, const char *name)
{

    if (isPresent(dir, name))
        return false; // 1
    if (strlen(name) < 1)
        return false; // 2
    if (name[0] == '-')
        return false; // 5
    if (strcmp(name, "..") == 0 || strcmp(name, ".") == 0)
        return false; // 3

    int i;
    for (i = 0; name[i] != '\0'; i++)
    {
        char ch = name[i];
        if (!(ch >= 'A' && ch <= 'Z') && !(ch >= 'a' && ch <= 'z') && !(ch >= '0' && ch <= '9') && !(ch == '.') && !(ch == '-') && !(ch == '_'))
        {
            return false;
        }
    }
    return true;
}

bool isNameConflict(Dir *dir, const char *name)
{
    Dir *subdirs = dir->subdir;
    File *subfiles = dir->subfile;

    while (subdirs)
    {
        if (strcmp(subdirs->name, name) == 0)
        {
            delete (subdirs);
            delete (subfiles);
            return true;
        }
        subdirs = subdirs->next;
    }

    while (subfiles)
    {
        if (strcmp(subfiles->name, name) == 0)
        {
            delete (subdirs);
            delete (subfiles);
            return true;
        }
        subfiles = subfiles->next;
    }

    delete (subdirs);
    delete (subfiles);
    return false;
}

bool isDirectoryEmpty(const Dir *dir)
{
    return !dir->subdir && !dir->subfile;
}

bool isDecendent(Dir *parent, Dir *child)
{
    if (isDirectoryEmpty(parent))
        return true;

    // search on parent level
    Dir *iterator = parent->subdir;
    while (iterator)
    {
        if (strcmp(iterator->name, child->name) == 0)
        {
            return true;
        }
        iterator = iterator->next;
    }

    // if not found, search recursively
    iterator = parent->subdir;
    while (iterator)
    {
        if (isDecendent(iterator, child))
        {
            return true;
        }
        iterator = iterator->next;
    }

    delete (iterator);
    return false;
}

//--------------------- END: Utility functions

//--------------------- START: Helper functions

void fileHelper(const Dir *dir, Tag tag, unsigned int &length, const File ***files)
{
    if (isDirectoryEmpty(dir))
        return;
    if (!dir->subfile)
        return;

    // look in current directory
    const File *dirFiles = dir->subfile;
    while (dirFiles)
    {
        if (dirFiles->tag == tag)
        {
            files[length] = &dirFiles;
            length++;
        }
        dirFiles = dirFiles->next;
    }
    delete (dirFiles);

    // look recursively in sub directories
    Dir *dirs = dir->subdir;
    while (dirs)
    {
        fileHelper(dirs, tag, length, files);
        dirs = dirs->next;
    }
    delete (dirs);
}

void deleteDirHelper(Dir *parent, Dir *child)
{
    // modify parent pointers of all it's files and directories
    File *fileIterator = child->subfile;
    Dir *dirIterator = child->subdir;

    while (fileIterator)
    {
        fileIterator->parent = parent->parent;
        fileIterator = fileIterator->next;
    }

    while (dirIterator)
    {
        dirIterator->parent = parent->parent;
        dirIterator = dirIterator->next;
    }

    delete (fileIterator);
    delete (dirIterator);
    delete (child);
}

//--------------------- END: Helper functions

int createFile(Dir *dir, const char *name, unsigned int size, Tag tag)
{
    if (!dir || !name)
        return 1;
    if (!isNameLegal(dir, name))
        return 2;
    if (isNameConflict(dir, name))
        return 3;

    // adding metadata
    struct File *newFile = new File();
    newFile->tag = tag;
    newFile->size = size;
    newFile->parent = dir;
    strcpy(newFile->name, name);

    // inserting file
    newFile->next = dir->subfile;
    dir->subfile = newFile;

    return 0;
}

int createDir(Dir *dir, const char *name)
{
    if (!dir || !name)
        return 1;
    if (!isNameLegal(dir, name))
        return 2;
    if (isNameConflict(dir, name))
        return 3;

    struct Dir *newDirectory = new Dir();
    newDirectory->parent = dir;
    strcpy(newDirectory->name, name);

    // inserting directory
    newDirectory->next = dir->subdir;
    dir->subdir = newDirectory;
    return 0;
}

int deleteFile(Dir *dir, const char *name)
{
    if (!dir || !name)
        return 1;
    if (!isPresent(dir, name))
        return 2;

    File *temp;

    if (strcmp(dir->subfile->name, name) == 0)
    {
        temp = dir->subfile;
        dir->subfile = dir->subfile->next;
        delete (temp);
    }
    else
    {
        File *current = dir->subfile;
        while (current->next)
        {
            if (strcmp(current->next->name, name) == 0)
            {
                temp = current->next;
                current->next = current->next->next;
                delete (temp);
                return 0;
            }
            else
            {
                current = current->next;
            }
        }
    }

    return 0;
}

int deleteDir(Dir *dir, const char *name, bool recursive)
{
    if (!dir || !name)
        return 1;
    if (!isPresent(dir, name))
        return 2;

    Dir *toDelete = dir->subdir;
    while (toDelete)
    {
        if (strcmp(toDelete->name, name) == 0)
        {
            break;
        }
    }

    if (!isDirectoryEmpty(toDelete) && !recursive)
    {
        deleteDirHelper(dir, toDelete);
        return 3;
    }

    // Delete all it's files
    File *fileIterator = toDelete->subfile;
    while (fileIterator)
    {
        File *next = fileIterator->next;
        delete (fileIterator);
        fileIterator = next;
    }

    // recurse on it's directories
    Dir *dirIterator = toDelete->subdir;
    while (dirIterator)
    {
        deleteDir(toDelete, dirIterator->name, recursive);
        dirIterator = dirIterator->next;
    }

    delete (fileIterator);
    delete (dirIterator);
    delete (toDelete);

    return 0;
}

unsigned int getSize(const Dir *dir)
{
    if (isDirectoryEmpty(dir))
        return 0;

    int size = 0;

    // Sum of all it's files
    File *fileIterator = dir->subfile;
    while (fileIterator)
    {
        size += fileIterator->size;
        fileIterator = fileIterator->next;
    }

    // Sum of all it's directories
    Dir *dirIterator = dir->subdir;
    while (dirIterator)
    {
        size += getSize(dirIterator);
        dirIterator = dirIterator->next;
    }

    delete (fileIterator);
    delete (dirIterator);

    return size;
}

int moveFile(File *tgt, Dir *dest)
{
    if (!tgt && !dest)
        return 1;
    if (tgt->parent == dest)
        return 2;
    if (isNameConflict(dest, tgt->name))
        return 3;

    tgt->parent = dest;
    tgt->next = dest->subfile;
    dest->subfile = tgt;
    return 0;
}

int moveDir(Dir *tgt, Dir *dest)
{
    if (!tgt && !dest)
        return 0;
    if (tgt->parent == dest)
        return 1;
    if (isNameConflict(dest, tgt->name))
        return 2;
    if (dest == tgt || isDecendent(tgt, dest))
        return 3;

    tgt->parent = dest;
    tgt->next = dest->subdir;
    dest->subdir = tgt;

    return 0;
}

const File **filesOfTag(const Dir *dir, Tag tag, unsigned int &length)
{
    length = 0;
    const File **files;
    fileHelper(dir, tag, length, &files);
    return files;
}
