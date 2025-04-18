from .lsp import Lsp, lsp_write_message, lsp_read_message

def init_clang():
    args = ['clangd', '--compile-commands-dir=.', '--log=error']
    clangd = Lsp(args)
    if clangd.stdin == None or clangd.stdout == None:
        exit(-1)

    lsp_write_message(clangd,{
        'jsonrpc': '2.0',
        'id': 1,
        'method': 'initialize',
        'params': {
            'processId': None,
            'rootUri': None,
            'capabilities': {},
        }
    })

    # Read Ok message
    lsp_read_message(clangd)

    return clangd
