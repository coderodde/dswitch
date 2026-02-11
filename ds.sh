rm -f ~/.dswitcher/ds_command.sh
export DSWITCH_SHELL=bash
~/.dswitcher/dswitch $@
unset DSWITCH_SHELL

if [ -f ~/.dswitcher/ds_command.sh ]; then
    . ~/.dswitcher/ds_command.sh
    rm -f ~/.dswitcher/ds_command.sh
fi
