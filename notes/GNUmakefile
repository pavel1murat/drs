tex_files=depth_of_interaction_001.tex           \
          bibliography.tex

name=depth_of_interaction_001

all: ${tex_files}
	if [ -f ${name}.dvi ] ; then rm ${name}.dvi; fi ;\
	latex ${name}.tex ; \
	dvips -o ${name}.ps ${name}.dvi ; \
	gv ${name}.ps

doi:
	echo "\def \hostname {`hostname`}"  > latex_input.tmp
	echo "\def \subdir   {paper}"      >> latex_input.tmp
	echo "\newif \ifpublic"            >> latex_input.tmp
	echo "\publicfalse"                >> latex_input.tmp
	pdflatex --job=zzx_prd ${name}.tex 

pdf:
	pdflatex  depth_of_interaction_001.tex

figures:
	$(MAKE) -C figures  $(OVERRIDES) sync

view:
	okular ${name}.pdf &

public_note:
	echo "\def \hostname {`hostname`}"  > latex_input.tmp
	echo "\def \subdir {public_note}"  >> latex_input.tmp
	echo "\newif \ifpublic"            >> latex_input.tmp
	echo "\publictrue"                 >> latex_input.tmp
	pdflatex --job=zzx_public_note ${name}.tex 

update_fcdflnx5:
	scp zzx_public_note.pdf fcdflnx5:/cdf/www/internal/physics/ewk/analyses/zzx/public/note/.

clean:
	rm *.aux *.*~ *.toc *.log *.dvi

#	ps2pdf  ${name}.ps ; \
#	xpdf ${name}.pdf
