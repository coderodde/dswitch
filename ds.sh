rm -f ~/.dswitcher/ds_command
~/.dswitcher/dswitch $@
if [ -f ~/.dsv2/ds_command ]; then
    source ~/.dswitcher/ds_command
    rm ~/.dswitcher/ds_command
fi
