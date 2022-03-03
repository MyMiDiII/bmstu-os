.PHONY : report clear main open

report : main open

main : ./tex/MakefileTex
	cd ./tex && make -f MakefileTex

open :
	xdg-open pdf/report.pdf

clear :
	rm -rf ./pdf/ ./out/
	cd ./tex && make -f MakefileTex clear
