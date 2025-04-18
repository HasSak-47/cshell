from os import listdir
from pprint import pprint
from lsp import *
from clangd import init_clang

def main():
    clangd = init_clang()

    for file in listdir('hot'):
        open_document(clangd, f'hot/{file}')

    pprint(get_all_symbols(clangd, 'Function'))


if __name__ == "__main__":
    main()
