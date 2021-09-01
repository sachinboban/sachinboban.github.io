---
title: "Linux File Permissions"
excerpt: Understanding the Linux File Permission
categories:
  - Linux
tags:
 - permissions
 - security
 - file system

show_date: true
toc: true
toc_sticky: true
share: false
---

Every Linux user have heard and have came across the following error message
while trying to read or modify a file or while trying to run a script (program):

```bash
$ ./install.sh
-bash: ./install.sh: Permission denied
```

I am sure you would have googled the error and might know by now how to fix it
as well:

```bash
$ chmod +x install.sh
```

For years I have used the `chmod` command as if a muscle memory. Recently I have
been trying to set up my old laptop as a personal Linux server at home. This led
me to read more about Linux file permissions and put everything together that I
have learned.

## Basics
Every object (file, directory and link) in Linux as well as Unix-like operating
systems has access permissions. From the perspective of an object, there are
three types of users:
1. *owner* of the object. By default, owner is the user who created the object.
2. owning *group* (set of uses that have same access rights to the object).
   Usually, group is the group of the directory the file is in.
3. *others*

**Note**: Owner and group of a file can be changed using the `chown` and
`chgrp` commands.
{: .notice--info}

Each of the above mentioned user types can have three types of permissions for
a file:
1. permission to read the file. For directories, this denotes the permission to
   list the contents fo the directory. 
2. permission to write to or modify the file. For directories, this denotes the
   permission to create and remove files in the directory.
3. permission to execute the file (i.e. run as a program). For directories, this
   denotes permission to access files in the directory.

**Note**: When it comes to directories, the meanings of the three types of 
permission are slightly different from that for regular files. We will explore 
this later in the article.
{: .notice--info}

Since there are 3 types of access permissions for each of the 3 types of user
for an object, there are 9 bits of permission information per object. Each of
these permission bit denotes whether the particular access mode is
permitted(`1`) or denited(`0`). There permissions can be represented in many
ways. One is the familiary textual format which we see while using the `ls -l`
command.
```bash
$ ls -l
total 16
drwxrwxr-x 2 sachin sachin 4096 Aug 26 17:34 docs        
drwxr--r-- 4 sachin sachin 4096 Aug 28 23:15 dotfiles    
drwxrwxr-x 3 sachin sachin 4096 Sep  1 15:20 dotfiles_old
drwxrwxr-x 4 sachin sachin 4096 Aug 27 17:38 workspace
```

The other (and more compact way) is as a set of 3 octal digits. You might have 
seen or used this while using the `chmod` command:
```bash
$ chmod 755 install.sh
```

## Viewing permissions
In order to view the permissions, we can use the `ls` command with the `-l`
option.

```bash
$ ls -l dotfiles/
total 12
-rw-rwxr-- 1 sachin sachin 1079 Aug 27 21:18 install.sh
-rw-rw-r-- 1 sachin sachin 1428 Aug 28 19:54 README.md 
drwxrwxr-x 3 sachin sachin 4096 Aug 29 19:01 vim
```
Our focus will be on the first column. `ls` depicts permissions as a
10-character string. The first character depicts the file type (`-` for a
regular file, a `d` for a directory or `l` for symbolic link). The remaining
9 characters in the string represents permissions for the 3 user types we
discussed earlier (3 for each).

**Note**: The first character represents more than file type (Special mode bits
to be exactt). But in general, for practical purposes, it can be interpreted
as file type.
{: .notice--info}

For each user type, the 3 characters denotes the read (`r` if permitted),
write (`w` if permitted) and execute (`x` if permitter). A hyphen (`-`) denotes
that the corresponding permission is denied.

Consider the example permission string : `-rw-rw-r--`

| file type | owner | group | other |
| :-------: | :---: | :---: | :---: |
|     -     |  rw-  |  rw-  |  r--  |

The permission string show above shows that the object is a regular file whose 
owner has read and write permissions and but no execution permission. The same 
applies to all users in the owning group as well. All other users only have read 
permssion on the file.

### Directory Examples
When it comes to directories, the meanings of the three types of permissions are
slightly different from that of regular files. We will explore this with a few
examples.

```bash
drwx------- 1 dummyuser dummyuser 4096 Aug 13 15:13 dummydir
```
`dummyuser` has full access to the `dummydir` directory. `dummyuser` can list,
create, rename or delete any file(s) regardless of the individual file
permissions. Accessing or modifying individual file(s) within this directory
depends on the respective file's permissions.

```bash
dr-x------- 1 dummyuser dummyuser 4096 Aug 13 15:13 dummydir
```
In this example `dummyuser` has full access to `dummydir`, but cannot create,
rename or delete any file within `dummydir`. Since `dummyuser` does have read
permissions for `dummydir`, user can list the contents of the directory.
`dummyuser` may access files within `dummydir` provided `dummyuser` has the
necessary permission for the respective file.

```bash
d-wx------- 1 dummyuser dummyuser 4096 Aug 13 15:13 dummydir
```
Since `dummyuser` do not have read permission for `dummydir`, user cannot see
contents of `dummydir`, i.e. cannot do `ls`. However, if `dummyuser` knows any
file with `dummydir`, user can list, create, rename or delete any file, or even
access it (if the file's permissions allow).

```bash
d--x------- 1 dummyuser dummyuser 4096 Aug 13 15:13 dummydir
```
In this case, `dummyuser` can only access those files (if file's permissions
allow) in `dummydir` which ther user knows about. Since `dummyuser` does not
have write permission for `dummydir`, `dummyuser` cannot create, rename or
delete any file.

**Note**: Directory permissions has nothing to do with individual file's or 
subdirectory's permissions.
{: .notice--info}

**TODO**: Directory without execute permission.
{: .notice--danger}

## Changing Permissions
The `chmod` command is used to change the access permissions of the named files.

**Note**: `chmod` never changes the permissions of symbolic links. However, for
each symbolic link listed on the command line, `chmod` changes the permissions
of the pointed-to-file.
{: .notice--info}

`chmod` accepts permission modes in:
* textual method, using character strings (`r`, `w`, `x` or combination)
* numeric method (as octal digits)

### Textual method
To change the permissions of a given file, `chmod` command is used as given
below:
```bash
chmod who=permissions filename
```
where *who* denotes the user(s) whose permission is being changed.
* `u`: owner of the file
* `g`: group to which the file belongs to
* `o`: other users
* `a`: all users (instead of `ugo`)

**Note**: A single `chmod` can be used to change permission of more than one
user. For example, `ug=rw` gives read and write to both the owner and users in
the owning group.
{: .notice--info}

The *permissions* are permissions to be assigned for the mentioned user(s) (`r`,
`w`, `x` or their combination).

**Note**: If permissions are given empty in the chmod, all permissions are
removed for the mentioned user(s.
{: .notice--info}

Consider the following example:
```bash
$ ls -l dummyfile
-rwxr-xr-x 3 dummyuser dummygroup 4096 Aug 13 15:13 dummyfile
$ chmod g= dummyfile
$ chmod o= dummyfile
$ ls -l dummyfile
-rwx------ 3 dummyuser dummygroup 4096 Aug 13 15:13 dummyfile
```
The `chmod` commands in the example above essentially revokes all access to
users in `dummygroup` and other users. The two `chmod` commands could have been
combined into a single `chmod` command as follows:
```bash
$ chmod go= dummyfile
```

Suppose you want to grant read-write permissions to all users a group.
```bash
$ ls -l dummyfile
-rwx------ 3 dummyuser dummygroup 4096 Aug 13 15:13 dummyfile
$ chmod g=rw dummyfile
$ ls -l dummyfile
-rwxrw---- 3 dummyuser dummygroup 4096 Aug 13 15:13 dummyfile
```

In the above example if you wish to give read-write access to group as well as
other users:
```bash
$ ls -l dummyfile
-rwx------ 3 dummyuser dummygroup 4096 Aug 13 15:13 dummyfile
$ chmod go=rw dummyfile
$ ls -l dummyfile
-rwxrw-rw- 3 dummyuser dummygroup 4096 Aug 13 15:13 dummyfile
```

#### Add or remove permissions
Suppose we want to add or remove permission for a file. Instead of mentioning
all the new permssions for a user, we can use `+` or `-` to add or remove
permission (in place of `=`).

As an example suppose you want to grant write permission for all the users in
the group:
```bash
$ chmod  g+w dummyfile
```

Consider an example where want to revoke execution permission for both the group 
as well as other users.
```bash
$ chmod  go-x dummyfile
```

Suppose we want to make a file read-only:
```bash
$ chmod a+r dummyfile
$ chmod a-wx dummyfile
$ ls -l dummyfile
-r--r--r-- 3 dummyuser dummygroup 4096 Aug 13 15:13 dummyfile
```

#### Copying Permissions
`chmod` can also be used to copy permission from one class of user to another.

Suppose we want to give same access permission to the group as the owner:
```bash
$ chmod g=u dummyfile
```

**Warning**: You cannot grant new permission(s) while copying permission. For 
example `chmod g=xo dummyfile` will throw an error.
{: .notice--danger}

### Numeric Method
`chmod` also be used to set file permissions using number (octal digits to be
exacl). This allows you to change permission for the three user classes at the
same time.
```bash
chmod xxx filename
```
where `xxx` is a 3-digit number (octal digits to be exact, i.e. 0-7). The 3
digits denotes permissions for the owner, group and other user in that order.
Each digit is nothing but 3 bits (`r`, `w` and `x` in that order) representaion
of the permission for the corresponding user class. `1` denotes that the 
corresponding permssion being granted, while `0` denotes the same as being
denied.

The following table summarizes the permissions for a user class:

| Binary | Octal | Permission(s)     | Text Representation |
| :----: | :---: | ----------------- | ------------------- |
|  000   |   0   | No access         | `---`               |
|  001   |   1   | Execute onnly     | `--x`               |
|  010   |   2   | Write only        | `-w-`               |
|  011   |   3   | Write and Execute | `-wx`               |
|  100   |   4   | Read only         | `r--`               |
|  101   |   5   | Read and Execute  | `r-x`               |
|  110   |   6   | Read and Write    | `rw-`               |
|  111   |   7   | Full Access       | `rwx`               |

Instead of remembering the entire table, you only need to remember the following
permissions:
```pseudocode
r=4
w=2
x=1
```
To get the required permission for a user class, add the required permissions
together. For example, `rw-` can be obtained as `4+2=6`. This is the same as the
one given in the table above.

**Note**: The actual operation you are doing is `bitwise OR` and not arithmetic
addition.
{: .notice--info}

Consider the following example:
```bash
$ chmod 755 dummyfile
```
* owner: `7=4+2+1` i.e. `rwx`
* group: `5=4+1` i.e. `r-x`
* others: `5=4+1` i.e. `r-x`

### Changing permissions in bulk
If you look up the [manual for `chmod`][man-chmod], you might have seen the `-R`
or `--recursive` option which is explained as:
> Recursively change permissions of directories and their contents.

Though it might seem as forward way to get the job done, `-R` indiscriminately
modifies all child folders and files, and might yield undesirable results. It is
sufficient to change the permissions of the parent directory to prevent
unauthorized access to anything below the parent. Certain services need specific
permissions for the service to work. For example, changing permissions for
`~/.ssh/` might lead to remote login no longer working.

See this [issue][ssh-fail] reported by a user.
{: .notice}

**Note**: If interested, you can read about
[why ssh file permissions are strict][ssh-strict].
{: .notice--info}

**Warning**: Using `chmod` recursivel on the root (`/`) directory can break the
system (for e.g. removing executable bit systemwide). This can be avoided by
using the `--preserve-root` option with `chmod`. Inorder to use this option
everytime while using `chmod` command, set an alias (in `bashrc`)
`alias chmod='chmod --preserve-root'`.
{: .notice--danger}

If it is necessary to change permissions for directory tree in bulk, use `find`.
For example, to change permissions of all sub-directories in the directory tree
of `dummydir` to `755`:
```bash
$ find dummydir -type d -exec chmod 755 {} +
```

To change permissions of all files in the directory tree of`dummydir` to `644`:
```bash
$ find dummydir -type f -exec chmod 644 {} +
```

## Changing Owner and Group
If you want to change ownership of a file or directory, you can use the `chown`
command:
```bash
$ chown dummyuser dummyfile
```
This sets `dummyuser` as the new owner of the file `dummyfile`.

Similarly you can also change the group of a file or a directory using `chgrp`
command.

## References
1. [Permissions definitions][perm-linfo] by LINFO.
2. [Structure of File Mode Bits][file-mode-gnu] on GNU Coreutils Manual.
3. [File permissions and attributes][archwiki-file-perm] on Arch Linux Wiki.
4. [`chmod` manual][man-chmod] on GNU Coreutils Manual.
5. [Why are ssh file permissions strict][ssh-strict]
6. [Cannot SSH after changing permissions in root folder][ssh-fail], reported on
   LinuxQuestions.org

[perm-linfo]: http://www.linfo.org/permissions.html
[file-mode-gnu]: https://www.gnu.org/software/coreutils/manual/html_node/Mode-Structure.html#Mode-Structure
[archwiki-file-perm]: https://wiki.archlinux.org/title/File_permissions_and_attributes
[man-chmod]: https://www.gnu.org/software/coreutils/manual/html_node/chmod-invocation.html#chmod-invocation
[ssh-strict]: https://superuser.com/questions/626904/why-are-ssh-file-permissions-strict
[ssh-fail]: https://www.linuxquestions.org/questions/linux-newbie-8/cannot-ssh-after-changing-permissions-in-root-folder-4175541223/