rm -f ~/.dswitcher/ds_command.sh
~/.dswitcher/dswitch $@

if [ -f ~/.dswitcher/ds_command.sh ]; then
    . ~/.dswitcher/ds_command.sh
    rm -f ~/.dswitcher/ds_command.sh
fi
