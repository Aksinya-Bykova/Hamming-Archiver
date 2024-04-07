## Task

Implement a noise-resistant file archiver without compression **HamArc**, combining several files into one archive. For noise-resistant coding, use [Hamming Codes](https://en.wikipedia.org/wiki/Hamming_code). The format for storing data in the archive is also part of the task.

## Requirements

* Merge multiple files ( >= 1) into one archive in .haf format (Hamming Archive File)
* Extract all or individual files from an archive
* Merge multiple archives into one
* Restore the archive if damaged, or report that this is impossible
* Return a list of files in the archive

## Implementation

Console application that supports the following command line arguments:

**-c, --create** - creating a new archive

**-f, --file=[ARHCNAME]** - name of the archive file

**-l, --list** - display a list of files in the archive

**-x, --extract** - extract files from the archive (if not specified, then all files)

**-a, --append** - add file to archive

**-d, --delete** - delete a file from the archive

**-A, --concatenate** - merge two archives

**File names are passed as free arguments**

**Arguments for encoding and decoding are also passed via the command line** (Names and types of arguments are part of the task)

### Startup examples

*hamarc --create --file=ARCHIVE FILE1 FILE2 FILE3*

*hamarc -l -f ARCHIVE*

*hamarc --contenate ARCHIVE1 ARCHIVE2 -f ARCHIVE3*


##NB

- Files to be archived may be very large
- Operations must work optimally (the final score for the work depends on this)
- The code must be structured and logically divided into modules (the final score for the work depends on this)
- Think about how you can decompose the problem
- Using tests will be an advantage
