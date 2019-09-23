# mhl-tool

The MHL tool is a reference implementation for creating and verifying MHL files (http://mediahashlist.org/). It's a cross-platform command line tool, available for Mac OS X, Linux and Windows that allows to create and verify MHL files.

> Please note that there are currently efforts on the way of specifying a version 2 of MHL. 
> 
> The specification process is taking place at the Advanced Data Management Subcommittee of the ASC Motion Imaging Technology Council at the [American Society of Cinematographers](https://theasc.com) (ASC). 

#### Context

In the file-based workflows of digital cinematography today the secure and reliable transfer of media files is one of the essential activities to be performed in a production. The Media Hash List specification ensures that the complete source media is transferred without any alterations.

#### Inventory list for folders

The Media Hash List (MHL) standard specifies how a common inventory list of a folder's content looks like.

MHL is an XML format, that basically lists all the files in a folder and all of its subfolders as well as corresponding checksums. This list is accompanied by additional information about the creation of the MHL file. 

Technically speaking, the MHL file comprehends all the information necessary to tell if the folder structure or any of the contained files was modified since the MHL file was created.

#### Reference implementation

This repository contains an implementation of the MHL standard (http://mediahashlist.org/mhl-specification/). 

The MHL tool has currently several subcommands: 'seal', 'verify', 'hash', 'file' and 'help'. They vary in complexity and in their input and output arguments: 'files and folders', 'hashes' and 'MHL files'. Below is an illustration of the relations between subcommands and arguments.

##### mhl seal

This is the preferred command to seal the contents of folders. 'mhl seal' takes folders as input and outputs an MHL file. The created MHL file references all files in the input folder and generates hashes for them.

##### mhl verify

This is the preferred command to verify folders by MHL files. 'mhl verify' takes folders as input, searches for MHL files in them and outputs information about the consistency and completeness of the MHL files.

##### mhl hash

This is the command to create hashes from files and to verify if a hash matches a file. 'mhl hash' either takes files as input and outputs hashes for them or it takes pairs of hashes and files and outputs matching info.

##### mhl file

This is the command to create MHL files from pairs of hashes and files. Furthermore, it is the command to parse MHL files and output the contained pairs of hashes and files.


#### Installation and build

Precompiled binaries for OS X, Windows and Linux can be downloaded from http://pomfort.com/download

#### Dependencies
MHL Tool has very little dependencies. The source code for xxhash (https://github.com/Cyan4973/xxHash) is included in the repository.

#### Building on OS X

Use XCode 6.4 and the XCode workspace at `dev_envs/MacOSX_10.7/MHL_Tools.xcworkspace`

#### Building on Linux

The makefile is located at `dev_envs/Ubuntu_12.04_x64`

Dependencies: `libxml2-devel` `openssl-devel`

After calling `make release` the results will be placed in `bin/Ubuntu_12.04_x64/Release`

Build instructions for CentOS 7:

	sudo yum groupinstall "Development Tools"
	sudo yum install openssl-devel
	sudo yum install libxml2-devel
	cd dev_envs/Ubuntu_12.04_x64
	make release

#### Building on Windows

It should be possible to build the sources on Windows using Visual Studio 2013 or 2010


 
 



