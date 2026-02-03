dswitch: dswitch.cpp
	g++ -std=c++17 -O3 dswitch.cpp -o dswitch

install: dswitch
	mkdir -p "$(HOME)/.dswitcher"
	echo "home $(HOME)" >> "$(HOME)/.dswitcher/tags"
	cp dswitch "$(HOME)/.dswitcher/dswitch"
	cp ds.sh  "$(HOME)/.dswitcher/ds.sh"
	chmod 755 "$(HOME)/.dswitcher/ds.sh"
	@grep -qxF 'ds() { "$$HOME/.dswitcher/ds.sh" "$$@"; }' "$(HOME)/.bashrc" || \
	echo 'ds() { "$$HOME/.dswitcher/ds.sh" "$$@"; }' >> "$(HOME)/.bashrc"

uninstall:
	rm -rf "$(HOME)/.dswitcher"
	@sed -i -e '/^ds() { "\$$HOME\/\.dswitcher\/ds\.sh" "\$$@"; }$$/d' "$(HOME)/.bashrc"

