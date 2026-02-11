# dswitch
A C++ remake of the directory switcher for Windows and *nix.

# Installing
Before installing, clone this repository with the commamnd:
```
git clone https://github.com/coderodde/dswitch.git
```
and change to directory:
```
cd dswitch
```

### Installing for Windows using cmd.exe
From `dswitch` directory, run:
```
ds_install.cmd
```

### Installing for Windows using PowerShell
From `dswitch` directory, run:
```
& ds_install.ps1
```

The above two will setup `dswitch` for both `cmd.exe` and PowerShell.

### Installing for Bash
From `dswitch` directory, run:
```
make
make install
```

# Uninstalling

### Uninstalling for Windows using cmd.exe
From `dswitch`, run:
```
ds_uninstall.cmd
```

### Uninstalling for Windows using PowerShell
From `dswitch`, run:
```
& ds_uninstall.ps1
```

### Uninstalling for Bash
From `dswitch`, run:
```
make uninstall
```

# Getting help
Just run: 
```
ds -h
```
for the help message explaining the entire CLI.

# Notes
1. After installing, you need to initiate a new command line/terminal session for the changes to take effect.
2. The actual tags are stored in `~/.dswitcher/tags` file. The format of each entry is `TAG DIR`.
3. `dswitch` relies on Levenshtein distance: if the given tag is not present in the tags file, the closest match will be selected.
4. Invoking `ds` without arguments all over again, the tool will loop between two most recent directories.
