.PHONY : report title clear main open

report : title main open

title : ./doc/report.docx
	-mkdir pdf
	unoconv -o ./pdf/title.pdf ./doc/report.docx

main : ./tex/MakefileTex
	cd ./tex && make -f MakefileTex
	pdfunite pdf/title.pdf pdf/report.pdf pdf/fullReport.pdf

open :
	okular pdf/fullReport.pdf

clear :
	rm -rf ./pdf/ ./out/
