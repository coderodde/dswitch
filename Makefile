dswitch: dswitch.cpp directory_entry.hpp directory_entry_table.hpp
	g++ -std=c++20 -O3 dswitch.cpp -o dswitch

install: dswitch
	mkdir -p "$(HOME)/.dswitcher"
	echo "home $(HOME)" >> "$(HOME)/.dswitcher/tags"
	cp dswitch "$(HOME)/.dswitcher/dswitch"
	cp ds.sh  "$(HOME)/.dswitcher/ds.sh"
	chmod 755 "$(HOME)/.dswitcher/ds.sh"
	@grep -qxF "alias ds='. $$HOME/.dswitcher/ds.sh'" "$(HOME)/.bashrc" || \
	echo "alias ds='. $$HOME/.dswitcher/ds.sh'" >> "$(HOME)/.bashrc"

uninstall:
	rm -rf "$(HOME)/.dswitcher"
	@sed -i'' "/^alias ds=/d" "$(HOME)/.bashrc" 2>/dev/null || \
	sed -i  "/^alias ds=/d" "$(HOME)/.bashrc"

