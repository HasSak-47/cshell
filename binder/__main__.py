from os import listdir
from pprint import pprint
from .lsp import *
from .clangd import init_clang, parse_func_sign

def main():
    clangd = init_clang()

    for file in listdir('hot'):
        open_document(clangd, f'hot/{file}')
    # for file in listdir('include'):
    #     open_document(clangd, f'include/{file}')

    syms = get_all_symbols(clangd)
    for sym in syms:
        if 'hot/' not in sym['location']['uri']:
            continue
        pprint((SymbolType( sym['kind'] ), get_signature(clangd, sym)))


if __name__ == "__main__":
    main()
