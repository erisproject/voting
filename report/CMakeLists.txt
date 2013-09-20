project(report)

include(${CMAKE_MODULE_PATH}/UseLATEX.cmake)
find_package(LATEX)

set(PDFLATEX_COMPILER xelatex)
set(BIBTEX_COMPILER biber)

ADD_LATEX_DOCUMENT(report.tex
    BIBFILES refs.bib
    MANGLE_TARGET_NAMES
    NO_DEFAULT)

