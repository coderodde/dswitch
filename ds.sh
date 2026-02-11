rm -f ~/.dswitcher/ds_command.sh
set DSWITCH_SHELL=bash
~/.dswitcher/dswitch $@
set DSWITCH_SHELL=

if [ -f ~/.dswitcher/ds_command.sh ]; then
    . ~/.dswitcher/ds_command.sh
    rm -f ~/.dswitcher/ds_command.sh
fi
