from os import listdir
from subprocess import Popen, PIPE
import json
from typing import Dict
from pprint import pprint

def lsp_write_message(lsp:Popen, msg: Dict):
    assert lsp.stdin is not None

    body = json.dumps(msg)
    header = f'Content-Length: {len(body)}\r\n\r\n'
    lsp.stdin.write(bytes(header + body, encoding='utf-8'))
    lsp.stdin.flush()
    if lsp.stdout == None:
        exit(-1)

# pyright lsp.stdout not null
def lsp_read_message(lsp: Popen):
    assert lsp.stdout is not None

    headers = {}
    while True:
        line : str = lsp.stdout.readline().decode('utf-8')
        if line == '\r\n' or line == '\n':
            break
        key, value = line.strip().split(':', 1)
        headers[key.strip()] = value.strip()

    content_length = int(headers.get('Content-Length', 0))
    content = lsp.stdout.read(content_length)

    return json.loads(content)

# def lsp_read_error(lsp: Popen):
#     assert lsp.stderr is not None
#     while True:
#         line: str = lsp.stderr.readline().decode('utf-8')
#         print('ERROR:', line)
#         if line == '':
#             break

def open_document(lsp:Popen, name: str):
    with open(name, 'r') as file:
        text = file.read()
        uri = f'file:///{name}'
        lsp_write_message(lsp, {
            'jsonrpc': '2.0',
            'method': 'textDocument/didOpen',
            'id': 1,
            'params': {
                'textDocument': {
                    'uri': uri,
                    'languageId': 'c',
                    'version': 1,
                    'text': text
                }
            }
        })

    return lsp_read_message(lsp)

def get_all_symbols(lsp:Popen):
    lsp_write_message(lsp, {
        'jsonrpc': '2.0',
        'id': 1,
        'method' : '$/progress',
    })

    lsp_write_message(lsp, {
        'jsonrpc': '2.0',
        'id': 1,
        'method' : 'workspace/symbol',
        'params' :{ 'query': '' }
    })

    return lsp_read_message(lsp)

def init_clang():
    args = ['clangd', '--compile-commands-dir=.', '--log=info']
    print(args)
    clangd = Popen(args, stdin=PIPE, stdout=PIPE)
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

    # Ok message
    lsp_read_message(clangd)

    for file in listdir('hot'):
        open_document(clangd, f'hot/{file}')

    # from time import sleep
    pprint(get_all_symbols(clangd))


    return clangd
