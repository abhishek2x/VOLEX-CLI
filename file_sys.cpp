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
    dirIterator = nullptr;
    delete (fileIterator);
    fileIterator = nullptr;

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
            subdirs = nullptr;
            delete (subfiles);
            subfiles = nullptr;
            return true;
        }
        subdirs = subdirs->next;
    }

    while (subfiles)
    {
        if (strcmp(subfiles->name, name) == 0)
        {
            delete (subdirs);
            subdirs = nullptr;
            delete (subfiles);
            subfiles = nullptr;
            return true;
        }
        subfiles = subfiles->next;
    }

    delete (subdirs);
    subdirs = nullptr;
    delete (subfiles);
    subfiles = nullptr;
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
    iterator = nullptr;
    return false;
}


bool isPresentFile(Dir *dir, const char *name)
{
    File *subFile = dir->subfile;
    while (subFile != nullptr)
    {
        if (strcmp(subFile->name, name) == 0)
            return true;
        subFile = subFile->next;
    }
    return false;
}

bool isPresentDir(Dir *dir, const char *name)
{
    Dir *subDir = dir->subdir;
    while (subDir != nullptr)
    {
        if (strcmp(subDir->name, name) == 0)
            return true;
        subDir = subDir->next;
    }
    return false;
}

//--------------------- END: Utility functions

//--------------------- START: Helper functions

// When deleting a directory [Not Recursively]
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
    fileIterator = nullptr;
    delete (dirIterator);
    dirIterator = nullptr;
    delete (child);
    child = nullptr;
}
// When deleting a directory [Recursively]
void deleteDirHelper2(Dir *dir)
{
    if (!dir->next)
        return;
    if (isDirectoryEmpty(dir))
        delete (dir);
    dir = nullptr;

    File *fileIterator = dir->subfile;
    while (fileIterator)
    {
        deleteFile(dir, fileIterator->name);
        fileIterator = fileIterator->next;
    }

    Dir *dirIterator = dir->subdir;
    while (dirIterator)
    {
        deleteDirHelper2(dirIterator);
        dirIterator = dirIterator->next;
    }
    delete (dir);
    dir = nullptr;
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
    if (dir == nullptr || name == nullptr)
        return 1;
    if (isPresentFile(dir, name) == false)
        return 2;
    File *prev = nullptr;
    File *curr = dir->subfile;
    while (curr != nullptr && strcmp(curr->name, name) != 0)
    {
        prev = curr;
        curr = curr->next;
    }
    if (curr == dir->subfile)
        dir->subfile = dir->subfile->next;
    else
        prev->next = curr->next;
    delete curr;
    curr = nullptr;
    return 0;
}

int deleteDir(Dir *dir, const char *name, bool recursive)
{
    if (dir == nullptr || name == nullptr)
        return 1;
    if (strcmp(dir->name, name) == 0)
        return 0;
    if (isPresentDir(dir, name) == false)
        return 2;
    Dir *prev = nullptr;
    Dir *curr = dir->subdir;
    while (curr != nullptr && strcmp(curr->name, name) != 0)
    {
        prev = curr;
        curr = curr->subdir;
    }
    if (curr == dir->subdir)
        dir->subdir = dir->subdir->subdir;
    else
        prev->subdir = curr->subdir;
    delete curr;
    curr = nullptr;
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
    fileIterator = nullptr;
    delete (dirIterator);
    dirIterator = nullptr;

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

    deleteFile(tgt->parent, tgt->name);
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
    const File **files = (const File **)malloc(sizeof(File) * 10);

    // fileHelper(dir, tag, length, &files);

    const File *dirFiles = dir->subfile;
    while (dirFiles)
    {
        if (dirFiles->tag == tag)
        {
            files[length] = dirFiles;
            length++;
        }
        dirFiles = dirFiles->next;
    }
    delete (dirFiles);
    dirFiles = nullptr;

    return files;
}
